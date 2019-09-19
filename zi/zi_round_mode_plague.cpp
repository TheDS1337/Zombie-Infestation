#include "zi_round_mode_plague.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_nemesis.h"

ConVar g_PlagueChance("zi_plague_chance", "1");
ConVar g_PlagueBossRatio("zi_plague_boss_ratio", "0.157");
ConVar g_PlagueSurvivorsToNemesisRatio("zi_plague_survivors_to_nemesis_ratio", "0.6");
ConVar g_PlagueInfectionRatio("zi_plague_infection_ratio", "0.45");

PlagueMode g_PlagueMode;

const char *PlagueMode::GetName()
{
	return "Plague";
}

int PlagueMode::GetChance()
{
	return g_PlagueChance.GetInt();
}

bool PlagueMode::IsInfectionAllowed()
{
	return false;
}

const char *PlagueMode::GetSound()
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

bool PlagueMode::OnPreSelection()
{
	m_BossRatio = g_PlagueBossRatio.GetFloat();
	m_SurvivorsToNemesisRatio = g_PlagueSurvivorsToNemesisRatio.GetFloat();
	m_InfectionRatio = g_PlagueInfectionRatio.GetFloat();

	if( m_BossRatio <= 0.0f || m_BossRatio >= 1.0f ||
		m_SurvivorsToNemesisRatio <= 0.0f || m_SurvivorsToNemesisRatio >= 1.0f || 
		m_InfectionRatio <= 0.0f || m_InfectionRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void PlagueMode::OnPostSelection()
{
	int aliveCount = ZIPlayer::AliveCount(), survivorsCount = 0, nemesisCount = 0, zombiesCount = 0;
	int bossCount = aliveCount * m_BossRatio;
	int maxSurvivors = bossCount * m_SurvivorsToNemesisRatio;
	int maxNemesis = bossCount - maxSurvivors;
	int maxZombies = (aliveCount - bossCount) * m_InfectionRatio;

	ZIPlayer *target = ZIRoundMode::m_RoundTarget;	// We start from you baby

	while( survivorsCount < maxSurvivors )
	{
		if( !target || (!target->m_IsInfected && GET_SURVIVOR(target)) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Disinfect(nullptr, &g_Survivor);
		survivorsCount++;

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	while( nemesisCount < maxNemesis )
	{
		if( !target || (!target->m_IsInfected && GET_SURVIVOR(target)) || (target->m_IsInfected && GET_NEMESIS(target)) )
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

	while( zombiesCount < maxZombies )
	{
		if( !target || (!target->m_IsInfected && GET_SURVIVOR(target)) || target->m_IsInfected )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Infect();
		zombiesCount++;

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#0032FF'>Plague Round !!!</font></center></pre>");

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !player->m_IsInfected && !GET_SURVIVOR(player) && player->m_pEntity->GetTeam() != CSGO_TEAM_CT )
		{
			player->m_pEntity->SetTeam(CSGO_TEAM_CT);
		}

		if( !player->m_IsBot )
		{
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(0, 50, 255, 100), Color(0, 50, 255, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Plague Round !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
