#ifndef _INCLUDE_GAMEUTILS_PROPER_H_
#define _INCLUDE_GAMEUTILS_PROPER_H_
#pragma once

#include "extension.h"

class BaseEntity;
class BaseProp;
class BaseWeapon;
class BaseGrenade;
class BasePlayer;

struct WeaponData;

#define CSGO_TEAM_NONE 0
#define CSGO_TEAM_SPECTATOR 1
#define CSGO_TEAM_T 2
#define CSGO_TEAM_CT 3

#define DMG_HEADSHOT 1 << 30

#define CSGO_MAX_WEAPONS 64
#define CSGO_MAX_WEAPON_IN_SLOT	6

#define CSGO_WEAPON_SLOT_PRIMARY 0
#define CSGO_WEAPON_SLOT_SECONDARY 1
#define CSGO_WEAPON_SLOT_MELEE 2
#define CSGO_WEAPON_SLOT_NADES 3
#define CSGO_WEAPON_SLOT_C4 4

#define CSGO_ADDON_NONE 0
#define CSGO_ADDON_FLASHBANG1 1 << 0
#define CSGO_ADDON_FLASHBANG2 1 << 1
#define CSGO_ADDON_HEGGRENADE 1 << 2
#define CSGO_ADDON_SMOKEGRENADE 1 << 3
#define CSGO_ADDON_C4 1 << 4
#define CSGO_ADDON_DEFUSEKIT 1 << 5
#define CSGO_ADDON_PRIMARY	1 << 6
#define CSGO_ADDON_SECONDARY 1 << 7
#define CSGO_ADDON_HOLSTER 1 << 8 
#define CSGO_ADDON_DECOY 1 << 9
#define CSGO_ADDON_MELEE 1 << 10
#define CSGO_ADDON_FACEMASK 1 << 11
#define CSGO_ADDON_TAGRENADE 1 << 12

#define WEAPONAMMO_HEGRENADE 14
#define WEAPONAMMO_FLASHBANG 15
#define WEAPONAMMO_SMOKEGRENADE 16
#define WEAPONAMMO_INFERNO 17
#define WEAPONAMMO_DECOY 18

#define SPECMODE_NONE 0
#define SPECMODE_FIRSTPERSON 4
#define SPECMODE_3RDPERSON 5
#define SPECMODE_FREELOOK 6

enum CSGORoundEndReason
{
	CSGORoundEnd_TargetBombed = 1,           /**< Target Successfully Bombed! */
	CSGORoundEnd_VIPEscaped,                 /**< The VIP has escaped! - Doesn't exist on CS:GO */
	CSGORoundEnd_VIPKilled,                  /**< VIP has been assassinated! - Doesn't exist on CS:GO */
	CSGORoundEnd_TEscaped,          /**< The terrorists have escaped! */
	CSGORoundEnd_CTStoppedEscape,            /**< The CTs have prevented most of the terrorists from escaping! */
	CSGORoundEnd_TStopped,          /**< Escaping terrorists have all been neutralized! */
	CSGORoundEnd_BombDefused,                /**< The bomb has been defused! */
	CSGORoundEnd_CTWin,                      /**< Counter-Terrorists Win! */
	CSGORoundEnd_TWin,               /**< Terrorists Win! */
	CSGORoundEnd_Draw,                       /**< Round Draw! */
	CSGORoundEnd_HostagesRescued,            /**< All Hostages have been rescued! */
	CSGORoundEnd_TargetSaved,                /**< Target has been saved! */
	CSGORoundEnd_HostagesNotRescued,         /**< Hostages have not been rescued! */
	CSGORoundEnd_TNotEscaped,       /**< Terrorists have not escaped! */
	CSGORoundEnd_VIPNotEscaped,              /**< VIP has not escaped! - Doesn't exist on CS:GO */
	CSGORoundEnd_GameStart,                  /**< Game Commencing! */
	CSGORoundEnd_TSurrender,        /**< Terrorists Surrender */
	CSGORoundEnd_CTSurrender,                /**< CTs Surrender */
	CSGORoundEnd_TPlanted,		   /**< Terrorists Planted the bomb */
	CSGORoundEnd_CTsReachedHostage		   /**< CTs Reached the hostage */
};

