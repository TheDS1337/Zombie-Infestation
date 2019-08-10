#ifndef _INCLUDE_ZI_BOSS_SURVIVOR_PROPER_H_
#define _INCLUDE_ZI_BOSS_SURVIVOR_PROPER_H_
#pragma once

#include "zi_humans.h"

#define GET_SURVIVOR(player) (player->m_pHumanLike == &g_Survivor ? &g_Survivor : nullptr)
#define GET_LAST_SURVIVOR(player) (player->m_pLastHumanLike == &g_Survivor ? &g_Survivor : nullptr)

class ZIModel;

class ZISurvivor final: public ZIHumanoid
{
public:
	const char *GetName() override;
	const char *GetDescription() override;
	ZIModel *GetModel() override;

	bool IsBoss() override;

	int GetHealth() override;
	float GetSpeed() override;
	float GetGravity() override;

	float GetDamageMultiplier();
};

extern ConVar g_SurvivorModel;
extern ConVar g_SurvivorHealth;
extern ConVar g_SurvivorSpeed;
extern ConVar g_SurvivorGravity;
extern ConVar g_SurvivorDamageMultiplier;

extern ZISurvivor g_Survivor;

#endif 