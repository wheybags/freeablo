// Adapted from https://wiki.videolan.org/LibVLC_SampleCode_SDL/
// Make sure VLC_PLUGIN_PATH env variable is set and using newish VLC (tested with 3.0.4).
// Currently a pretty hacky implementation, and there's bound to be a much easier way.
// Creates a new SDL window and GL context for video, as otherwise SDL_CreateRenderer alters
// the current GL context, causing a crash. Attempts to swap GL context back and forth when
// video is playing/stopped.
// VIDEO_DIRECT_WINDOW_RENDER is an attempt at rendering directly to an existing window,
// although seems to crash after video has finished. Also seems to require all the functions
// in sdl_gl_funcs.h/.cpp to be renamed to non-default GL names or crashes beforehand.
#include "video.h"

#include <condition_variable>
#include <faio/fafileobject.h>
#include <iostream>
#include <mutex>
#include <SDL.h>
#include <vlc/vlc.h>

#ifdef VIDEO_DIRECT_WINDOW_RENDER
#include <SDL_syswm.h>
#  if __APPLE__
#include "MacNSView.h"
#  endif
#endif

#define SDL_FULLSCREEN_MASK (SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_FULLSCREEN)

namespace Video
{
    static libvlc_instance_t *vlcInstance;
    static libvlc_media_player_t *vlcMediaPlayer;
    static libvlc_event_manager_t *vlcEventManager;
    static SDL_GLContext mainGlContext;
    static SDL_Window *mainWindow;
    static SDL_GLContext glContext;
    static SDL_Window *window;
    static Uint32 fullscreenFlagsSave;
    static std::mutex cvMutex;
    static bool videoPlaying;
    static std::condition_variable videoCompleteCV;
    static bool videoCleanupRequired;
    static std::condition_variable videoCleanupCompleteCV;

#ifdef VIDEO_DIRECT_WINDOW_RENDER
#  if __APPLE__
    static void *macContentView;
#  endif
#else
    struct videoCbContext {
        SDL_Renderer *renderer;
        SDL_Texture *texture;
    };
    static struct videoCbContext videoCbContext;

    static void *video_lock_cb(void *data, void **p_pixels);
    static void video_unlock_cb(void *data, void *id, void *const *p_pixels);
    static void video_display_cb(void *data, void *id);
#endif

    static void videoCleanup();
    static void vlc_event_callback(const struct libvlc_event_t *p_event, void *p_data);
    static int media_open_cb(void *opaque, void **datap, uint64_t *sizep);
    static ssize_t media_read_cb(void *opaque, unsigned char *buf, size_t len);
    static int media_seek_cb(void *opaque, uint64_t offset);
    static void media_close_cb(void *opaque);

