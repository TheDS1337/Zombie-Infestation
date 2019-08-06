#include "zi_boss_sniper.h"

ConVar g_SniperModel("zi_sniper_model", "Doom Slayer");
ConVar g_SniperHealth("zi_sniper_health", "2000.0");
ConVar g_SniperSpeed("zi_sniper_speed", "260.0");
ConVar g_SniperGravity("zi_sniper_gravity", "800.0");
ConVar g_SniperDamage("zi_sniper_damage", "1500.0");

ZISniper g_Sniper;

const char *ZISniper::GetName()
{
	return "Sniper";
}

const char *ZISniper::GetDescription()
{
	return "Sharpshooter";
}

ZIModel *ZISniper::GetModel()
{
	return ZIResources::GetModel(g_SniperModel.GetString(), ModelType_Survivor);
}

bool ZISniper::IsBoss()
{
	return true;
}

int ZISniper::GetHealth()
{
	return g_SniperHealth.GetFloat();
}

float ZISniper::GetSpeed()
{
	return g_SniperSpeed.GetFloat();
}

float ZISniper::GetGravity()
{
	return g_SniperGravity.GetFloat();
}

float ZISniper::GetDamage()
{
	return g_SniperDamage.GetFloat();
}