#include "zi_players.h"
#include "zi_geoip.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"
#include "zi_round_modes.h"
#include "zi_round_mode_single_infection.h"
#include "zi_weapons.h"
#include "zi_nades.h"
#include "zi_items.h"
#include "zi_resources.h"
#include "zi_timers.h"
#include "zi_menus.h"

int ZIPlayer::m_BloodSprayModelIndex = -1;
int ZIPlayer::m_BloodDropModelIndex = -1;

void ZIPlayer::Precache()
{
	m_BloodSprayModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/bloodspray.vmt", true);
	m_BloodDropModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/effects/blood_drop.vmt", true);
}

ZIPlayer *ZIPlayer::Find(const char *name)
{
	if( !name )
	{
		return nullptr;
	}

	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return nullptr;
	}

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || strcmp(player->m_Name, name) != 0 )
		{
			continue;
		}

		return player;
	}

	return nullptr;
}

ZIPlayer *ZIPlayer::Find(int id, bool userId)
{
	int size = ZICore::m_pOnlinePlayers.size();

	if( size < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return nullptr;
	}
	
	if( userId )
	{
		ZIPlayer *player = nullptr;
		
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || player->m_UserId != id )
			{
				continue;
			}

			return player;			
		}
	}
	else if( id > 0 && id <= g_pExtension->m_pGlobals->maxClients && id <= size )
	{
		return ZICore::m_pOnlinePlayers[id - 1];
	}

	return nullptr;
}

ZIPlayer *ZIPlayer::Find(edict_t *client)
{
	if( !client )
	{
		return nullptr;
	}

	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return nullptr;
	}

	ZIPlayer *player = nullptr;
	
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || player->m_pEdict != client )
		{
			continue;
		}

		return player;
	}

	return nullptr;
}

ZIPlayer *ZIPlayer::Find(BasePlayer *clientEntity)
{
	if( !clientEntity )
	{
		return nullptr;
	}

	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return nullptr;
	}

	ZIPlayer *player = nullptr;
	
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || player->m_pEntity != clientEntity )
		{
			continue;
		}

		return player;
	}

	return nullptr;
}

void ZIPlayer::RandomizeTeams()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return;
	}

	int team = CSGO_TEAM_NONE, TsCount = 0, CTsCount = 0;
	
	ZIPlayer *player = nullptr;
	BasePlayer *playerEnt = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		playerEnt = player->m_pEntity;
		team = playerEnt->GetTeam();

		// Spectators are out of the count
		if( team != CSGO_TEAM_T && team != CSGO_TEAM_CT )
		{
			continue;
		}

		// +60ish % are Ts
		if( TsCount > 1.5f * CTsCount )
		{
			team = CSGO_TEAM_CT;
		}
		// 60ish % are CTs
		else if( CTsCount > 1.5f * TsCount )
		{
			team = CSGO_TEAM_T;
		}
		else 
		{	
			team = RandomInt(0, 1) == 0 ? CSGO_TEAM_T : CSGO_TEAM_CT;
		}	

		if( team == CSGO_TEAM_T )
		{
			TsCount++;
		}
		else
		{
			CTsCount++;
		}

		playerEnt->SetTeam(team);
	}
}

int ZIPlayer::ConnectedCount(bool includeBots)
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int connectedCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || (player->m_IsBot && !includeBots) )
		{
			continue;
		}

		connectedCount++;		
	}

	return connectedCount;
}

int ZIPlayer::TsCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int TsCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || player->m_IsAlive || player->m_pEntity->GetTeam() != CSGO_TEAM_T )
		{
			continue;
		}

		TsCount++;
	}

	return TsCount;
}

int ZIPlayer::CTsCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int CTsCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || player->m_IsAlive || player->m_pEntity->GetTeam() != CSGO_TEAM_CT )
		{
			continue;
		}

		CTsCount++;
	}

	return CTsCount;
}

int ZIPlayer::PlayingCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int team = 0, playingCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		team = player->m_pEntity->GetTeam();

		if( team == CSGO_TEAM_T || team == CSGO_TEAM_CT )
		{
			playingCount++;
		}
	}

	return playingCount;
}

int ZIPlayer::AliveCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int aliveCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive )
		{
			continue;
		}		

		aliveCount++;		
	}

	return aliveCount;
}

int ZIPlayer::HumansCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int humansCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || player->m_IsInfected )
		{
			continue;
		}		

		humansCount++;		
	}

	return humansCount;
}

int ZIPlayer::SurvivorsCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int survivorsCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || player->m_IsInfected || !GET_SURVIVOR(player) )
		{
			continue;
		}

		survivorsCount++;
	}

	return survivorsCount;
}

int ZIPlayer::SnipersCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int snipersCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || player->m_IsInfected || !GET_SNIPER(player) )
		{
			continue;
		}

		snipersCount++;
	}

	return snipersCount;
}

int ZIPlayer::ZombiesCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int zombiesCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || !player->m_IsInfected )
		{
			continue;
		}		

		zombiesCount++;		
	}

	return zombiesCount;
}

int ZIPlayer::NemesisCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int nemesisCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || !player->m_IsInfected || !GET_NEMESIS(player) )
		{
			continue;
		}

		nemesisCount++;
	}

	return nemesisCount;
}

int ZIPlayer::AssassinsCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return -1;
	}

	int assassinsCount = 0;

	ZIPlayer *player = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player || !player->m_IsAlive || !player->m_IsInfected || !GET_ASSASSIN(player) )
		{
			continue;
		}

		assassinsCount++;
	}

	return assassinsCount;
}

ZIPlayer *ZIPlayer::RandomAlive()
{
	int size = ZICore::m_pOnlinePlayers.size();
	
	if( size < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return nullptr;
	}
	
	int random = -1;
	
	do 
	{
		random = RandomInt(0, AliveCount() - 1);
	} while( random >= size );

	int loops = 0;

	ZIPlayer *player = nullptr;
	
	while( !((player = ZICore::m_pOnlinePlayers[random]) && player->m_IsAlive) )
	{
		if( ++loops == size )
		{
			CONSOLE_DEBUGGER("Unable to find a suitable player after looping through all players.");
			return nullptr;
		}

		if( random == size )
		{
			random = 0;
		}

		random++;			
	}

	return player;
}

