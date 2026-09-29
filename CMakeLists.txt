cmake_minimum_required(VERSION 3.20)

project(doom-cpp DESCRIPTION "Linux Doom 1.10, ported to C++" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DOOM_LEGACY_WARNINGS "Show warnings from legacy sources" OFF)
option(DOOM_WARNINGS_AS_ERRORS "Enable warnings and treat them as errors" OFF)

# Compile the original .c files as C++ while they are migrated incrementally.
# The DOS drivers in ipx/ and sersrc/ are not part of the Linux executable.
set(DOOM_SOURCES
    linuxdoom-1.10/am_map.c
    linuxdoom-1.10/d_items.c
    linuxdoom-1.10/d_main.c
    linuxdoom-1.10/d_net.c
    linuxdoom-1.10/doomdef.c
    linuxdoom-1.10/doomstat.c
    linuxdoom-1.10/dstrings.c
    linuxdoom-1.10/f_finale.c
    linuxdoom-1.10/f_wipe.c
    linuxdoom-1.10/g_game.c
    linuxdoom-1.10/hu_lib.c
    linuxdoom-1.10/hu_stuff.c
    linuxdoom-1.10/i_main.c
    linuxdoom-1.10/i_net.c
    linuxdoom-1.10/i_sound.c
    linuxdoom-1.10/i_system.c
    linuxdoom-1.10/i_video.c
    linuxdoom-1.10/info.c
    linuxdoom-1.10/m_argv.c
    linuxdoom-1.10/m_bbox.c
    linuxdoom-1.10/m_cheat.c
    linuxdoom-1.10/m_fixed.c
    linuxdoom-1.10/m_menu.c
    linuxdoom-1.10/m_misc.c
    linuxdoom-1.10/m_random.c
    linuxdoom-1.10/m_swap.c
    linuxdoom-1.10/p_ceilng.c
    linuxdoom-1.10/p_doors.c
    linuxdoom-1.10/p_enemy.c
    linuxdoom-1.10/p_floor.c
    linuxdoom-1.10/p_inter.c
    linuxdoom-1.10/p_lights.c
    linuxdoom-1.10/p_map.c
    linuxdoom-1.10/p_maputl.c
    linuxdoom-1.10/p_mobj.c
    linuxdoom-1.10/p_plats.c
    linuxdoom-1.10/p_pspr.c
    linuxdoom-1.10/p_saveg.c
    linuxdoom-1.10/p_setup.c
    linuxdoom-1.10/p_sight.c
    linuxdoom-1.10/p_spec.c
    linuxdoom-1.10/p_switch.c
    linuxdoom-1.10/p_telept.c
    linuxdoom-1.10/p_tick.c
    linuxdoom-1.10/p_user.c
    linuxdoom-1.10/r_bsp.c
    linuxdoom-1.10/r_data.c
    linuxdoom-1.10/r_draw.c
    linuxdoom-1.10/r_main.c
    linuxdoom-1.10/r_plane.c
    linuxdoom-1.10/r_segs.c
    linuxdoom-1.10/r_sky.c
    linuxdoom-1.10/r_things.c
    linuxdoom-1.10/s_sound.c
    linuxdoom-1.10/sounds.c
    linuxdoom-1.10/st_lib.c
    linuxdoom-1.10/st_stuff.c
    linuxdoom-1.10/tables.c
    linuxdoom-1.10/v_video.c
    linuxdoom-1.10/w_wad.c
    linuxdoom-1.10/wi_stuff.c
    linuxdoom-1.10/z_zone.c
)

set_source_files_properties(${DOOM_SOURCES} PROPERTIES LANGUAGE CXX)
add_executable(doom ${DOOM_SOURCES})
target_include_directories(doom PRIVATE linuxdoom-1.10)
target_compile_definitions(doom PRIVATE NORMALUNIX LINUX)

# Retain the legacy arithmetic and aliasing behavior.
target_compile_options(doom PRIVATE -fno-strict-aliasing -fwrapv)
if(DOOM_LEGACY_WARNINGS OR DOOM_WARNINGS_AS_ERRORS)
    target_compile_options(doom PRIVATE -Wall -Wextra -Wpedantic)
else()
    target_compile_options(doom PRIVATE -w)
endif()
if(DOOM_WARNINGS_AS_ERRORS)
    target_compile_options(doom PRIVATE -Werror)
endif()

find_package(X11 REQUIRED)
if(NOT X11_XShm_INCLUDE_PATH)
    message(FATAL_ERROR "X11/extensions/XShm.h is required (install libxext-dev).")
endif()
target_link_libraries(doom PRIVATE X11::X11 X11::Xext m)

install(TARGETS doom RUNTIME DESTINATION bin)

include(CTest)
if(BUILD_TESTING)
    add_executable(random_tests tests/random_tests.cpp linuxdoom-1.10/m_random.c)
    target_include_directories(random_tests PRIVATE linuxdoom-1.10)
    target_compile_options(random_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(random_tests PRIVATE -Werror)
    endif()
    foreach(case sequence wraparound reset independence)
        add_test(NAME random.${case} COMMAND random_tests ${case})
    endforeach()

    add_executable(fixed_mul_tests tests/fixed_mul_tests.cpp)
    target_include_directories(fixed_mul_tests PRIVATE linuxdoom-1.10)
    target_compile_options(fixed_mul_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(fixed_mul_tests PRIVATE -Werror)
    endif()
    add_test(NAME fixed.multiply COMMAND fixed_mul_tests)
endif()

if(BUILD_TESTING)
    add_executable(render_boundary_tests tests/render_boundary_tests.cpp
        linuxdoom-1.10/v_video.c linuxdoom-1.10/m_bbox.c linuxdoom-1.10/tables.c
        linuxdoom-1.10/m_fixed.c)
    target_include_directories(render_boundary_tests PRIVATE linuxdoom-1.10)
    target_compile_definitions(render_boundary_tests PRIVATE NORMALUNIX LINUX)
    target_compile_options(render_boundary_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(render_boundary_tests PRIVATE -Werror)
    endif()
    add_test(NAME render.boundaries COMMAND render_boundary_tests)
endif()

if(BUILD_TESTING)
    add_executable(savegame_tests tests/savegame_tests.cpp linuxdoom-1.10/p_saveg.c)
    target_include_directories(savegame_tests PRIVATE linuxdoom-1.10)
    target_compile_definitions(savegame_tests PRIVATE NORMALUNIX LINUX)
    target_compile_options(savegame_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(savegame_tests PRIVATE -Werror)
    endif()
    add_test(NAME savegame.alignment COMMAND savegame_tests)
endif()

if(BUILD_TESTING)
    add_executable(fixed_div_tests tests/fixed_div_tests.cpp linuxdoom-1.10/m_fixed.c)
    target_include_directories(fixed_div_tests PRIVATE linuxdoom-1.10)
    target_compile_options(fixed_div_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(fixed_div_tests PRIVATE -Werror)
    endif()
    add_test(NAME fixed.divide COMMAND fixed_div_tests)
endif()

if(BUILD_TESTING)
    add_executable(lighting_tests tests/lighting_tests.cpp)
    target_include_directories(lighting_tests PRIVATE linuxdoom-1.10)
    target_compile_options(lighting_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(lighting_tests PRIVATE -Werror)
    endif()
    add_test(NAME render.lighting COMMAND lighting_tests)
endif()
