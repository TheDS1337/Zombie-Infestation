#ifndef _INCLUDE_ZI_ZOMBIE_PREDATOR_PROPER_H_
#define _INCLUDE_ZI_ZOMBIE_PREDATOR_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define PREDATOR_NAME "Predator"
#define PREDATOR_DESCRIPTION "An invisible zombie"
#define PREDATOR_MODEL "Predator"
#define PREDATOR_VIP false
#define PREDATOR_HEALTH 6000
#define PREDATOR_SPEED 260.0f
#define PREDATOR_GRAVITY 800.0f
#define PREDATOR_KNOCKBACK_RESISTANCE 1.0f

class PredatorZombie final: public ZIZombie
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

extern PredatorZombie g_PredatorZombie;

#endif