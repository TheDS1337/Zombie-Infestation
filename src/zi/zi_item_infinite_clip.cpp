#include "zi_item_infinite_clip.h"

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

void InfiniteClipItem::OnPostSelection(ZIPlayer *player)
{
	player->m_HasInfiniteClip = true;
}