#ifndef _INCLUDE_ZI_HUMAN_HUNTER_PROPER_H_
#define _INCLUDE_ZI_HUMAN_HUNTER_PROPER_H_
#pragma once

#include "zi_humans.h"

#define HUNTER_NAME "Hunter"
#define HUNTER_DESCRIPTION "kbir we bahloul"
#define HUNTER_MODELNAME "Zombie"
#define HUNTER_VIP false
#define HUNTER_HEALTH 100
#define HUNTER_ARMOR 50
#define HUNTER_SPEED 250.0f
#define HUNTER_GRAVITY 800.0f
#define HUNTER_KNOCKBACK_POWER 0.8f

class HunterHuman final: public ZISoldier
{
public:
	const char *GetName() override;
	const char *GetDescription() override;
	ZIModel *GetModel() override;

	bool IsVIP() override;

	int GetHealth() override;
	int GetArmor() override;
	float GetSpeed() override;
	float GetGravity() override;
	float GetKnockbackPower() override;

	HookReturn OnPostClientThink(ZIPlayer *player);
	bool OnPreClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info);
};

extern HunterHuman g_HunterHuman;

#endif