#include "zi_entry.h"
#include "zi_geoip.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"
#include "zi_item_tripmine.h"
#include "zi_item_jetpack_bazooka.h"
#include "zi_item_infection_bomb.h"
#include "zi_round_modes.h"
#include "zi_round_mode_nemesis.h"
#include "zi_round_mode_assassin.h"
#include "zi_round_mode_plague.h"
#include "zi_timers.h"
#include "zi_environment.h"
#include "zi_weapons.h"
#include "zi_nades.h"
#include "zi_commands.h"
#include "zi_spawns_manager.h"
#include "zi_sourcemod_bridge.h"

ConVar g_BulletTimeActivationInterval("zi_bullet_time_activation_interval", "1.5"); // RandomFloat(1.0, 2.0)
ConVar g_BulletTimeActivationRate("zi_bullet_time_activation_rate", "3.0");
ConVar g_BulletTimeSpeed("zi_bullet_time_speed", "0.25");
ConVar g_BulletTimeDuration("zi_bullet_time_duration", "5.0");

ZombieInfestation g_ZombieInfestation;

const char *ZombieInfestation::GetName() const
{
	return "Zombie Infestation";
}

const char *ZombieInfestation::GetDescription() const
{
	return "A strong comeback of the renowned classical Zombie Mod from CS 1.6";
}

const char *ZombieInfestation::GetVersion() const
{
	return "0.0.1.0a";
}

const char *ZombieInfestation::GetTag() const
{
	return "ZI";
}

IGameConfig *ZombieInfestation::GetConfig() const
{
	return m_pConfig;
}

bool ZombieInfestation::OnMetamodLoad(ISmmAPI *ismm, char *error, unsigned int maxlength, bool late)
{
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "round_start", true);
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "round_end", true);
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "game_start", true);
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "player_death", true);
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "player_team", true);
	g_pExtension->m_pGameEventManager->AddListener(g_pExtension, "player_changename", true);
	
	return true;
}

bool ZombieInfestation::OnMetamodUnload(char *error, unsigned int maxlength)
{
	return true;
}

bool ZombieInfestation::OnLoad(char *error, unsigned int maxlength, bool late)
{
	char buffer[256];
	LOOKUP_FOR_CONFIG(m_pConfig, "ZombieInfestation.games");
/*
	ConVarRef sv_sendtables("sv_sendtables");
	sv_sendtables.SetValue(1);

	// Fix for some hud limits
	int m_iBits = 0;

	if( m_pConfig->GetOffset("CSendProp::m_nBits", &m_iBits) && m_iBits > 0 )
	{
		void *m_iHealth = nullptr;

		if( m_pConfig->GetAddress("CBasePlayer::m_iHealth", &m_iHealth) && m_iHealth )
		{
			*(int *) ((char *) m_iHealth + m_iBits) = 32;
		}
		else
		{
			CONSOLE_DEBUGGER("Couldn't patch m_iHealth");
		}

		void *m_iClip1 = nullptr;

		if( m_pConfig->GetAddress("CBaseCombatWeapon::m_iClip1", &m_iClip1) && m_iClip1 )
		{
			*(int *) ((char *) m_iClip1 + m_iBits) = 16;
		}
		else
		{
			CONSOLE_DEBUGGER("Couldn't patch m_iClip1");
		}

		void *m_iPrimaryReserveAmmoCount = nullptr;

		if( m_pConfig->GetAddress("CBaseCombatWeapon::m_iPrimaryReserveAmmoCount", &m_iPrimaryReserveAmmoCount) && m_iPrimaryReserveAmmoCount )
		{
			*(int *) ((char *) m_iPrimaryReserveAmmoCount + m_iBits) = 16;
		}
		else
		{
			CONSOLE_DEBUGGER("Couldn't patch m_iPrimaryReserveAmmoCount");
		}			
	}
	else
	{
		CONSOLE_DEBUGGER("Failed to find SendProp's m_nBits");
	}
*/
	return true;
}

void ZombieInfestation::OnAllLoaded()
{
	// Load GeoIP database
	ZIGeoIP::Load();

	// Load hooks
	ZIHooks::AttachToServer();	
	
	// Register Zombie Classes (OnMapStart because of precached models...)
	ZICore::OnLoad();
	
	// SourceMod bridge to other plugins using the SourcePawn language
	ZISourceModBridge::Load();	

	g_TripmineItem.OnLoad();
}

void ZombieInfestation::OnUnload()
{
	g_TripmineItem.OnUnload();

	// Free all other SourcePawn plugin-related
	ZISourceModBridge::Free();
	
	ZICore::OnUnload();
	
	// Bye hooks
	ZIHooks::Release();	

	// Free GeoIP database
	ZIGeoIP::Free();	
}

void ZombieInfestation::OnCoreMapStart(edict_t *edictList, int edictCount, int clientMax)
{
	// Player materials
	ZIPlayer::Precache();

	// Nades materials
	ZINades::Precache();

	// Pre-cache resources
	ZIResources::Load();
	
	// Setup the apocalyptic environment
	ZIEnvironment::Setup();	

	// Get map spawns
	ZISpawnsManager::Load();	

	gamehelpers->ServerCommand("sv_cheats 1\n");
	gamehelpers->ServerCommand("sv_airaccelerate 1000\n");
	gamehelpers->ServerCommand("sv_clamp_unsafe_velocities 0\n");
	gamehelpers->ServerCommand("mp_freezetime 0\n");
	gamehelpers->ServerCommand("mp_autoteambalance 0;mp_limitteams 0\n");
	gamehelpers->ServerCommand("mp_do_warmup_period 0\n");
	gamehelpers->ServerCommand("mp_warmuptime 0\n");
	gamehelpers->ServerCommand("mp_playercashawards 0;mp_teamcashawards 0\n"); // hides the money hud
	gamehelpers->ServerCommand("mp_buy_anywhere 0;mp_buytime 0;mp_startmoney 9999\n");

	// Items
	g_TripmineItem.Precache();
	g_JetpackBazookaItem.Precache();
	
	/*
		TODO: Add an external CFG file

		* If this doesn't work, try to setup a 1-2 secs timer and see if it does.
	*/	

	ZICore::m_IsRoundEnd = true;
}

