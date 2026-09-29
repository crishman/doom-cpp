#include "p_local.h"
#include "doomstat.h"
#include "r_state.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

sector_t* sectors = nullptr;
fixed_t* textureheight = nullptr;
int leveltime = 0;
static int removed = 0;

static void Require(bool condition)
{
    if (!condition)
    {
        std::fprintf(stderr, "Stair regression failed\n");
        std::abort();
    }
}

void* Z_Malloc(int size, int, void*)
{
    void* memory = std::malloc(size);
    Require(memory != nullptr);
    // Reproduce nonzero, uninitialized zone memory.
    std::memset(memory, 0xee, size);
    return memory;
}
void P_AddThinker(thinker_t*) {}
void P_RemoveThinker(thinker_t*) { ++removed; }
void S_StartSound(void*, int) {}
boolean P_ChangeSector(sector_t*, boolean crush)
{
    Require(!crush);
    return false;
}
int P_FindSectorFromLineTag(line_t*, int start) { return start == -1 ? 0 : -1; }

// These services are used by other floor events, not staircase construction.
fixed_t P_FindNextHighestFloor(sector_t*, int) { std::abort(); }
fixed_t P_FindLowestFloorSurrounding(sector_t*) { std::abort(); }
fixed_t P_FindHighestFloorSurrounding(sector_t*) { std::abort(); }
fixed_t P_FindLowestCeilingSurrounding(sector_t*) { std::abort(); }
side_t* getSide(int, int, int) { std::abort(); }
sector_t* getSector(int, int, int) { std::abort(); }
int twoSided(int, int) { std::abort(); }

int main()
{
    for (stair_e mode : {build8, turbo16})
    {
        sector_t rooms[2]{};
        sectors = rooms;
        line_t connection{};
        connection.flags = ML_TWOSIDED;
        connection.frontsector = &rooms[0];
        connection.backsector = &rooms[1];
        line_t* edges[] = {&connection};
        for (auto& room : rooms)
        {
            room.lines = edges;
            room.linecount = 1;
            room.ceilingheight = 128 * FRACUNIT;
            room.floorpic = 7;
            room.special = 9;
        }
        removed = 0;
        Require(EV_BuildStairs(&connection, mode) == 1);
        const int step = (mode == build8 ? 8 : 16) * FRACUNIT;
        for (int i = 0; i < 2; ++i)
        {
            auto* floor = static_cast<floormove_t*>(rooms[i].specialdata);
            Require(floor != nullptr);
            Require(floor->type == raiseFloor && !floor->crush);
            Require(floor->floordestheight == (i + 1) * step);
            Require(floor->speed == (mode == build8 ? FLOORSPEED / 4 : FLOORSPEED * 4));
            for (int tick = 0; tick < 200 && rooms[i].specialdata; ++tick)
                T_MoveFloor(floor);
            Require(rooms[i].specialdata == nullptr);
            Require(rooms[i].floorheight == (i + 1) * step);
            Require(rooms[i].floorpic == 7 && rooms[i].special == 9);
            std::free(floor);
        }
        Require(removed == 2);
    }
}
