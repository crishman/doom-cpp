# Tests

Configure, build, and run the tests from the repository root:

```sh
cmake --preset warn
cmake --build --preset warn
ctest --test-dir build/warn --output-on-failure
```

The `warn` preset enables strict warnings as errors and AddressSanitizer/UndefinedBehaviorSanitizer. The tests need no WAD, X display, or sound server. CMake still checks the game's X11 build dependencies during configuration.

CTest registers four random-stream tests:

- `random.sequence`: both public APIs reproduce the original 256-draw sequence.
- `random.wraparound`: both streams repeat across three cycles, including the last and first values at each boundary.
- `random.reset`: clearing restores both streams after different numbers of draws; repeated clearing also works.
- `random.independence`: interleaved draws from one stream do not advance the other.

The expected sequence is a fixed fixture from the original table, starting at index one because draws advance before reading. It is not generated from the production table during the build. The production table also has compile-time size and FNV-1a checksum assertions.

Runtime checks remain active in release builds with `NDEBUG`. Set `BUILD_TESTING=OFF` when configuring to omit the test executable.