void ZombieInfestation::OnCoreMapEnd()
{
	// Free the memory used by certain systems (TODO: see where else we can free these, there may be some better calls aka forwards where we can use them instead)
	// Release here is needed because we never know the maxclients of the next map!

	ZIResources::Free();

	// Free map spawns
	ZISpawnsManager::Free();	

	// Free player-related stuff

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

		if( player->m_IsFrozen )
		{
			player->Unfreeze();
		}

		playerEnt->SetFOV(HUMAN_FOV);
		playerEnt->SetFlashLight(false);
		playerEnt->SetNightVision(false);

		// Remove previous timers, if any...
		RELEASE_TIMER(player->m_pRemoveProtectionTimer);
		RELEASE_TIMER(player->m_pZombieGrowlTimer);
		RELEASE_TIMER(player->m_pZombieBleedTimer);
		RELEASE_TIMER(player->m_pZombieUnfreezeTimer);

		// Close any opened menus
		RELEASE_MENU(player->m_pMainMenu);
		CLOSE_MENU(player->m_pWeaponsMenu);
		RELEASE_MENU(player->m_pItemsMenu);
		RELEASE_MENU(player->m_pHumanSelectionMenu);
		RELEASE_MENU(player->m_pZombieSelectionMenu);
	}

	// Forgot about the past
	ZICore::m_LastMode = nullptr;
}

void ZombieInfestation::OnClientConnected(int client)
{
	IGamePlayer *gameplayer = playerhelpers->GetGamePlayer(client);

	if( !gameplayer )
	{
		return;
	}

	edict_t *clientEdict = gameplayer->GetEdict();

	// Clients arent bots, I guess...
	if( clientEdict )
	{
		g_pExtension->m_pEngineServer->ClientCommand(clientEdict, "play %s", ZIResources::RandomLoadingSound());
	}
}

void ZombieInfestation::OnClientPutInServer(IGamePlayer *gameplayer)
{
	ZIPlayer *player = new ZIPlayer(gameplayer->GetEdict());

	if( player )
	{
		ZIHooks::AttachToClient(player->m_pEntity);	
	}
	else
	{
		CONSOLE_DEBUGGER("Couldn't register edict: %d", gameplayer->GetEdict());
	}	
}

void ZombieInfestation::OnClientSettingsChanged(int client)
{
}

void ZombieInfestation::OnClientDisconnected(int client)
{
	ZIPlayer *player = ZIPlayer::Find(client, false);

	if( player )
	{
		delete player;
	}
}

void ZombieInfestation::OnEntityCreated(BaseEntity *entity, const char *classname)
{
	ZIEnvironment::RemoveUndesirableEnts(entity, classname);

	if( strncmp(classname, "weapon_", 7) == 0 )
	{
		ZIHooks::AttachToWeapon((BaseWeapon *) entity);		
	}
	else if( strstr(classname, "_projectile") )
	{
		static char targetname[32];
		entity->GetKeyValue("targetname", targetname, sizeof(targetname));

		if( strcmp(targetname, "") == 0 )
		{
			ZIHooks::OnPostProjectileCreation((BaseGrenade *) entity, classname);
		}
	}

	// Items
	g_TripmineItem.OnPostEntityCreation(entity, classname);
}

void ZombieInfestation::OnEntityDestroyed(BaseEntity *entity)
{
}

void ZombieInfestation::OnPreFireEvent(IGameEvent *event, bool &returnValue, META_RES &metaResult)
{
	const char *eventName = event->GetName();

	if( !eventName )
	{
		return;
	}
	
	REGISTER_PRE_EVENT("round_end", OnPreRoundEndEvent);
	REGISTER_PRE_EVENT("player_death", OnPreClientDeathEvent);
	REGISTER_PRE_EVENT("player_team", OnPreClientChangeTeamEvent);
}

void ZombieInfestation::OnPostFireEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();
	
	if( !eventName )
	{
		return;
	}

	REGISTER_POST_EVENT("round_start", OnPostRoundStartEvent);
	REGISTER_POST_EVENT("game_start", OnPostGameStartEvent);	
	REGISTER_POST_EVENT("player_changename", OnPostClientChangeNameEvent);	
}

bool ZombieInfestation::OnPreEntityDestruction(BaseEntity *entity, const char *classname)
{
	// Remove any hooks
	EntityHook *hook = nullptr;

	for( auto iterator = EntityHook::totalHooks.begin(); iterator != EntityHook::totalHooks.end(); iterator++ )
	{
		hook = *iterator;

		if( !hook )
		{
			continue;
		}

		// Break the entire hook, only and only when the last hooked entity is destroyed
		if( strcmp(hook->GetClassname(), classname) == 0 && hook->GetHookedEntitiesCount() == 1 )
		{
			EntityHook::totalHooks.remove(hook);
			delete hook;
			break;
		}
	}

	if( strstr(classname, "_projectile") )
	{
		static char targetname[32];
		entity->GetKeyValue("targetname", targetname, sizeof(targetname));

		if( strcmp(targetname, "") == 0 )
		{
			return ZIHooks::OnPreProjectileDestruction((BaseGrenade *) entity, classname);
		}
	}

	return true;
}

