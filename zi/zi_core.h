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

enum ItemReturn;

namespace ZICore
{
	void OnLoad();
	void OnUnload();

	bool OnPreClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin);
	void OnPostClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin);

	bool OnPreClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper);
	void OnPostClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper);

	void OnClientLastHuman(ZIPlayer *player);
	void OnClientLastZombie(ZIPlayer *player);

	ItemReturn OnPreItemSelection(ZIItem *item, ZIPlayer *player);
	void OnPostItemSelection(ZIItem *item, ZIPlayer *player);

	void OnRoundModeStart();
	void OnRoundModeEnd();

	extern SourceHook::CVector<ZIPlayer *> m_pOnlinePlayers;

	extern bool m_IsModeStarted;
	extern bool m_IsRoundEnd;

	extern ZIRoundMode *m_CurrentMode;
	extern ZIRoundMode *m_LastMode;

	extern int m_Countdown;

	extern RoundModeWinner m_Winner;
	extern int m_Score[2];

	extern ITimer *m_pInfo;
	extern ITimer *m_pWarningTimer;
	extern ITimer *m_pCountdownTimer;
	extern ITimer *m_pStartModeTimer;
	extern ITimer *m_pAmbientSoundTimer;
	extern ITimer *m_pTeamsRandomization;
	extern ITimer *m_pBulletTime;

	extern IBaseMenu *m_pPrimaryWeaponsMenu;
	extern IBaseMenu *m_pSecondaryWeaponsMenu;	

	extern hud_obj_t m_RoundStateHud;
	extern hud_obj_t m_InfectionUpdatesHud;
	extern hud_obj_t m_StatsHud;
	extern hud_obj_t m_DamageHud;
};

#endif 
