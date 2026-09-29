cmake_minimum_required(VERSION 3.20)

project(doom-cpp
    DESCRIPTION "Linux Doom 1.10, ported to C++"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DOOM_LEGACY_WARNINGS "Show warnings from not-yet-ported 1997 sources" OFF)

# ---------------------------------------------------------------------------
# Sources
#
# The tree is still id's original C. It is compiled as C++ in place rather
# than renamed to .cpp, so that this commit is "it builds" and nothing else.
# Rename each file to .cpp in the commit that actually ports it.
#
# ipx/ and sersrc/ are deliberately absent: they are DOS real-mode drivers
# (dos.h, conio.h) and cannot be built on Linux at all.
# ---------------------------------------------------------------------------
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

# ---------------------------------------------------------------------------
# Compiler flags
#
# -fno-strict-aliasing and -fwrapv are load-bearing, not tuning. Doom type-puns
# constantly (see the offsetof-by-hand in d_net.c) and relies on signed
# overflow wrapping in its fixed-point math. Without these, gcc miscompiles
# the renderer at -O2 in ways that look like content bugs.
# ---------------------------------------------------------------------------
target_compile_options(doom PRIVATE -fno-strict-aliasing -fwrapv)

# -fpermissive is what lets 1997 C compile as C++ at all. Roughly 750 sites
# are rejected without it, but they are not 750 problems:
#
#   448   void (*)() -> actionf_p1     one typedef in d_think.h
#   ~150  void* -> T*                  unchecked malloc/Z_Malloc returns
#    48   pointer <-> int truncation   REAL 64-bit bugs, see below
#    26   int -> statenum_t
#
# The 48 truncations are genuine and will corrupt at runtime on x86-64;
# they cluster in p_saveg.c (29) and m_misc.c (14). Configure with the
# "warn" preset to list them. Drop -fpermissive once the tree is ported.
target_compile_options(doom PRIVATE -fpermissive)

if(DOOM_LEGACY_WARNINGS)
    target_compile_options(doom PRIVATE -Wall -Wextra)
else()
    # A warning-enabled build of unported 1997 code is thousands of lines of
    # noise. Turn warnings on per-file, in the commit that ports that file.
    target_compile_options(doom PRIVATE -w)
endif()

# ---------------------------------------------------------------------------
# X11 (i_video.c uses Xlib + the MIT-SHM extension + DGA)
# ---------------------------------------------------------------------------
find_package(X11 REQUIRED)

if(NOT X11_XShm_INCLUDE_PATH)
    message(FATAL_ERROR
        "X11/extensions/XShm.h not found, needed by linuxdoom-1.10/i_video.c.\n"
        "  Install it with:  sudo apt install libxext-dev")
endif()

target_link_libraries(doom PRIVATE X11::X11 X11::Xext m)

install(TARGETS doom RUNTIME DESTINATION bin)