bool ZombieInfestation::OnPreTerminateRound(float &delay, CSGORoundEndReason &reason)
{
	// A game restart ignores all of this trouble, no need for that..
	if( reason == CSGORoundEnd_GameStart )
	{
		ZICore::m_Winner = RoundModeWinner_Unknown;

		delay = 20.0f;

		float estimatedWarningDelay = delay - 8.0f;			// 8.0f is the length of warning_death.mp3

		if( estimatedWarningDelay > 0.0f )
		{
			ZICore::m_pWarningTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_Warning, estimatedWarningDelay, nullptr, TIMER_FLAG_NO_MAPCHANGE);
		}		
	}
	else
	{
		// Humans won
		if( ZIPlayer::HumansCount() > 0 )
		{
			reason = CSGORoundEnd_CTWin;
			ZICore::m_Winner = RoundModeWinner_Humans;
			ZICore::m_Score[RoundModeWinner_Humans]++;
		}
		// Zombies won
		else if( ZIPlayer::ZombiesCount() > 0 )
		{
			reason = CSGORoundEnd_TWin;
			ZICore::m_Winner = RoundModeWinner_Zombies;
			ZICore::m_Score[RoundModeWinner_Zombies]++;
		}
		// We simply dont know
		else
		{
			reason = CSGORoundEnd_Draw;
			ZICore::m_Winner = RoundModeWinner_Unknown;
		}
	}

	ZICore::m_IsRoundEnd = true;
	ZICore::m_IsModeStarted = false;
	ZICore::m_CurrentMode = nullptr;
	ZICore::m_Countdown = -1;

	RELEASE_TIMER(ZICore::m_pCountdownTimer);
	RELEASE_TIMER(ZICore::m_pStartModeTimer);
	RELEASE_TIMER(ZICore::m_pAmbientSoundTimer);

	char buffer[256];

	ZIPlayer *player = nullptr;
	BasePlayer *playerEnt = nullptr;

	// Before anything else, we wanna make sure nobody can see the "Terrorist or Counter-Terrorist" Vgui msg, for that we'll have to hide every hud element
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		playerEnt = player->m_pEntity;
//		playerEnt->SetHideHUD(playerEnt->GetHideHUD() | HIDEHUD_ALL);
	}

	const char *sound = nullptr;
	int *playerIndex = nullptr;

	switch( ZICore::m_Winner )
	{
	case RoundModeWinner_Humans:
		sound = ZIResources::RandomRoundEndSound(RoundEndSound_Humans);

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			playerIndex = &player->m_Index;

			if( !player->m_IsBot )
			{
				player->PlaySound(sound);
				UM_HudText(playerIndex, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(20, 20, 255, 100), Color(20, 20, 255, 100), 0, 2.0f, 1.0f, 3.0f, 0.0f, "Humans have defeated the plague!");
			}

			// Reward them alive players
			if( player->m_IsAlive && !player->m_IsInfected )
			{
				player->m_Points += 5;

				if( !player->m_IsBot )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "You got\x04 5 extra points\x01 for winning as a\x03 Human\x01.");					
					UM_SayText(playerIndex, 1, 0, true, buffer);
				}
			}
		}

		break;

	case RoundModeWinner_Zombies:
		sound = ZIResources::RandomRoundEndSound(RoundEndSound_Zombies);

		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			playerIndex = &player->m_Index;

			if( !player->m_IsBot )
			{
				player->PlaySound(sound);
				UM_HudText(playerIndex, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(255, 20, 20, 100), Color(255, 20, 20, 100), 0, 2.0f, 1.0f, 3.0f, 0.0f, "Zombies have taken over the world!");
			}

			// Reward them alive players
			if( player->m_IsAlive && player->m_IsInfected )
			{
				player->m_Points += 2;

				if( !player->m_IsBot )
				{
					ke::SafeSprintf(buffer, sizeof(buffer), "You got\x04 2 extra points\x01 for winning as a\x03 Zombie\x01.");
					UM_SayText(playerIndex, 1, 0, true, buffer);
				}
			}
		}

		break;

	default:
		for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
		{
			player = *iterator;

			if( !player )
			{
				continue;
			}

			playerIndex = &player->m_Index;

			if( !player->m_IsBot )
			{
				UM_HudText(playerIndex, 1, HudManager::AutoSelectChannel(player->m_Index, &ZICore::m_RoundStateHud), HUD_ROUND_STATUS_X, HUD_ROUND_STATUS_Y, Color(20, 255, 20, 100), Color(20, 255, 20, 100), 0, 2.0f, 1.0f, 3.0f, 0.0f, "No one won...");
			}
		}
	}

	ZICore::OnRoundModeEnd();
	return true;
}

void ZombieInfestation::OnPostTerminateRound(float delay, CSGORoundEndReason reason)
{
	delay -= 0.1;

	// Make a timer to randomize the teams and reset humans/zombies count right at the end of the round
	ZICore::m_pTeamsRandomization = timersys->CreateTimer(&ZICore::m_TimersCallback.m_TeamsRandomization, delay, nullptr, TIMER_FLAG_NO_MAPCHANGE);

	ZIPlayer *player = nullptr;

	// By round-end every entity is destroyed and cleared from the map, so we set the glow entity to nullptr, otherwise we'll be trying to delete an entity that doesn't exist on play spawn
	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		player->m_pGlowEntity = nullptr;
	}
}

void ZombieInfestation::OnPostRoundStartEvent(IGameEvent *event)
{
	// Remove any of the latest tasks
	RELEASE_TIMER(ZICore::m_pInfo);
	RELEASE_TIMER(ZICore::m_pWarningTimer);
	RELEASE_TIMER(ZICore::m_pTeamsRandomization);

	ZIPlayer *player = nullptr;
	BasePlayer *playerEnt = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		// Reset HUD
		playerEnt = player->m_pEntity;
//		playerEnt->SetHideHUD(playerEnt->GetHideHUD() & ~HIDEHUD_ALL);
	}

	ZICore::m_IsModeStarted = false;
	ZICore::m_IsRoundEnd = false;	
	ZICore::m_CurrentMode = nullptr;
	
	ZICore::m_CurrentMode = ZIRoundMode::Choose();
	ZICore::m_Countdown = -1;
	ZICore::m_Winner = RoundModeWinner_Unknown;

	ZICore::m_pInfo = timersys->CreateTimer(&ZICore::m_TimersCallback.m_Info, 2.0f, nullptr, TIMER_FLAG_NO_MAPCHANGE);
	ZICore::m_pCountdownTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_Countdown, RandomFloat(2.0f, 3.0f), nullptr, TIMER_FLAG_NO_MAPCHANGE);

	// Items
	g_TripmineItem.OnPostRoundStart();
	g_InfectionBombItem.OnPostRoundStart();
}

void ZombieInfestation::OnPostGameStartEvent(IGameEvent *event)
{
}

bool ZombieInfestation::OnPreRoundEndEvent(IGameEvent *event, META_RES &metaResult)
{
	metaResult = MRES_SUPERCEDE;
	return false;
}

