#ifndef _INCLUDE_ZI_BOSS_SNIPER_PROPER_H_
#define _INCLUDE_ZI_BOSS_SNIPER_PROPER_H_
#pragma once

#include "zi_humans.h"

#define GET_SNIPER(player) (player->m_pHumanLike == &g_Sniper ? &g_Sniper : nullptr)
#define GET_LAST_SNIPER(player) (player->m_pLastHumanLike == &g_Sniper ? &g_Sniper : nullptr)

class ZIModel;

class ZISniper final: public ZIHumanoid
{
public:
	const char *GetName() override;
	const char *GetDescription() override;
	ZIModel *GetModel() override;

	bool IsBoss() override;

	int GetHealth() override;
	float GetSpeed() override;
	float GetGravity() override;

	float GetDamage();
};

extern ConVar g_SniperModel;
extern ConVar g_SniperHealth;
extern ConVar g_SniperSpeed;
extern ConVar g_SniperGravity;
extern ConVar g_SniperDamage;

extern ZISniper g_Sniper;

#endif 