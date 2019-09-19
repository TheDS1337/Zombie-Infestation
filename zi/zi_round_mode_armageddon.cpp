#include "zi_round_mode_armageddon.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_nemesis.h"

ConVar g_ArmageddonChance("zi_armageddon_chance", "5");
ConVar g_ArmageddonSurvivorsToNemesisRatio("zi_armageddon_survivors_to_nemesis_ratio", "0.55");

ArmageddonMode g_ArmageddonMode;

const char *ArmageddonMode::GetName()
{
	return "Armageddon";
}

int ArmageddonMode::GetChance()
{
	return g_ArmageddonChance.GetInt();
}

bool ArmageddonMode::IsInfectionAllowed()
{
	return false;
}

const char *ArmageddonMode::GetSound()
{
	switch( RandomInt(1, 2) )
	{
	case 1:
		return "ZombieInfestation/round_mode_nemesis1.mp3";

	case 2:
		return "ZombieInfestation/round_mode_nemesis1.mp3";
	}

	return nullptr;
}

bool ArmageddonMode::OnPreSelection()
{
	m_SurvivorsToNemesisRatio = g_ArmageddonSurvivorsToNemesisRatio.GetFloat();

	if( m_SurvivorsToNemesisRatio <= 0.0f || m_SurvivorsToNemesisRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void ArmageddonMode::OnPostSelection()
{
	int aliveCount = ZIPlayer::AliveCount(), nemesisCount = 0;
	int maxNemesis = aliveCount * (1 - m_SurvivorsToNemesisRatio);

	ZIPlayer *target = ZIRoundMode::m_RoundTarget;	// We start from you baby
	
	while( nemesisCount < maxNemesis )
	{
		if( !target || (target->m_IsInfected && GET_NEMESIS(target)) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Infect(nullptr, &g_Nemesis);
		nemesisCount++;

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#9B14FF'>Armageddon Round !!!</font></center></pre>");

	ZIPlayer *player = nullptr;
	
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !player->m_IsInfected && !GET_SURVIVOR(player) )
		{
			player->Disinfect(nullptr, &g_Survivor);
		}	

		if( !player->m_IsBot )
		{
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_InfectionUpdatesHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(155, 20, 255, 100), Color(155, 20, 255, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Armageddon Round !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}	
}
