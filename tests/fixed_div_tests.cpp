#include "m_fixed.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <initializer_list>

namespace
{
constexpr fixed_t low = std::numeric_limits<fixed_t>::min();
constexpr fixed_t high = std::numeric_limits<fixed_t>::max();
struct DivisionError {};

void Check(fixed_t actual, fixed_t expected, fixed_t a, fixed_t b)
{
    if (actual != expected)
    {
        std::fprintf(stderr, "Division (%d, %d): expected %d, got %d\n", a, b, expected, actual);
        std::exit(EXIT_FAILURE);
    }
}

void CompareLegacy(fixed_t a, fixed_t b)
{
    // Only inputs for which the old abs() and float-to-int conversion are valid.
    if (a == low || b == low)
        return;
    const bool saturates = (std::abs(a) >> 14) >= std::abs(b);
    const double quotient = b == 0 ? 0 : static_cast<double>(a) / b * FRACUNIT;
    const fixed_t expected = saturates ? ((a ^ b) < 0 ? low : high)
                                      : static_cast<fixed_t>(quotient);
    Check(FixedDiv(a, b), expected, a, b);
    if (b != 0 && quotient >= -2147483648.0 && quotient < 2147483648.0)
        Check(FixedDiv2(a, b), static_cast<fixed_t>(quotient), a, b);
}

void ExpectError(fixed_t a, fixed_t b)
{
    try { (void)FixedDiv2(a, b); }
    catch (const DivisionError&) { return; }
    std::fprintf(stderr, "FixedDiv2(%d, %d) did not report an error\n", a, b);
    std::exit(EXIT_FAILURE);
}
}

// Replace the engine shutdown with an observable failure for direct division tests.
void I_Error([[maybe_unused]] const char* message, ...) { throw DivisionError{}; }

int main(int argc, char** argv)
{
    Check(FixedDiv(FRACUNIT, FRACUNIT), FRACUNIT, FRACUNIT, FRACUNIT);
    Check(FixedDiv(1, 3), 21845, 1, 3);
    Check(FixedDiv(-1, 3), -21845, -1, 3); // truncation toward zero
    Check(FixedDiv(1, -3), -21845, 1, -3);
    Check(FixedDiv(-1, -3), 21845, -1, -3);
    Check(FixedDiv(16383, 1), 1073676288, 16383, 1);
    Check(FixedDiv(16384, 1), high, 16384, 1); // deliberately early saturation
    Check(FixedDiv(-16384, 1), low, -16384, 1);
    Check(FixedDiv2(16384, 1), 1073741824, 16384, 1);
    Check(FixedDiv(0, 0), high, 0, 0);
    Check(FixedDiv(1, 0), high, 1, 0);
    Check(FixedDiv(-1, 0), low, -1, 0);

    constexpr fixed_t edges[] = {low + 1, -FRACUNIT, -32768, -16384, -16383,
        -3, -1, 0, 1, 3, 16383, 16384, 32767, FRACUNIT, high};
    for (fixed_t a : edges)
        for (fixed_t b : edges)
            CompareLegacy(a, b);
    // Probe either side of the early saturation threshold, with every sign pair.
    for (int b = 1; b <= 1024; ++b)
        for (int delta = -1; delta <= 1; ++delta)
            for (int signa : {-1, 1})
                for (int signb : {-1, 1})
                    CompareLegacy(signa * (b * 16384 + delta), signb * b);
    std::uint32_t seed = 1234567;
    for (int i = 0; i < 65536; ++i)
    {
        seed = seed * 1664525u + 1013904223u;
        const auto a = static_cast<fixed_t>(static_cast<std::int64_t>(seed) + low);
        seed = seed * 1664525u + 1013904223u;
        const auto b = static_cast<fixed_t>(static_cast<std::int64_t>(seed) + low);
        CompareLegacy(a, b);
    }
    if (argc == 2 && std::strcmp(argv[1], "baseline") == 0)
        return EXIT_SUCCESS;

    Check(FixedDiv(low, low), FRACUNIT, low, low);
    Check(FixedDiv(high, low), -65535, high, low);
    Check(FixedDiv(low, high), -65536, low, high);
    Check(FixedDiv(low, FRACUNIT), low, low, FRACUNIT);
    Check(FixedDiv(low, -FRACUNIT), high, low, -FRACUNIT);
    Check(FixedDiv(0, low), 0, 0, low);
    Check(FixedDiv(low, 0), low, low, 0);
    Check(FixedDiv2(low, FRACUNIT), low, low, FRACUNIT);
    Check(FixedDiv2(low, low), FRACUNIT, low, low);
    Check(FixedDiv2(high, FRACUNIT), high, high, FRACUNIT);
    ExpectError(0, 0);
    ExpectError(1, 0);
    ExpectError(-1, 0);
    ExpectError(low, -FRACUNIT);
    ExpectError(high, 1);
    ExpectError(low, 1);
    return EXIT_SUCCESS;
}
