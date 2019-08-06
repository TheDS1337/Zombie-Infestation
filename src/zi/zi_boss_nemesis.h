#ifndef _INCLUDE_ZI_BOSS_NEMESIS_PROPER_H_
#define _INCLUDE_ZI_BOSS_NEMESIS_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define GET_NEMESIS(player) (dynamic_cast<ZINemesis *> (player->m_pHumanLike))
#define GET_LAST_NEMESIS(player) (dynamic_cast<ZINemesis *> (player->m_pLastHumanLike))

class ZIModel;

class ZINemesis final: public ZIInfected
{
public:
	const char *GetName() override;
	const char *GetDescription() override;
	ZIModel *GetModel() override;
	const char *GetPainSound() override;

	bool IsBoss() override;

	int GetHealth() override;
	float GetSpeed() override;
	float GetGravity() override;
	float GetKnockbackResistance() override;

	float GetDamage();
};

extern ConVar g_NemesisModel;
extern ConVar g_NemesisHealth;
extern ConVar g_NemesisSpeed;
extern ConVar g_NemesisGravity;
extern ConVar g_NemesisKnockbackResistance;
extern ConVar g_NemesisDamage;
extern ConVar g_NemesisLeapForce;
extern ConVar g_NemesisLeapHeight;

extern ZINemesis g_Nemesis;

#endif 