void ZIPlayer::Initiate()
{
	m_pGamePlayer = playerhelpers->GetGamePlayer(m_pEdict);

	if( !m_pGamePlayer )
	{
		CONSOLE_DEBUGGER("CS:GO is not supported!");
		return;
	}

	m_UserId = m_pGamePlayer->GetUserId();
	m_IsBot = m_pGamePlayer->IsFakeClient();

	ke::SafeStrcpy(m_Name, 32, m_pGamePlayer->GetName());
	ke::SafeStrcpy(m_SteamId, 32, m_pGamePlayer->GetAuthString());
	ke::SafeStrcpy(m_IP, 32, m_pGamePlayer->GetIPAddress());

	ke::SafeSprintf(m_Health, 20, "0");
	ke::SafeSprintf(m_Armor, 20, "0");

	GeographicalLookup();

	m_IsVIP = CheckVIPStatus();
	m_AllowTeamChoosing = true;
	m_IsAlive = false;
	m_IsInfected = false;
	m_IsFirstZombie = false;
	m_IsLastHuman = false;
	m_IsLastZombie = false;
	m_IsFrozen = false;
	m_IsProtected = false;

	m_Points = 5;

	m_pDamageDealtToZombies = new float[g_pExtension->m_pGlobals->maxClients];

	m_CanGetPrimaryWeapon = false;
	m_CanGetSecondaryWeapon = false;
	m_HasInfiniteClip = false;	
	
	m_NextLeapTime = -1.0f;
	m_LastLeapTime = -1.0f;
	
	m_pInfector = nullptr;
	m_pDisinfector = nullptr;
	
	m_pHumanLike = nullptr;
	m_pLastHumanLike = nullptr;
	
	m_pChoosenHumanClass = nullptr;
	m_pChoosenZombieClass = nullptr;

	m_pStatsTimer = m_IsBot ? nullptr : timersys->CreateTimer(&ZICore::m_TimersCallback.m_Stats, 0.2f, this, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
	m_pSetModelTimer = nullptr;
	m_pRemoveProtectionTimer = nullptr;
//	m_pRespawnTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_Respawn, 3.0f, this, TIMER_FLAG_NO_MAPCHANGE);					// Check if we can respawn him
	m_pRespawnTimer = nullptr;
	m_pZombieGrowlTimer = nullptr;
	m_pZombieBleedTimer = nullptr;
	m_pZombieUnfreezeTimer = nullptr;

	// Items
	m_pTripminePlantingTimer = nullptr;
	m_pTripmineTakingTimer = nullptr;
	m_pZombieCooldownTimer = nullptr;

	m_pMainMenu = nullptr;
	m_pWeaponsMenu = nullptr;
	m_pItemsMenu = nullptr;
	m_pHumanSelectionMenu = nullptr;	
	m_pZombieSelectionMenu = nullptr;	
	
	m_pGlowEntity = nullptr;

	// Need not be destroyed at disconnect, these are automatically destroyed by the game dll
	m_pFirstViewModel = nullptr;
	m_pSecondViewModel = nullptr;

	// Items-related
	m_HasTripmine = false;

	m_HasJetpack = false;	
	m_JetpackFuel = 0;
	m_NextRocketTime = -1.0f;

	m_IsEnraged = false;	

/* It's a TODO, now its fucking out rounds, needs a workaround
	// Just before we push the pointer, we check if the round needs a restart (to start a mode)
	if( ZIPlayer::PlayingCount() == 1 )
	{
		gamehelpers->ServerCommand("mp_restartgame 10\n");
	}	
*/	
	if( ZICore::m_pOnlinePlayers.size() < (size_t) m_Index )
	{
		ZICore::m_pOnlinePlayers.push_back(this);
	}
	else
	{
		ZICore::m_pOnlinePlayers[m_Index - 1] = this;
	}
	
}

void ZIPlayer::GeographicalLookup()
{
	if( m_IsBot )
	{
		return;
	}

	if( !ZIGeoIP::GetCountry(m_IP, m_Country, sizeof(m_Country)) )
	{
		ke::SafeSprintf(m_Country, sizeof(m_Country), "Unknown");
	}

	if( !ZIGeoIP::GetCity(m_IP, m_City, sizeof(m_City)) )
	{
		ke::SafeSprintf(m_City, sizeof(m_City), "Unknown");
	}
}

bool ZIPlayer::CheckVIPStatus()
{
/*	AdminId identifier = m_pGamePlayer->GetAdminId();

	if( identifier == INVALID_ADMIN_ID )
	{
		return false;
	}

	
	// TODO: work this out
	return (adminsys->GetAdminFlags(identifier, Access_Real) & ADMFLAG_CUSTOM1);*/

	return true;
}

const char *ZIPlayer::GetIP()
{
	int portPos = FindCharInString(m_IP, ':', true);

	if( portPos > 0 )
	{
		m_IP[portPos] = '\0';
	}

	return m_IP;
}

ZIPlayer::ZIPlayer(edict_t *edict)
{
	if( !edict || edict->IsFree() )
	{
		return;
	}

	m_pEdict = edict;
	m_Index = gamehelpers->IndexOfEdict(edict);
	m_pEntity = (BasePlayer *) g_pExtension->m_pServerGameEntities->EdictToBaseEntity(m_pEdict);
	
	Initiate();	

	CONSOLE_DEBUGGER("Creating player %s, id=%d, size: %d", m_Name, m_Index, ZICore::m_pOnlinePlayers.size());
}

ZIPlayer::~ZIPlayer()
{
	CONSOLE_DEBUGGER("Removing player %s, id=%d, size: %d", m_Name, m_Index, ZICore::m_pOnlinePlayers.size());
	
	delete[] m_pDamageDealtToZombies; m_pDamageDealtToZombies = nullptr;
	
	RELEASE_TIMER(m_pStatsTimer);
	RELEASE_TIMER(m_pSetModelTimer);
	RELEASE_TIMER(m_pRemoveProtectionTimer);
	RELEASE_TIMER(m_pRespawnTimer);
	RELEASE_TIMER(m_pZombieGrowlTimer);
	RELEASE_TIMER(m_pZombieBleedTimer);
	RELEASE_TIMER(m_pZombieUnfreezeTimer);

	RELEASE_TIMER(m_pTripminePlantingTimer);
	RELEASE_TIMER(m_pTripmineTakingTimer);
	RELEASE_TIMER(m_pZombieCooldownTimer);

	RELEASE_MENU(m_pMainMenu);
	CLOSE_MENU(m_pWeaponsMenu);
	RELEASE_MENU(m_pItemsMenu);
	RELEASE_MENU(m_pHumanSelectionMenu);
	RELEASE_MENU(m_pZombieSelectionMenu);

	if( m_pGlowEntity )
	{
		m_pGlowEntity->AcceptInput("Kill"); m_pGlowEntity = nullptr;
	}

	// We are not concerned in player counts anymore...
	ZICore::m_pOnlinePlayers[m_Index - 1] = nullptr;

	static char buffer[256];

	ZIPlayer *target = nullptr;
	
	if( ZICore::m_IsModeStarted && m_pHumanLike  )
	{
		float oldHealthRatio = 0.0f;

		if( m_IsInfected && m_IsLastZombie && HumansCount() > 1 )
		{
			m_IsLastZombie = false;

			// Add some health up, for a faire gameplay
			if( m_pEntity->GetHealth() / m_pHumanLike->GetHealth() < 0.25f  )
			{
				oldHealthRatio = 0.5f;
			}

			while( (target = RandomAlive()) )
			{
				if( target == this )
				{
					continue;
				}

				target->m_IsLastZombie = true;

				// Infect him
				target->Infect(nullptr, GET_ZOMBIE(this) ? nullptr : (ZIInfected *) m_pHumanLike);

				ke::SafeSprintf(buffer, sizeof(buffer), "The last zombie has left,\x04 %s\x01 is the new zombie.", target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					target = *iterator;

					if( !target )
					{
						continue;
					}

					UM_SayText(&target->m_Index, 1, 0, true, buffer);
				}

				break;
			}
		}
		else if( m_IsLastHuman && ZombiesCount() > 1 )
		{
			m_IsLastHuman = false;

			// Add some health up, for a faire gameplay
			if( m_pEntity->GetHealth() / m_pHumanLike->GetHealth() < 0.25f )
			{
				oldHealthRatio = 0.5f;
			}

			while( (target = RandomAlive()) )
			{
				if( target == this )
				{
					continue;
				}

				target->m_IsLastHuman = true;

				// Disinfect him
				target->Disinfect(nullptr, GET_SOLDIER(this) ? nullptr : m_pHumanLike);

				ke::SafeSprintf(buffer, sizeof(buffer), "The last human has left,\x04 %s\x01 is the new human.", target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					target = *iterator;

					if( !target )
					{
						continue;
					}

					UM_SayText(&target->m_Index, 1, 0, true, buffer);
				}

				break;
			}			
		}

		if( oldHealthRatio > 0.0f )
		{
			target->m_pEntity->SetHealth(target->m_pHumanLike->GetHealth() * oldHealthRatio);
		}
	}
	else
	{
/*	GetClientTeam is breaking here
		int team = GetClientTeam(m_pGamePlayer);

		if( team == CSGO_TEAM_T && TsCount() == 1 && CTsCount() > 1 )
		{
			while( (target = RandomAlive()) )
			{
				if( target == this )
				{
					continue;
				}

				SetClientTeam(m_pEntity, CSGO_TEAM_T);
				break;
			}
		}
		else if( team == CSGO_TEAM_CT && CTsCount() == 1 && TsCount() > 1 )
		{
			while( (target = RandomAlive()) )
			{
				if( target == this )
				{
					continue;
				}

				SetClientTeam(m_pEntity, CSGO_TEAM_CT);
				break;
			}
		}

		ZICore::m_CurrentMode = ZIRoundMode::Choose();
*/
	}

	UpdateCount();	
}

bool ZIPlayer::operator == (const ZIPlayer &otherPlayer)
{
	return m_Index == otherPlayer.m_Index || m_UserId == otherPlayer.m_UserId || m_pEdict == otherPlayer.m_pEdict || m_pEntity == otherPlayer.m_pEntity;
}

bool ZIPlayer::Infect(ZIPlayer *attacker, ZIInfected *zclass)
{
	int zombiesCount = ZombiesCount();

	if( !ZICore::OnPreClientInfection(this, attacker, zclass == &g_Nemesis, zclass == &g_Assassin) && zombiesCount > 0 )
	{
		return false;
	}
	
	// Release any previous task
	RELEASE_TIMER(m_pZombieGrowlTimer);
	RELEASE_TIMER(m_pZombieBleedTimer);
	
	// Release opened menus
	RELEASE_MENU(m_pMainMenu);
	CLOSE_MENU(m_pWeaponsMenu); 
	RELEASE_MENU(m_pItemsMenu);
	RELEASE_MENU(m_pHumanSelectionMenu);
	RELEASE_MENU(m_pZombieSelectionMenu);

	if( m_pGlowEntity )
	{
		m_pGlowEntity->AcceptInput("Kill"); m_pGlowEntity = nullptr;
	}

	static char buffer[256];
	ZIPlayer *player = nullptr;
	
	if( attacker )
	{
		// Fake death message
		IGameEvent *deathEvent = g_pExtension->m_pGameEventManager->CreateEvent("player_death");

		if( deathEvent )
		{
			deathEvent->SetInt("userid", m_UserId);
			deathEvent->SetInt("attacker", attacker == this ? m_UserId : attacker->m_UserId);
			deathEvent->SetString("weapon", "prop_exploding_barrel");
			deathEvent->SetBool("headshot", false);

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator < ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player )
				{
					continue;
				}

				// Sending it directly to the server causes some round management issues
				// Events if at all were needed to be sent to the clients, it should ALWAYS be done this way
				// Otherwise, your event is gonna do more than you think it does, and will probably mess up with other data
				FireEventToClient(player->m_Index, deathEvent);
			}
		}

		// Zombies should get some reward for infecting, unlike humans...
		if( attacker != this )
		{
			attacker->m_Points += 2;

			static char buffer2[256];
			ke::SafeSprintf(buffer2, sizeof(buffer2), "You got\x04 2 extra points\x01 for infecting\x03 %s\x01.", m_Name);
			UM_SayText(&attacker->m_Index, 1, 0, true, buffer2);

			ke::SafeSprintf(buffer, sizeof(buffer), "%s was infected by %s...", m_Name, attacker->m_Name);
		}
		else
		{
			ke::SafeSprintf(buffer, sizeof(buffer), "%s injected himself with T-virus...", m_Name);
		}
	}
	else
	{
		ke::SafeSprintf(buffer, sizeof(buffer), "%s was infected...", m_Name);
	}

	if( ZICore::m_IsModeStarted )
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			if( !player->m_IsBot )
			{
				UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_InfectionUpdatesHud), HUD_INFECTION_UPDATES_X, HUD_INFECTION_UPDATES_Y, Color(255, 20, 20, 100), Color(255, 20, 20, 100), 0, 5.0f, 1.0f, 1.0f, 0.0f, buffer);
			}
		}
	}

	if( !m_pChoosenZombieClass )
	{
		m_pChoosenZombieClass = g_pZombieClasses[RandomInt(0, g_pZombieClasses.size() - 1)];

		if( !m_IsBot )
		{
			while( !m_IsVIP && m_pChoosenZombieClass->IsVIP() )
			{
				m_pChoosenZombieClass = g_pZombieClasses[RandomInt(0, g_pZombieClasses.size() - 1)];
			}

			ShowZombieSelectionMenu();
		}
	}

	// Right before we set the new m_pHumanLike, we store the old ones
	ZIZombie *zombie = GET_ZOMBIE(this);
	ZISoldier *soldier = GET_SOLDIER(this);	

	m_pHumanLike = zclass ? zclass : m_pChoosenZombieClass;
	
	// Only apply attributes if we have a valid zombie class pointer, to prevent server from crashing etc...
	if( m_pHumanLike )
	{
		m_pEntity->SetHealth(m_pHumanLike->GetHealth());
		m_pEntity->SetArmor(0);
		m_pEntity->SetSpeed(m_pHumanLike->GetSpeed());
		m_pEntity->SetGravity(m_pHumanLike->GetGravity());

		m_pSetModelTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_SetModel, 0.1f, this, TIMER_FLAG_NO_MAPCHANGE);

		// Infection sound (to prevent infection sounds from playing at round end)
		if( !ZICore::m_IsRoundEnd && !GET_NEMESIS(this) && !GET_ASSASSIN(this) )
		{
			if( !zombie )
			{
				// If no previous infection was found, our only chance is to get the sound from a normal infection
				zombie = GET_ZOMBIE(this);
			}	

			char *sound = nullptr;
		
			if( soldier && RandomInt(0, 3) == 0 )
			{
				sound = (char *) soldier->GetInfectionSound();
			}
			else
			{
				sound = (char *) zombie->GetInfectionSound();
			}		

			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, m_Index, CHAN_VOICE, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);		

			m_pZombieGrowlTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_ZombieGrowl, RandomFloat(40.0f, 70.0f), this, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
			m_pZombieBleedTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_ZombieBleed, 1.0f, this, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);	
		}		
	}	

	int maxClients = g_pExtension->m_pGlobals->maxClients;

	for( int i = 0; i < maxClients; i++ )
	{
		m_pDamageDealtToZombies[i] = 0.0f;		
	}

	if( m_pEntity->GetTeam() != CSGO_TEAM_T || ZICore::m_pStartModeTimer )
	{
		m_pEntity->SetTeam(CSGO_TEAM_T);
	}

	if( !m_IsBot ) //__BotSupport__
	{
		m_pEntity->SetFOV(ZOMBIE_FOV);
		m_pEntity->SetFlashLight(false);
		m_pEntity->SetNightVision(true, "ZombieInfestation/effects/zombie_vision.vmt");
	}
	
	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_PRIMARY);
	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_SECONDARY);
	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_NADES);
	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_C4);
		
	// Instantely set it as the active weapon
	m_pEntity->SelectItem("weapon_knife", 0);	

	// Throw some effects into the mix
	UM_ScreenFade(&m_Index, 1, RandomInt(1000, 1300), 0, FFADE_IN, Color(255, 0, 0, 200));
	UM_ScreenShake(&m_Index, 1, SHAKE_START, RandomFloat(10.0f, 15.0f), RandomFloat(1.0f, 5.0f), RandomFloat(3.0f, 5.0f));
	
	// Successful infection!
	m_IsInfected = true;
	m_IsFirstZombie = zombiesCount == 0 ? true : false;
	m_IsLastHuman = false;
	m_IsProtected = false;
	m_pLastHumanLike = m_pHumanLike;

	if( m_IsFirstZombie && ZICore::m_CurrentMode == &g_SingleInfectionMode )
	{
		m_pEntity->SetHealth(m_pHumanLike->GetHealth() * 3.0f);
	}

	m_CanGetPrimaryWeapon = false;
	m_CanGetSecondaryWeapon = false;
	
	m_pInfector = attacker;
	m_HasInfiniteClip = false;
	m_NextLeapTime = g_pExtension->m_pGlobals->curtime;

