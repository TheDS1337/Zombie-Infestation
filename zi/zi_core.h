#ifndef _INCLUDE_ZI_CORE_PROPER_H_
#define _INCLUDE_ZI_CORE_PROPER_H_
#pragma once

#include "zi_entry.h"
#include "zi_round_modes.h"


#define HUD_ROUND_STATUS_X -1.0
#define HUD_ROUND_STATUS_Y 0.17
#define HUD_INFECTION_UPDATES_X 0.05
#define HUD_INFECTION_UPDATES_Y 0.45
#define HUD_STATISTICS_X 0.02
#define HUD_STATISTICS_Y 0.5

class ZIPlayer;
class ZIRoundMode;
class ZIItem;
class ZITimersCallback;
class ZIMenusCallback;

enum ItemReturn;

class ZICore
{
public:
	static void OnLoad();
	static void OnUnload();

	static bool OnPreClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin);
	static void OnPostClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin);

	static bool OnPreClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper);
	static void OnPostClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper);

	static void OnClientLastHuman(ZIPlayer *player);
	static void OnClientLastZombie(ZIPlayer *player);

	static ItemReturn OnPreItemSelection(ZIItem *item, ZIPlayer *player);
	static void OnPostItemSelection(ZIItem *item, ZIPlayer *player);

	static void OnRoundModeStart();
	static void OnRoundModeEnd();

public:
	static SourceHook::CVector<ZIPlayer *> m_pOnlinePlayers;

	static bool m_IsModeStarted;
	static bool m_IsRoundEnd;

	static ZIRoundMode *m_CurrentMode;
	static ZIRoundMode *m_LastMode;		

	static int m_Countdown;

	static RoundModeWinner m_Winner;
	static int m_Score[2]; 

	static ITimer *m_pInfo;
	static ITimer *m_pWarningTimer;
	static ITimer *m_pCountdownTimer;
	static ITimer *m_pStartModeTimer;
	static ITimer *m_pAmbientSoundTimer;
	static ITimer *m_pTeamsRandomization;
	static ITimer *m_pBulletTime;

	static IBaseMenu *m_pPrimaryWeaponsMenu;
	static IBaseMenu *m_pSecondaryWeaponsMenu;

	static ZITimersCallback m_TimersCallback;
	static ZIMenusCallback m_MenusCallback;

	static hud_obj_t m_RoundStateHud;
	static hud_obj_t m_InfectionUpdatesHud;
	static hud_obj_t m_StatsHud;
	static hud_obj_t m_DamageHud;
};

#endif 
