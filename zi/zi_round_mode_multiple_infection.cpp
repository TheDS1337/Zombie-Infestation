#include "zi_round_mode_multiple_infection.h"
#include "zi_players.h"

ConVar g_MultipleInfectionChance("zi_multiple_infection_chance", "300");
ConVar g_MultipleInfectionRatio("zi_multiple_infection_ratio", "0.3");

MultipleInfectionMode g_MultipleInfectionMode;

const char *MultipleInfectionMode::GetName()
{
	return "Multiple Infection";
}

int MultipleInfectionMode::GetChance()
{
	return g_MultipleInfectionChance.GetInt();
}

bool MultipleInfectionMode::IsInfectionAllowed()
{
	return true;
}

const char *MultipleInfectionMode::GetSound()
{
	return "ZombieInfestation/round_mode_multiple_infection1.mp3";
};

bool MultipleInfectionMode::OnPreSelection()
{
	m_InfectionRatio = g_MultipleInfectionRatio.GetFloat();

	if( m_InfectionRatio <= 0.0f || m_InfectionRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void MultipleInfectionMode::OnPostSelection()
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

	static char buffer[256];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#FF3200'>Multiple Infection !!!</font></center></pre>");

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
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(255, 50, 0, 100), Color(255, 50, 0, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Multiple Infection !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
