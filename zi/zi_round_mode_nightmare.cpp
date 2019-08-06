#include "zi_round_mode_nightmare.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"

ConVar g_NightmareChance("zi_nightmare_chance", "5");
ConVar g_NightmareInfectionRatio("zi_nightmare_infection_ratio", "0.4");
ConVar g_NightmareSurvivorsToSnipersRatio("zi_nightmare_survivors_to_snipers_ratio", "0.4");
ConVar g_NightmareNemesisToAssassinsRatio("zi_nightmare_nemesis_to_assassins_ratio", "0.5");

NightmareMode g_NightmareMode;

const char *NightmareMode::GetName()
{
	return "Nightmare";
}

int NightmareMode::GetChance()
{
	return g_NightmareChance.GetInt();
}

bool NightmareMode::IsInfectionAllowed()
{
	return false;
}

const char *NightmareMode::GetSound()
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

bool NightmareMode::OnPreSelection()
{
	m_InfectionRatio = g_NightmareInfectionRatio.GetFloat();
	m_SurvivorsToSnipersRatio = g_NightmareSurvivorsToSnipersRatio.GetFloat();
	m_NemesisToAssassinsRatio = g_NightmareNemesisToAssassinsRatio.GetFloat();

	if( m_InfectionRatio <= 0.0f || m_InfectionRatio >= 1.0f ||
		m_SurvivorsToSnipersRatio <= 0.0f || m_SurvivorsToSnipersRatio >= 1.0f ||
		m_NemesisToAssassinsRatio <= 0.0f || m_NemesisToAssassinsRatio >= 1.0f )
	{
		return false;
	}

	return true;
}

void NightmareMode::OnPostSelection()
{
	int aliveCount = ZIPlayer::AliveCount(), survivorsCount = 0, snipersCount = 0, nemesisCount = 0;
	int maxDisinfected = aliveCount * (1 - m_InfectionRatio);
	int maxSurvivors = maxDisinfected * m_SurvivorsToSnipersRatio;
	int maxSnipers = maxDisinfected - maxSurvivors;
	int maxNemesis = (aliveCount - maxDisinfected) * m_NemesisToAssassinsRatio;

	ZIPlayer *target = ZIRoundMode::m_RoundTarget;	// We start from you baby

	while( survivorsCount++ < maxSurvivors )
	{
		if( !target || (!target->m_IsInfected && GET_SURVIVOR(target)) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Disinfect(nullptr, &g_Survivor);

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	while( snipersCount++ < maxSnipers )
	{
		if( !target || (!target->m_IsInfected && (GET_SURVIVOR(target) || GET_SNIPER(target))) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Disinfect(nullptr, &g_Sniper);

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	while( nemesisCount++ < maxNemesis )
	{
		if( !target || (!target->m_IsInfected && (GET_SURVIVOR(target) || GET_SNIPER(target))) || (target->m_IsInfected && GET_NEMESIS(target)) )
		{
			// No target found, choose one for the next iteration
			target = ZIPlayer::RandomAlive();
			continue;
		}

		target->Infect(nullptr, &g_Nemesis);

		// Search for a next target
		target = ZIPlayer::RandomAlive();
	}

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#FF1414'>Nightmare Round !!!</font></center></pre>");

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !player->m_IsInfected && !GET_SURVIVOR(target) && !GET_SNIPER(target) )
		{
			target->Infect(nullptr, &g_Assassin);
		}

		if( !player->m_IsBot )
		{
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(0, 50, 255, 100), Color(0, 50, 255, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, "Nightmare Round !!!");
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