bool ZombieInfestation::OnPreClientDeathEvent(IGameEvent *event, META_RES &metaResult)
{
	ZIPlayer *attacker = ZIPlayer::Find(event->GetInt("attacker"), true);

	if( !attacker || !attacker->m_IsInfected )
	{
		return false;
	}

//	event->SetString("weapon", "prop_exploding_barrel");
	event->SetBool("headshot", false);		

	metaResult = MRES_HANDLED;
	return true;
}

bool ZombieInfestation::OnPreClientChangeTeamEvent(IGameEvent *event, META_RES &metaResult)
{
	ZIPlayer *player = ZIPlayer::Find(event->GetInt("userid"), true);

	if( !player )
	{
		return true;
	}

	if( player->m_AllowTeamChoosing )
	{
		player->m_AllowTeamChoosing = false;
		return true;
	}

	metaResult = MRES_SUPERCEDE;
	return false;
}

void ZombieInfestation::OnPostClientChangeNameEvent(IGameEvent *event)
{
	ZIPlayer *player = ZIPlayer::Find(event->GetInt("userid"), true);

	if( !player )
	{
		return;
	}

	ke::SafeStrcpy(player->m_Name, 32, event->GetString("newname"));
	CONSOLE_DEBUGGER("Player changed name: %s", player->m_Name);	
}

bool ZombieInfestation::OnPreClientCommand(edict_t *client, const CCommand &args)
{
	ZIPlayer *player = ZIPlayer::Find(client);

	if( !player )
	{
		return false;		
	}

	const char *command = args[0];

	if( strcmp(command, "+lookatweapon") == 0 )
	{
		return ZICommands::OnPreLookAtWeaponCommand(player, args);
	}
	else if( strcmp(command, "nightvision") == 0 )
	{
		return ZICommands::OnPreNightVisionCommand(player, args);
	}
	else if( strcmp(command, "drop") == 0 )
	{
		return ZICommands::OnPreDropCommand(player, args);
	}
	else if( strcmp(command, "buy") == 0 || strcmp(command, "rebuy") == 0 || strcmp(command, "autobuy") == 0 )
	{
		return ZICommands::OnPreBuyCommand(player, args);
	}	
	
	return true;	
}

void ZombieInfestation::OnPostClientCommand(edict_t *client, const CCommand &args)
{
	ZIPlayer *player = ZIPlayer::Find(client);

	if( !player )
	{
		return;
	}
	
	g_TripmineItem.OnPostClientCommand(player, args);
}

void ZombieInfestation::OnPostClientSpawn(ZIPlayer *player)
{
	BasePlayer *playerEnt = player->m_pEntity;
	int team = playerEnt->GetTeam();

	if( team != CSGO_TEAM_T && team != CSGO_TEAM_CT )
	{
		return;
	}

	player->m_IsAlive = true;
	player->m_IsLastHuman = false;
	player->m_IsLastZombie = false;
	player->m_pLastHumanLike = nullptr;

	// We create the second viewmodel because it doesn't exist at first!
//	playerEnt->CreateViewModel(1);

	// Now, we simply store the addresses, since they're not going to change until client disconnects anyways.
//	player->m_pFirstViewModel = playerEnt->GetViewModel(0);
//	player->m_pSecondViewModel = playerEnt->GetViewModel(1);

	if( ZICore::m_IsModeStarted )
	{
		player->Infect();
	}
	else
	{
		player->Disinfect();
	}

	// Teleport him to a spawn point
	ZISpawnsManager::Teleport(player);		

	RELEASE_TIMER(player->m_pRespawnTimer);
	player->m_pRespawnTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_Respawn, 2.0f, player, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
}

void ZombieInfestation::OnPostClientThink(ZIPlayer *player)
{
	if( !player->m_IsAlive )
	{
		return;
	}	

	if( RandomInt(0, 10) != 0 )
	{
		return;
	}

	BasePlayer *playerEnt = player->m_pEntity;
	
	int health = playerEnt->GetHealth(), armor = playerEnt->GetArmor();

	AddCommas(health > 0 ? health : 0, player->m_Health, 20);
	AddCommas(armor > 0 ? armor : 0, player->m_Armor, 20);

	// Update gravity, ugly way to do it but the only one atm... TODO: find an alternative
	if( playerEnt->GetMoveType() != MOVETYPE_LADDER )
	{		
		playerEnt->SetGravity(player->m_pHumanLike->GetGravity());
	}	

	BaseWeapon *weaponEnt = playerEnt->GetActiveWeapon();

	if( weaponEnt )
	{
		if( !player->m_IsInfected )
		{
			ZIWeapon *weapon = ZIWeapon::Find(weaponEnt);

			if( !weapon )
			{
				return;
			}

			if( player->m_HasInfiniteClip )
			{
				weaponEnt->SetClip(weapon->GetClip());
			}

			playerEnt->SetWeaponAmmo(weaponEnt, weapon->GetAmmo());
		}		
	}	
}