/*
	int health = GetClientHealth(m_pEntity), armor = GetClientArmor(m_pEntity);

	AddCommas(health > 0 ? health : 0, player->m_Health, 20);
	AddCommas(armor > 0 ? armor : 0, player->m_Armor, 20);
*/
	// Update player count
	UpdateCount();	

	// Effects
	CreateParticleSystem(m_pEntity->GetOrigin(), m_pEntity->GetAngles(), "gas_cannister_idle_billow", m_pEntity, "primary", 3.0f);		
	
	ZICore::OnPostClientInfection(this, attacker, zclass == &g_Nemesis, zclass == &g_Assassin);	
	
	return true;
}

bool ZIPlayer::Disinfect(ZIPlayer *attacker, ZIHumanoid *hclass)
{
	ZIInfected *infected = dynamic_cast<ZIInfected *> (hclass);

	if( hclass && infected )
	{
		CONSOLE_DEBUGGER("Trying to disinfect player with an infected class.");
		return false;
	}

	if( !ZICore::OnPreClientDisinfection(this, attacker, hclass == &g_Survivor, hclass == &g_Sniper) && HumansCount() > 0 )
	{
		return false;
	}

	// Release any previous task
	RELEASE_TIMER(m_pSetModelTimer);
	RELEASE_TIMER(m_pZombieGrowlTimer);
	RELEASE_TIMER(m_pZombieBleedTimer);
	RELEASE_TIMER(m_pZombieUnfreezeTimer);
	
	// Release opened menus
	RELEASE_MENU(m_pMainMenu);
	CLOSE_MENU(m_pWeaponsMenu);
	RELEASE_MENU(m_pItemsMenu);
	RELEASE_MENU(m_pHumanSelectionMenu);
	RELEASE_MENU(m_pZombieSelectionMenu);

	if( m_pGlowEntity )
	{
		m_pGlowEntity->AcceptInput("Kill"); m_pGlowEntity = nullptr;
	}
	
	static char buffer[256];
	ZIPlayer *player = nullptr;

	if( attacker )
	{
		// Fake death message
		IGameEvent *deathEvent = g_pExtension->m_pGameEventManager->CreateEvent("player_death");

		if( deathEvent )
		{
			deathEvent->SetInt("userid", m_UserId);
			deathEvent->SetInt("attacker", attacker == this ? m_UserId : attacker->m_UserId);
			deathEvent->SetString("weapon", "healthshot");
			deathEvent->SetBool("headshot", false);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator < ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player )
				{
					continue;
				}

				// Sending it directly to the server causes some round management issues
				// Events if at all were needed to be sent to the clients, it should ALWAYS be done this way
				// Otherwise, your event is gonna do more than you think it does, and will probably mess up with other data
				FireEventToClient(player->m_Index, deathEvent);
			}
		}

		// Humans should get some reward for vaccinating their friends
		if( attacker != this )
		{
			attacker->m_Points++;

			static char buffer2[256];
			ke::SafeSprintf(buffer2, sizeof(buffer2), "You got\x04 1 extra point\x01 for disinfecting\x03 %s\x01.", m_Name);
			UM_SayText(&attacker->m_Index, 1, 0, true, buffer2);

			ke::SafeSprintf(buffer, sizeof(buffer), "%s gave %s an antidote...", m_Name, attacker->m_Name);
		}
		else
		{
			ke::SafeSprintf(buffer, sizeof(buffer), "%s has used an antidote...", m_Name);
		}
	}
	else
	{
		ke::SafeSprintf(buffer, sizeof(buffer), "%s was cured...", m_Name);
	}

	if( ZICore::m_IsModeStarted )
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			if( !player->m_IsBot )
			{
				UM_HudText(&player->m_Index, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_InfectionUpdatesHud), HUD_INFECTION_UPDATES_X, HUD_INFECTION_UPDATES_Y, Color(20, 155, 255, 100), Color(20, 155, 255, 100), 0, 5.0f, 1.0f, 1.0f, 0.0f, buffer);
			}
		}
	}

	if( !m_pChoosenHumanClass )
	{
		m_pChoosenHumanClass = g_pHumanClasses[RandomInt(0, g_pHumanClasses.size() - 1)];

		if( !m_IsBot )
		{
			while( !m_IsVIP && m_pChoosenHumanClass->IsVIP() )
			{
				m_pChoosenHumanClass = g_pHumanClasses[RandomInt(0, g_pHumanClasses.size() - 1)];
			}

			ShowHumanSelectionMenu();
		}
	}

	m_pHumanLike = hclass ? hclass : m_pChoosenHumanClass;	

	// Only apply attributes if we have a valid human class pointer, to prevent server from crashing etc...
	if( m_pHumanLike )
	{
		m_pEntity->SetHealth(m_pHumanLike->GetHealth());
		m_pEntity->SetSpeed(m_pHumanLike->GetSpeed());
		m_pEntity->SetGravity(m_pHumanLike->GetGravity());

		m_pSetModelTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_SetModel, 0.1f, this, TIMER_FLAG_NO_MAPCHANGE);
		
		if( GET_SURVIVOR(this) )
		{
			m_pEntity->SetArmor(0);

			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_PRIMARY);
			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_SECONDARY);
			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_NADES);
	
			m_pEntity->GiveItem("weapon_xm1014");
			m_pEntity->GiveItem("weapon_hegrenade");

			m_HasInfiniteClip = true;
		}
		else if( GET_SNIPER(this) )
		{
			m_pEntity->SetArmor(0);

			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_PRIMARY);
			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_SECONDARY);
			m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_NADES);
	
			m_pEntity->GiveItem("weapon_awp");
			m_pEntity->GiveItem("weapon_flashbang");
		}
		// Disinfection sound
		else
		{
			ZISoldier *soldier = GET_SOLDIER(this);

			// Check if he changed the class, if he did then his armor should be set to that classes armor
			// Players shouldn't cheat having armor by choosing classes with high armor and then switching to their superior class back
			if( m_pEntity->GetArmor() < soldier->GetArmor() || m_pHumanLike != m_pLastHumanLike )
			{
				m_pEntity->SetArmor(soldier->GetArmor());
			}
			
			if( ZICore::m_IsModeStarted )
			{
				const char *sound = soldier->GetDisinfectionSound();

				CellRecipientFilter filter;				
				g_pExtension->m_pEngineSound->EmitSound(filter, m_Index, CHAN_ITEM, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);	
			}

			// If by any chance he had an infinite clip last time he was human, give him some clip to remember the name
			if( m_HasInfiniteClip )
			{
				int weaponSlot = -1;

				BaseWeapon *weaponEnt = nullptr;

				for( int weapon = 0; weapon < CSGO_MAX_WEAPONS; weapon++ )
				{
					if( !(weaponEnt = m_pEntity->GetWeapons(weapon)) )
					{
						continue;
					}

					weaponSlot = GetItemSlot(weaponEnt->GetClassname());

					if( weaponSlot != CSGO_WEAPON_SLOT_PRIMARY && weaponSlot != CSGO_WEAPON_SLOT_SECONDARY )
					{
						continue;
					}

					weaponEnt->SetClip(999);
				}
			}

			m_HasInfiniteClip = false;
		}
	}

	int maxClients = g_pExtension->m_pGlobals->maxClients;

	for( int i = 0; i < maxClients; i++ )
	{
		m_pDamageDealtToZombies[i] = 0.0f;
	}

	if( ZICore::m_IsModeStarted && m_pEntity->GetTeam() != CSGO_TEAM_CT || ZICore::m_pStartModeTimer )
	{
		m_pEntity->SetTeam(CSGO_TEAM_CT);
	}

	if( !m_IsBot ) //__BotSupport__
	{
		m_pEntity->SetFOV(HUMAN_FOV);
		m_pEntity->SetFlashLight(false);
		m_pEntity->SetNightVision(false);
	}

	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_NADES);
	m_pEntity->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_C4);

	// If everything went as promised, then we return these
	m_IsInfected = false;
	m_IsFirstZombie = false;	
	m_IsLastZombie = false;
	m_IsProtected = false;
	m_pLastHumanLike = m_pHumanLike;	

	m_CanGetPrimaryWeapon = true;
	m_CanGetSecondaryWeapon = true;
	
	if( !m_IsBot )
	{
		ShowWeaponsMenu(CSGO_WEAPON_SLOT_PRIMARY);
	}

	m_pDisinfector = attacker;	
	m_NextLeapTime = g_pExtension->m_pGlobals->curtime;	
