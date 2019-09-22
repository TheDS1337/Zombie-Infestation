#include "zi_entry.h"
#include "zi_core.h"
#include "zi_items.h"
#include "zi_round_modes.h"
#include "zi_weapons.h"
#include "zi_timers.h"
#include "zi_menus.h"
#include "zi_sourcemod_bridge.h"

// Human classes
#include "zi_human_private.h"
#include "zi_human_hunter.h"

// Zombie classes
#include "zi_zombie_classic.h"
#include "zi_zombie_raptor.h"
#include "zi_zombie_fleshpound.h"
#include "zi_zombie_predator.h"

// Round modes
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

// Items
#include "zi_item_tripmine.h"
#include "zi_item_infinite_clip.h"
#include "zi_item_jetpack_bazooka.h"
#include "zi_item_antidote.h"
#include "zi_item_rage.h"
#include "zi_item_infection_bomb.h"

namespace ZICore
{
	SourceHook::CVector<ZIPlayer *> m_pOnlinePlayers;

	bool m_IsModeStarted = false;
	bool m_IsRoundEnd = false;

	ZIRoundMode *m_CurrentMode = nullptr;
	ZIRoundMode *m_LastMode = nullptr;

	int m_Countdown = -1;

	RoundModeWinner m_Winner = RoundModeWinner_Unknown;
	int m_Score[RoundModeWinner_Max];

	ITimer *m_pInfo = nullptr;
	ITimer *m_pWarningTimer = nullptr;
	ITimer *m_pCountdownTimer = nullptr;
	ITimer *m_pStartModeTimer = nullptr;
	ITimer *m_pAmbientSoundTimer = nullptr;
	ITimer *m_pTeamsRandomization = nullptr;
	ITimer *m_pBulletTime = nullptr;

	IBaseMenu *m_pPrimaryWeaponsMenu = nullptr;
	IBaseMenu *m_pSecondaryWeaponsMenu = nullptr;

	hud_obj_t m_RoundStateHud;
	hud_obj_t m_InfectionUpdatesHud;
	hud_obj_t m_StatsHud;
	hud_obj_t m_DamageHud;

	void OnLoad()
	{
		// Human classes
		ZISoldier::Register(&g_PrivateHuman);
		ZISoldier::Register(&g_HunterHuman);

		// Zombie classes
		ZIZombie::Register(&g_ClassicZombie);
		ZIZombie::Register(&g_RaptorZombie);
		ZIZombie::Register(&g_FleshpoundZombie);
		ZIZombie::Register(&g_PredatorZombie);

		// Round modes
		ZIRoundMode::Register(&g_SingleInfectionMode);
		ZIRoundMode::Register(&g_MultipleInfectionMode);
		ZIRoundMode::Register(&g_SwarmMode);
		ZIRoundMode::Register(&g_SurvivorMode);
		ZIRoundMode::Register(&g_SniperMode);
		ZIRoundMode::Register(&g_NemesisMode);
		ZIRoundMode::Register(&g_AssassinMode);
		ZIRoundMode::Register(&g_PlagueMode);
		ZIRoundMode::Register(&g_ArmageddonMode);
		ZIRoundMode::Register(&g_AvSMode);
		ZIRoundMode::Register(&g_NightmareMode);

		// Items
		ZIItem::Register(&g_TripmineItem);
		ZIItem::Register(&g_InfiniteClipItem);
		ZIItem::Register(&g_JetpackBazookaItem);
		ZIItem::Register(&g_AntidoteItem);
		ZIItem::Register(&g_RageItem);
		ZIItem::Register(&g_InfectionBombItem);

		m_pPrimaryWeaponsMenu = ZIWeapon::BuildPrimaryWeaponsMenu();
		m_pSecondaryWeaponsMenu = ZIWeapon::BuildSecondaryWeaponsMenu();		
	}

	void OnUnload()
	{
		RELEASE_TIMER(m_pWarningTimer);
		RELEASE_TIMER(m_pCountdownTimer);
		RELEASE_TIMER(m_pStartModeTimer);
		RELEASE_TIMER(m_pAmbientSoundTimer);
		RELEASE_TIMER(m_pTeamsRandomization);
		RELEASE_TIMER(m_pBulletTime);

		RELEASE_MENU(m_pPrimaryWeaponsMenu);
		RELEASE_MENU(m_pSecondaryWeaponsMenu);

		RELEASE_POINTERS_ARRAY(m_pOnlinePlayers);

		g_pHumanClasses.clear();
		g_pZombieClasses.clear();
		g_pRoundModes.clear();
	}

