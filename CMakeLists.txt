cmake_minimum_required(VERSION 3.20)

# Without this, CMake puts a default warning level in CMAKE_CXX_FLAGS for MSVC
# and every target that sets its own emits D9025 "overriding '/W1' with '/w'".
# NEW leaves the choice to us, which is what doom_target_flags already makes.
if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW)
endif()

project(doom-cpp DESCRIPTION "Linux Doom 1.10, ported to C++" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DOOM_LEGACY_WARNINGS "Show warnings from legacy sources" OFF)
option(DOOM_WARNINGS_AS_ERRORS "Enable warnings and treat them as errors" OFF)

# The engine's platform layer is X11 and POSIX (i_video, i_sound, i_net,
# i_system), so the game executable is Unix-only. The tests touch none of it
# and build anywhere, which is what lets CI run them on more than one OS.
if(UNIX)
    set(DOOM_BUILD_GAME_DEFAULT ON)
else()
    set(DOOM_BUILD_GAME_DEFAULT OFF)
endif()
option(DOOM_BUILD_GAME "Build the doom executable (requires X11; Unix only)"
       ${DOOM_BUILD_GAME_DEFAULT})

# Warning and arithmetic flags are spelled differently per compiler family, and
# MSVC rejects the GCC forms outright. Everything that needs them goes through
# here so the two stay in step.
function(doom_target_flags target)
    if(MSVC)
        if(DOOM_LEGACY_WARNINGS OR DOOM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE /w)
        endif()
        if(DOOM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        # Load-bearing, not tuning: the tree type-puns constantly and its
        # fixed-point maths relies on signed overflow wrapping. MSVC has no
        # equivalent spelling; it does not perform either transformation.
        target_compile_options(${target} PRIVATE -fno-strict-aliasing -fwrapv)
        if(DOOM_LEGACY_WARNINGS OR DOOM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        else()
            target_compile_options(${target} PRIVATE -w)
        endif()
        if(DOOM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

# Every test target was the same eight lines; only the sources, the target name
# and the registered ctest names differed. TEST is the ctest name and is kept
# verbatim so existing names and docs/testing.md stay correct.
function(doom_add_test target)
    cmake_parse_arguments(ARG "" "TEST" "SOURCES;CASES" ${ARGN})
    add_executable(${target} ${ARG_SOURCES})
    target_include_directories(${target} PRIVATE linuxdoom-1.10)
    doom_target_flags(${target})
    if(ARG_CASES)
        foreach(case ${ARG_CASES})
            add_test(NAME ${ARG_TEST}.${case} COMMAND ${target} ${case})
        endforeach()
    else()
        add_test(NAME ${ARG_TEST} COMMAND ${target})
    endif()
endfunction()

# Engine sources and tests require C++20.
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

if(DOOM_BUILD_GAME)
    add_executable(doom ${DOOM_SOURCES})
    target_include_directories(doom PRIVATE linuxdoom-1.10)
    target_compile_definitions(doom PRIVATE NORMALUNIX LINUX)
    doom_target_flags(doom)

    find_package(X11 REQUIRED)
    if(NOT X11_XShm_INCLUDE_PATH)
        message(FATAL_ERROR "X11/extensions/XShm.h is required (install libxext-dev).")
    endif()
    target_link_libraries(doom PRIVATE X11::X11 X11::Xext m)

    install(TARGETS doom RUNTIME DESTINATION bin)
endif()

include(CTest)
if(BUILD_TESTING)
    # These compile no part of the platform layer, so they need neither X11 nor
    # NORMALUNIX/LINUX. The limit macros in doomtype.h resolve identically with
    # and without LINUX, so dropping the define changes nothing.
    doom_add_test(random_tests TEST random
        SOURCES tests/random_tests.cpp linuxdoom-1.10/m_random.cpp
        CASES   sequence wraparound reset independence)

    doom_add_test(fixed_mul_tests TEST fixed.multiply
        SOURCES tests/fixed_mul_tests.cpp)

    doom_add_test(render_boundary_tests TEST render.boundaries
        SOURCES tests/render_boundary_tests.cpp linuxdoom-1.10/v_video.cpp
                linuxdoom-1.10/m_bbox.cpp linuxdoom-1.10/tables.cpp
                linuxdoom-1.10/m_fixed.cpp)

    doom_add_test(savegame_tests TEST savegame.alignment
        SOURCES tests/savegame_tests.cpp linuxdoom-1.10/p_saveg.cpp)

    doom_add_test(fixed_div_tests TEST fixed.divide
        SOURCES tests/fixed_div_tests.cpp linuxdoom-1.10/m_fixed.cpp)

    doom_add_test(lighting_tests TEST render.lighting
        SOURCES tests/lighting_tests.cpp)

    doom_add_test(stair_tests TEST floor.stairs
        SOURCES tests/stair_tests.cpp linuxdoom-1.10/p_floor.cpp)
endif()