/*
	int health = GetClientHealth(m_pEntity), armor = GetClientArmor(m_pEntity);

	AddCommas(health > 0 ? health : 0, player->m_Health, 20);
	AddCommas(armor > 0 ? armor : 0, player->m_Armor, 20);
*/	
	// Update player count
	UpdateCount();	

	// Effects
	CreateParticleSystem(m_pEntity->GetOrigin(), m_pEntity->GetAngles(), "ambient_smokestack_noise", m_pEntity, "primary", 3.0f);	

	ZICore::OnPostClientDisinfection(this, attacker, hclass == &g_Survivor, hclass = &g_Sniper);	

	return true;
}

void ZIPlayer::Gib(Vector force)
{
	CellRecipientFilter soundFilter;
	g_pExtension->m_pEngineSound->EmitSound(soundFilter, m_Index, CHAN_BODY, "ZombieInfestation/player_gibbed.mp3", -1, "ZombieInfestation/player_gibbed.mp3", RandomFloat(0.2f, 0.6f)/*VOL_NORM*/, ATTN_NORM, 0);

	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/pgib_p1.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/pgib_p2.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/pgib_p3.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/pgib_p4.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/pgib_p5.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/hgibs_jaw.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/hgibs_scapula.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/hgibs_scapula.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p1.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p2.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p3.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p4.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p5.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/rgib_p6.mdl", 5);
	SpawnSpecificGibs(m_pEntity, 1, RandomFloat(750.0f, 1200.0f), RandomFloat(1500.0f, 2200.0f), "models/gibs/gibhead.mdl", 5);

	// Make the corps invisible
	BaseEntity *ragdollEnt = m_pEntity->GetRagdoll();

	if( ragdollEnt )
	{
		m_pEntity->AcceptInput("Kill");
	}

//	Vector force = info.GetDamageForce();
//	VectorNormalize(force);

//	force *= RandomFloat(10.0f, 100.0f);

	// Blood
	CellRecipientFilter bloodFilter;
	TE_BloodSprite(bloodFilter, 0.0f, m_pEntity->GetOrigin(), force/*Vector(RandomFloat(-50.0f, 50.0f), RandomFloat(-50.0f, 50.0f), RandomFloat(0.0f, 100.0f))*/, Color(255, 0, 0, 200), ZIPlayer::m_BloodSprayModelIndex, ZIPlayer::m_BloodDropModelIndex, 150);
}

