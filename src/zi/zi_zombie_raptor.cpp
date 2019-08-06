#include "zi_zombie_raptor.h"

RaptorZombie g_RaptorZombie;

const char *RaptorZombie::GetName()
{
	return RAPTOR_NAME;
}

const char *RaptorZombie::GetDescription()
{
	return RAPTOR_DESCRIPTION;
}

ZIModel *RaptorZombie::GetModel()
{
	return ZIResources::GetModel(RAPTOR_MODEL, ModelType_Zombie);
}

bool RaptorZombie::IsVIP()
{
	return RAPTOR_VIP;
}

int RaptorZombie::GetHealth()
{
	return RAPTOR_HEALTH;
}

float RaptorZombie::GetSpeed()
{
	return RAPTOR_SPEED;
}

float RaptorZombie::GetGravity()
{
	return RAPTOR_GRAVITY;
}

float RaptorZombie::GetKnockbackResistance()
{
	return RAPTOR_KNOCKBACK_RESISTANCE;
}