void ZombieInfestation::OnPostClientRunCommand(ZIPlayer *player, CUserCmd *userCmd, IMoveHelper *moveHelper)
{
	if( !player->m_IsAlive )
	{
		return;
	}

	BasePlayer *playerEnt = player->m_pEntity;

	if( playerEnt->GetMoveType() != MOVETYPE_NONE )
	{
		Vector boostVelocity;
		int flags = playerEnt->GetFlags();

		if( player->m_IsInfected && GET_NEMESIS(player) )
		{
			float currentTime = g_pExtension->m_pGlobals->curtime;

			// Not there, yet.
			if( currentTime - player->m_NextLeapTime < 0.0f )
			{
				return;
			}			

			if( player->m_IsBot || ((userCmd->buttons & (IN_JUMP | IN_DUCK)) == (IN_JUMP | IN_DUCK)) )
			{
				if( VectorLength(playerEnt->GetVelocity()) < 80.0f || !(flags & FL_ONGROUND) )
				{
					return;
				}

				QAngle angles = playerEnt->GetEyeAngles();
				AngleVectors(angles, &boostVelocity);

				float force = g_NemesisLeapForce.GetFloat(), height = g_NemesisLeapHeight.GetFloat();

				// Leap may cause perturbations in the X-Y plane!
				boostVelocity.x *= force;
				boostVelocity.y *= force;
				boostVelocity.z = height;

				// Boost towards the specific direction
				playerEnt->Teleport(nullptr, nullptr, &boostVelocity);

				// Register the leap
				player->m_LastLeapTime = currentTime;
				player->m_NextLeapTime = currentTime + RandomFloat(0.1f, 0.3f);

				// Bots keep jumping since they really dont press jump keys.. so add an extra delay for them to tell them that they cant keep doing that
				if( player->m_IsBot )
				{
					// Make him look like he's ducking
					userCmd->buttons |= IN_DUCK;
					player->m_NextLeapTime += RandomFloat(2.0f, 5.0f);										
				}
			}
		}
		else if( ((userCmd->buttons & IN_USE) || (player->m_IsBot && RandomInt(0, 10) == 0)) && !(flags & FL_ONGROUND) )
		{
			boostVelocity = playerEnt->GetVelocity();

			// Going upwards? am I a joke to you?
			if( boostVelocity.z > 0.0f )
			{
				return;
			}

			// Safety safety
			boostVelocity.z = -100.0f;

			// Apply parachute
			playerEnt->Teleport(nullptr, nullptr, &boostVelocity);
		}
	}	
}

HookReturn ZombieInfestation::OnPreClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace)
{
	if( ZICore::m_IsRoundEnd || !ZICore::m_IsModeStarted )
	{
		HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
	}

	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( !attacker || player == attacker )
	{
		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	if( attacker->m_IsAlive && player->m_IsInfected == attacker->m_IsInfected )
	{
		HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
	}	

	HOOK_RETURN_VOID(MRES_IGNORED, false);
}

void ZombieInfestation::OnPostClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace)
{
	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( !attacker || player == attacker )
	{
		return;
	}

	// Human attacking zombie
	if( player->m_IsInfected && !attacker->m_IsInfected )
	{
/*
		// Not bullet damage
		if (!(damage_type & DMG_BULLET))
			return;

		// Knockback only if damage is done to victim
		if (damage <= 0.0 || GetHamReturnStatus() == HAM_SUPERCEDE || get_tr2(tracehandle, TR_pHit) != victim)
			return;

*/
		if( !trace->DidHit() || trace->m_pEnt != (CBaseEntity *) player->m_pEntity )
		{
			return;
		}

		ZIWeapon *weapon = ZIWeapon::FindActive(attacker);
		
		if( weapon )
		{
			if( !trace->DidHit() || trace->m_pEnt != (CBaseEntity *) player->m_pEntity )
			{
				return;
			}

			if( VectorLength(trace->endpos - trace->startpos) > g_ZombieKnockbackDistance.GetFloat() )
			{
				return;
			}

			float power = info.GetDamage() * weapon->GetKnockbackPower();

			if( !player->m_IsInfected || power <= 1.0f )
			{
				return;
			}

			ZISoldier *soldier = GET_SOLDIER(attacker);

			// Humans also have their own knockback power
			if( soldier && soldier->GetKnockbackPower() > 0.0f )
			{
				power *= soldier->GetKnockbackPower();
			}

			// The direction must be a normalized vector!!!
			Vector boostVelocity = direction;

			// Effects only infected players (Zombies and their Bosses)
			ZIInfected *infected = GET_INFECTED(player);

			float resistance = 0.0f;

			if( infected )
			{
				resistance = infected->GetKnockbackResistance();
			}

			// Zombie resistance
			if( resistance > 0.0f )
			{
				boostVelocity /= resistance;
			}

			BasePlayer *playerEnt = player->m_pEntity;
			int flags = playerEnt->GetFlags();

			// Check how hard we would push him, depending on him standing still or not.
			if( (flags & (FL_DUCKING | FL_ONGROUND)) == (FL_DUCKING | FL_ONGROUND) )
			{
				power *= g_ZombieKnockbackDistance.GetFloat();
			}

			// Applu x/y plane knockback
			boostVelocity.x *= power;
			boostVelocity.y *= power;
			
			// Boost toward the Z direction are not allowed!
			boostVelocity.z = 0.0f;

			// Add the client velocity
			boostVelocity += playerEnt->GetVelocity();

			// Before we push the client, we make sure he's not in air;	
			int onground = flags & FL_ONGROUND;

			if( onground != 0 )
			{
				flags &= ~FL_ONGROUND;
				playerEnt->SetFlags(flags);
			}

			// Apply the boost	
			playerEnt->Teleport(nullptr, nullptr, &boostVelocity);

			// Reset the fact that he may on groun
			flags |= onground;
			playerEnt->SetFlags(flags);
		}
	}

	// Zombie attacking human
	else if( attacker->m_IsInfected && !player->m_IsInfected )
	{
	}	
}

