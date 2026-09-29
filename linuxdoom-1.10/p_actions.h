// Typed state actions shared by the state table and its implementations.
#ifndef DOOM_P_ACTIONS_H
#define DOOM_P_ACTIONS_H

#include <cstddef>
#include <type_traits>

struct mobj_s;
struct player_s;
struct pspdef_s;

// Actor and weapon states use distinct signatures. Keep both fields explicit
// so callers never reinterpret a function pointer or read an inactive union.
struct state_action_t
{
    void (*actor)(mobj_s*) = nullptr;
    void (*weapon)(player_s*, pspdef_s*) = nullptr;

    constexpr state_action_t(std::nullptr_t) {}

    // Template deduction lets existing {NULL} entries select the null overload.
    template<class Actor>
    constexpr state_action_t(void (*function)(Actor*)) : actor(function)
    {
        static_assert(std::is_same_v<Actor, mobj_s>);
    }

    template<class Player, class Psprite>
    constexpr state_action_t(void (*function)(Player*, Psprite*)) : weapon(function)
    {
        static_assert(std::is_same_v<Player, player_s>);
        static_assert(std::is_same_v<Psprite, pspdef_s>);
    }
};

void A_Light0(player_s* player, pspdef_s* psp);
void A_WeaponReady(player_s* player, pspdef_s* psp);
void A_Lower(player_s* player, pspdef_s* psp);
void A_Raise(player_s* player, pspdef_s* psp);
void A_Punch(player_s* player, pspdef_s* psp);
void A_ReFire(player_s* player, pspdef_s* psp);
void A_FirePistol(player_s* player, pspdef_s* psp);
void A_Light1(player_s* player, pspdef_s* psp);
void A_FireShotgun(player_s* player, pspdef_s* psp);
void A_Light2(player_s* player, pspdef_s* psp);
void A_FireShotgun2(player_s* player, pspdef_s* psp);
void A_CheckReload(player_s* player, pspdef_s* psp);
void A_OpenShotgun2(player_s* player, pspdef_s* psp);
void A_LoadShotgun2(player_s* player, pspdef_s* psp);
void A_CloseShotgun2(player_s* player, pspdef_s* psp);
void A_FireCGun(player_s* player, pspdef_s* psp);
void A_GunFlash(player_s* player, pspdef_s* psp);
void A_FireMissile(player_s* player, pspdef_s* psp);
void A_Saw(player_s* player, pspdef_s* psp);
void A_FirePlasma(player_s* player, pspdef_s* psp);
void A_BFGsound(player_s* player, pspdef_s* psp);
void A_FireBFG(player_s* player, pspdef_s* psp);
void A_BFGSpray(mobj_s* actor);
void A_Explode(mobj_s* actor);
void A_Pain(mobj_s* actor);
void A_PlayerScream(mobj_s* actor);
void A_Fall(mobj_s* actor);
void A_XScream(mobj_s* actor);
void A_Look(mobj_s* actor);
void A_Chase(mobj_s* actor);
void A_FaceTarget(mobj_s* actor);
void A_PosAttack(mobj_s* actor);
void A_Scream(mobj_s* actor);
void A_SPosAttack(mobj_s* actor);
void A_VileChase(mobj_s* actor);
void A_VileStart(mobj_s* actor);
void A_VileTarget(mobj_s* actor);
void A_VileAttack(mobj_s* actor);
void A_StartFire(mobj_s* actor);
void A_Fire(mobj_s* actor);
void A_FireCrackle(mobj_s* actor);
void A_Tracer(mobj_s* actor);
void A_SkelWhoosh(mobj_s* actor);
void A_SkelFist(mobj_s* actor);
void A_SkelMissile(mobj_s* actor);
void A_FatRaise(mobj_s* actor);
void A_FatAttack1(mobj_s* actor);
void A_FatAttack2(mobj_s* actor);
void A_FatAttack3(mobj_s* actor);
void A_BossDeath(mobj_s* actor);
void A_CPosAttack(mobj_s* actor);
void A_CPosRefire(mobj_s* actor);
void A_TroopAttack(mobj_s* actor);
void A_SargAttack(mobj_s* actor);
void A_HeadAttack(mobj_s* actor);
void A_BruisAttack(mobj_s* actor);
void A_SkullAttack(mobj_s* actor);
void A_Metal(mobj_s* actor);
void A_SpidRefire(mobj_s* actor);
void A_BabyMetal(mobj_s* actor);
void A_BspiAttack(mobj_s* actor);
void A_Hoof(mobj_s* actor);
void A_CyberAttack(mobj_s* actor);
void A_PainAttack(mobj_s* actor);
void A_PainDie(mobj_s* actor);
void A_KeenDie(mobj_s* actor);
void A_BrainPain(mobj_s* actor);
void A_BrainScream(mobj_s* actor);
void A_BrainDie(mobj_s* actor);
void A_BrainAwake(mobj_s* actor);
void A_BrainSpit(mobj_s* actor);
void A_SpawnSound(mobj_s* actor);
void A_SpawnFly(mobj_s* actor);
void A_BrainExplode(mobj_s* actor);

#endif
