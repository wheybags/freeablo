// Adapted from:
//  https://github.com/mpv-player/mpv-examples/blob/master/libmpv/sdl/main.c
//  https://github.com/mpv-player/mpv-examples/blob/master/libmpv/streamcb/simple-streamcb.c
#include "video.h"

#include <SDL.h>
#include <condition_variable>
#include <faio/fafileobject.h>
#include <iostream>
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <mpv/stream_cb.h>
#include <mutex>

namespace Video
{
    static mpv_handle* mpv;
    static mpv_render_context* mpv_gl;
    static bool redraw;
    static std::mutex cvMutex;
    static bool videoPlaying;
    static std::condition_variable videoCompleteCV;

    static void check_error(int status);
    static void* get_proc_address_mpv(void* fn_ctx, const char* name);
    static void on_mpv_redraw(void* ctx);
    static int media_open_fn(void* user_data, char* uri, mpv_stream_cb_info* info);
    static int64_t media_read_fn(void* cookie, char* buf, uint64_t nbytes);
    static int64_t media_seek_fn(void* cookie, int64_t offset);
    static void media_close_fn(void* cookie);

    void init()
    {
        // NOTE: This module does not handle initialising SDL.
        // SDL_Init, SDL_CreateWindow and SDL_GL_CreateContext should have already been called.

        mpv = mpv_create();
        if (!mpv)
            std::cerr << "Failed creating MPV context" << std::endl;
        check_error(mpv_initialize(mpv));

        mpv_opengl_init_params mpvOpenglInitParams = {};
        mpvOpenglInitParams.get_proc_address = get_proc_address_mpv;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_API_TYPE, (void*)MPV_RENDER_API_TYPE_OPENGL},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &mpvOpenglInitParams},
            {MPV_RENDER_PARAM_INVALID, NULL} // Terminate parameters.
        };

        // This makes mpv use the currently set GL context. It will use the callback
        // (passed via params) to resolve GL builtin functions, as well as extensions.
        if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
            std::cerr << "failed to initialize mpv GL context" << std::endl;

        // When a new frame should be drawn with mpv_opengl_cb_draw().
        // (Separate from the normal event handling mechanism for the sake of
        //  users which run OpenGL on a different thread.)
        mpv_render_context_set_update_callback(mpv_gl, on_mpv_redraw, NULL);

        check_error(mpv_stream_cb_add_ro(mpv, "fafile", NULL, media_open_fn));
    }

    void quit()
    {
        mpv_render_context_free(mpv_gl);
        mpv_terminate_destroy(mpv);
    }

    void update()
    {
        // If this is ever jittery we could make a redraw condition_variable
        // and wait on it for longer than a frame.
        if (videoPlaying && redraw)
        {
            int w, h;
            SDL_Window* window = SDL_GL_GetCurrentWindow();
            SDL_GetWindowSize(window, &w, &h);
            // Specify the default framebuffer (0) as target. This will
            // render onto the entire screen. If you want to show the video
            // in a smaller rectangle or apply fancy transformations, you'll
            // need to render into a separate FBO and draw it manually.
            mpv_opengl_fbo openGlFbo = {};
            openGlFbo.fbo = 0;
            openGlFbo.w = w;
            openGlFbo.h = h;
            openGlFbo.internal_format = 0;
            // Flip rendering (needed due to flipped GL coordinate system).
            int flipY = 1;
            mpv_render_param params[] = {
                {MPV_RENDER_PARAM_OPENGL_FBO, &openGlFbo}, {MPV_RENDER_PARAM_FLIP_Y, &flipY}, {MPV_RENDER_PARAM_INVALID, NULL} // Terminate parameters.
            };
            // See render_gl.h on what OpenGL environment mpv expects, and
            // other API details.
            mpv_render_context_render(mpv_gl, params);
            SDL_GL_SwapWindow(window);
        }

        // Handle mpv events.
        while (1)
        {
            mpv_event* mp_event = mpv_wait_event(mpv, 0);
            if (mp_event->event_id == MPV_EVENT_NONE)
                break;
            if (mp_event->event_id == MPV_EVENT_END_FILE)
            {
                // Video playback complete.
                SDL_ShowCursor(SDL_ENABLE);
                std::unique_lock<std::mutex> lk(cvMutex);
                videoPlaying = false;
                videoCompleteCV.notify_all();
            }
        }
    }

    void playVideo(const std::string& path)
    {
        std::cout << "Playing video: " << path << std::endl;

        SDL_ShowCursor(SDL_DISABLE);

        std::string pathWithUri = std::string("fafile://") + path;

        const char* cmd[] = {"loadfile", pathWithUri.c_str(), NULL};
        check_error(mpv_command(mpv, cmd));

        videoPlaying = true;

        update();
    }

    void stopVideo()
    {
        const char* cmd[] = {"stop", NULL};
        check_error(mpv_command(mpv, cmd));
    }

    bool waitForVideoComplete(int ms)
    {
        auto tWait = std::chrono::milliseconds(ms);
        std::unique_lock<std::mutex> lk(cvMutex);
        if (videoCompleteCV.wait_for(lk, tWait, [] { return !videoPlaying; }))
            return true;
        return false;
    }

    static void check_error(int status)
    {
        if (status < 0)
            std::cerr << "MPV API error: " << mpv_error_string(status) << std::endl;
    }

    static void* get_proc_address_mpv(void* fn_ctx, const char* name)
    {
        (void)fn_ctx;
        return SDL_GL_GetProcAddress(name);
    }

    static void on_mpv_redraw(void* ctx)
    {
        // Do the rendering in the main loop for thread safety.
        (void)ctx;
        redraw = true;
    }

    static int media_open_fn(void* user_data, char* uri, mpv_stream_cb_info* info)
    {
        (void)user_data;
        std::string prefix = "fafile://";
        std::string path = std::string(&uri[prefix.length()]);
        FAIO::FAFileObject* fp = new FAIO::FAFileObject(path);
        info->cookie = fp;
        info->read_fn = media_read_fn;
        info->seek_fn = media_seek_fn;
        info->close_fn = media_close_fn;
        return fp ? 0 : MPV_ERROR_LOADING_FAILED;
    }

    static int64_t media_read_fn(void* cookie, char* buf, uint64_t nbytes)
    {
        FAIO::FAFileObject* fp = (FAIO::FAFileObject*)cookie;
        return fp->FAfread(buf, 1, nbytes);
    }

    static int64_t media_seek_fn(void* cookie, int64_t offset)
    {
        FAIO::FAFileObject* fp = (FAIO::FAFileObject*)cookie;
        return fp->FAfseek(offset, SEEK_SET);
    }

    static void media_close_fn(void* cookie)
    {
        FAIO::FAFileObject* fp = (FAIO::FAFileObject*)cookie;
        delete fp;
    }
}
