#ifndef _INCLUDE_ZI_COMMANDS_PROPER_H_
#define _INCLUDE_ZI_COMMANDS_PROPER_H_
#pragma once

#include "zi_core.h"

class ZIPlayer;

namespace ZICommands
{
	extern int m_Commander;

	// Extend this to support SteamId/userid
	ZIPlayer *FindTarget(const char *auth);

	// Hooks on some game commands
	bool OnPreLookAtWeaponCommand(ZIPlayer *player, const CCommand &args);
	bool OnPreNightVisionCommand(ZIPlayer *player, const CCommand &args);
	bool OnPreDropCommand(ZIPlayer *player, const CCommand &args);
	bool OnPreBuyCommand(ZIPlayer *player, const CCommand &args);

	// General commands goes here
	bool OnClientSayCommand(ZIPlayer *player, CCommand &cmd);

	// Admin commands
	void MakeHuman(ZIPlayer *target, ZIPlayer *admin);
	void MakeZombie(ZIPlayer *target, ZIPlayer *admin);
	void MakeSurvivor(ZIPlayer *target, ZIPlayer *admin);
	void MakeSniper(ZIPlayer *target, ZIPlayer *admin);
	void MakeNemesis(ZIPlayer *target, ZIPlayer *admin);
	void MakeAssassin(ZIPlayer *target, ZIPlayer *admin);
	void StartMultipleInfection(ZIPlayer *admin);
	void StartSwarm(ZIPlayer *admin);
	void StartPlague(ZIPlayer *admin);
	void StartArmageddon(ZIPlayer *admin);
	void StartAssassinsVSSnipers(ZIPlayer *admin);
	void StartNightmare(ZIPlayer *admin);

	void Respawn(ZIPlayer *target, ZIPlayer *admin);

	// Player commands
};

#endif 