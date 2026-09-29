#include "m_random.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>

namespace
{
// Golden output captured from the original table: reset starts at index zero,
// and the first draw advances to index one. Independent of the production table.
constexpr int expected[] = {
    8, 109, 220, 222, 241, 149, 107, 75, 248, 254, 140, 16, 66, 74, 21, 211,
    47, 80, 242, 154, 27, 205, 128, 161, 89, 77, 36, 95, 110, 85, 48, 212,
    140, 211, 249, 22, 79, 200, 50, 28, 188, 52, 140, 202, 120, 68, 145, 62,
    70, 184, 190, 91, 197, 152, 224, 149, 104, 25, 178, 252, 182, 202, 182, 141,
    197, 4, 81, 181, 242, 145, 42, 39, 227, 156, 198, 225, 193, 219, 93, 122,
    175, 249, 0, 175, 143, 70, 239, 46, 246, 163, 53, 163, 109, 168, 135, 2,
    235, 25, 92, 20, 145, 138, 77, 69, 166, 78, 176, 173, 212, 166, 113, 94,
    161, 41, 50, 239, 49, 111, 164, 70, 60, 2, 37, 171, 75, 136, 156, 11,
    56, 42, 146, 138, 229, 73, 146, 77, 61, 98, 196, 135, 106, 63, 197, 195,
    86, 96, 203, 113, 101, 170, 247, 181, 113, 80, 250, 108, 7, 255, 237, 129,
    226, 79, 107, 112, 166, 103, 241, 24, 223, 239, 120, 198, 58, 60, 82, 128,
    3, 184, 66, 143, 224, 145, 224, 81, 206, 163, 45, 63, 90, 168, 114, 59,
    33, 159, 95, 28, 139, 123, 98, 125, 196, 15, 70, 194, 253, 54, 14, 109,
    226, 71, 17, 161, 93, 186, 87, 244, 138, 20, 52, 123, 251, 26, 36, 17,
    46, 52, 231, 232, 76, 31, 221, 84, 37, 216, 165, 212, 106, 197, 242, 98,
    43, 39, 175, 254, 145, 190, 84, 118, 222, 187, 136, 120, 163, 236, 249, 0,
};
static_assert(std::size(expected) == 256);

using RandomFunction = int (*)();

void Expect(int actual, int wanted, const char* context, std::size_t draw)
{
    // Do not use assert(): these checks must also run with NDEBUG builds.
    if (actual != wanted)
    {
        std::fprintf(stderr, "%s, draw %zu: expected %d, got %d\n",
                     context, draw + 1, wanted, actual);
        std::exit(EXIT_FAILURE);
    }
}

void ExpectSequence(RandomFunction next, const char* context)
{
    for (std::size_t i = 0; i < std::size(expected); ++i)
        Expect(next(), expected[i], context, i);
}

void TestSequence()
{
    M_ClearRandom();
    ExpectSequence(P_Random, "gameplay sequence");
    M_ClearRandom();
    ExpectSequence(M_Random, "UI sequence");
}

void TestWraparound()
{
    M_ClearRandom();
    // Check both sides of each boundary, across three complete cycles.
    for (int cycle = 0; cycle < 3; ++cycle)
    {
        ExpectSequence(P_Random, "gameplay wraparound");
        ExpectSequence(M_Random, "UI wraparound");
    }
    Expect(P_Random(), 8, "gameplay after three cycles", 0);
    Expect(M_Random(), 8, "UI after three cycles", 0);
}

void TestReset()
{
    M_ClearRandom();
    for (int i = 0; i < 37; ++i)
        P_Random();
    for (int i = 0; i < 519; ++i)
        M_Random();
    M_ClearRandom();
    ExpectSequence(P_Random, "gameplay reset");
    ExpectSequence(M_Random, "UI reset");

    P_Random();
    M_Random();
    M_ClearRandom();
    M_ClearRandom();
    Expect(P_Random(), 8, "repeated gameplay reset", 0);
    Expect(M_Random(), 8, "repeated UI reset", 0);
}

void TestIndependence()
{
    M_ClearRandom();
    for (std::size_t i = 0; i < std::size(expected); ++i)
    {
        for (std::size_t noise = 0; noise < i % 7 + 1; ++noise)
            M_Random();
        Expect(P_Random(), expected[i], "UI draws must not advance gameplay", i);
    }

    M_ClearRandom();
    for (std::size_t i = 0; i < std::size(expected); ++i)
    {
        for (std::size_t noise = 0; noise < i % 11 + 1; ++noise)
            P_Random();
        Expect(M_Random(), expected[i], "gameplay draws must not advance UI", i);
    }
}
} // namespace

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        if (std::strcmp(argv[1], "sequence") == 0)
            TestSequence();
        else if (std::strcmp(argv[1], "wraparound") == 0)
            TestWraparound();
        else if (std::strcmp(argv[1], "reset") == 0)
            TestReset();
        else if (std::strcmp(argv[1], "independence") == 0)
            TestIndependence();
        else
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    std::fprintf(stderr, "Usage: random_tests sequence|wraparound|reset|independence\n");
    return EXIT_FAILURE;
}
