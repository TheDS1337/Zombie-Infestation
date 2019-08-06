#include "zi_item_infection_bomb.h"

InfectionBombItem g_InfectionBombItem;

const char *InfectionBombItem::GetName()
{
	return INFECTION_BOMB_NAME;
}

bool InfectionBombItem::IsVIP()
{
	return INFECTION_BOMB_VIP;
}

int InfectionBombItem::GetCost()
{
	return INFECTION_BOMB_COST;
}

bool InfectionBombItem::OnPreSelection(ZIPlayer *player)
{
	if( ZICore::m_IsRoundEnd || !ZICore::m_CurrentMode->IsInfectionAllowed() || HasInfectionBomb(player) )
	{
		return false;
	}
	else if( m_InfectionBombsCounter < INFECTION_BOMB_LIMIT )
	{
		static char buffer[8];
		ke::SafeSprintf(buffer, sizeof(buffer), "%d/%d", m_InfectionBombsCounter, INFECTION_BOMB_LIMIT);

		AdditionalInfo(buffer);
		return true;
	}	

	return false;
}

void InfectionBombItem::OnPostSelection(ZIPlayer *player)
{
	player->m_pEntity->GiveItem("weapon_tagrenade");

	m_InfectionBombsCounter++;
}

bool InfectionBombItem::HasInfectionBomb(ZIPlayer *player)
{
	BasePlayer *playerEnt = player->m_pEntity;
	BaseWeapon *nadeEnt = nullptr;

	while( (nadeEnt = playerEnt->GetItemFromSlot(CSGO_WEAPON_SLOT_NADES)) )
	{
		if( strcmp(nadeEnt->GetClassname(), "weapon_tagrenade") != 0 )
		{
			continue;
		}

		return true;
	}

	return false;
}

void InfectionBombItem::OnPostRoundStart()
{
	m_InfectionBombsCounter = 0;
}