/*
* Use this only to compare thinking functions
*/
#if defined _WINDOWS
typedef void (__thiscall *TF)(CBaseEntity *);
#elif defined _LINXU
typedef void (*TF)(CBaseEntity *);
#endif

// Wrapper over thinkfunc_t
struct ThinkFuncData
{
	TF m_pfnThink;
	string_t m_iszContext;
	int m_nNextThinkTick;
	int m_nLastThinkTick;
};

class BaseEntity
{
public:
	static BaseEntity *CreateEntity(const char *classname);
	static BaseEntity *FindEntity(BaseEntity *startingEnt, const char *classname);
	static BaseEntity *FindEntityInSphere(BaseEntity *startingEnt, const char *classname, const Vector &center, float radius);
	static BaseEntity *FindEntityByNetClass(int start, const char *classname);

	static void SetInputVariant(bool value);
	static void SetInputVariant(int value);
	static void SetInputVariant(float value);
	static void SetInputVariant(const char *value);
	static void SetInputVariant(Color value);
	static void SetInputVariant(Vector value, bool posVector);
	static void SetInputVariant(BaseEntity *value);

	bool AcceptInput(const char *input, BaseEntity *activator = nullptr, BaseEntity *caller = nullptr, int outputId = 0);

	void Spawn();
	void Activate();

	void Teleport(Vector *origin, QAngle *angles, Vector *velocity);

	void Ignite(float duration, bool NPCOnly = false, float size = 0.0, bool called = false);
	void Extinguish();

	int TakeDamage(CTakeDamageInfo2 info);

	const char *GetClassname();

	void SetModel(const char *model);
	void SetSize(const Vector &min, const Vector &max);

	int GetHealth();																// Sendprop
	void SetHealth(int value);														// Sendprop

	float GetGravity();
	void SetGravity(float value);

	int GetModelIndex();															// Sendprop
	void SetModelIndex(int modelIndex);												// Sendprop
	const char *GetModelName();
	
	int GetMoveType();
	void SetMoveType(int value);

	int GetSolidType();
	void SetSolidType(int value);

	int GetSolidFlags();
	void SetSolidFlags(int value);

	int GetCollisionGroup();
	void SetCollisionGroup(int value);

	int GetFlags();
	void SetFlags(int value);

	int GetSpawnFlags();															// Sendprop
	void SetSpawnFlags(int value);													// Sendprop

	int GetEffects();																// Sendprop
	void SetEffects(int value);														// Sendprop

	BaseEntity *GetOwner();
	void SetOwner(BaseEntity *ownerEnt);

	BaseEntity *GetOwnerEntity();
	void SetOwnerEntity(BaseEntity *ownerEnt);

	BaseEntity *GetParent();
	void SetParent(BaseEntity *parent);

	/*virtual*/ Vector GetOrigin();														// Sendprop
	void SetOrigin(Vector value);													// Sendprop

	Vector GetVelocity();															// Sendprop
	Vector GetVelocity(AngularImpulse &angVelocity);
	void SetVelocity(Vector value);													// Sendprop

	Vector GetAbsVelocity();														// Sendprop
	void SetAbsVelocity(Vector value);												// Sendprop
	
	CUtlVector<ThinkFuncData> *GetThinkFunctions();
	TF GetThink();
	void SetThink(TF func);

	int GetThinkFuncId(const char *context);
	int RegisterThinkFuncId(const char *context);
	bool IsWillingToThink();
	TF SetThinkFunc(TF func, float thinkTime, const char *context);

	int GetNextThinkTick();
	void SetNextThinkTick(int value);

	int GetTakeDamage();
	void SetTakeDamage(int value);

	// Interesting headers to look at (for values to be used with a lot of these functions such as EF_x flags)
	// const.h
	// shareddefs.h

	// TODO: Check if the prop here needs to be retransmitted over the network (set edict on change of state)
	Color GetRenderColor();															// Sendprop
	void SetRenderColor(Color color);												// Sendprop
	RenderFx_t GetRenderFx();														// Sendprop
	void SetRenderFx(RenderFx_t value);												// Sendprop
	RenderMode_t GetRenderMode();													// Sendprop
	void SetRenderMode(RenderMode_t value);											// Sendprop

