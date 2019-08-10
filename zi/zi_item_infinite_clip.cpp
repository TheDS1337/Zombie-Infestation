#include "zi_item_infinite_clip.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"

InfiniteClipItem g_InfiniteClipItem;

const char *InfiniteClipItem::GetName()
{
	return INFINITE_CLIP_NAME;
}

bool InfiniteClipItem::IsVIP()
{
	return INFINITE_CLIP_VIP;
}

int InfiniteClipItem::GetCost()
{
	return INFINITE_CLIP_COST;
}

ItemReturn InfiniteClipItem::OnPreSelection(ZIPlayer *player)
{
	if( player->m_IsInfected || GET_SURVIVOR(player) || GET_SNIPER(player) )
	{
		return ItemReturn_DontShow;
	}
	else if( player->m_HasInfiniteClip )
	{
		return ItemReturn_NotAvailable;
	}

	return ItemReturn_Show;
}

void InfiniteClipItem::OnPostSelection(ZIPlayer *player)
{
	player->m_HasInfiniteClip = true;
}