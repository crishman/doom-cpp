#ifndef DOOM_R_DISTANCE_H
#define DOOM_R_DISTANCE_H

#include "tables.h"

#include <algorithm>
#include <cstdint>
#include <limits>

// Doom's table-based distance approximation, independent of renderer globals.
inline fixed_t R_PointDistance(fixed_t x, fixed_t y, fixed_t originx, fixed_t originy)
{
    std::int64_t dx = static_cast<std::int64_t>(x) - originx;
    std::int64_t dy = static_cast<std::int64_t>(y) - originy;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    if (dy > dx)
        std::swap(dx, dy);

    // FixedDiv(0, 0) saturates; it is not a valid slope-table index.
    if (dx == 0)
        return 0;
    // A distance cannot be smaller than either component.
    if (dx > std::numeric_limits<fixed_t>::max())
        return std::numeric_limits<fixed_t>::max();

    const auto major = static_cast<fixed_t>(dx);
    const auto minor = static_cast<fixed_t>(dy);
    // 0 <= minor <= major and major > 0, so the index is in [0, SLOPERANGE].
    const int slope = FixedDiv(minor, major) >> DBITS;
    const unsigned angle = FineAngleIndex(tantoangle[slope] + ANG90);
    return FixedDiv(major, finesine[angle]);
}

#endif
