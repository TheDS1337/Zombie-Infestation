#include "zi_items.h"

SourceHook::CVector<ZIItem *> g_pHumansItems;
SourceHook::CVector<ZIItem *> g_pZombiesItems;

int ZIItem::Register(ZIItem *item, ItemTeam team)
{
	if( !item )
	{
		return -1;
	}

	switch( team )
	{
	case ItemTeam_Humans:
		item->m_Index = g_pHumansItems.size();
		item->m_Team = ItemTeam_Humans;

		g_pHumansItems.push_back(item);

		break;

	case ItemTeam_Zombies:
		item->m_Index = g_pZombiesItems.size();
		item->m_Team = ItemTeam_Zombies;

		g_pZombiesItems.push_back(item);

		break;
	}

	return item->m_Index;
}

ZIItem *ZIItem::Find(const char *name, ItemTeam team)
{
	SourceHook::CVector<ZIItem *> *items = nullptr;

	switch( team )
	{
	case ItemTeam_Humans:
		items = &g_pHumansItems;

	case ItemTeam_Zombies:
		items = &g_pZombiesItems;
	}

	if( items )
	{
		ZIItem *item = nullptr;

		for( auto iterator = items->begin(); iterator != items->end(); iterator++ )
		{
			item = *iterator;

			if( !item )
			{
				continue;
			}

			if( strcmp(item->GetName(), name) == 0 )
			{
				return item;
			}
		}
	}

	return nullptr;
}

int ZIItem::GetIndex()
{
	return m_Index;
}

ItemTeam ZIItem::GetTeam()
{
	return m_Team;
}

const char *ZIItem::AdditionalInfo()
{
	return m_AdditionalInfo;
}

void ZIItem::AdditionalInfo(const char *info)
{
	m_AdditionalInfo = (char *) info;
}

bool ZIItem::OnPreSelection(ZIPlayer *player)
{
	return true;
}