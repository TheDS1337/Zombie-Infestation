#include "zi_boss_survivor.h"

ConVar g_SurvivorModel("zi_survivor_model", "Doom Slayer");
ConVar g_SurvivorHealth("zi_survivor_health", "3000.0");
ConVar g_SurvivorSpeed("zi_survivor_speed", "260.0");
ConVar g_SurvivorGravity("zi_survivor_gravity", "800.0");
ConVar g_SurvivorDamageMultiplier("zi_survivor_damage_multiplier", "1.5");

ZISurvivor g_Survivor;

const char *ZISurvivor::GetName()
{
	return "Survivor";
}

const char *ZISurvivor::GetDescription()
{
	return "Survives anything";
}

ZIModel *ZISurvivor::GetModel()
{
	return ZIResources::GetModel(g_SurvivorModel.GetString(), ModelType_Survivor);
}

bool ZISurvivor::IsBoss()
{
	return true;
}

int ZISurvivor::GetHealth()
{
	return g_SurvivorHealth.GetFloat();
}

float ZISurvivor::GetSpeed()
{
	return g_SurvivorSpeed.GetFloat();
}

float ZISurvivor::GetGravity()
{
	return g_SurvivorGravity.GetFloat();
}

float ZISurvivor::GetDamageMultiplier()
{
	return g_SurvivorDamageMultiplier.GetFloat();
}