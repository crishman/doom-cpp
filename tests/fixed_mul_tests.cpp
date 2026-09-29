#include "m_fixed.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace
{
constexpr fixed_t minimum = std::numeric_limits<fixed_t>::min();
constexpr fixed_t maximum = std::numeric_limits<fixed_t>::max();

void Check(fixed_t a, fixed_t b, fixed_t expected)
{
    const fixed_t actual = FixedMul(a, b);
    if (actual != expected)
    {
        std::fprintf(stderr, "FixedMul(%d, %d): expected %d, got %d\n",
                     a, b, expected, actual);
        std::exit(EXIT_FAILURE);
    }
}

// Characterize the original implementation on the supported compiler/platform.
fixed_t LegacyFixedMul(fixed_t a, fixed_t b)
{
    return ((long long)a * (long long)b) >> FRACBITS;
}

fixed_t NextInput(std::uint32_t& state)
{
    state = state * 1664525u + 1013904223u;
    // Convert every bit pattern to a representable signed value.
    return static_cast<fixed_t>(state <= 0x7fffffffu
        ? static_cast<std::int64_t>(state)
        : static_cast<std::int64_t>(state) - 0x100000000LL);
}
}

int main()
{
    Check(0, minimum, 0);
    Check(FRACUNIT, FRACUNIT, FRACUNIT);
    Check(-FRACUNIT, FRACUNIT, -FRACUNIT);
    Check(FRACUNIT, -FRACUNIT, -FRACUNIT);
    Check(-FRACUNIT, -FRACUNIT, FRACUNIT);
    Check(FRACUNIT / 2, FRACUNIT / 2, FRACUNIT / 4);
    Check(3 * FRACUNIT / 2, -FRACUNIT / 2, -3 * FRACUNIT / 4);

    // Sub-unit products round toward negative infinity, not toward zero.
    Check(1, 1, 0);
    Check(-1, 1, -1);
    Check(1, -1, -1);
    Check(-1, -1, 0);
    Check(FRACUNIT - 1, FRACUNIT - 1, FRACUNIT - 2);
    Check(1 - FRACUNIT, FRACUNIT - 1, 1 - FRACUNIT);

    Check(minimum, FRACUNIT, minimum);
    Check(maximum, FRACUNIT, maximum);
    Check(minimum, -FRACUNIT, minimum);
    Check(maximum, 2 * FRACUNIT, -2);
    Check(minimum, minimum, 0);
    Check(maximum, maximum, -65536);
    Check(minimum, maximum, 32768);

    constexpr fixed_t edges[] = {
        minimum, minimum + 1, maximum - 1, maximum,
        -2 * FRACUNIT, -FRACUNIT - 1, -FRACUNIT, 1 - FRACUNIT,
        -FRACUNIT / 2, -1, 0, 1, FRACUNIT / 2,
        FRACUNIT - 1, FRACUNIT, FRACUNIT + 1, 2 * FRACUNIT
    };
    for (fixed_t a : edges)
        for (fixed_t b : edges)
            Check(a, b, LegacyFixedMul(a, b));

    std::uint32_t state = 0x12345678u;
    for (int i = 0; i < 65536; ++i)
    {
        const fixed_t a = NextInput(state);
        const fixed_t b = NextInput(state);
        Check(a, b, LegacyFixedMul(a, b));
    }
    return EXIT_SUCCESS;
}
