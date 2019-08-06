#include "zi_round_mode_assassins_vs_snipers.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_sniper.h"
#include "zi_boss_assassin.h"

ConVar g_AvSChance("zi_avs_chance", "5");
ConVar g_AvSSnipersToAssassinsRatio("zi_avs_snipers_to_assassins_ratio", "0.5");

AssassinsVSSnipersMode g_AvSMode;

const char *AssassinsVSSnipersMode::GetName()
{
	return "Assassins VS Snipers";
}

int AssassinsVSSnipersMode::GetChance()
{
	return g_AvSChance.GetInt();
}

bool AssassinsVSSnipersMode::IsInfectionAllowed()
{
	return false;
}

const char *AssassinsVSSnipersMode::GetSound()
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

bool AssassinsVSSnipersMode::OnPreSelection()
{
	m_SnipersToAssassinsRatio = g_AvSSnipersToAssassinsRatio.GetFloat();

	if( m_SnipersToAssassinsRatio <= 0.0f || m_SnipersToAssassinsRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void AssassinsVSSnipersMode::OnPostSelection()
{
	int aliveCount = ZIPlayer::AliveCount(), assassinsCount = 0;
	int maxAssassins = aliveCount * (1 - m_SnipersToAssassinsRatio);

	ZIPlayer *target = ZIRoundMode::m_RoundTarget;	// We start from you baby
	
	while( assassinsCount++ < maxAssassins )
	{
		if( !target || (target->m_IsInfected && GET_ASSASSIN(target)) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Infect(nullptr, &g_Assassin);
		
		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#14FF9B'>Assassins VS Snipers Round !!!</font></center></pre>");

	ZIPlayer *player = nullptr;
	
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !player->m_IsInfected && !GET_SNIPER(player) )
		{
			player->Disinfect(nullptr, &g_Sniper);
		}	

		if( !player->m_IsBot )
		{
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_InfectionUpdatesHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(155, 20, 255, 100), Color(155, 20, 255, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Assassins VS Snipers Round !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}	
}