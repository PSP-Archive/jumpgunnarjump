cmake_minimum_required(VERSION 3.14)
project(jumpgunnarjump)




add_executable(jumpgunnarjump main.c engine.c)
target_link_libraries(jumpgunnarjump SDL SDL_mixer SDL_image SDL_ttf pspgu GL pspge pspaudio pspctrl psphprm pspirkeyb pspdisplay vorbisfile ogg vorbis jpeg png psppower freetype z bz2 pspvfpu )

if(PSP)
create_pbp_file(
    TARGET jumpgunnarjump
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE jumpgunnarjump
    BUILD_PRX
    )

endif()