bool ZIPlayer::Freeze(float duration)
{
	if( !m_IsAlive || m_IsFrozen )
	{
		return false;
	}

	m_IsFrozen = true;

	m_pEntity->SetMoveType(MOVETYPE_NONE);
/*
	// Freeze icon?
	if( get_pcvar_num(cvar_grenade_frost_hudicon) )
	{
		message_begin(MSG_ONE_UNRELIABLE, g_MsgDamage, _, victim)
		write_byte(0) // damage save
		write_byte(0) // damage take
		write_long(DMG_DROWN) // damage type - DMG_FREEZE
		write_coord(0) // x
		write_coord(0) // y
		write_coord(0) // z
		message_end()
	}	
*/
	CellRecipientFilter filter;
	g_pExtension->m_pEngineSound->EmitSound(filter, m_Index, CHAN_BODY, "ZombieInfestation/zombie_frozen1.mp3", -1, "ZombieInfestation/zombie_frozen1.mp3", VOL_NORM, ATTN_NORM, 0);

	// Some blue screen	
	UM_ScreenFade(&m_Index, 1, 0, 0, FFADE_STAYOUT, Color(0, 100, 200, 100));

	// Some glow
	m_pGlowEntity = CreateEntityGlow(m_pEntity, 1, Color(0, 100, 200, 100), "primary");

	// Create a new one 
	m_pZombieUnfreezeTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_ZombieUnfreeze, duration, this, TIMER_FLAG_NO_MAPCHANGE);

	return true;
}

