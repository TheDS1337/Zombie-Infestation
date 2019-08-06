#ifndef _INCLUDE_ZI_COMMANDS_PROPER_H_
#define _INCLUDE_ZI_COMMANDS_PROPER_H_
#pragma once

#include "zi_core.h"

class ZIPlayer;

enum AdminActions
{
	AdminAction_MakeHuman,
	AdminAction_MakeZombie,
	AdminAction_MakeSurvivor,
	AdminAction_MakeSniper,
	AdminAction_MakeNemesis,
	AdminAction_MakeAssassin,
	AdminAction_StartMultipleInfection,
	AdminAction_StartSwarm,
	AdminAction_StartPlague,
	AdminAction_StartArmageddon,
	AdminAction_StartAssassinsVSSnipers,
	AdminAction_StartNightmare
};

class ZICommands
{
public:
	static int m_Commander;

	// Extend this to support SteamId/userid
	static ZIPlayer *FindTarget(const char *auth);

	// Hooks on some game commands
	static bool OnPreLookAtWeaponCommand(ZIPlayer *player, const CCommand &args);
	static bool OnPreNightVisionCommand(ZIPlayer *player, const CCommand &args);
	static bool OnPreDropCommand(ZIPlayer *player, const CCommand &args);
	static bool OnPreBuyCommand(ZIPlayer *player, const CCommand &args);

	// General commands goes here
	static bool OnClientSayCommand(ZIPlayer *player, CCommand &cmd);

	// Admin commands
	static void MakeHuman(ZIPlayer *target, ZIPlayer *admin);
	static void MakeZombie(ZIPlayer *target, ZIPlayer *admin);
	static void MakeSurvivor(ZIPlayer *target, ZIPlayer *admin);
	static void MakeSniper(ZIPlayer *target, ZIPlayer *admin);
	static void MakeNemesis(ZIPlayer *target, ZIPlayer *admin);
	static void MakeAssassin(ZIPlayer *target, ZIPlayer *admin);
	static void StartMultipleInfection(ZIPlayer *admin);
	static void StartSwarm(ZIPlayer *admin);
	static void StartPlague(ZIPlayer *admin);
	static void StartArmageddon(ZIPlayer *admin);
	static void StartAssassinsVSSnipers(ZIPlayer *admin);
	static void StartNightmare(ZIPlayer *admin);

//	static void Respawn(ZIPlayer *target, ZIPlayer *admin);	TODO

	// Player commands
};

extern ZICommands g_Commands;

#endif 