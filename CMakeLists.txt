cmake_minimum_required(VERSION 3.20)

project(doom-cpp DESCRIPTION "Linux Doom 1.10, ported to C++" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DOOM_LEGACY_WARNINGS "Show warnings from legacy sources" OFF)
option(DOOM_WARNINGS_AS_ERRORS "Enable warnings and treat them as errors" OFF)

# Engine sources use C++17 while they are migrated incrementally.
# The DOS drivers in ipx/ and sersrc/ are not part of the Linux executable.
set(DOOM_SOURCES
    linuxdoom-1.10/am_map.cpp
    linuxdoom-1.10/d_items.cpp
    linuxdoom-1.10/d_main.cpp
    linuxdoom-1.10/d_net.cpp
    linuxdoom-1.10/doomdef.cpp
    linuxdoom-1.10/doomstat.cpp
    linuxdoom-1.10/dstrings.cpp
    linuxdoom-1.10/f_finale.cpp
    linuxdoom-1.10/f_wipe.cpp
    linuxdoom-1.10/g_game.cpp
    linuxdoom-1.10/hu_lib.cpp
    linuxdoom-1.10/hu_stuff.cpp
    linuxdoom-1.10/i_main.cpp
    linuxdoom-1.10/i_net.cpp
    linuxdoom-1.10/i_sound.cpp
    linuxdoom-1.10/i_system.cpp
    linuxdoom-1.10/i_video.cpp
    linuxdoom-1.10/info.cpp
    linuxdoom-1.10/m_argv.cpp
    linuxdoom-1.10/m_bbox.cpp
    linuxdoom-1.10/m_cheat.cpp
    linuxdoom-1.10/m_fixed.cpp
    linuxdoom-1.10/m_menu.cpp
    linuxdoom-1.10/m_misc.cpp
    linuxdoom-1.10/m_random.cpp
    linuxdoom-1.10/m_swap.cpp
    linuxdoom-1.10/p_ceilng.cpp
    linuxdoom-1.10/p_doors.cpp
    linuxdoom-1.10/p_enemy.cpp
    linuxdoom-1.10/p_floor.cpp
    linuxdoom-1.10/p_inter.cpp
    linuxdoom-1.10/p_lights.cpp
    linuxdoom-1.10/p_map.cpp
    linuxdoom-1.10/p_maputl.cpp
    linuxdoom-1.10/p_mobj.cpp
    linuxdoom-1.10/p_plats.cpp
    linuxdoom-1.10/p_pspr.cpp
    linuxdoom-1.10/p_saveg.cpp
    linuxdoom-1.10/p_setup.cpp
    linuxdoom-1.10/p_sight.cpp
    linuxdoom-1.10/p_spec.cpp
    linuxdoom-1.10/p_switch.cpp
    linuxdoom-1.10/p_telept.cpp
    linuxdoom-1.10/p_tick.cpp
    linuxdoom-1.10/p_user.cpp
    linuxdoom-1.10/r_bsp.cpp
    linuxdoom-1.10/r_data.cpp
    linuxdoom-1.10/r_draw.cpp
    linuxdoom-1.10/r_main.cpp
    linuxdoom-1.10/r_plane.cpp
    linuxdoom-1.10/r_segs.cpp
    linuxdoom-1.10/r_sky.cpp
    linuxdoom-1.10/r_things.cpp
    linuxdoom-1.10/s_sound.cpp
    linuxdoom-1.10/sounds.cpp
    linuxdoom-1.10/st_lib.cpp
    linuxdoom-1.10/st_stuff.cpp
    linuxdoom-1.10/tables.cpp
    linuxdoom-1.10/v_video.cpp
    linuxdoom-1.10/w_wad.cpp
    linuxdoom-1.10/wi_stuff.cpp
    linuxdoom-1.10/z_zone.cpp
)

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
    add_executable(random_tests tests/random_tests.cpp linuxdoom-1.10/m_random.cpp)
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
        linuxdoom-1.10/v_video.cpp linuxdoom-1.10/m_bbox.cpp linuxdoom-1.10/tables.cpp
        linuxdoom-1.10/m_fixed.cpp)
    target_include_directories(render_boundary_tests PRIVATE linuxdoom-1.10)
    target_compile_definitions(render_boundary_tests PRIVATE NORMALUNIX LINUX)
    target_compile_options(render_boundary_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(render_boundary_tests PRIVATE -Werror)
    endif()
    add_test(NAME render.boundaries COMMAND render_boundary_tests)
endif()

if(BUILD_TESTING)
    add_executable(savegame_tests tests/savegame_tests.cpp linuxdoom-1.10/p_saveg.cpp)
    target_include_directories(savegame_tests PRIVATE linuxdoom-1.10)
    target_compile_definitions(savegame_tests PRIVATE NORMALUNIX LINUX)
    target_compile_options(savegame_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(savegame_tests PRIVATE -Werror)
    endif()
    add_test(NAME savegame.alignment COMMAND savegame_tests)
endif()

if(BUILD_TESTING)
    add_executable(fixed_div_tests tests/fixed_div_tests.cpp linuxdoom-1.10/m_fixed.cpp)
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

if(BUILD_TESTING)
    add_executable(stair_tests tests/stair_tests.cpp linuxdoom-1.10/p_floor.cpp)
    target_include_directories(stair_tests PRIVATE linuxdoom-1.10)
    target_compile_definitions(stair_tests PRIVATE NORMALUNIX LINUX)
    target_compile_options(stair_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(DOOM_WARNINGS_AS_ERRORS)
        target_compile_options(stair_tests PRIVATE -Werror)
    endif()
    add_test(NAME floor.stairs COMMAND stair_tests)
endif()