void ZIPlayer::Unfreeze()
{
	if( !m_IsAlive || !m_IsFrozen )
	{
		return;
	}

	m_IsFrozen = false;

	// Remove any previous attempts to unfreeze the player
	RELEASE_TIMER(m_pZombieUnfreezeTimer);

	// Set speed to null
	m_pEntity->SetMoveType(MOVETYPE_WALK);

	// Remove the blue screen
	UM_ScreenFade(&m_Index, 1, 1000, 0, FFADE_IN, Color(0, 100, 200, 100));

	// Remove the glow
	if( m_pGlowEntity )
	{
		m_pGlowEntity->AcceptInput("Kill"); m_pGlowEntity = nullptr;
	}
	
	CellRecipientFilter filter;
	g_pExtension->m_pEngineSound->EmitSound(filter, m_Index, CHAN_BODY, "ZombieInfestation/zombie_unfrozen1.mp3", -1, "ZombieInfestation/zombie_unfrozen1.mp3", VOL_NORM, ATTN_NORM, 0);	

	// Break glass
	Vector pos = m_pEntity->GetOrigin();
	pos.z += 25.0f;

	TE_BreakModel(filter, 0.0f, pos, QAngle(0.0f, 0.0f, 0.0f), Vector(20.0f, 20.0f, 20.0f), Vector(RandomFloat(-50.0f, 50.0f), RandomFloat(-50.0f, 50.0f), 25.0f), ZINades::m_GlassModelIndex[RandomInt(0, ARRAY_SIZE(ZINades::m_GlassModelIndex) - 1)], 10, 10, 25.0f, BREAK_GLASS);
}