	bool OnPreClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin)
	{
		if( ZISourceModBridge::m_pPrePlayerInfection )
		{
			ZISourceModBridge::m_pPrePlayerInfection->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPrePlayerInfection->PushCell(attacker ? attacker->m_Index : 0);
			ZISourceModBridge::m_pPrePlayerInfection->PushCell(nemesis ? 1 : 0);
			ZISourceModBridge::m_pPrePlayerInfection->PushCell(assassin ? 1 : 0);

			cell_t result = Pl_Continue;
			ZISourceModBridge::m_pPrePlayerInfection->Execute(&result);

			if( result == Pl_Handled )
			{
				return false;
			}
		}

		return true;
	}

	void OnPostClientInfection(ZIPlayer *player, ZIPlayer *attacker, bool nemesis, bool assassin)
	{
		g_TripmineItem.OnPostClientInfection(player);
		g_JetpackBazookaItem.OnPostClientInfection(player);
		g_RageItem.OnPostClientInfection(player);

		if( ZISourceModBridge::m_pPostPlayerInfection )
		{
			ZISourceModBridge::m_pPostPlayerInfection->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPostPlayerInfection->PushCell(attacker ? attacker->m_Index : 0);
			ZISourceModBridge::m_pPostPlayerInfection->PushCell(nemesis ? 1 : 0);
			ZISourceModBridge::m_pPostPlayerInfection->PushCell(assassin ? 1 : 0);
			ZISourceModBridge::m_pPostPlayerInfection->Execute(NULL);
		}
	}

	bool OnPreClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper)
	{
		if( ZISourceModBridge::m_pPrePlayerDisinfection )
		{
			ZISourceModBridge::m_pPrePlayerDisinfection->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPrePlayerDisinfection->PushCell(attacker ? attacker->m_Index : 0);
			ZISourceModBridge::m_pPrePlayerDisinfection->PushCell(survivor ? 1 : 0);
			ZISourceModBridge::m_pPrePlayerDisinfection->PushCell(sniper ? 1 : 0);

			cell_t result = Pl_Continue;
			ZISourceModBridge::m_pPrePlayerDisinfection->Execute(&result);

			if( result == Pl_Handled )
			{
				return false;
			}
		}

		return true;
	}

	void OnPostClientDisinfection(ZIPlayer *player, ZIPlayer *attacker, bool survivor, bool sniper)
	{
		g_RageItem.OnPostClientDisinfection(player);

		if( ZISourceModBridge::m_pPostPlayerDisinfection )
		{
			ZISourceModBridge::m_pPostPlayerDisinfection->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPostPlayerDisinfection->PushCell(attacker ? attacker->m_Index : 0);
			ZISourceModBridge::m_pPostPlayerDisinfection->PushCell(survivor ? 1 : 0);
			ZISourceModBridge::m_pPostPlayerDisinfection->PushCell(sniper ? 1 : 0);
			ZISourceModBridge::m_pPostPlayerDisinfection->Execute(NULL);
		}
	}

	void OnClientLastHuman(ZIPlayer *player)
	{
		if( ZISourceModBridge::m_pPlayerLastHuman )
		{
			ZISourceModBridge::m_pPlayerLastHuman->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPlayerLastHuman->Execute(NULL);
		}
	}

	void OnClientLastZombie(ZIPlayer *player)
	{
		if( ZISourceModBridge::m_pPlayerLastZombie )
		{
			ZISourceModBridge::m_pPlayerLastZombie->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPlayerLastZombie->Execute(NULL);
		}
	}

	ItemReturn OnPreItemSelection(ZIItem *item, ZIPlayer *player)
	{
		if( ZISourceModBridge::m_pPreItemSelection )
		{
			ZISourceModBridge::m_pPreItemSelection->PushCell(item ? item->GetIndex() : -1);
			ZISourceModBridge::m_pPreItemSelection->PushCell(player ? player->m_Index : 0);

			cell_t result = Pl_Continue;
			ZISourceModBridge::m_pPreItemSelection->Execute(&result);

			return (ItemReturn) result;
		}

		return ItemReturn_Show;
	}

	void OnPostItemSelection(ZIItem *item, ZIPlayer *player)
	{
		if( ZISourceModBridge::m_pPostItemSelection )
		{
			ZISourceModBridge::m_pPostItemSelection->PushCell(item ? item->GetIndex() : -1);
			ZISourceModBridge::m_pPostItemSelection->PushCell(player ? player->m_Index : 0);
			ZISourceModBridge::m_pPostItemSelection->Execute(NULL);
		}
	}

	void OnRoundModeStart()
	{
		if( ZISourceModBridge::m_pRoundModeStart )
		{
			ZISourceModBridge::m_pRoundModeStart->PushCell(m_CurrentMode ? m_CurrentMode->GetIndex() : -1);
			ZISourceModBridge::m_pRoundModeStart->PushCell(ZIRoundMode::m_RoundTarget ? ZIRoundMode::m_RoundTarget->m_Index : 0);
			ZISourceModBridge::m_pRoundModeStart->Execute(NULL);
		}
	}

	void OnRoundModeEnd()
	{
		if( ZISourceModBridge::m_pRoundModeEnd )
		{
			ZISourceModBridge::m_pRoundModeEnd->PushCell(m_Winner);
			ZISourceModBridge::m_pRoundModeEnd->PushCell(ZIRoundMode::m_RoundTarget ? ZIRoundMode::m_RoundTarget->m_Index : 0);
			ZISourceModBridge::m_pRoundModeEnd->Execute(NULL);
		}

		// Free our target so we can look for a new one the next round
		ZIRoundMode::m_RoundTarget = nullptr;
	}
}