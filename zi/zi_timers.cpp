#include "zi_timers.h"
#include "zi_players.h"
#include "zi_zombies.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_item_tripmine.h"
#include "zi_item_rage.h"
#include "zi_round_modes.h"
#include "zi_environment.h"
#include "zi_resources.h"

namespace ZITimersCallback
{
	Info m_Info;
	Warning m_Warning;
	Countdown m_Countdown;
	StartMode m_StartMode;
	AmbientSound m_AmbientSound;
	TeamsRandomization m_TeamsRandomization;
	BulletTime m_BulletTime;
	Stats m_Stats;
	SetModel m_SetModel;
	RemoveProtection m_RemoveProtection;
	Respawn m_Respawn;
	ZombieGrowl m_ZombieGrowl;
	ZombieBleed m_ZombieBleed;
	ZombieUnfreeze m_ZombieUnfreeze;

	ResultType Info::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = nullptr;
		int *playerIndex = nullptr;

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || player->m_IsBot )
			{
				continue;
			}

			playerIndex = &player->m_Index;

			UM_SayText(playerIndex, 1, 0, false, "\x03****\x04 Zombie Infestation\x01 CS:GO Mod by\x08 .DS\x03 ****");
			UM_SayText(playerIndex, 1, 0, true, "Write\x04 !menu\x01 in chat to open the main menu.");
			UM_SayText(playerIndex, 1, 0, true, "You can also write\x04 !help\x01 for more\x03 useful\x05 commands\x01 and\x05 binds\x01.");
		}

		return Pl_Continue;
	}

	void Info::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType Warning::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = nullptr;

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || player->m_IsBot )
			{
				continue;
			}

			// Send warning
			gamehelpers->HintTextMsg(player->m_Index, "<pre><center><font color='#990000'>The T-Virus has been set loose...</font></center></pre>");

			// Play sound
			player->PlaySound("ZombieInfestation/warning_death.mp3");
		}

		ZICore::m_pWarningTimer = nullptr;
		return Pl_Stop;
	}

	void Warning::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType Countdown::OnTimer(ITimer *timer, void *data)
	{
		// If at any given time, no mode was selected, then we have a problemo
		if( !ZICore::m_CurrentMode )
		{
			// TODO END ROUND
			CONSOLE_DEBUGGER("Cannot start round mode!");

			ZICore::m_pCountdownTimer = nullptr;
			return Pl_Stop;
		}

		static char buffer[64];

		ZIPlayer *player = nullptr;

		if( ZICore::m_Countdown == -1 )
		{
			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player )
				{
					continue;
				}

				player->PlaySound("ZombieInfestation/warning_biohazard.mp3");
			}

			ZICore::m_Countdown = 10;

			RELEASE_TIMER(ZICore::m_pCountdownTimer);
			ZICore::m_pCountdownTimer = timersys->CreateTimer(&m_Countdown, 13.0f, nullptr, TIMER_FLAG_NO_MAPCHANGE);
		}
		else if( ZICore::m_Countdown > 0 )
		{
			if( ZICore::m_Countdown == 10 )
			{
				// Play the song once and for all...
				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player )
					{
						continue;
					}

					player->PlaySound("ZombieInfestation/round_start_countdown.mp3");
				}

				// Delete the old timer and make a new repeating one
				RELEASE_TIMER(ZICore::m_pCountdownTimer);
				ZICore::m_pCountdownTimer = timersys->CreateTimer(&m_Countdown, 1.0, nullptr, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
			}

			g_pSM->Format(buffer, sizeof(buffer), "<pre><center>Infection in\t<font color='#2EFE2E'>%d</font></center></pre>", ZICore::m_Countdown);

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->HintTextMsg(player->m_Index, buffer);
			}

			ZICore::m_Countdown--;
		}
		else
		{
			ZIRoundMode::Start(ZICore::m_CurrentMode);
			return Pl_Stop;
		}

		return Pl_Continue;
	}

	void Countdown::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType StartMode::OnTimer(ITimer *timer, void *data)
	{
		ZIRoundMode *mode = (ZIRoundMode *) data;

		if( !mode )
		{
			ZICore::m_pStartModeTimer = nullptr;
			return Pl_Stop;
		}

		const char *sound = mode->GetSound();
		ZIPlayer *player = nullptr;

		if( sound )
		{
			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player )
				{
					continue;
				}

				// Stop any sound caused by the countdown
				player->PlaySound(sound);
			}
		}

		mode->OnPostSelection();

		// Forward to other modules/plugins
		ZICore::OnRoundModeStart();
		ZICore::m_IsModeStarted = true;

		int aliveCount = ZIPlayer::AliveCount();

		if( aliveCount == ZIPlayer::HumansCount() || aliveCount == ZIPlayer::ZombiesCount() )
		{
			CONSOLE_DEBUGGER("Something is wrong with this game mode! (%s)", mode->GetName());
		}

		// Play ambient sound
		ZICore::m_pAmbientSoundTimer = timersys->CreateTimer(&m_AmbientSound, RandomFloat(3.0f, 5.0f), mode->GetAmbientSound(), TIMER_FLAG_NO_MAPCHANGE);

		ZICore::m_pStartModeTimer = nullptr;
		return Pl_Stop;
	}

	void StartMode::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType AmbientSound::OnTimer(ITimer *timer, void *data)
	{
		// Keep playing the previous sound (TODO: will see if it's more fun to randomize them)
		AmbientSoundInfo *sound = (AmbientSoundInfo *) data;

		if( !sound )
		{
			return Pl_Continue;
		}

		ZIPlayer *player = nullptr;

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			player->PlaySound(sound->sound);
		}

		// Start the ambient sound after its finished
		ZICore::m_pAmbientSoundTimer = timersys->CreateTimer(&m_AmbientSound, sound->duration + 0.2, sound, TIMER_FLAG_NO_MAPCHANGE);
		return Pl_Stop;
	}

	void AmbientSound::OnTimerEnd(ITimer *timer, void *data)
	{
		ZIPlayer *player = nullptr;

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || player->m_IsBot )
			{
				continue;
			}

			// Stop any ambient sound
			player->StopSound();
		}
	}

	ResultType TeamsRandomization::OnTimer(ITimer *timer, void *data)
	{
		// Randomize the teams
		ZIPlayer::RandomizeTeams();

		ZICore::m_pTeamsRandomization = nullptr;
		return Pl_Stop;
	}

	void TeamsRandomization::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType BulletTime::OnTimer(ITimer *timer, void *data)
	{
		static ConVarRef host_timescale("host_timescale");

		bool *start = (bool *) data;

		if( start && *start )
		{
			float speed = g_BulletTimeSpeed.GetFloat();

			// Start our bullet time
			host_timescale.SetValue(speed);

			// We don't have to keep it forever, it becomes ugly
			*start = false;
			ZICore::m_pBulletTime = timersys->CreateTimer(&m_BulletTime, g_BulletTimeDuration.GetFloat() * speed, start, TIMER_FLAG_NO_MAPCHANGE);

			//	Possibly some visual effects, too
		}
		else
		{
			host_timescale.SetValue(1.0f);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				// Play sound
				player->PlaySound("ZombieInfestation/bullettime_end.mp3");
			}
		}

		ZICore::m_pBulletTime = nullptr;
		return Pl_Stop;
	}

	void BulletTime::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType Stats::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player )
		{
			return Pl_Continue;
		}

		static char buffer[256];

		ZIPlayer *target = player;

		if( player->m_IsAlive )
		{
			if( player->m_IsInfected )
			{
				ke::SafeSprintf(buffer, sizeof(buffer), "%s,\n\nHealth: %s\nPoints: %d", target->m_pHumanLike->GetName(), target->m_Health, target->m_Points);
			}
			else
			{
				ke::SafeSprintf(buffer, sizeof(buffer), "%s,\n\nHealth: %s\nArmor: %s\nPoints: %d", target->m_pHumanLike->GetName(), target->m_Health, target->m_Armor, target->m_Points);
			}
		}
		else
		{
			BasePlayer *playerEnt = player->m_pEntity;

			if( playerEnt->GetSpecMode() != SPECMODE_FIRSTPERSON )
			{
				return Pl_Continue;
			}

			target = ZIPlayer::Find(playerEnt->GetSpecTarget());

			if( !target || !target->m_IsAlive || !target->m_pHumanLike )
			{
				return Pl_Continue;
			}

			// Spectating someone else

			if( target->m_IsBot )
			{
				if( target->m_IsInfected )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s (BOT)\n\n\n%s,\nHealth: %s\nPoints: %d", target->m_Name, target->m_pHumanLike->GetName(), target->m_Health, target->m_Points);
				}
				else
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s (BOT)\n\n\n%s,\nHealth: %s\nArmor: %s\nPoints: %d", target->m_Name, target->m_pHumanLike->GetName(), target->m_Health, target->m_Armor, target->m_Points);
				}
			}
			else
			{
				if( target->m_IsInfected )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s\nFrom: %s, %s\n\n\n%s,\nHealth: %s\nPoints: %d", target->m_Name, target->m_Country, target->m_City, target->m_pHumanLike->GetName(), target->m_Health, target->m_Points);
				}
				else
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s\nFrom: %s, %s\n\n\n%s,\nHealth: %s\nArmor: %s\nPoints: %d", target->m_Name, target->m_Country, target->m_City, target->m_pHumanLike->GetName(), target->m_Health, target->m_Armor, target->m_Points);
				}
			}
		}

		Color color = target->m_IsInfected ? Color(255, 155, 20, 100) : Color(20, 155, 255, 100);
		UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_StatsHud), HUD_STATISTICS_X, HUD_STATISTICS_Y, color, color, 0, 0.0f, 6.0f, 1.1f, 0.0f, buffer);

		return Pl_Continue;
	}

	void Stats::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType SetModel::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player || !player->m_IsAlive || !player->m_pHumanLike )
		{
			player->m_pSetModelTimer = nullptr;
			return Pl_Stop;
		}

		ZIModel *model = player->m_pHumanLike->GetModel();

		if( model )
		{
			BasePlayer *playerEnt = player->m_pEntity;

			playerEnt->SetModel(model->GetPlayerModel());
			playerEnt->SetArmsmodel(model->GetArmsModel());
		}

		// Only put on the glow after the player got his new model
		if( player->m_IsInfected && GET_NEMESIS(player) )
		{
			player->m_pGlowEntity = CreateEntityGlow(player->m_pEntity, 1, Color(255, 0, 0, 255), "primary");
		}
		else if( GET_SNIPER(player) )
		{
			player->m_pGlowEntity = CreateEntityGlow(player->m_pEntity, 1, Color(0, 255, 155, 255), "primary");
		}

		player->m_pSetModelTimer = nullptr;
		return Pl_Stop;
	}

	void SetModel::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType RemoveProtection::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player )
		{
			player->m_pRemoveProtectionTimer = nullptr;
			return Pl_Stop;
		}

		player->m_IsProtected = false;

		player->m_pRemoveProtectionTimer = nullptr;
		return Pl_Stop;
	}

	void RemoveProtection::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType Respawn::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player || player->m_IsAlive )
		{
			player->m_pRespawnTimer = nullptr;
			return Pl_Stop;
		}

		BasePlayer *playerEnt = player->m_pEntity;

		if( !ZICore::m_IsModeStarted && !ZICore::m_IsRoundEnd )
		{
			int team = playerEnt->GetTeam();

			// Incase he was a spectator (may happen on PutInServer)
			if( team != CSGO_TEAM_T && team != CSGO_TEAM_CT )
			{
				playerEnt->SetTeam(RandomInt(0, 1) == 0 ? CSGO_TEAM_T : CSGO_TEAM_CT);
			}

			playerEnt->Respawn();
		}

		player->m_pRespawnTimer = nullptr;
		return Pl_Stop;
	}

	void Respawn::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType ZombieGrowl::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player )
		{
			return Pl_Continue;
		}

		ZIInfected *infected = GET_INFECTED(player);

		if( infected )
		{
			const char *sound = infected->GetGrowlSound();

			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, player->m_Index, CHAN_VOICE, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);
		}

		return Pl_Continue;
	}

	void ZombieGrowl::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType ZombieBleed::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player )
		{
			return Pl_Continue;
		}

		BasePlayer *playerEnt = player->m_pEntity;

		int flags = playerEnt->GetFlags();;

		if( !(flags & FL_ONGROUND) )
		{
			return Pl_Continue;
		}

		// TODO: this works but it doesnt spawn blood atm, possible fixes are trying other decals or check these offsets maybe they're wrong (play with origin also)
		//		Or, try another temp entity that has blood. Adios  

		Vector pos = playerEnt->GetOrigin();

		if( flags & FL_DUCKING )
		{
			pos[2] -= 27.0f; /*18.0f*/
		}
		else
		{
			pos[2] -= 36.0f;
		}

		// We need bloodstrain found on hammer textures!!!!!!!
		static const int BLOOD_DECALS[] = { 47, 48, 49, 50, 51, 52,		// Subrect
									55, 56, 57, 58, 59,					// Flesh Subrect
									95, 96, 97, 98, 99,
									101, 102, 103, 104, 105, 106 };

		CellRecipientFilter filter;
		TE_WorldDecal(filter, 0.0f, pos, BLOOD_DECALS[RandomInt(0, ARRAY_SIZE(BLOOD_DECALS) - 1)]);

		return Pl_Continue;
	}

	void ZombieBleed::OnTimerEnd(ITimer *timer, void *data)
	{
	}

	ResultType ZombieUnfreeze::OnTimer(ITimer *timer, void *data)
	{
		ZIPlayer *player = (ZIPlayer *) data;

		if( !player )
		{
			return Pl_Continue;
		}

		player->Unfreeze();

		player->m_pZombieUnfreezeTimer = nullptr;
		return Pl_Stop;
	}

	void ZombieUnfreeze::OnTimerEnd(ITimer *timer, void *data)
	{
	}
}