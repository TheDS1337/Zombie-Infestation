#include "zi_item_antidote.h"

AntidoteItem g_AntidoteItem;

const char *AntidoteItem::GetName()
{
	return ANTIDOTE_NAME;
}

bool AntidoteItem::IsVIP()
{
	return ANTIDOTE_VIP;
}

int AntidoteItem::GetCost()
{
	return ANTIDOTE_COST;
}

bool AntidoteItem::OnPreSelection(ZIPlayer *player)
{
	// Antidote isn't allowed for last zombie as it can fuck up the round mode
	if( ZICore::m_IsRoundEnd || !ZICore::m_CurrentMode->IsInfectionAllowed() || player->m_IsFirstZombie || player->m_IsLastZombie )
	{
		return false;
	}

	return true;
}

void AntidoteItem::OnPostSelection(ZIPlayer *player)
{
	player->Disinfect(player);
}