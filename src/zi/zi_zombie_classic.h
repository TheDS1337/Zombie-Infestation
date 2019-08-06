#ifndef _INCLUDE_ZI_ZOMBIE_CLASSIC_PROPER_H_
#define _INCLUDE_ZI_ZOMBIE_CLASSIC_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define CLASSIC_NAME "Classic"
#define CLASSIC_DESCRIPTION "A classic zombie"
#define CLASSIC_MODEL "Zombie"
#define CLASSIC_VIP false
#define CLASSIC_HEALTH 6000
#define CLASSIC_SPEED 260.0f
#define CLASSIC_GRAVITY 800.0f
#define CLASSIC_KNOCKBACK_RESISTANCE 1.0f

class ClassicZombie final: public ZIZombie
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

extern ClassicZombie g_ClassicZombie;

#endif