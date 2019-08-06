#ifndef _INCLUDE_ZI_ENTRY_PROPER_H_
#define _INCLUDE_ZI_ENTRY_PROPER_H_
#pragma once

#include "extension.h"
#include "zi_hooks.h"

struct HookReturn;
template<class Type> struct HookReturnValue;

class ZIPlayer;

class ZombieInfestation final: public IGameMod
{
public:
	const char *GetName() const;
	const char *GetDescription() const;
	const char *GetVersion() const;
	const char *GetTag() const;
	IGameConfig *GetConfig() const;

	bool OnMetamodLoad(ISmmAPI *ismm, char *error, unsigned int maxlength, bool late) override;
	bool OnMetamodUnload(char *error, unsigned int maxlength) override;

	bool OnLoad(char *error, unsigned int maxlength, bool late) override;
	void OnAllLoaded() override;
	void OnUnload() override;

	void OnCoreMapStart(edict_t *edictList, int edictCount, int clientMax) override;
	void OnCoreMapEnd() override;

	void OnClientConnected(int client) override;
	void OnClientPutInServer(IGamePlayer *gameplayer) override;
	void OnClientSettingsChanged(int client) override;
	void OnClientDisconnected(int client) override;

	void OnEntityCreated(BaseEntity *entity, const char *classname) override;
	void OnEntityDestroyed(BaseEntity *entity) override;

	void OnPreFireEvent(IGameEvent *event, bool &returnValue, META_RES &metaResult) override;
	void OnPostFireEvent(IGameEvent *event) override;

	bool OnPreClientCommand(edict_t *client, const CCommand &args) override;
	void OnPostClientCommand(edict_t *client, const CCommand &args) override;	

	// Extras
	bool OnPreEntityDestruction(BaseEntity *entity, const char *classname);
	bool OnPreTerminateRound(float &delay, CSGORoundEndReason &reason);
	void OnPostTerminateRound(float delay, CSGORoundEndReason reason);

	void OnPostRoundStartEvent(IGameEvent *event);	
	void OnPostGameStartEvent(IGameEvent *event);
	bool OnPreRoundEndEvent(IGameEvent *event, META_RES &metaResult);
	bool OnPreClientDeathEvent(IGameEvent *event, META_RES &metaResult);
	bool OnPreClientChangeTeamEvent(IGameEvent *event, META_RES &metaResult);
	void OnPostClientChangeNameEvent(IGameEvent *event);
	
	void OnPostClientSpawn(ZIPlayer *player);
	void OnPostClientThink(ZIPlayer *player);
	void OnPostClientRunCommand(ZIPlayer *player, CUserCmd *userCmd, IMoveHelper *moveHelper);
	HookReturn OnPreClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace);
	void OnPostClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace);
	HookReturnValue<int> OnPreClientTakeDamage(ZIPlayer *player, CTakeDamageInfo2 &info);
	void OnPostClientTakeDamage(ZIPlayer *player, CTakeDamageInfo2 &info);
	bool OnPreClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info);
	void OnPostClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info);
	HookReturnValue<bool> OnPreClientWeaponCanUse(ZIPlayer *player, BaseWeapon *weaponEnt);
	HookReturn OnPreClientWeaponEquip(ZIPlayer *player, BaseWeapon *weaponEnt);
	HookReturnValue<bool> OnPreClientWeaponSwitch(ZIPlayer *player, BaseWeapon *weaponEnt, int viewModelId);
	void  OnPostClientWeaponSwitch(ZIPlayer *player, BaseWeapon *weaponEnt, int viewModelId);

	HookReturn OnPreWeaponSetTransmit(BaseWeapon *weaponEnt, CCheckTransmitInfo *info, bool always);

	HookReturnValue<int> OnPreEngineEmitSound(IRecipientFilter& filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *sample,
		float volume, soundlevel_t soundlevel, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
		bool updatePositions, float soundtime, int speakerEntity, void *unknown);
	HookReturnValue<int> OnPreEngineEmitSound2(IRecipientFilter &filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *sample,
		float volume, float attenuation, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
		bool updatePositions, float soundtime, int speakerEntity, void *unknown);

	IGameConfig *m_pConfig;
};

extern ConVar g_BulletTimeActivationInterval;
extern ConVar g_BulletTimeActivationRate;
extern ConVar g_BulletTimeSpeed;
extern ConVar g_BulletTimeDuration;

extern ZombieInfestation g_ZombieInfestation;

#define GET_GAME_MOD() IGameMod *g_pGameMod = &g_ZombieInfestation;

#endif 
