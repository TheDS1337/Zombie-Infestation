#ifndef _INCLUDE_ZI_ZOMBIE_RAPTOR_PROPER_H_
#define _INCLUDE_ZI_ZOMBIE_RAPTOR_PROPER_H_
#pragma once

#include "zi_zombies.h"

#define RAPTOR_NAME "Raptor"
#define RAPTOR_DESCRIPTION "A way too fast zombie"
#define RAPTOR_MODEL "Raptor"
#define RAPTOR_VIP true
#define RAPTOR_HEALTH 4500
#define RAPTOR_SPEED 280.0f
#define RAPTOR_GRAVITY 800.0f
#define RAPTOR_KNOCKBACK_RESISTANCE 0.5f

class RaptorZombie final: public ZIZombie
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

extern RaptorZombie g_RaptorZombie;

#endif