void ZIPlayer::ClearEffects()
{
	// Start the search from our entity
	BaseEntity *effectEnt = m_pEntity;

	while( (effectEnt = BaseEntity::FindEntity(effectEnt, "info_particle_system")) )
	{
		if( effectEnt->GetParent() != m_pEntity )
		{
			continue;
		}

		effectEnt->AcceptInput("Kill");
	}
}

void ZIPlayer::ShowMainMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		return;
	}

	RELEASE_MENU(m_pMainMenu);
	m_pMainMenu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_Main);

	if( m_pMainMenu )
	{
		m_pMainMenu->SetDefaultTitle("Zombie Infestation Menu");

		// Weapons
		m_pMainMenu->AppendItem("", ItemDrawInfo("Weapons", (m_CanGetPrimaryWeapon || m_CanGetSecondaryWeapon) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED));

		// Extra items
		m_pMainMenu->AppendItem("", ItemDrawInfo("Extra Items", ITEMDRAW_DEFAULT));

		// Human classes
		m_pMainMenu->AppendItem("", ItemDrawInfo("Human Classes", ITEMDRAW_DEFAULT));

		// Zombie classes
		m_pMainMenu->AppendItem("", ItemDrawInfo("Zombie Classes", ITEMDRAW_DEFAULT));		

		// Teleport
		m_pMainMenu->AppendItem("", ItemDrawInfo("Teleport (Only if stuck)", (!m_IsAlive || m_pEntity->CanGetInSpace(m_pEntity->GetOrigin(), m_pEntity->GetFlags() & FL_DUCKING ? true : false)) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT));

		// Join Spectator/Team
		m_pMainMenu->AppendItem("", ItemDrawInfo(m_AllowTeamChoosing ? "Join the game" : "Spectate", ITEMDRAW_DEFAULT));

		m_pMainMenu->SetMenuOptionFlags(m_pMainMenu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
		m_pMainMenu->Display(m_Index, MENU_TIME_FOREVER);
	}
}

void ZIPlayer::ShowWeaponsMenu(int slot)
{
	if( slot != CSGO_WEAPON_SLOT_PRIMARY && slot != CSGO_WEAPON_SLOT_SECONDARY )
	{
		return;
	}

	// Infected or not a regular human?
	if( !m_IsAlive || m_IsInfected || !GET_SOLDIER(this) )
	{
		return;
	}

	CLOSE_MENU(m_pWeaponsMenu);

	if( slot == CSGO_WEAPON_SLOT_PRIMARY )
	{
		m_pWeaponsMenu = ZICore::m_pPrimaryWeaponsMenu;
	}
	else
	{
		m_pWeaponsMenu = ZICore::m_pSecondaryWeaponsMenu;
	}

	if( m_pWeaponsMenu )
	{		
		m_pWeaponsMenu->Display(m_Index, MENU_TIME_FOREVER);
	}
}

void ZIPlayer::ShowItemsMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		return;
	}

	if( !m_IsAlive )
	{
		UM_SayText(&m_Index, 1, 0, true, "You have to be\x04 alive\x01.");
		return;
	}

	if( g_pExtraItems.size() < 1 )
	{
		UM_SayText(&m_Index, 1, 0, true, "This class doesn't have any items.");
		return;
	}	

	RELEASE_MENU(m_pItemsMenu);
	m_pItemsMenu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_Items);

	if( m_pItemsMenu )
	{
		m_pItemsMenu->SetDefaultTitle("Extra Items");

		static char buffer[128];

		ZIItem *item = nullptr;
		ItemReturn itemReturn = ItemReturn_Show;

		for( auto iterator = g_pExtraItems.begin(); iterator != g_pExtraItems.end(); iterator++ )
		{
			item = *iterator;

			if( !item )
			{
				continue;
			}

			// Calling this here before item's AdditionalInfo so it can update it
			itemReturn = item->OnPreSelection(this);

			if( itemReturn == ItemReturn_DontShow )
			{
				continue;
			}

			const char *additionalInfo = item->AdditionalInfo();

			if( item->IsVIP() )
			{
				if( additionalInfo && *additionalInfo )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s [%s] [%d Points] (VIP only)", item->GetName(), additionalInfo, item->GetCost());
				}
				else
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s [%d Points] (VIP only)", item->GetName(), item->GetCost());
				}

				m_pItemsMenu->AppendItem("", ItemDrawInfo(buffer, m_IsVIP && itemReturn == ItemReturn_Show ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED));
			}
			else
			{
				if( additionalInfo && *additionalInfo )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s [%s] [%d Points]", item->GetName(), additionalInfo, item->GetCost());
				}
				else
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "%s [%d Points]", item->GetName(), item->GetCost());
				}	

				m_pItemsMenu->AppendItem("", ItemDrawInfo(buffer, itemReturn == ItemReturn_Show ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED));			
			}
		}
		
		m_pItemsMenu->SetMenuOptionFlags(m_pItemsMenu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT | MENUFLAG_BUTTON_EXITBACK);
		m_pItemsMenu->Display(m_Index, MENU_TIME_FOREVER);
	}
}

