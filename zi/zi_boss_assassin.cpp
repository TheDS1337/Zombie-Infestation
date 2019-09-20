#include "zi_boss_assassin.h"

ConVar g_AssassinModel("zi_assassin_model", "Alien");
ConVar g_AssassinHealth("zi_assassin_health", "30000.0");
ConVar g_AssassinSpeed("zi_assassin_speed", "350.0");
ConVar g_AssassinGravity("zi_assassin_gravity", "300.0");
ConVar g_AssassinKnockbackResistance("zi_assassin_knockback_resistance", "0.0");
ConVar g_AssassinDamage("zi_assassin_damage", "250.0");

ZIAssassin g_Assassin;

const char *ZIAssassin::GetName()
{
	return "Assassin";
}

const char *ZIAssassin::GetDescription()
{
	return "Really fast, real danger";
}

ZIModel *ZIAssassin::GetModel()
{
	return ZIResources::GetModel(g_AssassinModel.GetString(), ModelType_Nemesis);
}

bool ZIAssassin::IsBoss()
{
	return true;
}

int ZIAssassin::GetHealth()
{
	return g_AssassinHealth.GetFloat();
}

float ZIAssassin::GetSpeed()
{
	return g_AssassinSpeed.GetFloat();
}

float ZIAssassin::GetGravity()
{
	return g_AssassinGravity.GetFloat();
}

float ZIAssassin::GetKnockbackResistance()
{
	return g_AssassinKnockbackResistance.GetFloat();
}

float ZIAssassin::GetDamage()
{
	return g_AssassinDamage.GetFloat();
}

const char *ZIAssassin::GetPainSound()
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