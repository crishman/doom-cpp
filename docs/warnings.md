# Strict warning cleanup

The `warn` preset builds C++17 with `-Wall -Wextra -Wpedantic -Werror`
and ASan/UBSan. `-fpermissive` is removed: invalid C++ conversions now fail
in every preset. The legacy arithmetic flags `-fwrapv` and
`-fno-strict-aliasing` remain.

```sh
cmake --preset warn
cmake --build --preset warn
```

Repeated diagnostics from shared headers are grouped below by compiler warning.

| Warning | Cause | Fix |
| --- | --- | --- |
| `literal-suffix` | A string literal immediately followed by a macro looks like a C++ user-defined literal. | Separate string literals and macros with whitespace in messages, paths, and the zone diagnostic. Text is unchanged. |
| `write-strings` | String literals were exposed through mutable `char*` pointers. | Use `const char*` for messages, resource names, sound names, sprite names, command-line searches, and their consuming interfaces. Keep writable buffers mutable. |
| `permissive`: object pointers | C implicitly converts `void*`; C++ requires an explicit conversion. | Add typed `static_cast` conversions at allocation and WAD cache boundaries. Socket addresses use `sockaddr` pointers and `socklen_t` lengths. |
| `permissive`: callbacks | Menu callbacks were passed through `void*`. | Give `M_StartMessage` a `void (*)(int)` callback parameter. |
| `permissive`: enums | Integers from input, state tables, arithmetic, or stored bytes were assigned to enums. | Make conversions explicit at enum boundaries; use named constants where possible. This does not add validation for arbitrary malformed saved games or demos. |
| `permissive`: pointer truncation; `int-to-pointer-cast` | Addresses were narrowed to 32-bit `int`. | Use `offsetof` for packet layout, direct array indexing for pixel expansion, and `uintptr_t` for legacy save indices and allocator ownership markers. Parse `-statcopy` without `atoi` truncation. |
| `permissive`: configuration strings | Default strings and pointers to string settings were stored as integers and `int*`. | Separate integer and string fields with typed constructors. Load/save according to the field type, bound the text scan, and manage replacement string allocations. |
| `permissive`: string array size | An eight-character WAD name was initialized into an eight-byte C++ character array, leaving no space for the terminator. | Increase the local name slots to nine bytes. |
| `register` | C++17 removed the `register` storage specifier. | Remove it; the compiler controls register allocation. |
| `narrowing` | Automap shape coordinates implicitly converted floating-point constants to fixed-point integers in brace initializers. | Explicitly convert to `fixed_t`, preserving truncation. |
| `missing-field-initializers` | Static sound/animation/menu tables omitted trailing fields. | Give those fields explicit zero defaults; fully initialize automap events. Correct the exit event's misplaced type and key fields. |
| `unused-parameter`, `unused-but-set-parameter` | Shared callback signatures and platform stubs have parameters unused by this implementation. | Mark those parameters `[[maybe_unused]]`; retain the signatures. |
| `unused-variable` | Anonymous enum definitions accidentally declared unused variables such as `main_e` and `specials_e`. | Keep the enumerators and remove the unused variables. |
| `unused-but-set-variable` | Dead local bookkeeping remained in HUD, automap, door, renderer, and gameplay code. | Remove dead values while retaining required calls. Check the save-description read count instead of discarding it. |
| `sign-compare` | Signed counters were compared with unsigned sizes or angles. | Use consistent comparison types or explicit lower/upper bounds. Iterate text to its terminator. Preserve unsigned angle-wrap comparisons. Accept shared memory only when the segment is large enough. |
| `sequence-point` | Event queue indices were incremented and assigned in the same expression. | Compute `(index + 1) & mask` with one assignment. |
| `format` | `%p` requires `void*`; typed pointers were passed directly. | Pass explicit `void*` arguments and use `%p` instead of narrowing an address for `%x`. |
| `format-overflow` | Resource-name and quickload-message buffers were too small for the possible formatted output. | Enlarge them to hold the complete output. Menu message lines use `std::string` instead of a fixed 40-byte copy buffer. |
| `misleading-indentation` | The `Z_ChangeTag` macro visually placed two statements under an unbraced `if`. | Use a `do { ... } while (0)` macro and align the unconditional statement correctly. |
| `enum-compare` | Sky selection compared `gamemode` with mission constants. | Compare those constants with `gamemission`. |
| `switch` | Switches over game state did not mention `GS_INVALID`. | Explicitly handle the redraw sentinel without dispatching gameplay work. |
| `deprecated-declarations` | Keyboard translation used `XKeycodeToKeysym`. | Use `XkbKeycodeToKeysym` with group 0 and level 0. |
| `implicit-fallthrough` | Ceiling/floor actions and secret-exit handling intentionally share case bodies. | Mark the intended paths with `[[fallthrough]]`. |
| `bool-compare` | Deathmatch supports modes 0, 1, and 2 but was stored as `bool`. | Store the mode as `int`, preserving mode 2. |
| `parentheses` | `!flags & ML_TWOSIDED` negated the whole flag value before masking. | Use `!(flags & ML_TWOSIDED)`. |
| `alloc-size` | The WAD directory started with a one-byte allocation cast to a larger structure. | Start with `nullptr`; the existing `realloc` allocates the real directory. |
| `int-in-bool-context` | Intermission rendering tested the constant `commercial`. | Test `gamemode == commercial`. |

## Verification and limits

- The `warn` executable builds and links with warnings treated as errors and
  without `-fpermissive`.
- A focused harness linked against the sanitized game objects checks configuration
  defaults, decimal/hex overrides, string replacement, mismatched value types,
  unknown keys, and save/load round trips. It passes ASan/UBSan/LeakSanitizer.
- Gameplay, networking, legacy PseudoColor rendering, and save-game compatibility
  were not runtime-tested in this cleanup. Save files still use native structure
  layouts; pointer-width fixes do not make them portable across architectures.
