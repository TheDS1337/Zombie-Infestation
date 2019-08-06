#include "zi_round_modes.h"
#include "zi_players.h"
#include "zi_timers.h"
#include "zi_resources.h"

int ZIRoundMode::m_TotalChances = 0;
ZIPlayer *ZIRoundMode::m_RoundTarget = nullptr;

SourceHook::CVector<ZIRoundMode *> g_pRoundModes;

int ZIRoundMode::Register(ZIRoundMode *mode)
{
	if( !mode )
	{
		return - 1;
	}

	mode->m_Index = g_pRoundModes.size();
	m_TotalChances += mode->GetChance();
	
	g_pRoundModes.push_back(mode);

	return mode->m_Index;
}

ZIRoundMode *ZIRoundMode::Find(const char *name)
{
	ZIRoundMode *mode = nullptr;

	for( auto iterator = g_pRoundModes.begin(); iterator != g_pRoundModes.end(); iterator++ )
	{
		mode = *iterator;

		if( !mode )
		{
			continue;
		}

		if( strcmp(mode->GetName(), name) == 0 )
		{
			return mode;
		}
	}

	return nullptr;
}

ZIRoundMode *ZIRoundMode::Choose()
{
	ZIRoundMode *mode = nullptr;

	do
	{
		// Search for a new mode
		mode = GetFromTotalChances(RandomInt(1, m_TotalChances));
		
		if( !mode )
		{
			continue;
		}
		
		// Was last mode and not the one with the highest chance? I.E. First infection
		if( strcmp(mode->GetName(), "Single Infection") == 0 )
		{
			break;
		}
		else if( ZICore::m_LastMode != mode )
		{
			break;
		}
	} while( !mode->OnPreSelection() );
	
	if( !mode )
	{
		CONSOLE_DEBUGGER("Warning! no mode was chosen");
	}
		
	return mode;
}

void ZIRoundMode::Start(ZIRoundMode *mode, ZIPlayer *target)
{
	if( !mode )
	{
		CONSOLE_DEBUGGER("Invalid round mode.");
		return;
	}
	
	if( ZICore::m_IsRoundEnd || ZICore::m_IsModeStarted )
	{
		CONSOLE_DEBUGGER("A mode is already on progress or the round is ending....");
		return;
	}

	// This needs to be done here rather than OnTimerEnd due to the fact that the timer is a repeating timer
	// Stoping the sound every second of the countdown will not let the sound play at all so better do it after we're finished
	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || player->m_IsBot )
		{
			continue;
		}

		// Stop any sound caused by the countdown
		player->StopSound();
	}

	// Stop it from ticking
	RELEASE_TIMER(ZICore::m_pCountdownTimer);
	RELEASE_TIMER(ZICore::m_pStartModeTimer);
	RELEASE_TIMER(ZICore::m_pAmbientSoundTimer);

	// Flag these here in order for the first infection/disinfection to occure
	ZICore::m_IsModeStarted = false;
	ZICore::m_CurrentMode = mode;
	ZICore::m_LastMode = mode;
	ZICore::m_Countdown = -1;

	m_RoundTarget = target ? target : ZIPlayer::RandomAlive();	
	
	if( !m_RoundTarget )
	{
		CONSOLE_DEBUGGER("Warning! no target was found");
	}	

	// Start the mode after a short delay (fixes the HUD not appearing)
	ZICore::m_pStartModeTimer = timersys->CreateTimer(&ZITimersCallback::m_StartMode, 0.5f, mode, TIMER_FLAG_NO_MAPCHANGE);
}

ZIRoundMode *ZIRoundMode::GetFromTotalChances(int chance)
{
	// Way out of bounds
	if( chance > m_TotalChances )
	{
		return nullptr;
	}
	
	ZIRoundMode *mode = nullptr;

	int chances = 0;

	for( auto iterator = g_pRoundModes.begin(); iterator != g_pRoundModes.end(); iterator++ )
	{
		mode = *iterator;

		if( !mode )
		{
			continue;
		}

		chances += mode->GetChance();

		if( chance <= chances )
		{
			return mode;
		}
	}

	return nullptr;
}

int ZIRoundMode::GetIndex()
{
	return m_Index;
}

const char *ZIRoundMode::GetSound()
{
	return nullptr;
}

AmbientSoundInfo *ZIRoundMode::GetAmbientSound()
{
	static AmbientSoundInfo ambientSound;

	switch( RandomInt(1, 2) )
	{
	case 1:
		ambientSound = { "ZombieInfestation/round_ambient_infection1.mp3", 75.0f };
		break;

	case 2:
		ambientSound = { "ZombieInfestation/round_ambient_infection2.mp3", 283.0f };
		break;
	}

	return &ambientSound;
}

bool ZIRoundMode::OnPreSelection()
{
	return true;
}
