#ifndef _INCLUDE_ZI_HUMAN_PRIVATE_PROPER_H_
#define _INCLUDE_ZI_HUMAN_PRIVATE_PROPER_H_
#pragma once

#include "zi_humans.h"

#define PRIVATE_NAME "Private"
#define PRIVATE_DESCRIPTION "na9sh mel kbar"
#define PRIVATE_MODELNAME "Human"
#define PRIVATE_VIP false
#define PRIVATE_HEALTH 100
#define PRIVATE_ARMOR 150
#define PRIVATE_SPEED 250.0f
#define PRIVATE_GRAVITY 800.0f
#define PRIVATE_KNOCKBACK_POWER 1.5f

class PrivateHuman final: public ZISoldier
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
};

extern PrivateHuman g_PrivateHuman;

#endif