    void init()
    {
        // NOTE: This module does not handle initialising SDL.
        // SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) should have already been called.

        // Check for VLC_PLUGIN_PATH environmental variable.
        const char *pluginPath = getenv("VLC_PLUGIN_PATH");
        if (pluginPath != NULL)
            std::cout << "VLC_PLUGIN_PATH: " << pluginPath << std::endl;
        else
        {
#if __APPLE__
            // Test/hack fall back to default plugging path for mac.
            pluginPath = "/Applications/VLC.app/Contents/MacOS/plugins";
            std::cout << "VLC_PLUGIN_PATH defaulting to: " << pluginPath << std::endl;
            setenv("VLC_PLUGIN_PATH", pluginPath, 1);
#else
            std::cout << "VLC_PLUGIN_PATH not set" << std::endl;
#endif
        }

        mainWindow = SDL_GL_GetCurrentWindow();
        mainGlContext = SDL_GL_GetCurrentContext();

        window = SDL_CreateWindow(
                mainWindow ? SDL_GetWindowTitle(mainWindow) : "",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                1280, 960, // Any value, window currently always fullscreen.
                SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
        if (window == NULL)
            std::cerr << "Could not create window: " << SDL_GetError() << std::endl;

        glContext = SDL_GL_CreateContext(window);

        if (mainWindow && mainGlContext)
        {
//            SDL_HideWindow(window);
            SDL_GL_MakeCurrent(mainWindow, mainGlContext);
            SDL_RaiseWindow(mainWindow);
        }

        vlcInstance = libvlc_new(0, NULL);
        if (vlcInstance == NULL)
        {
            const char *error_msg = libvlc_errmsg();
            std::cerr << "VLC init failed: " << (error_msg ? error_msg : "Unknown error") << std::endl;
        }

        vlcMediaPlayer = libvlc_media_player_new(vlcInstance);
        // Aspect ratio doesn't work with libvlc_video_set_callbacks (https://trac.videolan.org/vlc/ticket/8122).
        //libvlc_video_set_aspect_ratio(vlcMediaPlayer, "16:9");

        vlcEventManager = libvlc_media_player_event_manager(vlcMediaPlayer);

        libvlc_event_attach(vlcEventManager, libvlc_MediaPlayerEndReached, vlc_event_callback, NULL);
        libvlc_event_attach(vlcEventManager, libvlc_MediaPlayerStopped, vlc_event_callback, NULL);
    }

    void quit()
    {
        libvlc_event_detach(vlcEventManager, libvlc_MediaPlayerEndReached, vlc_event_callback, NULL);
        libvlc_event_detach(vlcEventManager, libvlc_MediaPlayerStopped, vlc_event_callback, NULL);
        libvlc_media_player_release(vlcMediaPlayer);
        libvlc_release(vlcInstance);
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
    }

    void update()
    {
        if (!videoPlaying && videoCleanupRequired)
        {
            videoCleanup();
        }
    }

    void playVideo(const std::string& path)
    {
        if (videoPlaying || videoCleanupRequired)
        {
            std::cerr << "Error: attempting to play video before last video complete: " << path << std::endl;
            return;
        }

        std::cout << "Playing video: " << path << std::endl;

        if (mainWindow && mainGlContext)
        {
//            SDL_HideWindow(mainWindow);
            // SDL_RaiseWindow doesn't seem to work with two fullscreen windows.
            // Disable fullscreen of parent window.
            Uint32 flags = SDL_GetWindowFlags(mainWindow);
            fullscreenFlagsSave = flags & SDL_FULLSCREEN_MASK;
            SDL_SetWindowFullscreen(mainWindow, flags & ~SDL_FULLSCREEN_MASK);

            SDL_GL_MakeCurrent(window, glContext);

//            SDL_ShowWindow(window);
            SDL_RaiseWindow(window);
        }

#ifdef VIDEO_DIRECT_WINDOW_RENDER
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
#  if __APPLE__
        NSWindow *nsWindow = wmInfo.info.cocoa.window;
        macContentView = MacNSViewGetContentViewFromWindow(nsWindow);
        libvlc_media_player_set_nsobject(vlcMediaPlayer, macContentView);
#  elif defined(_WIN32)
        HWND hwnd = wmInfo.info.win.window;
        libvlc_media_player_set_hwnd(vlcMediaPlayer, hwnd);
#  else
        uint32_t xid = wmInfo.info.x11.?;
        libvlc_media_player_set_xwindow(vlcMediaPlayer, xid);
#  endif
#else
        int width, height;
        SDL_GL_GetDrawableSize(window, &width, &height);

        videoCbContext.renderer = SDL_CreateRenderer(window, -1, 0);
        if (!videoCbContext.renderer) {
            std::cerr << "Couldn't create renderer: " << SDL_GetError() << std::endl;
        }
        videoCbContext.texture = SDL_CreateTexture(
                videoCbContext.renderer,
                SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING,
                width, height);
        if (!videoCbContext.texture) {
            std::cerr << "Couldn't create texture: " << SDL_GetError() << std::endl;
        }

        libvlc_video_set_callbacks(
                vlcMediaPlayer, video_lock_cb, video_unlock_cb,
                video_display_cb, &videoCbContext);
        libvlc_video_set_format(vlcMediaPlayer, "RV16", width, height, width*2);
#endif

        std::string *p = new std::string(path);
        libvlc_media_t *media = libvlc_media_new_callbacks(
                vlcInstance, media_open_cb, media_read_cb,
                media_seek_cb, media_close_cb, p);
        libvlc_media_player_set_media(vlcMediaPlayer, media);
        libvlc_media_release(media);

        SDL_ShowCursor(SDL_DISABLE);
        libvlc_media_player_play(vlcMediaPlayer);

        videoPlaying = true;
        videoCleanupRequired = true;
    }

    void stopVideo()
    {
        libvlc_media_player_stop(vlcMediaPlayer);
    }

    bool waitForVideoComplete(int ms)
    {
        auto tWait = std::chrono::milliseconds(ms);
        std::unique_lock<std::mutex> lk(cvMutex);
        if (videoCompleteCV.wait_for(lk, tWait, []{return !videoPlaying;}))
            if (videoCleanupCompleteCV.wait_for(lk, tWait, []{return !videoCleanupRequired;}))
                return true;
        return false;
    }

    static void videoCleanup()
    {
        SDL_ShowCursor(SDL_ENABLE);

#ifdef VIDEO_DIRECT_WINDOW_RENDER
#  if __APPLE__
        libvlc_media_player_set_nsobject(vlcMediaPlayer, NULL);
        MacNSViewFreeContentView(macContentView);
#  elif defined(_WIN32)
        libvlc_media_player_set_hwnd(vlcMediaPlayer, NULL);
#  else
        libvlc_media_player_set_xwindow(vlcMediaPlayer, NULL);
#  endif
#else
        // SDL_DestroyTexture is done by SDL_DestroyRenderer.
        SDL_DestroyRenderer(videoCbContext.renderer);
#endif

        if (mainWindow && mainGlContext)
        {
//            SDL_HideWindow(window);
            SDL_GL_MakeCurrent(mainWindow, mainGlContext);
            SDL_SetWindowFullscreen(mainWindow, fullscreenFlagsSave);
//            SDL_ShowWindow(mainWindow);
            SDL_RaiseWindow(mainWindow);
        }

        std::unique_lock<std::mutex> lk(cvMutex);
        videoCleanupRequired = false;
        videoCleanupCompleteCV.notify_all();
    }

    static void vlc_event_callback(const struct libvlc_event_t *p_event, void *p_data)
    {
        (void)p_data;
        switch (p_event->type)
        {
        case libvlc_MediaPlayerEndReached:
        case libvlc_MediaPlayerStopped:
        {
            std::unique_lock<std::mutex> lk(cvMutex);
            videoPlaying = false;
            videoCompleteCV.notify_all();
            break;
        }
        default:
            break;
        }
    }

    static int media_open_cb(void *opaque, void **datap, uint64_t *sizep)
    {
        std::string *path = (std::string *)opaque;
        FAIO::FAFileObject *f = new FAIO::FAFileObject(*path);
        delete path;
        *datap = f;
        *sizep = f->FAsize();
        f->FAfseek(0, SEEK_SET);
        return 0;
    }
    static ssize_t media_read_cb(void *opaque, unsigned char *buf, size_t len)
    {
        FAIO::FAFileObject *f = (FAIO::FAFileObject *)opaque;
        return f->FAfread(buf, 1, len);
    }
    static int media_seek_cb(void *opaque, uint64_t offset)
    {
        FAIO::FAFileObject *f = (FAIO::FAFileObject *)opaque;
        return f->FAfseek(offset, SEEK_SET);
    }
    static void media_close_cb(void *opaque)
    {
        FAIO::FAFileObject *f = (FAIO::FAFileObject *)opaque;
        delete f;
    }

#ifndef VIDEO_DIRECT_WINDOW_RENDER
    static void *video_lock_cb(void *data, void **p_pixels) {
        struct videoCbContext *c = (struct videoCbContext *)data;
        int pitch;
        SDL_LockTexture(c->texture, NULL, p_pixels, &pitch);
        return NULL;
    }

    static void video_unlock_cb(void *data, void *id, void *const *p_pixels) {
        (void)id;
        (void)p_pixels;
        struct videoCbContext *c = (struct videoCbContext *)data;
        SDL_UnlockTexture(c->texture);
    }

    static void video_display_cb(void *data, void *id) {
        (void)id;
        struct videoCbContext *c = (struct videoCbContext *)data;
//        SDL_RenderClear(c->renderer);
        SDL_RenderCopy(c->renderer, c->texture, NULL, NULL);
        SDL_RenderPresent(c->renderer);
    }
#endif
}
