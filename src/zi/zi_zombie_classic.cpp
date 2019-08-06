#include "zi_zombie_classic.h"

ClassicZombie g_ClassicZombie;

const char *ClassicZombie::GetName()
{
	return CLASSIC_NAME;
}

const char *ClassicZombie::GetDescription()
{
	return CLASSIC_DESCRIPTION;
}

ZIModel *ClassicZombie::GetModel()
{
	return ZIResources::GetModel(CLASSIC_MODEL, ModelType_Zombie);
}

bool ClassicZombie::IsVIP()
{
	return CLASSIC_VIP;
}

int ClassicZombie::GetHealth()
{
	return CLASSIC_HEALTH;
}

float ClassicZombie::GetSpeed()
{
	return CLASSIC_SPEED;
}

float ClassicZombie::GetGravity()
{
	return CLASSIC_GRAVITY;
}

float ClassicZombie::GetKnockbackResistance()
{
	return CLASSIC_KNOCKBACK_RESISTANCE;
}