CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL_mixer VERSION 2.0.1)

set(SRC_C_FILES
    effects_internal.c
    effect_position.c
    effect_stereoreverse.c
    load_aiff.c
    load_voc.c
    mixer.c
    music.c
    music_cmd.c
    music_flac.c
    music_fluidsynth.c
    music_mad.c
    music_mikmod.c
    music_modplug.c
    music_mpg123.c
    music_nativemidi.c
    music_ogg.c
    music_smpeg.c
    music_timidity.c
    music_wav.c
    playmus.c
    playwave.c
)

set(SRC_H_FILES
    effects_internal.h
    load_aiff.h
    load_voc.h
    mixer.h
    music.h
    music_cmd.h
    music_flac.h
    music_fluidsynth.h
    music_mad.h
    music_mikmod.h
    music_modplug.h
    music_mpg123.h
    music_nativemidi.h
    music_ogg.h
    music_smpeg.h
    music_timidity.h
    music_wav.h
    SDL_mixer.h
)

set(SRC_FILES ${SRC_C_FILES} ${SRC_H_FILES})

add_library(SDL_mixer ${SRC_FILES})
target_link_libraries(SDL_mixer SDL2)
target_include_directories(SDL_mixer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(SDL_mixer PRIVATE
    MUSIC_WAV
    MUSIC_MID
)
