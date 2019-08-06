#include "zi_round_mode_sniper.h"
#include "zi_players.h"
#include "zi_zombies.h"
#include "zi_boss_sniper.h"

ConVar g_SniperChance("zi_sniper_chance", "1");

SniperMode g_SniperMode;

const char *SniperMode::GetName()
{
	return "Sniper";
}

int SniperMode::GetChance()
{
	return g_SniperChance.GetInt();
}

bool SniperMode::IsInfectionAllowed()
{
	return false;
}

const char *SniperMode::GetSound()
{
	switch( RandomInt(1, 2) )
	{
	case 1:
		return "ZombieInfestation/round_mode_survivor1.mp3";

	case 2:
		return "ZombieInfestation/round_mode_survivor2.mp3";
	}

	return nullptr;
}

void SniperMode::OnPostSelection()
{
	ZIPlayer *target = ZIRoundMode::m_RoundTarget;

	if( !target )
	{
		return;
	}

	target->Disinfect(nullptr, &g_Sniper);

	static char buffer[128];
	ke::SafeSprintf(buffer, sizeof(buffer), "<pre><center><font color='#14FFFF'>%s is a Sniper!!!</font></center></pre>", target->m_Name);

	ZIPlayer *player = nullptr;
	
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_IsAlive && !GET_SNIPER(player) )
		{
			player->Infect();
		}

		if( !player->m_IsBot )
		{
//			UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(20, 20, 255, 100), Color(20, 20, 255, 100), 1, 1.0f, 1.0f, 5.0f, 0.0f, buffer);
			gamehelpers->HintTextMsg(player->m_Index, buffer);
		}
	}
}
