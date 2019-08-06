#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#pragma once

#include "sdk/smsdk_ext.h"
#include <toolframework/itoolentity.h>
#include <IEngineTrace.h>
#include <IEngineSound.h>
#include <iserver.h>
#include <server_class.h>
#include <igameevents.h>
#include <iplayerinfo.h>
#include <filesystem.h>
#include <usercmd.h>
#include <in_buttons.h>
#include <ai_activity.h>
#include <extensions/IBinTools.h>
#include <extensions/ISDKHooks.h>
#include <extensions/ISDKTools.h>
#include <tier1/convar.h>
#include <tier1/utlvector.h>
#include <mathlib/vector.h>
#include <vstdlib/random.h>
#include "CDetour/detours.h"
#include "CellRecipientFilter.h"
#include "hud_manager.h"
#include "sh_string.h"
#include "sh_vector.h"
#include "sh_list.h"

// Here begins our journey
#include "IGameMod.h"
#include "stringtable.h"
#include "takedamageinfo2.h"
#include "sound.h"
#include "gameutils.h"
#include "gamerules.h"
#include "tempentities.h"
#include "user_messages.h"
#include "macros.h"

//#define GAMEMOD_GUNGAMESURF
#define GAMEMOD_ZOMBIEINFESTATION

class Evolutionary final: public SDKExtension,
						  public IClientListener,
						  public ISMEntityListener,
						  public IGameEventListener2,
						  public IConCommandBaseAccessor
{
public:
	/** Returns name */
	const char *GetExtensionName();	

	/** Returns description string */
	const char *GetExtensionDescription();

	/** Returns log tag */
	const char *GetExtensionTag();	

	/** Returns version string */
	const char *GetExtensionVerString();	

	/**
	 * @brief This is called after the initial loading sequence has been processed.
	 *
	 * @param error		Error message buffer.
	 * @param maxlen	Size of error message buffer.
	 * @param late		Whether or not the module was loaded after map load.
	 * @return			True to succeed loading, false to fail.
	 */
	bool SDK_OnLoad(char *error, size_t maxlength, bool late) override;
	
	/**
	 * @brief This is called right before the extension is unloaded.
	 */
	void SDK_OnUnload() override;

	/**
	 * @brief This is called once all known extensions have been loaded.
	 * Note: It is is a good idea to add natives here, if any are provided.
	 */
	void SDK_OnAllLoaded() override;

	/**
	 * @brief Called when the pause state is changed.
	 */
	//virtual void SDK_OnPauseChange(bool paused);

	/**
	 * @brief this is called when Core wants to know if your extension is working.
	 *
	 * @param error		Error message buffer.
	 * @param maxlen	Size of error message buffer.
	 * @return			True if working, false otherwise.
	 */
	bool QueryRunning(char *error, size_t maxlength) override;
public:
#if defined SMEXT_CONF_METAMOD
	/**
	 * @brief Called when Metamod is attached, before the extension version is called.
	 *
	 * @param error			Error buffer.
	 * @param maxlen		Maximum size of error buffer.
	 * @param late			Whether or not Metamod considers this a late load.
	 * @return				True to succeed, false to fail.
	 */
	bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) override;

	/**
	 * @brief Called when Metamod is detaching, after the extension version is called.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param error			Error buffer.
	 * @param maxlen		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	bool SDK_OnMetamodUnload(char *error, size_t maxlen) override;

	/**
	 * @brief Called when Metamod's pause state is changing.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param paused		Pause state being set.
	 * @param error			Error buffer.
	 * @param maxlen		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlen);
#endif

	// IExtensionSys
	void OnCoreMapStart(edict_t *edictList, int edictCount, int clientMax) override;
	void OnCoreMapEnd() override;

	// IVEngineServer
	void OnPreClientCommand(edict_t *client, const CCommand &args);
	void OnPostClientCommand(edict_t *, const CCommand &args);

	// IClientListener
	void OnClientConnected(int client) override;
	void OnClientPutInServer(int client) override;
//	void OnClientDisconnecting(int client) override;
	void OnClientDisconnected(int client) override;/*
	void OnClientAuthorized(int client, const char *authstring) override;
	void OnServerActivated(int max_clients) override;
	void OnClientPostAdminCheck(int client) override;
	void OnMaxPlayersChanged(int newvalue) override;*/
	void OnClientSettingsChanged(int client) override;	

	// ISMEntityListener
	void OnEntityCreated(CBaseEntity *entity, const char *classname) override;
	void OnEntityDestroyed(CBaseEntity *entity) override;

	// IGameEventListener2
	void FireGameEvent(IGameEvent *event) override;
	int GetEventDebugID() override;

	// IConCommandBaseAccessor
	bool RegisterConCommandBase(ConCommandBase *cvar) override;

	bool OnPreFireEvent(IGameEvent *event, bool dontBroadcast);	

	// IEngineSound
/*	int OnPreEngineEmitSound(IRecipientFilter& filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *sample, 
		float volume, soundlevel_t soundlevel, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins, 
		bool updatePositions, float soundtime, int speakerEntity, void *unknown);
	int OnPreEngineEmitSound2(IRecipientFilter &filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *sample,
		float volume, float attenuation, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins, 
		bool updatePositions, float soundtime, int speakerEntity, void *unknown);
	void OnPreEngineEmitAmbientSound(int entityindex, const Vector &position, const char *sample, float volume, soundlevel_t soundlevel, int flags, 
		int pitch, float delay);*/
		
