#ifndef _INCLUDE_ZI_BOSS_ASSASSIN_PROPER_H_
#define _INCLUDE_ZI_BOSS_ASSASSIN_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define GET_ASSASSIN(player) (dynamic_cast<ZIAssassin *> (player->m_pHumanLike))
#define GET_LAST_ASSASSIN(player) (dynamic_cast<ZIAssassin *> (player->m_pLastHumanLike))

class ZIModel;

class ZIAssassin final: public ZIInfected
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

extern ConVar g_AssassinModel;
extern ConVar g_AssassinHealth;
extern ConVar g_AssassinSpeed;
extern ConVar g_AssassinGravity;
extern ConVar g_AssassinKnockbackResistance;
extern ConVar g_AssassinDamage;

extern ZIAssassin g_Assassin;

#endif 
