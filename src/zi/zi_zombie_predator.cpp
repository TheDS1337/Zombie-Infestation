#include "zi_zombie_predator.h"

PredatorZombie g_PredatorZombie;

const char *PredatorZombie::GetName()
{
	return PREDATOR_NAME;
}

const char *PredatorZombie::GetDescription()
{
	return PREDATOR_DESCRIPTION;
}

ZIModel *PredatorZombie::GetModel()
{
	return ZIResources::GetModel(PREDATOR_MODEL, ModelType_Zombie);
}

bool PredatorZombie::IsVIP()
{
	return PREDATOR_VIP;
}

int PredatorZombie::GetHealth()
{
	return PREDATOR_HEALTH;
}

float PredatorZombie::GetSpeed()
{
	return PREDATOR_SPEED;
}

float PredatorZombie::GetGravity()
{
	return PREDATOR_GRAVITY;
}

float PredatorZombie::GetKnockbackResistance()
{
	return PREDATOR_KNOCKBACK_RESISTANCE;
}