public:

	IVEngineServer *m_pEngineServer;
	IServerGameDLL *m_pServerGameDLL;
	IServerTools *m_pServerTools;
	IServerGameClients *m_pServerGameClients;
	IServerGameEnts *m_pServerGameEntities;
	IServerPluginHelpers *m_pServerPluginHelpers;
	IEngineTrace *m_pEngineTrace;
	IEngineSound *m_pEngineSound;	
	IGameEventManager2 *m_pGameEventManager;
	ICvar *m_pConsoleVars;
	INetworkStringTableContainer *m_pNetworkStringTable;
	IFileSystem *m_pFileSystem;
	IBinTools *m_pBinTools;
	ISDKHooks *m_pSDKHooks;
	ISDKTools *m_pSDKTools;

	CGlobalVars *m_pGlobals;

	IGameConfig *m_pCoreConfig;
	IGameConfig *m_pSDKConfig;
	IGameConfig *m_pHooksConfig;
	IGameConfig *m_pCStrikeConfig;
	
	IMenuStyle *m_pMenuStyle;

	IBasicTrie *m_pChatColors;
	
	SourceHook::List<ICallWrapper *> m_pBinCallWrappers;

	// Vars that shouldn't be redefined everytime...
	
	// Offsets
	unsigned int m_iClassname;
	unsigned int m_iHealth;
	unsigned int m_ArmorValue;
	unsigned int m_bHasHeavyArmor;
	unsigned int m_bHasHelmet;
	unsigned int m_flLaggedMovementValue;
	unsigned int m_flGravity;
	unsigned int m_iFOV;
	unsigned int m_iDefaultFOV;
	unsigned int m_bHasNightVision;
	unsigned int m_bNightVisionOn;
	unsigned int m_szArmsModel;
	unsigned int m_hViewModel;
	unsigned int m_hRagdoll;
	unsigned int m_hActiveWeapon;
	unsigned int m_hLastWeapon;
	unsigned int m_hMyWeapons;
	unsigned int m_bHasDefuser;
	unsigned int m_bInBuyZone;
	unsigned int m_bSpotted;
	unsigned int m_iObserverMode;
	unsigned int m_hObserverTarget;
	unsigned int m_bIsHoldingLookAtWeapon;	
	unsigned int m_nViewModelIndex;	             
	unsigned int m_iViewModelIndex;
	unsigned int m_iWorldModelIndex;
	unsigned int m_iClip1;
	unsigned int m_iClip2;
	unsigned int m_iAmmo;
	unsigned int m_iPrimaryReserveAmmoCount;
	unsigned int m_hGroundEntity;
	unsigned int m_nButtons;
	unsigned int m_MoveType;
	unsigned int m_nSolidType;
	unsigned int m_usSolidFlags;
	unsigned int m_CollisionGroup;
	unsigned int m_fFlags;	
	unsigned int m_spawnflags;
	unsigned int m_clrRender;
	unsigned int m_nRenderFX;
	unsigned int m_nRenderMode;
	unsigned int m_fEffects;
	unsigned int m_hOwner;
	unsigned int m_hOwnerEntity;
	unsigned int m_pParent;
	unsigned int m_flLifetime;
	unsigned int m_flDissolveStartTime;
	unsigned int m_vecOrigin;
	unsigned int m_vecVelocity;
	unsigned int m_vecAbsVelocity;
	unsigned int m_aThinkFunctions;
	unsigned int m_pfnThink;
	unsigned int m_nNextThinkTick;
	unsigned int m_nModelIndex;
	unsigned int m_ModelName;
	unsigned int m_nSkin;
	unsigned int m_nBody;
	unsigned int m_iHideHUD;
	unsigned int m_iAddonBits;
	unsigned int m_iPrimaryAddon;
	unsigned int m_iSecondaryAddon;
	unsigned int m_flVelocityModifier;
	unsigned int m_takedamage;
	unsigned int m_OnUser1;
	unsigned int m_OnUser2;
	unsigned int m_OnUser3;
	unsigned int m_OnUser4;

	// Grenades	
	unsigned int m_hThrower;
	unsigned int m_flDamage;
	unsigned int m_DmgRadius;
	unsigned int m_bIsLive;
	unsigned int m_vInitialVelocity;

	// Dynamic props
	unsigned int m_bShouldGlow;
	unsigned int m_nGlowStyle;
	unsigned int m_clrGlow;
	unsigned int m_flGlowMaxDist;

	// View Models
	unsigned int m_nViewModelIndex2;
	unsigned int m_nSequence;
	unsigned int m_flPlaybackRate;

	// GameRuleZZ	
	BaseEntity *m_pGameRulesProxyEnt;

	// Offsets
	unsigned int m_iRoundTime;
	unsigned int m_fRoundStartTime;
	unsigned int m_bCTCantBuy;

	// Global Entity list (see entitylist.h/cpp)
	void *m_pEntityList;
		
private:
	
	// Temporary, definitely a TODO
	bool GetDataMapOffsets(BaseEntity *entity);
	bool GetSendPropOffsets();

	bool GetGameRulesOffsets();
};

extern Evolutionary *g_pExtension;

// TODO: Remove this
extern FILE *g_pDebugFile;

#endif 