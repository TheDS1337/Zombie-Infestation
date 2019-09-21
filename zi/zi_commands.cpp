#include "zi_commands.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"
#include "zi_round_modes.h"
#include "zi_round_mode_single_infection.h"
#include "zi_round_mode_multiple_infection.h"
#include "zi_round_mode_swarm.h"
#include "zi_round_mode_survivor.h"
#include "zi_round_mode_sniper.h"
#include "zi_round_mode_nemesis.h"
#include "zi_round_mode_assassin.h"
#include "zi_round_mode_plague.h"
#include "zi_round_mode_armageddon.h"
#include "zi_round_mode_assassins_vs_snipers.h"
#include "zi_round_mode_nightmare.h"

#define MY_STEAM_ID "DS"	// TODO

namespace ZICommands
{
	int m_Commander = 0;

	ZIPlayer *FindTarget(const char *auth)
	{
		int index = 0;
		char name[64];

		cmd_target_info_t info;

		info.pattern = auth;
		info.admin = 0;
		info.targets = &index;
		info.max_targets = 1;
		info.flags = COMMAND_FILTER_NO_MULTI;
		info.target_name = name;
		info.target_name_maxlength = sizeof(name);

		playerhelpers->ProcessCommandTarget(&info);

		if( info.num_targets < 1 )
		{
			return nullptr;
		}

		return ZIPlayer::Find(index, false);
	}

	bool OnPreLookAtWeaponCommand(ZIPlayer *player, const CCommand &args)
	{
		if( player->m_IsInfected )
		{
			//		UM_ScreenFade(&player->m_Index, 1, 1000, 0, ToggleClientNV(player->m_pEntity) ? FFADE_STAYOUT : FFADE_IN, ZOMBIE_NVG_COLOR);

					// Zombies shouldn't be able to inspect their claws
			return false;
		}

		return true;
	}

	bool OnPreNightVisionCommand(ZIPlayer *player, const CCommand &args)
	{
		if( !player->m_IsAlive || player->m_IsInfected )
		{
			return false;
		}

		if( player->m_IsAlive )
		{
			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, player->m_Index, CHAN_ITEM, "items/flashlight1.wav", -1, "items/flashlight1.wav", VOL_NORM, ATTN_NORM, 0);

			player->m_pEntity->ToggleFlashLight();

			// Block the nightvision thingy
			return false;
		}