HookReturnValue<int> ZombieInfestation::OnPreClientTakeDamage(ZIPlayer *player, CTakeDamageInfo2 &info)
{
	if( ZICore::m_IsRoundEnd || !ZICore::m_IsModeStarted )
	{
		HOOK_RETURN_VALUE(MRES_SUPERCEDE, int, 1, false);
	}

	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( !attacker )
	{
		HOOK_RETURN_VALUE(MRES_IGNORED, int, 0, false);
	}

	BasePlayer *playerEnt = player->m_pEntity;
	
	ZIInfected *infected = GET_INFECTED(player);
	ZISurvivor *survivor = GET_SURVIVOR(attacker);
	ZISniper *sniper = GET_SNIPER(attacker);

	int damageType = info.GetDamageType();

	if( player->m_IsInfected && infected )
	{
		char *sound = nullptr;

		if( damageType & DMG_FALL )
		{
			sound = (char *) infected->GetFallSound();
		}
		else if( damageType & DMG_BURN )
		{
			if( !player->m_IsEnraged )
			{
				playerEnt->Extinguish();;
				playerEnt->Ignite(RandomFloat(4.0f, 6.0f));

				// Make sure we dont have a sound yet, since the burn can last even after the attacker have attacked the player...
				if( RandomInt(0, 10) == 0 )
				{
					sound = (char *) infected->GetBurnSound();
				}
			}
		}
		// The rest should be played now and then, since I can empty it entire clip on zombie in a matter of a second, biaatch
		else if( RandomInt(0, 2) == 0 )
		{
			sound = (char *) infected->GetPainSound();			
		}

		if( sound && *sound )
		{
			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, player->m_Index, CHAN_VOICE, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);
		}
	}

	// Zombie threw a nade
	else if( attacker->m_IsInfected )
	{
		if( damageType & DMG_BLAST )
		{
			HOOK_RETURN_VALUE(MRES_SUPERCEDE, int, 1, false);
		}
	}

	if( player == attacker )
	{
		if( player->m_IsInfected )
		{
			HOOK_RETURN_VALUE(MRES_IGNORED, int, 0, false);
		}

		if( damageType & (DMG_BURN | DMG_BLAST) )
		{
			// Allow all sorts of damage, except burn or blast damage...
			HOOK_RETURN_VALUE(MRES_SUPERCEDE, int, 1, false);
		}		
	}
	else if( attacker->m_IsAlive )
	{
		if( player->m_IsInfected == attacker->m_IsInfected || player->m_IsProtected )
		{
			HOOK_RETURN_VALUE(MRES_SUPERCEDE, int, 1, false);
		}

		float damage = 0.0f;

		// Human attacking zombie
		if( player->m_IsInfected && !attacker->m_IsInfected )
		{
			int playerId = player->m_Index - 1;

			if( survivor )
			{
				info.ScaleDamage(survivor->GetDamageMultiplier());
			}
			else if( sniper )
			{
				BaseWeapon *weaponEnt = attacker->m_pEntity->GetActiveWeapon();
								
				if( weaponEnt && strncmp(weaponEnt->GetClassname(), "weapon_awp", 10) == 0 )
				{
					info.SetDamage(sniper->GetDamage());
				}
			}
			// VIP players deal a little bit more damage than usual players
			else if( player->m_IsVIP )
			{
				info.ScaleDamage(1.2f);
			}

			damage = info.GetDamage();

			// Frozen zombies shouldn't take much damage
			if( player->m_IsFrozen && !survivor && !sniper )
			{
				attacker->m_pDamageDealtToZombies[playerId] += damage * RandomFloat(0.25f, 0.5f);
			}
			else
			{
				attacker->m_pDamageDealtToZombies[playerId] += damage;
			}

			int points = attacker->m_pDamageDealtToZombies[playerId] / 750.0f;

			if( points > 0 )
			{
				attacker->m_Points += points;
				attacker->m_pDamageDealtToZombies[playerId] -= points * 750.0f;				
			}	

			// Apply damage multipification
			HOOK_RETURN_VALUE(MRES_HANDLED, int, 1, true);
		}

		// Zombie attacking human
		else if( attacker->m_IsInfected && !player->m_IsInfected )
		{
			info.ScaleDamage(RandomFloat(0.5, 0.8));

			ZINemesis *nemesis = GET_NEMESIS(attacker);
			ZIAssassin *assassin = GET_ASSASSIN(attacker);

			if( nemesis )
			{
				BaseWeapon *weaponEnt = playerEnt->GetActiveWeapon();
				
				if( !weaponEnt || strncmp(weaponEnt->GetClassname(), "weapon_knife", 12) != 0 )
				{
					HOOK_RETURN_VALUE(MRES_IGNORED, int, 1, false);
				}

				info.SetDamage(nemesis->GetDamage());

				// The nemesis instantely kills its prey, it doesnt need any amor protection...
				HOOK_RETURN_VALUE(MRES_HANDLED, int, 1, true);
			}
			else if( assassin )
			{
				BaseWeapon *weaponEnt = playerEnt->GetActiveWeapon();
				
				if( !weaponEnt || strncmp(weaponEnt->GetClassname(), "weapon_knife", 12) != 0 )
				{
					HOOK_RETURN_VALUE(MRES_IGNORED, int, 1, false);
				}

				info.SetDamage(assassin->GetDamage());

				// The assassin instantely kills its prey, it doesnt need any amor protection...
				HOOK_RETURN_VALUE(MRES_HANDLED, int, 1, true);
			}

			survivor = GET_SURVIVOR(player);
			sniper = GET_SNIPER(player);
			
			// Survivors/Snipers dont have any armor, so any damage is delt to them, without the need of infecting them ofcourse
			if( ZIPlayer::HumansCount() > 1 && !survivor && !sniper )
			{
				int armor = playerEnt->GetArmor();
				
				if( armor > 0 )
				{
					CellRecipientFilter filter;
					g_pExtension->m_pEngineSound->EmitSound(filter, player->m_Index, CHAN_BODY, "ZombieInfestation/human_armor_damage.mp3", -1, "ZombieInfestation/human_armor_damage.mp3", VOL_NORM, ATTN_NORM, 0);

					damage = info.GetDamage();

					if( !ZICore::m_CurrentMode->IsInfectionAllowed() )
					{
						HOOK_RETURN_VALUE(MRES_IGNORED, int, 0, false);
					}					

					armor -= damage;

					// Show damage
					static char buffer[16];
					AddCommas((int) damage, buffer, sizeof(buffer));
					UM_HudText(&attacker->m_Index, 1, HudManager::AutoSelectChannel(attacker->m_Index, &ZICore::m_DamageHud), 0.5f, 0.4f, Color(200, 50, 0), Color(200, 0, 50), 0, 0.02f, 0.02f, 2.5f, 0.1f, buffer);

					playerEnt->SetArmor(armor > 0 ? armor : 0);
					playerEnt->SetVelocityModifier(0.5f);

					// Block the real damage		
					HOOK_RETURN_VALUE(MRES_SUPERCEDE, int, 1, false);					
				}
				else if( ZICore::m_CurrentMode->IsInfectionAllowed() )
				{
					player->Infect(attacker);
				}
			}
		}				
	}

	HOOK_RETURN_VALUE(MRES_IGNORED, int, 0, false);
}

