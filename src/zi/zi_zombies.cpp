#include "zi_zombies.h" 
#include "zi_items.h"

ConVar g_ZombieKnockbackDistance("zi_zombie_knockback_distance", "220.0");
ConVar g_ZombieKnockbackDucking("zi_zombie_knockback_ducking", "0.5");	// RandomFloat(0.4f, 0.6f)

SourceHook::CVector<ZIZombie *> g_pZombieClasses;

bool ZIInfected::IsInfected()
{
	return true;
}

const char *ZIInfected::GetGrowlSound()
{
	switch( RandomInt(1, 5) )
	{
	case 1:
		return "ZombieInfestation/zombie_growl1.mp3";

	case 2:
		return "ZombieInfestation/zombie_growl2.mp3";

	case 3:
		return "ZombieInfestation/zombie_growl3.mp3";

	case 4:
		return "ZombieInfestation/zombie_growl4.mp3";

	case 5:
		return "ZombieInfestation/zombie_growl5.mp3";
	}

	return nullptr;
}

const char *ZIInfected::GetInfectionSound()
{
	switch( RandomInt(1, 4) )
	{
	case 1:
		return "ZombieInfestation/zombie_infection1.mp3";

	case 2:
		return "ZombieInfestation/zombie_infection2.mp3";

	case 3:
		return "ZombieInfestation/zombie_infection3.mp3";

	case 4:
		return "ZombieInfestation/zombie_infection4.mp3";
	}

	return nullptr;
}

const char *ZIInfected::GetFallSound()
{
	return "ZombieInfestation/zombie_fall1.mp3";
}

const char *ZIInfected::GetPainSound()
{
	switch( RandomInt(1, 5) )
	{
	case 1:
		return "ZombieInfestation/zombie_pain1.mp3";

	case 2:
		return "ZombieInfestation/zombie_pain2.mp3";

	case 3:
		return "ZombieInfestation/zombie_pain3.mp3";

	case 4:
		return "ZombieInfestation/zombie_pain4.mp3";

	case 5:
		return "ZombieInfestation/zombie_pain5.mp3";
	}

	return nullptr;
}

const char *ZIInfected::GetBurnSound()
{
	switch( RandomInt(1, 2) )
	{
	case 1:
		return "ZombieInfestation/zombie_burn1.mp3";

	case 2:
		return "ZombieInfestation/zombie_burn2.mp3";
	}

	return nullptr;
}

const char *ZIInfected::GetDeathSound()
{
	switch( RandomInt(1, 5) )
	{
	case 1:
		return "ZombieInfestation/zombie_death1.mp3";

	case 2:
		return "ZombieInfestation/zombie_death2.mp3";

	case 3:
		return "ZombieInfestation/zombie_death3.mp3";

	case 4:
		return "ZombieInfestation/zombie_death4.mp3";

	case 5:
		return "ZombieInfestation/zombie_death5.mp3";
	}

	return nullptr;
}

int ZIZombie::Register(ZIZombie *zombie)
{
	if( !zombie )
	{
		return -1;
	}

	zombie->m_Index = g_pZombieClasses.size();
	g_pZombieClasses.push_back(zombie);

	return zombie->m_Index;
}

ZIZombie *ZIZombie::Find(const char *name)
{
	ZIZombie *zombie = nullptr;

	for( auto iterator = g_pZombieClasses.begin(); iterator != g_pZombieClasses.end(); iterator++ )
	{
		zombie = *iterator;

		if( !zombie )
		{
			continue;
		}

		if( strcmp(zombie->GetName(), name) == 0 )
		{
			return zombie;
		}
	}

	return nullptr;
}

int ZIZombie::GetIndex()
{
	return m_Index;
}

bool ZIZombie::IsBoss()
{
	return false;
}

SourceHook::CVector<ZIItem *> *ZIZombie::GetItems()
{
	return &g_pZombiesItems;		
}