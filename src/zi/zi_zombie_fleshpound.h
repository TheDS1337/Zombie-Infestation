#ifndef _INCLUDE_ZI_ZOMBIE_FLESHPOUND_PROPER_H_
#define _INCLUDE_ZI_ZOMBIE_FLESHPOUND_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define FLESHPOUND_NAME "Fleshpound"
#define FLESHPOUND_DESCRIPTION "A raging zombie"
#define FLESHPOUND_MODEL "Fleshpound"
#define FLESHPOUND_VIP false
#define FLESHPOUND_HEALTH 6000
#define FLESHPOUND_SPEED 260.0f
#define FLESHPOUND_GRAVITY 800.0f
#define FLESHPOUND_KNOCKBACK_RESISTANCE 1.0f

class FleshpoundZombie final: public ZIZombie
{
public:
	const char *GetName() override;
	const char *GetDescription() override;
	ZIModel *GetModel() override;

	bool IsVIP() override;

	int GetHealth() override;
	float GetSpeed() override;
	float GetGravity() override;
	float GetKnockbackResistance() override;
};

extern FleshpoundZombie g_FleshpoundZombie;

#endif