#include "r_light.h"

#include <array>
#include <cstdio>
#include <cstdlib>

int main()
{
    std::array<lighttable_t, NUMCOLORMAPS * 256> maps{};
    std::array<lighttable_t, NUMCOLORMAPS * 256> replacement{};
    lighttable_t* bound[LIGHTLEVELS][MAXLIGHTZ]{};

    // Bind twice to catch accidental retention of the first loaded palette address.
    for (auto* base : {maps.data(), replacement.data()})
    {
        lighting::BindDistanceMaps(base, bound);
        for (int light = 0; light < LIGHTLEVELS; ++light)
            for (int distance = 0; distance < MAXLIGHTZ; ++distance)
            {
                // Independent copy of the original runtime arithmetic. These
                // positive inputs never reach FixedDiv's saturation threshold.
                const int startmap = ((LIGHTLEVELS - 1 - light) * 2) * NUMCOLORMAPS / LIGHTLEVELS;
                int scale = static_cast<int>(static_cast<double>(SCREENWIDTH / 2 * FRACUNIT)
                    / ((distance + 1) << LIGHTZSHIFT) * FRACUNIT);
                scale >>= LIGHTSCALESHIFT;
                int level = startmap - scale / 2;
                if (level < 0) level = 0;
                if (level >= NUMCOLORMAPS) level = NUMCOLORMAPS - 1;
                if (lighting::distance_indices[light][distance] != level
                    || bound[light][distance] != base + level * 256)
                {
                    std::fprintf(stderr, "Lighting mismatch at light %d, distance %d\n", light, distance);
                    return EXIT_FAILURE;
                }
            }
    }
    return EXIT_SUCCESS;
}
