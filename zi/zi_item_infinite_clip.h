#ifndef _INCLUDE_ZI_ITEM_INFINITE_CLIP_PROPER_H_
#define _INCLUDE_ZI_ITEM_INFINITE_CLIP_PROPER_H_
#pragma once

#include "zi_items.h"

#define INFINITE_CLIP_NAME "Infinite Clip"
#define INFINITE_CLIP_VIP false
#define INFINITE_CLIP_COST 15

class InfiniteClipItem final: public ZIItem
{
public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;

	void OnPostSelection(ZIPlayer *player) override;
};

extern InfiniteClipItem g_InfiniteClipItem;

#endif