	bool GetKeyValue(const char *key, char *value, int valueLen);
	bool SetKeyValue(const char *key, char *value);

	bool GetKeyValue(const char *key, bool *value);
	bool SetKeyValue(const char *key, bool value);

	bool GetKeyValue(const char *key, int &value);
	bool SetKeyValue(const char *key, int value);

	bool GetKeyValue(const char *key, float &value);
	bool SetKeyValue(const char *key, float value);

	bool GetKeyValue(const char *key, Vector &value);
	bool SetKeyValue(const char *key, Vector value);

	bool GetKeyValue(const char *key, QAngle &value);
	bool SetKeyValue(const char *key, QAngle value);
};

class BaseProp: public BaseEntity
{
public:
	bool IsGlowing();
	void SetGlow(bool on);

	int GetGlowStyle();
	void SetGlowStyle(int value);

	Color GetGlowColor();
	void SetGlowColor(Color color);

	float GetGlowMaxDist();
	void SetGlowMaxDist(float value);
};

class BaseAnimating: public BaseEntity
{
public:
	int GetSkin();																	// Sendprop
	void SetSkin(int skin);															// Sendprop

	int GetBody();																	// Sendprop
	void SetBody(int body);															// Sendprop

	int GetSequence();																// Sendprop
	void SetSequence(int value);													// Sendprop

	float GetPlaybackRate();														// Sendprop
	void SetPlaybackRate(float value);												// Sendprop

	int SelectWeightedSequence(Activity activity);
};

class BaseWeapon: public BaseEntity
{
public:
	int GetViewModel();																// Sendprop
	void SetViewModel(int value);													// Sendprop

	int GetViewModelIndex();														// Sendprop
	void SetViewModelIndex(int value);												// Sendprop

	int GetWorldModelIndex();														// Sendprop
	void SetWorldModelIndex(int value);												// Sendprop

	bool IsUsingClip1();
	bool IsUsingClip2();

	int GetClip();																	// Sendprop
	void SetClip(int value);														// Sendprop
};

class BaseGrenade: public BaseEntity
{
public:
	void Detonate();

	Vector GetInitialVelocity();													// Sendprop
	void SetInitialVelocity(Vector value);											// Sendprop

	BasePlayer *GetThrower();														// Sendprop
	void SetThrower(BasePlayer *throwerEnt);										// Sendprop

	float GetDamage();																// Sendprop
	void SetDamage(float value);													// Sendprop

	float GetDamageRadius();														// Sendprop
	void SetDamageRadius(float value);												// Sendprop

	bool IsLive();																	// Sendprop
	void SetLive(bool live);														// Sendprop	
};

class BaseViewModel: public BaseAnimating
{
public:
	int GetViewModelIndex();														// Sendprop
	void SetViewModelIndex(int value);												// Sendprop	

	int UpdateTransmitState();
};

class BasePlayer: public BaseEntity
{
public:
	void Kill(bool explode = true, bool force = false);
	void Respawn();

	void SetName(const char *name);

	void SetProgressBarTime(int time);	

	bool CanGetInSpace(Vector &origin, bool ducking);

	BaseWeapon *GiveItem(const char *item, int subtype = 0, bool removeIfNotCarried = false);
	int GiveAmmo(int amount, int ammotype, bool suppressSound = false);
	void EquipItem(BaseEntity *itemEntity);
	bool RemoveItem(BaseEntity *itemEntity);
	BaseWeapon *GetItemFromSlot(int slot);

	void SelectItem(const char *classname, int subType);

	int GetItemsCountFromSlot(int slot);

	int GetItemsFromSlot(int slot, SourceHook::CVector<WeaponData> *items);
	void RemoveItemFromSlot(int slot, const char *cls);
	void RemoveItemsFromSlot(int slot);

	Vector GetOrigin();						
	QAngle GetAngles();		
	QAngle GetEyeAngles();

	BaseEntity *GetAimTarget();	
	Vector GetAimTarget2();

	int GetArmor();																	// Sendprop
	void SetArmor(int value, bool helmet = false);									// Sendprop

	float GetSpeed();																// Sendprop
	void SetSpeed(float value);														// Sendprop

	float GetVelocityModifier();													// Sendprop
	void SetVelocityModifier(float value);											// Sendprop

