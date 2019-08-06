#ifndef _INCLUDE_ZI_ITEM_ANTIDOTE_PROPER_H_
#define _INCLUDE_ZI_ITEM_ANTIDOTE_PROPER_H_
#pragma once

#include "zi_items.h"

#define ANTIDOTE_NAME "T-virus Antidote"
#define ANTIDOTE_VIP false
#define ANTIDOTE_COST 1

class AntidoteItem final: public ZIItem
{
public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;	

	bool OnPreSelection(ZIPlayer *player) override;
	void OnPostSelection(ZIPlayer *player) override;
};

extern AntidoteItem g_AntidoteItem;

#endif