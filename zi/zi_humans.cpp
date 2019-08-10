#include "zi_humans.h"
#include "zi_items.h"

SourceHook::CVector<ZISoldier *> g_pHumanClasses;

bool ZIHumanoid::IsInfected()
{
	return false;
}

int ZISoldier::Register(ZISoldier *soldier)
{
	if( !soldier )
	{
		return -1;
	}

	soldier->m_Index = g_pHumanClasses.size();
	g_pHumanClasses.push_back(soldier);

	return soldier->m_Index;
}

ZISoldier *ZISoldier::Find(const char *name)
{
	ZISoldier *soldier = nullptr;

	for( auto iterator = g_pHumanClasses.begin(); iterator != g_pHumanClasses.end(); iterator++ )
	{
		soldier = *iterator;

		if( !soldier )
		{
			continue;
		}

		if( strcmp(soldier->GetName(), name) == 0 )
		{
			return soldier;
		}
	}

	return nullptr;
}

int ZISoldier::GetIndex()
{
	return m_Index;
}

const char *ZISoldier::GetInfectionSound()
{
	switch( RandomInt(1, 5) )
	{
	case 1:
		return "ZombieInfestation/human_infection1.mp3";

	case 2:
		return "ZombieInfestation/human_infection2.mp3";

	case 3:
		return "ZombieInfestation/human_infection2.mp3";

	case 4:
		return "ZombieInfestation/human_infection4.mp3";

	case 5:
		return "ZombieInfestation/human_infection5.mp3";
	}

	return nullptr;
}

const char *ZISoldier::GetDisinfectionSound()
{
	switch( RandomInt(1, 2) )
	{
	case 1:
		return "ZombieInfestation/human_disinfection1.mp3";

	case 2:
		return "ZombieInfestation/human_disinfection2.mp3";	
	}

	return nullptr;
}

bool ZISoldier::IsBoss()
{
	return false;
}