void ZIPlayer::ShowHumanSelectionMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		CONSOLE_DEBUGGER("No style was found");
		return;
	}

	if( g_pHumanClasses.size() < 1 )
	{
		UM_SayText(&m_Index, 1, 0, true, "No registered human classes to show.");
		return;
	}

	RELEASE_MENU(m_pHumanSelectionMenu);
	m_pHumanSelectionMenu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_HumanSelection);

	if( m_pHumanSelectionMenu )
	{
		m_pHumanSelectionMenu->SetDefaultTitle("Human Selection");

		static char buffer[128];

		ZISoldier *soldier = nullptr;

		for( auto iterator = g_pHumanClasses.begin(); iterator != g_pHumanClasses.end(); iterator++ )
		{
			soldier = *iterator;

			if( !soldier )
			{
				continue;
			}

			if( soldier->IsVIP() )
			{
				ke::SafeSprintf(buffer, sizeof(buffer), "%s [%s] (VIP only)", soldier->GetName(), soldier->GetDescription());
				m_pHumanSelectionMenu->AppendItem("", ItemDrawInfo(buffer, ITEMDRAW_DISABLED));
			}
			else
			{
				ke::SafeSprintf(buffer, sizeof(buffer), "%s [%s]", soldier->GetName(), soldier->GetDescription());
				m_pHumanSelectionMenu->AppendItem("", ItemDrawInfo(buffer, ITEMDRAW_DEFAULT));
			}
		}

		m_pHumanSelectionMenu->SetMenuOptionFlags(m_pHumanSelectionMenu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT | MENUFLAG_BUTTON_EXITBACK);
		m_pHumanSelectionMenu->Display(m_Index, MENU_TIME_FOREVER);
	}	
}

void ZIPlayer::ShowZombieSelectionMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		return;
	}

	if( g_pZombieClasses.size() < 1 )
	{
		UM_SayText(&m_Index, 1, 0, true, "No registered zombie classes to show.");
		return;
	}

	RELEASE_MENU(m_pZombieSelectionMenu);
	m_pZombieSelectionMenu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_ZombieSelection);

	if( m_pZombieSelectionMenu )
	{
		m_pZombieSelectionMenu->SetDefaultTitle("Zombie Selection");

		static char buffer[128];

		ZIZombie *zombie = nullptr;

		for( auto iterator = g_pZombieClasses.begin(); iterator != g_pZombieClasses.end(); iterator++ )
		{
			zombie = *iterator;

			if( !zombie )
			{
				continue;
			}

			if( zombie->IsVIP() )
			{
				ke::SafeSprintf(buffer, sizeof(buffer),  "%s [%s] (VIP only)", zombie->GetName(), zombie->GetDescription());
				m_pZombieSelectionMenu->AppendItem("", ItemDrawInfo(buffer, ITEMDRAW_DISABLED));
			}
			else 
			{
				ke::SafeSprintf(buffer, sizeof(buffer), "%s [%s]", zombie->GetName(), zombie->GetDescription());
				m_pZombieSelectionMenu->AppendItem("", ItemDrawInfo(buffer, ITEMDRAW_DEFAULT));
			}			
		}

		m_pZombieSelectionMenu->SetMenuOptionFlags(m_pZombieSelectionMenu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT | MENUFLAG_BUTTON_EXITBACK);
		m_pZombieSelectionMenu->Display(m_Index, MENU_TIME_FOREVER);
	}
}

void ZIPlayer::PlaySound(const char *sound, ...)
{
	if( m_IsBot )
	{
		return;
	}

	// Stop any previously played sound
	StopSound();

	// Play the new one
	static char buffer[256];

	va_list variables;
	va_start(variables, sound);
	ke::SafeVsprintf(buffer, sizeof(buffer), sound, variables);
	va_end(variables);

	g_pExtension->m_pEngineServer->ClientCommand(m_pEdict, "play %s", buffer);
}

void ZIPlayer::StopSound()
{
	if( m_IsBot )
	{
		return;
	}

	g_pExtension->m_pEngineServer->ClientCommand(m_pEdict, "stopsound");
}

void ZIPlayer::UpdateCount()
{
	if( ZICore::m_pOnlinePlayers.size() < 1 )
	{
		CONSOLE_DEBUGGER("No players are created.");
		return;
	}	

	static bool calledOnLastHuman = false, calledOnLastZombie = false;

	int iterator = 0;

	ZIPlayer *player = nullptr;

	if( HumansCount() == 1 )
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || !player->m_IsAlive || player->m_IsInfected )
			{
				continue;
			}

			player->m_IsLastHuman = true;

			if( !calledOnLastHuman )
			{
				ZICore::OnClientLastHuman(player); // This is used for sub plugins

				calledOnLastHuman = true;			
			}
		}
	}
	else
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || !player->m_IsAlive || player->m_IsInfected )
			{
				continue;
			}

			player->m_IsLastHuman = false;		
		}

		calledOnLastHuman = false;
	}

	if( ZombiesCount() == 1 )
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || !player->m_IsAlive || !player->m_IsInfected )
			{
				continue;
			}			

			player->m_IsLastZombie = true;

			if( !calledOnLastZombie )
			{
				ZICore::OnClientLastZombie(player);

				calledOnLastZombie = true;
			}
		}
	}
	else
	{
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player || !player->m_IsAlive || !player->m_IsInfected )
			{
				continue;
			}

			player->m_IsLastZombie = false;		
		}

		calledOnLastZombie = false;
	}
}