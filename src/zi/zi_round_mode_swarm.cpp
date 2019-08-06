#include "zi_round_mode_swarm.h"
#include "zi_players.h"

ConVar g_SwarmChance("zi_swarm_chance", "200");
ConVar g_SwarmInfectionRatio("zi_swarm_infection_ratio", "0.45");

SwarmMode g_SwarmMode;

const char *SwarmMode::GetName()
{
	return "Swarm";
}

int SwarmMode::GetChance()
{
	return g_SwarmChance.GetInt();
}

bool SwarmMode::IsInfectionAllowed()
{
	return false;
}

const char *SwarmMode::GetSound()
{
	return "ZombieInfestation/round_mode_multiple_infection1.mp3";
};

bool SwarmMode::OnPreSelection()
{
	m_InfectionRatio = g_SwarmInfectionRatio.GetFloat();

	if( m_InfectionRatio <= 0.0f || m_InfectionRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void SwarmMode::OnPostSelection()
{
	int aliveCount = ZIPlayer::AliveCount(), zombiesCount = 0;
	int maxZombies = aliveCount * m_InfectionRatio;

	ZIPlayer *target = ZIRoundMode::m_RoundTarget;	// We start from you baby

	while( zombiesCount++ < maxZombies )
	{
		if( !target || target->m_IsInfected )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Infect();

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#14FF14'>Swarm Round !!!</font></center></pre>");

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
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(20, 255, 20, 100), Color(20, 255, 20, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Swarm Round !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
