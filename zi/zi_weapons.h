#ifndef _INCLUDE_ZI_WEAPONS_PROPER_H_
#define _INCLUDE_ZI_WEAPONS_PROPER_H_
#pragma once

#include "zi_players.h"

#define CSGO_WEAPONS 34

class ZIWeapon
{
public:
	const char *m_pName;
	const char *m_pClassname;

	int m_Slot;
	int m_Clip;
	int m_Ammo;

	float m_KnockbackPower;	

public:
	ZIWeapon(const char *name, const char *classname, int slot, int clip, int ammo, float knockbackPower);

	const char *GetName();
	const char *GetClassname();

	int GetSlot();
	int GetClip();
	int GetAmmo();

	float GetKnockbackPower();

	static ZIWeapon *Find(const char *classname);
	static ZIWeapon *Find(BaseWeapon *weaponEnt);
	static ZIWeapon *FindActive(ZIPlayer *owner);

	static IBaseMenu *BuildPrimaryWeaponsMenu();
	static IBaseMenu *BuildSecondaryWeaponsMenu();
};

extern ZIWeapon g_CSGOWeapons[];

#endif 