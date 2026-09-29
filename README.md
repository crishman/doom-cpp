# doom-cpp

A pet project based on id Software's Linux Doom 1.10: modernize the C++ code,
measure performance, and explore CPU optimizations. This README tracks the
main steps; detailed changes live in Git history.

The game currently uses the software renderer and X11.
Original release notes remain in [README.TXT](README.TXT).

## Build and run

Requires Linux with X11, a C++20 compiler, CMake 3.20+, Ninja, and X11/Xext
development libraries. Verified locally with GCC 15.2 and Clang 18.1.8; these
are tested versions, not minimum compiler requirements.

```sh
cmake --preset rel
cmake --build --preset rel
DOOMWADDIR="$HOME/wads" ./build/rel/doom -3
```

Supply your own Doom WAD, for example `doomu.wad` or `doom2.wad`, in
`DOOMWADDIR`. Game data is not included. `-3` selects 3× display scaling.
The legacy sound server is not built by CMake.

| Preset | Purpose |
| --- | --- |
| `rel` | Optimized build with debug symbols |
| `dev` | Debug build with ASan/UBSan |
| `warn` | Debug, ASan/UBSan, strict warnings as errors |

Each preset also has a `-clang` variant, such as `warn-clang`.

## Checks

```sh
cmake --preset warn-clang
cmake --build --preset warn-clang
ctest --test-dir build/warn-clang --output-on-failure
clang-tidy -p build/warn-clang linuxdoom-1.10/p_floor.cpp
```

The 10 tests require no WAD or display. Use `-DDOOM_BUILD_GAME=OFF` at configure
time to build tests without X11. CI covers GCC/Clang game builds on Linux and
tests on Linux/macOS; Windows/MSVC tests are experimental.
See [test coverage](docs/testing.md) and [warning cleanup](docs/warnings.md).

## Modernization journal

1. **Done:** CMake builds → 64-bit and sanitizer fixes → strict warnings →
   constexpr arithmetic/tables and regression tests → `.cpp` sources and C++20 →
   focused clang-tidy checks → GCC/Clang builds and CI.
2. **Next:** reproducible performance measurements → profile rendering and presentation
   → targeted CPU optimizations; continue C++ cleanup as code changes.
3. **Deferred:** GPU rendering and NoGraphicsAPI integration. The inspected
   NoGraphicsAPI version supports only headless use on Linux.

Engine code is in `linuxdoom-1.10/`; tests are in `tests/`. The historical
`ipx/`, `sersrc/`, and `sndserv/` sources are outside the CMake build.

## License

See [LICENSE.TXT](LICENSE.TXT) for GPL v2. WAD assets are distributed separately.
