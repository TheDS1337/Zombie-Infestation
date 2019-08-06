#include "zi_round_mode_single_infection.h"
#include "zi_players.h"

ConVar g_SingleInfectionChance("zi_single_infection_chance", "100");

SingleInfectionMode g_SingleInfectionMode;

const char *SingleInfectionMode::GetName()
{
	return "Single Infection";
}

int SingleInfectionMode::GetChance()
{
	return g_SingleInfectionChance.GetInt();
}

bool SingleInfectionMode::IsInfectionAllowed()
{
	return true;
}

void SingleInfectionMode::OnPostSelection()
{
	ZIPlayer *target = ZIRoundMode::m_RoundTarget;

	if( !target )
	{
		return;
	}

	target->Infect();

	static char buffer[196];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#FF1414'>%s was infected with the T-virus!</font>\n<font color='#141414'>KILL HIM BEFORE HE INFECTS MORE</font></center></pre>", target->m_Name);

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