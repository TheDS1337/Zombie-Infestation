#include "zi_zombie_fleshpound.h"

FleshpoundZombie g_FleshpoundZombie;

const char *FleshpoundZombie::GetName()
{
	return FLESHPOUND_NAME;
}

const char *FleshpoundZombie::GetDescription()
{
	return FLESHPOUND_DESCRIPTION;
}

ZIModel *FleshpoundZombie::GetModel()
{
	return ZIResources::GetModel(FLESHPOUND_MODEL, ModelType_Zombie);
}

bool FleshpoundZombie::IsVIP()
{
	return FLESHPOUND_VIP;
}

int FleshpoundZombie::GetHealth()
{
	return FLESHPOUND_HEALTH;
}

float FleshpoundZombie::GetSpeed()
{
	return FLESHPOUND_SPEED;
}

float FleshpoundZombie::GetGravity()
{
	return FLESHPOUND_GRAVITY;
}

float FleshpoundZombie::GetKnockbackResistance()
{
	return FLESHPOUND_KNOCKBACK_RESISTANCE;
}