		return true;
	}

	bool OnPreDropCommand(ZIPlayer *player, const CCommand &args)
	{
		if( !player->m_IsInfected && !GET_SURVIVOR(player) && !GET_SNIPER(player) )
		{
			return true;
		}

		return false;
	}

	bool OnPreBuyCommand(ZIPlayer *player, const CCommand &args)
	{
		if( player->m_CanGetPrimaryWeapon )
		{
			player->ShowWeaponsMenu(CSGO_WEAPON_SLOT_PRIMARY);
		}
		else if( player->m_CanGetSecondaryWeapon )
		{
			player->ShowWeaponsMenu(CSGO_WEAPON_SLOT_SECONDARY);
		}

		return false;
	}

	bool OnClientSayCommand(ZIPlayer *player, CCommand &cmd)
	{
		// Reading after the first char (., / or !)
		const char *firstArg = cmd.Arg(1) + 1;
		const char *targetAuth = cmd.Arg(2);

		ZIPlayer *target = nullptr;

		if( strcmp(firstArg, "menu") == 0 )
		{
			player->ShowMainMenu();
		}
		else if( strcmp(firstArg, "zhuman") == 0 )
		{
			player->ShowHumanSelectionMenu();
		}
		else if( strcmp(firstArg, "zzombie") == 0 )
		{
			player->ShowZombieSelectionMenu();
		}
		else if( strcmp(firstArg, "zitems") == 0 )
		{
			player->ShowItemsMenu();
		}
		else if( strcmp(firstArg, "zhelp") == 0 )
		{
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "Welcome to the Zombie Infestation CS:GO game modification!");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "Here all the available commands:");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "nightvision\t- Make sure to type on console \"bind n nightvision\" this way you can enable/disable your flashlight.");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "\t\t\t\t\tThis way, you can enable/disable your flashlight.");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "say !menu\t- ZI main menu");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "say !hlcass\t- Human classes menu");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "say !zlcass\t- Zombie classes menu");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "say !items\t- Extra items menu");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "");
			gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CONSOLE, "At last, we hope you enjoy playing on our server!");

			UM_SayText(&player->m_Index, 1, 0, true, "\x04** All the information have been printed on your console, please check it out.");
		}

		if( strcmp(firstArg, "human") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeHuman(target, player);
			}
			else
			{
				MakeHuman(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "zombie") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeZombie(target, player);
			}
			else
			{
				MakeZombie(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "survivor") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeSurvivor(target, player);
			}
			else
			{
				MakeSurvivor(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "sniper") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeSniper(target, player);
			}
			else
			{
				MakeSniper(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "nemesis") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeNemesis(target, player);
			}
			else
			{
				MakeNemesis(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "assassin") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				MakeAssassin(target, player);
			}
			else
			{
				MakeAssassin(player, player);
			}

			return false;
		}
		else if( strcmp(firstArg, "multi") == 0 )
		{
			StartMultipleInfection(player);
			return false;
		}
		else if( strcmp(firstArg, "swarm") == 0 )
		{
			StartSwarm(player);
			return false;
		}
		else if( strcmp(firstArg, "plague") == 0 )
		{
			StartPlague(player);
			return false;
		}
		else if( strcmp(firstArg, "armageddon") == 0 )
		{
			StartArmageddon(player);
			return false;
		}
		else if( strcmp(firstArg, "avs") == 0 )
		{
			StartAssassinsVSSnipers(player);
			return false;
		}
		else if( strcmp(firstArg, "nightmare") == 0 )
		{
			StartNightmare(player);
			return false;
		}
		else if( strcmp(firstArg, "respawn") == 0 )
		{
			// Attempt to find target
			if( targetAuth && *targetAuth )
			{
				target = FindTarget(targetAuth);

				if( !target )
				{
					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, "Coudln't find your target");
					return false;
				}

				Respawn(player, target);
			}
			else
			{
				Respawn(player, player);
			}

			return false;
		}

		return true;
	}

	void MakeHuman(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "Infection has not started yet.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( (!target->m_IsInfected && !GET_SURVIVOR(target) && !GET_SNIPER(target)) || target->m_IsFirstZombie || target->m_IsLastZombie )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into a human.");
			return;
		}

		target->Disinfect();

		static char buffer[256];

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into human.", admin->m_Name, target->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into human.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void MakeZombie(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( (target->m_IsInfected && !GET_NEMESIS(target) && !GET_ASSASSIN(target)) || target->m_IsLastHuman )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into a zombie.");
			return;
		}

		static char buffer[256];
		ZIPlayer *player = nullptr;

		if( ZICore::m_IsModeStarted )
		{
			target->Infect();

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into zombie.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into zombie.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
		else
		{
			ZIRoundMode::Start(&g_SingleInfectionMode, target);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a single infection mode with %s.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> started a single infection mode with %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
	}

	void MakeSurvivor(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( GET_SURVIVOR(target) || target->m_IsFirstZombie || target->m_IsLastZombie )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into a survivor.");
			return;
		}

		static char buffer[256];
		ZIPlayer *player = nullptr;

		if( ZICore::m_IsModeStarted )
		{
			target->Disinfect(nullptr, &g_Survivor);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into survivor.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into survivor.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
		else
		{
			ZIRoundMode::Start(&g_SurvivorMode, target);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a survivor mode with %s.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> started a survivor mode with %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
	}

	void MakeSniper(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( GET_SNIPER(target) || target->m_IsFirstZombie || target->m_IsLastZombie )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into a sniper.");
			return;
		}

		static char buffer[256];
		ZIPlayer *player = nullptr;

		if( ZICore::m_IsModeStarted )
		{
			target->Disinfect(nullptr, &g_Sniper);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into sniper.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into sniper.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
		else
		{
			ZIRoundMode::Start(&g_SniperMode, target);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a sniper mode with %s.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> started a sniper mode with %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
	}

	void MakeNemesis(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( GET_NEMESIS(target) || target->m_IsLastHuman )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into a nemesis.");
			return;
		}

		static char buffer[256];
		ZIPlayer *player = nullptr;

		if( ZICore::m_IsModeStarted )
		{
			target->Infect(nullptr, &g_Nemesis);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into nemesis.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into nemesis.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
		else
		{
			ZIRoundMode::Start(&g_NemesisMode, target);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a nemesis mode with %s.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> started a nemesis mode with %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
	}

	void MakeAssassin(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( !target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player isnt alive.");
			return;
		}

		if( GET_ASSASSIN(target) || target->m_IsLastHuman )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player cant be turned into an assassin.");
			return;
		}

		static char buffer[256];
		ZIPlayer *player = nullptr;

		if( ZICore::m_IsModeStarted )
		{
			target->Infect(nullptr, &g_Assassin);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s turned %s into assassin.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> turned %s <%s><%s> into assassin.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
		else
		{
			ZIRoundMode::Start(&g_AssassinMode, target);

			if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
			{
				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started an assassin mode with %s.", admin->m_Name, target->m_Name);

				for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
				{
					player = *iterator;

					if( !player || player->m_IsBot )
					{
						continue;
					}

					gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
				}

				g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> started an assassin mode with %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
				g_pSM->LogMessage(myself, buffer);
			}
		}
	}

	void StartMultipleInfection(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_MultipleInfectionMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a multiple infection mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started a multiple infection mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void StartSwarm(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_SwarmMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a swarm mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started a swarm mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void StartPlague(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_PlagueMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a plague mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started a plague mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void StartArmageddon(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_ArmageddonMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started an armageddon mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started an armageddon mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void StartAssassinsVSSnipers(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_AvSMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started an assassins vs snipers mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started an assassins vs snipers mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void StartNightmare(ZIPlayer *admin)
	{
		if( !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( ZICore::m_IsModeStarted )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "A gameplay is already in progress.");
			return;
		}

		ZIRoundMode::Start(&g_NightmareMode);

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			static char buffer[256];
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s started a nightmare mode.", admin->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s> started a nightmare mode.", admin->m_Name, admin->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}

	void Respawn(ZIPlayer *target, ZIPlayer *admin)
	{
		if( !target || !admin )
		{
			return;
		}

		if( ZICore::m_IsRoundEnd )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This command is not available right now.");
			return;
		}

		if( target->m_IsAlive )
		{
			gamehelpers->TextMsg(admin->m_Index, TEXTMSG_DEST_CHAT, "This player is already alive.");
			return;
		}

		BasePlayer *playerEnt = target->m_pEntity;
		playerEnt->Respawn();

		int team = playerEnt->GetTeam();

		if( !target->m_pLastHumanLike && team != CSGO_TEAM_T && team != CSGO_TEAM_CT )
		{
			target->Disinfect();
		}
		else
		{
			ZIInfected *infected = GET_LAST_INFECTED(target);

			if( infected )
			{
				target->Infect(nullptr, infected);
			}
			else if( target->m_pLastHumanLike )
			{
				target->Disinfect(nullptr, target->m_pLastHumanLike);
			}
		}

		static char buffer[256];

		if( strcmp(admin->m_Name, MY_STEAM_ID) != 0 )
		{
			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s respawned %s.", admin->m_Name, target->m_Name);

			ZIPlayer *player = nullptr;

			for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
			{
				player = *iterator;

				if( !player || player->m_IsBot )
				{
					continue;
				}

				gamehelpers->TextMsg(player->m_Index, TEXTMSG_DEST_CHAT, buffer);
			}

			g_pSM->Format(buffer, sizeof(buffer), "ADMIN %s <%s><%s> respawned %s <%s><%s>.", admin->m_Name, admin->m_SteamId, admin->GetIP(), target->m_Name, target->m_SteamId, target->GetIP());
			g_pSM->LogMessage(myself, buffer);
		}
	}
}