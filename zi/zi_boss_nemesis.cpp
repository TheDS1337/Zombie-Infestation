#include "zi_boss_nemesis.h"

ConVar g_NemesisModel("zi_nemesis_model", "Alien");
ConVar g_NemesisHealth("zi_nemesis_health", "120000.0");
ConVar g_NemesisSpeed("zi_nemesis_speed", "270.0");
ConVar g_NemesisGravity("zi_nemesis_gravity", "550.0");
ConVar g_NemesisKnockbackResistance("zi_nemesis_knockback_resistance", "0.0");
ConVar g_NemesisDamage("zi_nemesis_damage", "250.0");
ConVar g_NemesisLeapForce("zi_nemesis_leap_force", "550.0");	// RandomFloat(500.0f, 650.0f)
ConVar g_NemesisLeapHeight("zi_nemesis_leap_force", "300.0");

ZINemesis g_Nemesis;

const char *ZINemesis::GetName()
{
	return "Nemesis";
}

const char *ZINemesis::GetDescription()
{
	return "Kills every last one";
}

ZIModel *ZINemesis::GetModel()
{
	return ZIResources::GetModel(g_NemesisModel.GetString(), ModelType_Nemesis);
}

bool ZINemesis::IsBoss()
{
	return true;
}

int ZINemesis::GetHealth()
{
	return g_NemesisHealth.GetFloat();
}

float ZINemesis::GetSpeed()
{
	return g_NemesisSpeed.GetFloat();
}

float ZINemesis::GetGravity()
{
	return g_NemesisGravity.GetFloat();
}

float ZINemesis::GetKnockbackResistance()
{
	return g_NemesisKnockbackResistance.GetFloat();
}

float ZINemesis::GetDamage()
{
	return g_NemesisDamage.GetFloat();
}

const char *ZINemesis::GetPainSound()
{
	switch( RandomInt(1, 3) )
	{
	case 1:
		return "ZombieInfestation/nemesis_pain1.mp3";

	case 2:
		return "ZombieInfestation/nemesis_pain2.mp3";

	case 3:
		return "ZombieInfestation/nemesis_pain3.mp3";
	}

	return nullptr;
}