	bool IsScoped();																// Sendprop
	void SetScoped(bool value);														// Sendprop

	int GetFOV();																	// Sendprop
	void SetFOV(int value);															// Sendprop

	bool IsFlashLightOn();															// Sendprop
	void SetFlashLight(bool value);													// Sendprop
	bool ToggleFlashLight();														// Sendprop

	bool IsNightVisionOn();															// Sendprop
	void SetNightVision(bool value, const char *overlay = nullptr);					// Sendprop
	bool ToggleNightVision();														// Sendprop

	const char *GetArmsmodel();														// Sendprop
	void SetArmsmodel(const char *model);											// Sendprop

	BaseViewModel *CreateViewModel(int id);

	BaseViewModel *GetViewModel(int id);											// Sendprop
	void SetViewModel(int id, BaseViewModel *viewModelEnt);							// Sendprop

	BaseEntity *GetRagdoll();														// Sendprop

	BaseWeapon *GetActiveWeapon();													// Sendprop
	void SetActiveWeapon(BaseWeapon *weaponEnt);									// Sendprop

	BaseWeapon *GetLastWeapon();													// Sendprop
	void SetLastWeapon(BaseWeapon *weaponEnt);										// Sendprop

	BaseWeapon *GetWeapons(int id);													// Sendprop
	void SetWeapons(int id, BaseWeapon *weaponEnt);									// Sendprop
	int GetIndexInMyWeapons(BaseWeapon *weapon);
	int GetIndexInMyWeapons(const char *classname);	

	int GetTeam();
	void SetTeam(int team);

	int GetMVPs();																	// Sendprop
	void SetMVPs(int value);														// Sendprop

	int GetHideHUD();																// Sendprop
	void SetHideHUD(int value);														// Sendprop

	int GetAddon();																	// Sendprop
	void SetAddon(int value);														// Sendprop

	int GetPrimaryAddon();															// Sendprop
	void SetPrimaryAddon(int value);												// Sendprop

	int GetSecondaryAddon();														// Sendprop
	void SetSecondaryAddon(int value);												// Sendprop

	int GetWeaponAmmo(BaseWeapon *weaponEnt);										// Sendprop
	void SetWeaponAmmo(BaseWeapon *weaponEnt, int value);							// Sendprop

	int GetSpecMode();																// Sendprop
	BasePlayer *GetSpecTarget();													// Sendprop

	BaseEntity *GetGroundEntity();													// Sendprop

	int GetButtons();	
};

struct WeaponData
{
	int id;					// Id in the m_hMyWeapons field
	BaseEntity *entity;		// Entity
	const char *classname;	// Classname (no need to keep searching for it)

	WeaponData()
	{
		Reset();
	}

	WeaponData(BaseEntity *ent, int index)
	{
		Push(ent, index);
	}

	void Reset()
	{
		this->id = -1;
		this->entity = nullptr;
		this->classname = nullptr;
	}

	void Push(BaseEntity *ent, int index)
	{
		if( !ent )
		{
			return;
		}

		this->entity = ent;
		this->classname = ent->GetClassname();
		this->id = index;
	}
};

extern int GetItemSlot(const char *classname);

extern float GetFlameEntityLifeTime(BaseEntity *entity);							// Sendprop
extern void  SetFlameEntityLifeTime(BaseEntity *entity, float time);				// Sendprop

extern float GetEntityDissolveTime(BaseEntity *entity);								// Sendprop
extern void  SetEntityDissolveTime(BaseEntity *entity, float time);					// Sendprop

///////////////////	GIBS	 /////////////////////////
extern void SpawnGib(BaseEntity *gibEnt, const char *model);
extern void InitGib(BaseEntity *gibEnt, BasePlayer *victimEnt, float minSpeed, float maxSpeed);
extern void SpawnSpecificGibs(BasePlayer *victimEnt, int gibs, float minSpeed, float maxSpeed, const char *model, float lifeTime);

extern BaseProp *CreateEntityGlow(BaseEntity *entity, int style, Color color, const char *attachement);
extern BaseEntity *CreateParticleSystem(Vector pos, QAngle angles, const char *effect, BaseEntity *parentEnt = nullptr, const char *attachement = nullptr, float killDelay = -1.0f);

#endif 