void ZombieInfestation::OnPostClientTakeDamage(ZIPlayer *player, CTakeDamageInfo2 &info)
{	
	static char buffer[16];

	BasePlayer *playerEnt = player->m_pEntity;
	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( attacker )
	{
		// Human attacking zombie
		if( player->m_IsInfected && !attacker->m_IsInfected )
		{
			if( GET_ASSASSIN(player) || player->m_IsFirstZombie )
			{
				playerEnt->SetVelocityModifier(1.0f);
			}

			// Show damage			
			AddCommas((int) info.GetDamage(), buffer, sizeof(buffer));
			UM_HudText(&attacker->m_Index, 1, HudManager::AutoSelectChannel(attacker->m_Index, &ZICore::m_DamageHud), 0.5f, 0.4f, Color(0, 200, 50), Color(50, 200, 0), 0, 0.02f, 0.02f, 2.5f, 0.1f, buffer);
		}

		// Zombie attacking human
		else if( attacker->m_IsInfected && !player->m_IsInfected )
		{
			if( player->m_IsLastHuman )
			{
				playerEnt->SetVelocityModifier(1.0f);
			}
		}

		return;
	}

	BaseEntity *inflictor = (BaseEntity *) gamehelpers->ReferenceToEntity(info.GetInflictor());

	if( inflictor )
	{
		attacker = ZIPlayer::Find((BasePlayer *) inflictor->GetOwnerEntity());

		if( attacker )
		{
			// Show damage			
			AddCommas((int) info.GetDamage(), buffer, sizeof(buffer));
			UM_HudText(&attacker->m_Index, 1, HudManager::AutoSelectChannel(attacker->m_Index, &ZICore::m_DamageHud), 0.5f, 0.4f, Color(100, 200, 0), Color(0, 200, 100), 0, 0.02f, 0.02f, 2.5f, 0.1f, buffer);
		}
	}
}

bool ZombieInfestation::OnPreClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info)
{
	// The first viewmodel is hidden by the game dll, we need to do the same for the second.
/*	BaseViewModel *viewmodelEnt = player->m_pSecondViewModel;

	if( viewmodelEnt )
	{
		viewmodelEnt->SetFlags(viewmodelEnt->GetFlags() | EF_NODRAW);
		viewmodelEnt->UpdateTransmitState();
	}
*/
	BasePlayer *playerEnt = player->m_pEntity;

	// Remove any attached flames
	if( playerEnt->GetFlags() & FL_ONFIRE )
	{
		playerEnt->Extinguish(); CONSOLE_DEBUGGER("Removing the fiery...");
	}

	// Remove the freeze if he was frozen..
	if( player->m_IsFrozen )
	{
		player->Unfreeze();
	}

	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	ZINemesis *nemesis = attacker ? GET_NEMESIS(attacker) : nullptr;
	ZIAssassin *assassin = attacker ? GET_ASSASSIN(attacker) : nullptr;
	ZISurvivor *survivor = attacker ? GET_SURVIVOR(attacker) : nullptr;
	ZISniper *sniper = attacker ? GET_SNIPER(attacker) : nullptr;

	if( info.GetDamageType() & DMG_BLAST || ((playerEnt->GetHealth() - info.GetDamage()) < 0.0f && (nemesis || assassin || survivor || sniper || GET_NEMESIS(player) || GET_ASSASSIN(player))) )
	{
		player->Gib(info.GetDamageForce());
	}
	// Make it look like his soul is going up
	else if( !player->m_IsInfected )
	{
		BaseEntity *ragdollEnt = playerEnt->GetRagdoll();

		if( ragdollEnt )
		{
			// Prep the ragdoll for dissolving
			char targetname[32];
			ke::SafeSprintf(targetname, 32, "ragdoll_%d", player->m_Index);

			ragdollEnt->SetKeyValue("targetname", targetname);

			// Make it dissolve
			BaseEntity *dissolverEnt = BaseEntity::CreateEntity("env_entity_dissolver");

			if( dissolverEnt )
			{
				dissolverEnt->SetKeyValue("target", targetname);
				dissolverEnt->SetKeyValue("dissolvetype", "0");
				dissolverEnt->AcceptInput("Dissolve");
				dissolverEnt->AcceptInput("Kill");
			}
		}
	}

	// Allow death
	return true;
}

