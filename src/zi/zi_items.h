#ifndef _INCLUDE_ZI_ITEMS_PROPER_H_
#define _INCLUDE_ZI_ITEMS_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_players.h"

enum ItemTeam
{
	ItemTeam_Humans = 0,
	ItemTeam_Zombies	
};

class ZIItem
{
private:
	int m_Index;
	ItemTeam m_Team;
	char *m_AdditionalInfo;

public:
	static int Register(ZIItem *item, ItemTeam team);
	static ZIItem *Find(const char *name, ItemTeam team);

	int GetIndex();
	ItemTeam GetTeam();

	virtual const char *GetName() = 0;	
	virtual bool IsVIP() = 0;
	virtual int GetCost() = 0;
	const char *AdditionalInfo();
	void AdditionalInfo(const char *info);	

	virtual bool OnPreSelection(ZIPlayer *player);
	virtual void OnPostSelection(ZIPlayer *player) = 0;
};

extern SourceHook::CVector<ZIItem *> g_pHumansItems;
extern SourceHook::CVector<ZIItem *> g_pZombiesItems;

#endif 