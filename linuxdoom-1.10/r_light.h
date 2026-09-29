#ifndef DOOM_R_LIGHT_H
#define DOOM_R_LIGHT_H

#include "r_main.h"

#include <array>
#include <cstdint>

namespace lighting
{
inline constexpr int distance_map = 2;
inline constexpr int colors_per_map = 256;

constexpr auto MakeDistanceIndices()
{
    std::array<std::array<byte, MAXLIGHTZ>, LIGHTLEVELS> indices{};
    for (int light = 0; light < LIGHTLEVELS; ++light)
    {
        const int start = ((LIGHTLEVELS - 1 - light) * 2) * NUMCOLORMAPS / LIGHTLEVELS;
        for (int distance = 0; distance < MAXLIGHTZ; ++distance)
        {
            const int scale = FixedDiv(SCREENWIDTH / 2 * FRACUNIT,
                                       (distance + 1) << LIGHTZSHIFT) >> LIGHTSCALESHIFT;
            const int level = start - scale / distance_map;
            indices[light][distance] = static_cast<byte>(
                level < 0 ? 0 : level >= NUMCOLORMAPS ? NUMCOLORMAPS - 1 : level);
        }
    }
    return indices;
}

// The indices are fixed; the loaded COLORMAP address is only known at runtime.
inline constexpr auto distance_indices = MakeDistanceIndices();
static_assert(distance_indices.size() == LIGHTLEVELS);
static_assert(distance_indices[0].size() == MAXLIGHTZ);
static_assert(NUMCOLORMAPS <= 256);

constexpr bool CheckDistanceIndices()
{
    for (int light = 0; light < LIGHTLEVELS; ++light)
        for (int distance = 0; distance < MAXLIGHTZ; ++distance)
        {
            const int value = distance_indices[light][distance];
            if (value >= NUMCOLORMAPS)
                return false;
            if (distance && value < distance_indices[light][distance - 1])
                return false;
            if (light && value > distance_indices[light - 1][distance])
                return false;
        }
    return true;
}

constexpr std::uint32_t DistanceChecksum()
{
    std::uint32_t hash = 2166136261u;
    for (const auto& row : distance_indices)
        for (byte value : row)
            hash = (hash ^ value) * 16777619u;
    return hash;
}
static_assert(CheckDistanceIndices());
// Baseline from the original 320-pixel renderer's 16 x 128 table.
static_assert(DistanceChecksum() == 0x6511ba95u);
static_assert(distance_indices[0][0] == 0);
static_assert(distance_indices[0][MAXLIGHTZ - 1] == NUMCOLORMAPS - 1);
static_assert(distance_indices[LIGHTLEVELS - 1][MAXLIGHTZ - 1] == 0);

inline void BindDistanceMaps(lighttable_t* maps,
                             lighttable_t* (&dest)[LIGHTLEVELS][MAXLIGHTZ])
{
    for (int light = 0; light < LIGHTLEVELS; ++light)
        for (int distance = 0; distance < MAXLIGHTZ; ++distance)
            dest[light][distance] = maps + distance_indices[light][distance] * colors_per_map;
}
}

#endif
