#ifndef _INCLUDE_ZI_PLAYERS_PROPER_H_
#define _INCLUDE_ZI_PLAYERS_PROPER_H_
#pragma once

#include "zi_core.h"

#define HUMAN_FOV 90
#define ZOMBIE_FOV 110

/*
	From ZP 6.0:

		zp_nvg_color_R 0 // Zombie custom nightvision color (red)
		zp_nvg_color_G 150 // Zombie custom nightvision color (green)
		zp_nvg_color_B 100 // Zombie custom nightvision color (blue)

	Here's the deal... we cannot use DLIght in Source unfortunately (it's not good, mostly causes FPS troubles)
		so the catch here is to use a combo of Fade message + default NVG (green color)

	In order to input the desired colors, we need to work with this: Green + ZOMBIE_NVG_COLOR = desired color * 2 (all RGB except for alpha, which sould be 100)
*/
#define ZOMBIE_NVG_COLOR Color(0, 50, 250, 100)

class ZIHumanoid;
class ZISoldier;
class ZIInfected;
class ZIZombie;
class ZIModel;

class ZIPlayer 
{
public:
	static int m_BloodSprayModelIndex;
	static int m_BloodDropModelIndex;

	static void Precache();

	static ZIPlayer *Find(const char *name);
	static ZIPlayer *Find(int id, bool userId);
	static ZIPlayer *Find(edict_t *client);
	static ZIPlayer *Find(BasePlayer *clientEntity);

	static void RandomizeTeams();	

	static int ConnectedCount(bool includeBots = true);
	static int TsCount();
	static int CTsCount();
	static int PlayingCount();
	static int AliveCount();
	static int HumansCount();
	static int SurvivorsCount();
	static int SnipersCount();
	static int ZombiesCount();
	static int NemesisCount();
	static int AssassinsCount();

	static ZIPlayer *RandomAlive();		

private:
	char m_IP[32];

	void Initiate();

	void GeographicalLookup();
	bool CheckVIPStatus();

public:
	int m_Index;
	edict_t *m_pEdict;
	BasePlayer *m_pEntity;
	int m_UserId;
	IGamePlayer *m_pGamePlayer;

	char m_Name[32];
	char m_SteamId[32];
	char m_Country[64];
	char m_City[64];

	char m_Health[20];
	char m_Armor[20];

	bool m_AllowTeamChoosing;
	bool m_IsBot;
	bool m_IsVIP;
	bool m_IsAlive;
	bool m_IsInfected;	
	bool m_IsFirstZombie;
	bool m_IsLastHuman;
	bool m_IsLastZombie;
	bool m_IsFrozen;
	bool m_IsProtected;

	int m_Points;

	bool m_CanGetPrimaryWeapon;
	bool m_CanGetSecondaryWeapon;
	bool m_HasInfiniteClip;	
	
	float *m_pDamageDealtToZombies;	

	float m_LastLeapTime;
	float m_NextLeapTime;

	ZIPlayer *m_pInfector;
	ZIPlayer *m_pDisinfector;

	ZIHumanoid *m_pHumanLike;
	ZIHumanoid *m_pLastHumanLike;

	ZISoldier *m_pChoosenHumanClass;
	ZIZombie *m_pChoosenZombieClass;
	
	ITimer *m_pStatsTimer;
	ITimer *m_pSetModelTimer;
	ITimer *m_pRemoveProtectionTimer;
	ITimer *m_pRespawnTimer;
	ITimer *m_pZombieGrowlTimer;
	ITimer *m_pZombieBleedTimer;
	ITimer *m_pZombieUnfreezeTimer;

	// Items
	ITimer *m_pTripminePlantingTimer;
	ITimer *m_pTripmineTakingTimer;
	ITimer *m_pZombieCooldownTimer;

	IBaseMenu *m_pMainMenu;
	IBaseMenu *m_pWeaponsMenu;
	IBaseMenu *m_pItemsMenu;
	IBaseMenu *m_pHumanSelectionMenu;
	IBaseMenu *m_pZombieSelectionMenu;		

	BaseProp *m_pGlowEntity;

	BaseViewModel *m_pFirstViewModel;
	BaseViewModel *m_pSecondViewModel;

	// Items-related vars
	bool m_HasTripmine;
	SourceHook::List<BaseEntity *> m_pTripmines;

	bool m_HasJetpack;
	int m_JetpackFuel;
	float m_NextRocketTime;	

	bool m_IsEnraged;

public:
	ZIPlayer(edict_t *edict);
	~ZIPlayer();

	bool operator == (const ZIPlayer &otherPlayer);

	const char *GetIP();

	bool Infect(ZIPlayer *attacker = nullptr, ZIInfected *zclass = nullptr);
	bool Disinfect(ZIPlayer *attacker = nullptr, ZIHumanoid *hclass = nullptr);

	void Gib(Vector force);

	bool Freeze(float duration);
	void Unfreeze();

	void ClearEffects();

	void ShowMainMenu();
	void ShowItemsMenu();
	void ShowWeaponsMenu(int slot);
	void ShowHumanSelectionMenu();
	void ShowZombieSelectionMenu();	

	void PlaySound(const char *sound, ...);
	void StopSound();

	void UpdateCount();	
};

#endif 
