#include "v_video.h"
#include "tables.h"
#include "r_distance.h"
#include "m_bbox.h"
#include "m_swap.h"

#include <array>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

void V_DrawPatchFlipped(int x, int y, int scrn, patch_t* patch);
void I_Error(const char* message, ...)
{
    std::fprintf(stderr, "Unexpected engine error: %s\n", message);
    std::abort();
}
byte* I_AllocLow([[maybe_unused]] int length) { std::abort(); }

int main()
{
    // Coincident points caused FixedDiv(0,0) to index tantoangle at 67108863.
    for (fixed_t coordinate : {0, 1, -FRACUNIT, INT_MIN, INT_MAX})
        if (R_PointDistance(coordinate, coordinate, coordinate, coordinate) != 0)
            return EXIT_FAILURE;
    if (R_PointDistance(INT_MIN, 0, 0, 0) != INT_MAX
        || R_PointDistance(INT_MAX, 0, INT_MIN, 0) != INT_MAX
        || R_PointDistance(0, INT_MIN, 0, INT_MAX) != INT_MAX)
        return EXIT_FAILURE;

    // Ordinary nonzero distances must retain the original lookup/rounding.
    for (int i = 1; i <= 4096; ++i)
    {
        const fixed_t dx = i * 32767;
        const fixed_t dy = (i % 17) * 193;
        const fixed_t major = dx > dy ? dx : dy;
        const fixed_t minor = dx > dy ? dy : dx;
        const auto angle = (tantoangle[FixedDiv(minor, major) >> DBITS] + ANG90)
                           >> ANGLETOFINESHIFT;
        const fixed_t expected = FixedDiv(major, finesine[angle]);
        if (R_PointDistance(dx, dy, 0, 0) != expected
            || R_PointDistance(-dx, -dy, 0, 0) != expected
            || R_PointDistance(dy, dx, 0, 0) != expected
            || R_PointDistance(0, 0, dx, dy) != expected)
        {
            std::fprintf(stderr, "Distance regression for (%d,%d)\n", dx, dy);
            return EXIT_FAILURE;
        }
    }
    if (R_PointDistance(1, 0, 0, 0) != 1
        || R_PointDistance(0, 1, 0, 0) != 1)
        return EXIT_FAILURE;

    // Check both halves of the angle range, including fractional angle bits.
    for (unsigned index = 0; index < FINEANGLES; ++index)
    {
        const angle_t angle = (index << ANGLETOFINESHIFT) | 0x7ffffu;
        if (FineTangentIndex(angle) != index % (FINEANGLES / 2)
            || FineTangentIndex(angle + ANG180) != FineTangentIndex(angle)
            || finetangent[FineTangentIndex(angle)] != finetangent[index % (FINEANGLES / 2)])
            return EXIT_FAILURE;
        if (FineAngleIndex(angle) != index || finesine[FineAngleIndex(angle)] != finesine[index])
            return EXIT_FAILURE;
    }

    // A synthetic 12-column patch, with two posts and transparent rows between.
    alignas(patch_t) std::array<byte, 512> storage{};
    auto* patch = new (storage.data()) patch_t{};
    patch->width = SHORT(12);
    patch->height = SHORT(6);
    patch->leftoffset = SHORT(2);
    patch->topoffset = SHORT(1);
    int next = static_cast<int>(offsetof(patch_t, columnofs)) + 12 * 4;
    for (int col = 0; col < 12; ++col)
    {
        const int offset = LONG(next);
        std::memcpy(storage.data() + offsetof(patch_t, columnofs) + col * 4, &offset, 4);
        for (int row : {0, 4})
        {
            storage[next++] = static_cast<byte>(row);
            storage[next++] = 2;
            storage[next++] = 0;
            storage[next++] = static_cast<byte>(1 + col * 6 + row);
            storage[next++] = static_cast<byte>(2 + col * 6 + row);
            storage[next++] = 0;
        }
        storage[next++] = 255;
    }

    std::array<byte, SCREENWIDTH * SCREENHEIGHT> frame;
    constexpr int origins[][2] = {
        {20, 20}, {-3, 20}, {SCREENWIDTH - 4, 20}, {20, -1},
        {20, SCREENHEIGHT - 2}, {-3, -1}, {SCREENWIDTH - 4, SCREENHEIGHT - 2},
        {-17, 86}, {-10, 86}, {-20, 20}, {SCREENWIDTH, 20},
        {20, -6}, {20, SCREENHEIGHT}, {INT_MIN, INT_MIN}, {INT_MAX, INT_MAX}
    };
    for (int scrn : {0, 1})
    for (bool flipped : {false, true})
    for (const auto& origin : origins)
    {
        frame.fill(0xa5);
        screens[scrn] = frame.data();
        M_ClearBox(dirtybox);
        // Inputs include the stored offsets; avoid overflowing extreme values.
        const int x = origin[0] == INT_MAX ? INT_MAX : origin[0] + 2;
        const int y = origin[1] == INT_MAX ? INT_MAX : origin[1] + 1;
        (flipped ? V_DrawPatchFlipped : V_DrawPatch)(x, y, scrn, patch);
        const auto left = static_cast<long long>(x) - 2;
        const auto top = static_cast<long long>(y) - 1;
        for (int sy = 0; sy < SCREENHEIGHT; ++sy)
        for (int sx = 0; sx < SCREENWIDTH; ++sx)
        {
            const auto col = sx - left;
            const auto row = sy - top;
            int expected = 0xa5;
            if (col >= 0 && col < 12 && row >= 0 && row < 6 && row != 2 && row != 3)
                expected = static_cast<int>(1 + (flipped ? 11 - col : col) * 6 + row);
            if (frame[sy * SCREENWIDTH + sx] != expected)
            {
                std::fprintf(stderr, "Patch (%d,%d), flipped=%d: wrong pixel at (%d,%d)\n",
                             x, y, flipped, sx, sy);
                return EXIT_FAILURE;
            }
        }
        if (scrn == 1 && (dirtybox[BOXLEFT] != INT_MAX || dirtybox[BOXRIGHT] != INT_MIN))
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
