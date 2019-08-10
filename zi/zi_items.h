#ifndef _INCLUDE_ZI_ITEMS_PROPER_H_
#define _INCLUDE_ZI_ITEMS_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_players.h"

enum ItemReturn
{
	ItemReturn_Show = 0,
	ItemReturn_NotAvailable,
	ItemReturn_DontShow	
};

class ZIItem
{
private:
	int m_Index;	
	char *m_AdditionalInfo;

public:
	static int Register(ZIItem *item);
	static ZIItem *Find(const char *name);

	int GetIndex();	

	virtual const char *GetName() = 0;	
	virtual bool IsVIP() = 0;
	virtual int GetCost() = 0;
	const char *AdditionalInfo();
	void AdditionalInfo(const char *info);	

	virtual ItemReturn OnPreSelection(ZIPlayer *player);
	virtual void OnPostSelection(ZIPlayer *player) = 0;
};

extern SourceHook::CVector<ZIItem *> g_pExtraItems;

#endif 