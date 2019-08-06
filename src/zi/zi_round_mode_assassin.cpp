#include "zi_round_mode_assassin.h"
#include "zi_players.h"
#include "zi_zombies.h"
#include "zi_boss_assassin.h"

ConVar g_AssassinChance("zi_assassin_chance", "1");

AssassinMode g_AssassinMode;

const char *AssassinMode::GetName()
{
	return "Assassin";
}

int AssassinMode::GetChance()
{
	return g_AssassinChance.GetInt();
}

bool AssassinMode::IsInfectionAllowed()
{
	return false;
}

const char *AssassinMode::GetSound()
{
	switch( RandomInt(1, 2) )
	{
	case 1:
		return "ZombieInfestation/round_mode_nemesis1.mp3";

	case 2:
		return "ZombieInfestation/round_mode_nemesis2.mp3";
	}

	return nullptr;
}

void AssassinMode::OnPostSelection()
{
	ZIPlayer *target = ZIRoundMode::m_RoundTarget;

	if( !target )
	{
		return;
	}

	target->Infect(nullptr, &g_Assassin);

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#FFFF14'>%s is an Assassin!!!</font></center></pre>", target->m_Name);

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !player->m_IsInfected && player->m_pEntity->GetTeam() != CSGO_TEAM_CT )
		{
			player->m_pEntity->SetTeam(CSGO_TEAM_CT);
		}

		if( !player->m_IsBot )
		{			
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(255, 20, 20, 100), Color(255, 20, 20, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, buffer);
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
