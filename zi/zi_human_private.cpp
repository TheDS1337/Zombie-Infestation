#include "zi_human_private.h"

PrivateHuman g_PrivateHuman;

const char *PrivateHuman::GetName()
{
	return PRIVATE_NAME;
}

const char *PrivateHuman::GetDescription()
{
	return PRIVATE_DESCRIPTION;
}

ZIModel *PrivateHuman::GetModel()
{
	return ZIResources::RandomModel(ModelType_Human);
}

bool PrivateHuman::IsVIP()
{
	return PRIVATE_VIP;
}

int PrivateHuman::GetHealth()
{
	return PRIVATE_HEALTH;
}

int PrivateHuman::GetArmor()
{
	return PRIVATE_ARMOR;
}

float PrivateHuman::GetSpeed()
{
	return PRIVATE_SPEED;
}

float PrivateHuman::GetGravity()
{
	return PRIVATE_GRAVITY;
}

float PrivateHuman::GetKnockbackPower()
{
	return PRIVATE_KNOCKBACK_POWER;
}