#include "zi_item_antidote.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"

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

ItemReturn AntidoteItem::OnPreSelection(ZIPlayer *player)
{
	if( !player->m_IsInfected || GET_NEMESIS(player) || GET_ASSASSIN(player) )
	{
		return ItemReturn_DontShow;
	}
	// Antidote isn't allowed for last zombie as it can fuck up the round mode
	else if( ZICore::m_IsRoundEnd || !ZICore::m_CurrentMode->IsInfectionAllowed() || player->m_IsFirstZombie || player->m_IsLastZombie )
	{
		return ItemReturn_NotAvailable;
	}

	return ItemReturn_Show;
}

void AntidoteItem::OnPostSelection(ZIPlayer *player)
{
	player->Disinfect(player);
}