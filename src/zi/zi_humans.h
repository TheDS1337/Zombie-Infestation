#ifndef _INCLUDE_ZI_HUMANS_PROPER_H_
#define _INCLUDE_ZI_HUMANS_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_players.h"
#include "zi_resources.h"

#define GET_SOLDIER(player) (dynamic_cast<ZISoldier *> (player->m_pHumanLike))
#define GET_LAST_SOLDIER(player) (dynamic_cast<ZISoldier *> (player->m_pLastHumanLike))

class ZIItem;
class ZIModel;

class ZIHumanoid
{
public:
	virtual const char *GetName() = 0;
	virtual const char *GetDescription() = 0;
	virtual ZIModel *GetModel() = 0;

	virtual bool IsInfected();
	virtual bool IsBoss() = 0;

	virtual int GetHealth() = 0;
	virtual float GetSpeed() = 0;
	virtual float GetGravity() = 0;
};

class ZISoldier: public ZIHumanoid
{
private:
	int m_Index;

public:
	static int Register(ZISoldier *soldier);
	static ZISoldier *Find(const char *name);

	int GetIndex();

	virtual const char *GetInfectionSound();
	virtual const char *GetDisinfectionSound();

	bool IsBoss() final override;
	virtual bool IsVIP() = 0;
	virtual int GetArmor() = 0;
	virtual float GetKnockbackPower() = 0;
	SourceHook::CVector<ZIItem *> *GetItems();	
};

extern SourceHook::CVector<ZISoldier *> g_pHumanClasses;

#endif 
