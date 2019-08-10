#ifndef _INCLUDE_ZI_ZOMBIES_PROPER_H_
#define _INCLUDE_ZI_ZOMBIES_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_resources.h"

#define GET_INFECTED(player) (dynamic_cast<ZIInfected *> (player->m_pHumanLike))
#define GET_LAST_INFECTED(player) dynamic_cast<ZIInfected *> (player->m_pLastHumanLike))

#define GET_ZOMBIE(player) (dynamic_cast<ZIZombie *> (player->m_pHumanLike))
#define GET_LAST_ZOMBIE(player) (dynamic_cast<ZIZombie *> (player->m_pLastHumanLike))

class ZIItem;
class ZIModel;

class ZIInfected: public ZIHumanoid
{
public:
	bool IsInfected() final override;

	virtual const char *GetGrowlSound();
	virtual const char *GetInfectionSound();
	virtual const char *GetFallSound();
	virtual const char *GetPainSound();
	virtual const char *GetBurnSound();
	virtual const char *GetDeathSound();

	virtual float GetKnockbackResistance() = 0;
};

class ZIZombie: public ZIInfected
{
private:
	int m_Index;

public:
	static int Register(ZIZombie *zombie);	
	static ZIZombie *Find(const char *name);

	int GetIndex();

	bool IsBoss() final override;
	virtual bool IsVIP() = 0;	
};

extern ConVar g_ZombieKnockbackDistance;
extern ConVar g_ZombieKnockbackDucking;	

extern SourceHook::CVector<ZIZombie *> g_pZombieClasses;

#endif 
