// Weapon ID #26. Used by weapons:
// Shaman shield, Unknown#210
#include "weapon_private.h"
#include "shared.h"

INCLUDE_ASM("weapon/nonmatchings/w_026", EntityWeaponAttack);

void func_ptr_80170004(Entity* self) {}

void func_ptr_80170008(Entity* self) {}

void func_ptr_8017000C(Entity* self) {}

void func_ptr_80170010(Entity* self) {}

void func_ptr_80170014(Entity* self) {}

int GetWeaponId(void) { return 26; }

INCLUDE_ASM("weapon/nonmatchings/w_026", func_BA000_8017B37C);

INCLUDE_ASM("weapon/nonmatchings/w_026", EntityWeaponShieldSpell);

INCLUDE_ASM("weapon/nonmatchings/w_026", func_ptr_80170024);

void func_ptr_80170028(Entity* self) {}

void WeaponUnused2C(void) {}

void WeaponUnused30(void) {}

void WeaponUnused34(void) {}

void WeaponUnused38(void) {}

void WeaponUnused3C(void) {}
