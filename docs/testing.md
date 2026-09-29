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

`fixed.multiply` covers fixed-point multiplication: signs, fractions, negative rounding, extreme inputs, and 32-bit result wrapping. It also compares against the original implementation for a boundary-value matrix and 65,536 deterministic input pairs. The original implementation's signed shift and narrowing behavior are characterized on the compiler/platform running the test; the new implementation expresses that behavior explicitly using unsigned bit extraction. Representative cases are also checked with `static_assert` in `m_fixed.h`.

`render.boundaries` checks unsigned angle-to-sine indices across a full turn and compares framebuffer pixels for synthetic patches. Cases cover all screen edges, fully offscreen positions, nonzero patch offsets, transparent gaps, widths beyond the legacy eight-entry column declaration, and horizontal flipping. No WAD or display is needed.

The renderer boundary test also checks coincident-point distance (the zero-divisor regression), extreme coordinate differences, and ordinary nonzero distances against the original lookup calculation.

`savegame.alignment` archives players, actors, and all special types with eight different buffer starting offsets. It verifies encoded indices, unchanged live pointers, record sizes and terminators, plus player save/load restoration. The existing native-struct save layout and four-byte padding are preserved; this is not a portable save-format migration. State actions now default to null so empty test state tables can be value-initialized.

`fixed.divide` checks signs, truncation toward zero, the original early saturation threshold, zero divisors, and `INT_MIN`. It compares ordinary inputs with the original floating-point calculation over an edge matrix, saturation boundaries, and 65,536 deterministic pairs. Direct `FixedDiv2` failures are checked using an `I_Error` test substitute. Both division functions support constant evaluation for valid inputs; invalid direct divisions remain runtime engine errors and cannot form constant expressions. `FixedDiv(0, 0)` deliberately retains its legacy `INT_MAX` result.

The immutable gamma table is checked during compilation: five 256-entry ramps, per-row checksums of all original values, endpoint values, and nondecreasing brightness within and across gamma levels. These checks run whenever `v_video.c` is compiled; no runtime gamma-table test is needed.

`render.lighting` compares all 2,048 precomputed distance-lighting indices against the original runtime calculation and verifies colormap pointer binding to two different buffers. Compile-time assertions check dimensions, index bounds, brightness ordering, representative endpoints, and the original table checksum. Lighting that depends on view size remains calculated at runtime.
