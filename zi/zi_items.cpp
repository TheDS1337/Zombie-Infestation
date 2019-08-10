#include "zi_items.h"

SourceHook::CVector<ZIItem *> g_pExtraItems;

int ZIItem::Register(ZIItem *item)
{
	if( !item )
	{
		return -1;
	}	

	item->m_Index = g_pExtraItems.size();	
	g_pExtraItems.push_back(item);

	return item->m_Index;
}

ZIItem *ZIItem::Find(const char *name)
{
	ZIItem *item = nullptr;

	for( auto iterator = g_pExtraItems.begin(); iterator != g_pExtraItems.end(); iterator++ )
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

	return nullptr;
}

int ZIItem::GetIndex()
{
	return m_Index;
}

const char *ZIItem::AdditionalInfo()
{
	return m_AdditionalInfo;
}

void ZIItem::AdditionalInfo(const char *info)
{
	m_AdditionalInfo = (char *) info;
}

ItemReturn ZIItem::OnPreSelection(ZIPlayer *player)
{
	return ItemReturn_Show;
}