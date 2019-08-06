#include "zi_round_mode_nemesis.h"
#include "zi_players.h"
#include "zi_zombies.h"
#include "zi_boss_nemesis.h"

ConVar g_NemesisChance("zi_nemesis_chance", "1");

NemesisMode g_NemesisMode;

const char *NemesisMode::GetName()
{
	return "Nemesis";
}

int NemesisMode::GetChance()
{
	return g_NemesisChance.GetInt();
}

bool NemesisMode::IsInfectionAllowed()
{
	return false;
}

const char *NemesisMode::GetSound()
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

void NemesisMode::OnPostSelection()
{
	ZIPlayer *target = ZIRoundMode::m_RoundTarget;

	if( !target )
	{
		return;
	}

	target->Infect(nullptr, &g_Nemesis);

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#FF1414'>%s is a Nemesis!!!</font></center></pre>", target->m_Name);

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