void ZombieInfestation::OnPostClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info)
{	
	BasePlayer *playerEnt = player->m_pEntity;

	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( attacker && player != attacker )
	{
		attacker->m_Points++;

		static char buffer[256];
		ke::SafeSprintf(buffer, sizeof(buffer), "You got\x04 1 extra point\x01 for killing\x03 %s\x01.", player->m_Name);

		UM_SayText(&attacker->m_Index, 1, 0, true, buffer);
	}	

// Reset them back to their ugly FOV __BotSupport__
	if( !player->m_IsBot )
	{
		playerEnt->SetFOV(ZOMBIE_FOV);
		playerEnt->SetFlashLight(false);
		playerEnt->SetNightVision(true, "ZombieInfestation/effects/zombie_vision.vmt");
	}

	// Remove previous timers, if any...
	RELEASE_TIMER(player->m_pRemoveProtectionTimer);
	RELEASE_TIMER(player->m_pZombieGrowlTimer);
	RELEASE_TIMER(player->m_pZombieBleedTimer);
	RELEASE_TIMER(player->m_pZombieUnfreezeTimer);

	// Close any opened menus to not confuse the player
	RELEASE_MENU(player->m_pMainMenu);
	CLOSE_MENU(player->m_pWeaponsMenu);
	RELEASE_MENU(player->m_pItemsMenu);
	RELEASE_MENU(player->m_pHumanSelectionMenu);
	RELEASE_MENU(player->m_pZombieSelectionMenu);

	BaseProp *glowEnt = player->m_pGlowEntity;

	if( glowEnt )
	{
		glowEnt->AcceptInput("Kill"); player->m_pGlowEntity = nullptr;
	}

	ZIInfected *infected = GET_INFECTED(player);

	if( player->m_IsInfected && infected )
	{
		// Play some sad music Alexa
		const char *sound = infected->GetDeathSound();

		CellRecipientFilter filter;
		g_pExtension->m_pEngineSound->EmitSound(filter, player->m_Index, CHAN_VOICE, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);
	}	

	if( !ZICore::m_IsModeStarted )
	{
		ZICore::m_CurrentMode = ZIRoundMode::Choose();		
	}

	// Bullet Time	
	static ConVarRef host_timescale("host_timescale");
	
	// If it's a boss, end any currently-playing bullet-time and start a new one, for the BOSS!
	if( GET_NEMESIS(player) || GET_ASSASSIN(player) )
	{
		RELEASE_TIMER(ZICore::m_pBulletTime);			
	}	

	// Only start bullet-time if it isn't running already
	if( !ZICore::m_pBulletTime )
	{
		static float startTime = 0.0f;
		float endTime = g_pExtension->m_pGlobals->curtime, killingDuration = 0.0f;

		static int killedZombiesCount = 0;

		if( killedZombiesCount = 0 )
		{
			startTime = endTime;
		}

		if( player->m_IsInfected && attacker && !attacker->m_IsInfected )
		{
			killingDuration = endTime - startTime;

			BaseWeapon *weaponEnt = attacker->m_pEntity->GetActiveWeapon();

			if( (GET_NEMESIS(player) && (ZICore::m_CurrentMode == &g_NemesisMode || ZICore::m_CurrentMode == &g_PlagueMode)) || (GET_ASSASSIN(player) && ZICore::m_CurrentMode == &g_AssassinMode) || (!GET_SURVIVOR(attacker) && !GET_SNIPER(attacker) && weaponEnt && strncmp(weaponEnt->GetClassname(), "weapon_knife", 10) == 0) || (killingDuration > 0.0f && killingDuration <= g_BulletTimeActivationInterval.GetFloat() && ++killedZombiesCount / killingDuration >= g_BulletTimeActivationRate.GetFloat()) )
			{
				// Just before we actually start it, we play the sound and then start it (prevents the sound from being pithced down)
				ZIPlayer *playerIterator = nullptr;

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					playerIterator = *iterator;

					if( !playerIterator || playerIterator->m_IsBot )
					{
						continue;
					}

					// Play sound
					playerIterator->PlaySound("ZombieInfestation/bullettime_start.mp3");
				}

				static bool start;				
				start = true;				

				ZICore::m_pBulletTime = timersys->CreateTimer(&ZITimersCallback::m_BulletTime, RandomFloat(0.8f, 1.2f), &start, TIMER_FLAG_NO_MAPCHANGE);						
			}
		}
		else
		{
			killedZombiesCount = 0;
			startTime = endTime;
		}
	}

	player->m_IsAlive = false;
	player->m_pHumanLike = nullptr;

	// This needs to be here, players can die outta no where, we prevent the check for an attacker...
	player->UpdateCount();	
}

HookReturnValue<bool> ZombieInfestation::OnPreClientWeaponCanUse(ZIPlayer *player, BaseWeapon *weaponEnt)
{
	const char *classname = weaponEnt->GetClassname();

	if( player->m_IsInfected )
	{
		if( strncmp(classname, "weapon_knife", 12) != 0 && strcmp(classname, "weapon_tagrenade") != 0 )
		{
			HOOK_RETURN_VALUE(MRES_SUPERCEDE, bool, false, false);
		}
	}

	HOOK_RETURN_VALUE(MRES_IGNORED, bool, true, false);
}

HookReturn ZombieInfestation::OnPreClientWeaponEquip(ZIPlayer *player, BaseWeapon *weaponEnt)
{
	const char *classname = weaponEnt->GetClassname();

	if( player->m_IsInfected )
	{
		if( strncmp(classname, "weapon_knife", 12) != 0 && strcmp(classname, "weapon_tagrenade") != 0 )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}
	}	

	HOOK_RETURN_VOID(MRES_IGNORED, false);
}

HookReturnValue<bool> ZombieInfestation::OnPreClientWeaponSwitch(ZIPlayer *player, BaseWeapon *weaponEnt, int viewModelId)
{
	HOOK_RETURN_VALUE(MRES_IGNORED, bool, true, false);
}

void ZombieInfestation::OnPostClientWeaponSwitch(ZIPlayer *player, BaseWeapon *weaponEnt, int viewModelId)
{		
}

HookReturn ZombieInfestation::OnPreWeaponSetTransmit(BaseWeapon *weaponEnt, CCheckTransmitInfo *info, bool always)
{
/* 
	BUGGY, The zombies get their knives removed but there's a downside to that... their model animation instantely goes to "Idle" or something,
	could be just the need to change the model sequence which I dont know how right now, need to do some research about it.... and then TRY it,
	if it didn't work then I will have to find another method, try changing models etc... it's more fun than annoying

	ZIPlayer *owner = ZIPlayer::Find(GetEntityOwnerEntity(weaponEnt));

	if( owner )
	{
		ZIPlayer *observer = ZIPlayer::Find(info->m_pClientEnt);

		if( !observer || owner == observer )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		// BOTs do not complain
		if( owner->m_IsAlive && !observer->m_IsBot )
		{
			// Dont care about humans
			if( !owner->m_IsInfected )
			{
				HOOK_RETURN_VOID(MRES_IGNORED, false);
			}

			CBaseEntity *observerEnt = observer->m_pEntity;

			// Another playing is inspecting a Zombie
			if( observer->m_IsAlive )
			{
				HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
			}
			// Allow knife when inspecting someone ONLY in first person mode
			else if( GetClientSpecMode(observerEnt) == SPECMODE_FIRSTPERSON && GetClientSpecTarget(observerEnt) != owner->m_pEntity )
			{
				HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
			}
		}
	}	
*/

	HOOK_RETURN_VOID(MRES_IGNORED, false);
}

HookReturnValue<int> ZombieInfestation::OnPreEngineEmitSound(IRecipientFilter& filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, soundlevel_t soundlevel, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown)
{
	HOOK_RETURN_VALUE(MRES_IGNORED, int, -1, false);
}

HookReturnValue<int> ZombieInfestation::OnPreEngineEmitSound2(IRecipientFilter &filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, float attenuation, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown)
{
	HOOK_RETURN_VALUE(MRES_IGNORED, int, -1, false);
}