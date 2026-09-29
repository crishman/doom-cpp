#include "p_local.h"
#include "p_saveg.h"
#include "doomstat.h"
#include "r_state.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

player_t players[MAXPLAYERS]{};
boolean playeringame[MAXPLAYERS]{};
state_t states[NUMSTATES]{};
mobjinfo_t mobjinfo[NUMMOBJTYPES]{};
thinker_t thinkercap{};
ceiling_t* activeceilings[MAXCEILINGS]{};
sector_t sector_storage[2]{};
sector_t* sectors = sector_storage;
line_t* lines = nullptr;
side_t* sides = nullptr;
int numsectors = 0;
int numlines = 0;

// Unused engine services: fail if a test unexpectedly enters these paths.
void I_Error(const char* message, ...) { std::fprintf(stderr, "%s\n", message); std::abort(); }
void* Z_Malloc(int, int, void*) { std::abort(); }
void Z_Free(void*) { std::abort(); }
void P_AddThinker(thinker_t*) { std::abort(); }
void P_RemoveMobj(mobj_t*) { std::abort(); }
void P_InitThinkers() { std::abort(); }
void P_SetThingPosition(mobj_t*) { std::abort(); }
void P_AddActiveCeiling(ceiling_t*) { std::abort(); }
void P_AddActivePlat(plat_t*) { std::abort(); }
void P_MobjThinker(mobj_t*) {}
void T_MoveCeiling(ceiling_t*) {}
void T_VerticalDoor(vldoor_t*) {}
void T_MoveFloor(floormove_t*) {}
void T_PlatRaise(plat_t*) {}
void T_LightFlash(lightflash_t*) {}
void T_StrobeFlash(strobe_t*) {}
void T_Glow(glow_t*) {}

// Reports where it failed: a bare message is useless when the only view of the
// run is a CI log from another platform.
#define Require(cond) RequireAt((cond), #cond, __LINE__)
void RequireAt(bool condition, const char* text, int line)
{
    if (!condition)
    {
        std::fprintf(stderr, "Savegame regression failed at line %d: %s\n", line, text);
        std::exit(1);
    }
}
byte* Pad(byte* p)
{
    while (reinterpret_cast<std::uintptr_t>(p) % 4)
        ++p;
    return p;
}
void Link(thinker_t* thinker)
{
    thinkercap.next = thinker;
    thinker->next = &thinkercap;
    thinker->prev = &thinkercap;
}

// Takes an already-thunked think_t rather than the typed thinker, so T can no
// longer be deduced from it and is named at each call site.
template<class T>
void CheckSpecial(byte* begin, think_t action, int marker)
{
    T original{};
    original.sector = sectors + 1;
    original.thinker.function = action;
    Link(&original.thinker);
    if (!action)
        activeceilings[0] = reinterpret_cast<ceiling_t*>(&original);
    save_p = begin;
    P_ArchiveSpecials();
    T saved;
    std::memcpy(&saved, Pad(begin + 1), sizeof(saved));
    Require(*begin == marker);
    Require(reinterpret_cast<std::uintptr_t>(saved.sector) == 1);
    Require(original.sector == sectors + 1);
    Require(save_p == Pad(begin + 1) + sizeof(T) + 1);
    Require(save_p[-1] == 7); // tc_endspecials
    activeceilings[0] = nullptr;
}

int main()
{
    alignas(16) std::array<byte, 8192> buffer{};
    for (int offset = 0; offset < 8; ++offset)
    {
        byte* begin = buffer.data() + offset;
        playeringame[0] = true;
        players[0] = {};
        players[0].health = 73;
        players[0].psprites[0].state = states + 3;
        save_p = begin;
        P_ArchivePlayers();
        Require(save_p == Pad(begin) + sizeof(player_t));
        player_t saved;
        std::memcpy(&saved, Pad(begin), sizeof(saved));
        Require(reinterpret_cast<std::uintptr_t>(saved.psprites[0].state) == 3);
        Require(players[0].psprites[0].state == states + 3);
        players[0] = {};
        save_p = begin;
        P_UnArchivePlayers();
        Require(players[0].health == 73 && players[0].psprites[0].state == states + 3);

        mobj_t actor{};
        actor.state = states + 5;
        actor.player = players;
        actor.thinker.function = P_Thinker<P_MobjThinker>;
        Link(&actor.thinker);
        save_p = begin;
        P_ArchiveThinkers();
        mobj_t saved_actor;
        std::memcpy(&saved_actor, Pad(begin + 1), sizeof(saved_actor));
        Require(*begin == 1 && save_p[-1] == 0);
        Require(save_p == Pad(begin + 1) + sizeof(mobj_t) + 1);
        Require(reinterpret_cast<std::uintptr_t>(saved_actor.state) == 5);
        Require(reinterpret_cast<std::uintptr_t>(saved_actor.player) == 1);
        Require(actor.state == states + 5 && actor.player == players);

        CheckSpecial<ceiling_t>(begin, P_Thinker<T_MoveCeiling>, 0);
        CheckSpecial<ceiling_t>(begin, nullptr, 0);
        CheckSpecial<vldoor_t>(begin, P_Thinker<T_VerticalDoor>, 1);
        CheckSpecial<floormove_t>(begin, P_Thinker<T_MoveFloor>, 2);
        CheckSpecial<plat_t>(begin, P_Thinker<T_PlatRaise>, 3);
        CheckSpecial<lightflash_t>(begin, P_Thinker<T_LightFlash>, 4);
        CheckSpecial<strobe_t>(begin, P_Thinker<T_StrobeFlash>, 5);
        CheckSpecial<glow_t>(begin, P_Thinker<T_Glow>, 6);
    }
}
