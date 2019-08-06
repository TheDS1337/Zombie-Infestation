#include "zi_hooks.h"
#include "zi_core.h"
#include "zi_humans.h"
#include "zi_human_hunter.h"
#include "zi_item_tripmine.h"
#include "zi_item_jetpack_bazooka.h"
#include "zi_item_rage.h"
#include "zi_round_modes.h"
#include "zi_environment.h"
#include "zi_timers.h"
#include "zi_weapons.h"
#include "zi_nades.h"
#include "zi_commands.h"

static ConCommand *g_pSayCommand = nullptr, *g_pSayTeamCommand = nullptr;

DETOUR_HOOK_DECLARE(TerminateRound);

#if defined _WINDOWS
DETOUR_DECL_MEMBER3(TerminateRound, void, CSGORoundEndReason, reason, int, unknown, int, unknown2)
{
	float delay;
	__asm movss delay, xmm1;
#elif defined _LINUX
	DETOUR_DECL_MEMBER4(TerminateRound, void, float, delay, CSGORoundEndReason, reason, int, unknown, int, unknown2)
{
#endif

	if( !g_ZombieInfestation.OnPreTerminateRound(delay, reason) )
	{
		return;
	}

#if defined _WINDOWS
	__asm movss xmm1, delay;
	DETOUR_MEMBER_CALL(TerminateRound)(reason, unknown, unknown2);
#elif defined _LINUX
	DETOUR_MEMBER_CALL(TerminateRound)(delay, reason, unknown, unknown2);
#endif

	g_ZombieInfestation.OnPostTerminateRound(delay, reason);
}

class CCSBot;

DETOUR_HOOK_DECLARE(CanControlBot);
DETOUR_DECL_MEMBER2(CanControlBot, bool, CCSBot *, bot, bool, unknown)
{
	return false;
}

DETOUR_HOOK_DECLARE(Event_Killed);
DETOUR_DECL_MEMBER1(Event_Killed, void, CTakeDamageInfo2 &, info)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) this);

	if( !player )
	{
		return;
	}

	bool value = false, latestValue = false;

	// This needs to be here other wise the player wouldn't be registered dead, since Event_Killed calls TerminatesRound before executing the post hooks
	DETOUR_CHECK_RETURN(g_ZombieInfestation.OnPreClientDeath(player, info));
	DETOUR_CHECK_RETURN(g_HunterHuman.OnPreClientDeath(player, info));			// This should be here, right before we dissolve the body

	if( value )
	{
		DETOUR_MEMBER_CALL(Event_Killed)(info);	

		// Post death	
		g_ZombieInfestation.OnPostClientDeath(player, info);

		// Items
		g_TripmineItem.OnPostClientDeath(player);
		g_JetpackBazookaItem.OnPostClientDeath(player);
		g_RageItem.OnPostClientDeath(player);
	}
}

DETOUR_HOOK_DECLARE(Remove);

#if defined _WINDOWS
DETOUR_DECL_MEMBER0(Remove, void)
{
	BaseEntity *entity = (BaseEntity *) (((IServerNetworkable *) this)->GetBaseEntity());
#elif defined _LINUX
DETOUR_DECL_STATIC1(Remove, void, BaseEntity *, entity)
{
#endif
	if( !entity )
	{
		return;
	}

	bool value = false, latestValue = false;

	const char *classname = entity->GetClassname();

	DETOUR_CHECK_RETURN(g_ZombieInfestation.OnPreEntityDestruction(entity, classname));
	DETOUR_CHECK_RETURN(g_TripmineItem.OnPreEntityDestruction(entity, classname));
		
	if( value )
	{
#if defined _WINDOWS
		DETOUR_MEMBER_CALL(Remove)();
#elif defined _LINUX
		DETOUR_STATIC_CALL(Remove)(entity);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnPostClientSpawn();
SH_DECL_MANUALHOOK0_void(ClientSpawn, 0, 0, 0)

static void OnPostClientThink();
SH_DECL_MANUALHOOK0_void(ClientPostThink, 0, 0, 0)

static void OnPostClientRunCommand(CUserCmd *, IMoveHelper *);
SH_DECL_MANUALHOOK2_void(ClientRunCommand, 0, 0, 0, CUserCmd *, IMoveHelper *)

static void OnPreClientTraceAttack(CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace);
static void OnPostClientTraceAttack(CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace);
SH_DECL_MANUALHOOK3_void(ClientTraceAttack, 0, 0, 0, CTakeDamageInfo2 &, const Vector &, trace_t *)

static int OnPreClientTakeDamage(CTakeDamageInfo2 &info);
static int OnPostClientTakeDamage(CTakeDamageInfo2 &info);
SH_DECL_MANUALHOOK1(ClientTakeDamage, 0, 0, 0, int, CTakeDamageInfo2 &)

static bool OnPreClientWeaponCanUse(BaseWeapon *);
SH_DECL_MANUALHOOK1(ClientWeaponCanUse, 0, 0, 0, bool, BaseWeapon *)

static void OnPreClientWeaponEquip(BaseWeapon *weaponEnt);
SH_DECL_MANUALHOOK1_void(ClientWeaponEquip, 0, 0, 0, BaseWeapon *)

static bool OnPreClientWeaponSwitch(BaseWeapon *weaponEnt, int viewmodelId);
static bool OnPostClientWeaponSwitch(BaseWeapon *weaponEnt, int viewmodelId);
SH_DECL_MANUALHOOK2(ClientWeaponSwitch, 0, 0, 0, bool, BaseWeapon *, int)

static void OnPreWeaponSetTransmit(CCheckTransmitInfo *info, bool always);
SH_DECL_MANUALHOOK2_void(WeaponSetTransmit, 0, 0, 0, CCheckTransmitInfo *, bool)

class CEconItemView;

static void OnPostProjectileSpawn();
SH_DECL_MANUALHOOK0_void(ProjectileSpawn, 0, 0, 0)

static void OnPreProjectileThink();
SH_DECL_MANUALHOOK0_void(ProjectileThink, 0, 0, 0)

static void OnPostProjectileStartTouch(BaseEntity *other);
SH_DECL_MANUALHOOK1_void(ProjectileStartTouch, 0, 0, 0, BaseEntity *)

static void OnPreProjectileDetonate();
SH_DECL_MANUALHOOK0_void(ProjectileDetonate, 0, 0, 0)

static void OnPreEngineLightStyle(int style, const char *lightStyle);
SH_DECL_HOOK2_void(IVEngineServer, LightStyle, SH_NOATTRIB, 0, int, const char *)

static int OnPreEngineEmitSound(IRecipientFilter& filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, soundlevel_t soundlevel, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown);
SH_DECL_HOOK18(IEngineSound, EmitSound, SH_NOATTRIB, 0, int, IRecipientFilter &, int, int, const char *, unsigned int, const char *, float, float, int, int, int, const Vector *, const Vector *, CUtlVector<Vector> *, bool, float, int, void *)

static int OnPreEngineEmitSound2(IRecipientFilter &filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, float attenuation, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown);
SH_DECL_HOOK18(IEngineSound, EmitSound, SH_NOATTRIB, 1, int, IRecipientFilter &, int, int, const char *, unsigned int, const char *, float, soundlevel_t, int, int, int, const Vector *, const Vector *, CUtlVector<Vector> *, bool, float, int, void *)


static void OnPreSetCommandClient(int clientId);
SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, 0, int)
SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, 0, const CCommand &);

static void OnPreClientFakeCommand(edict_t *client, const char *cmd);
SH_DECL_HOOK2_void(IServerPluginHelpers, ClientCommand, SH_NOATTRIB, 0, edict_t *, const char *)

static void OnPreConVarChange(ConVar *cvar, const char *oldString, float oldValue);
SH_DECL_HOOK3_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, 0, ConVar *, const char *, float)

static void OnPostClientSpawn()
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	g_ZombieInfestation.OnPostClientSpawn(player);		
}

static void OnPostClientThink()
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	g_ZombieInfestation.OnPostClientThink(player);
	g_HunterHuman.OnPostClientThink(player);	
}

static void OnPostClientRunCommand(CUserCmd *userCmd, IMoveHelper *moveHelper)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	g_ZombieInfestation.OnPostClientRunCommand(player, userCmd, moveHelper);
	g_JetpackBazookaItem.OnPostClientRunCommand(player, userCmd, moveHelper);	
}

static void OnPreClientTraceAttack(CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	HookReturn value, latestValue;

	HOOK_CHECK_RETURN_VOID(g_ZombieInfestation.OnPreClientTraceAttack(player, info, direction, trace));
	HOOK_CHECK_RETURN_VOID(g_RageItem.OnPreClientTraceAttack(player, info));

	if( value.modified )
	{
		RETURN_META_MNEWPARAMS(value.ret, ClientTraceAttack, (info, direction, trace));
	}
	else
	{
		RETURN_META(value.ret);
	}
}

static void OnPostClientTraceAttack(CTakeDamageInfo2 &info, const Vector &direction, trace_t *trace)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	g_ZombieInfestation.OnPostClientTraceAttack(player, info, direction, trace);		
}

static int OnPreClientTakeDamage(CTakeDamageInfo2 &info)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META_VALUE(MRES_IGNORED, 0);
	}

	HookReturnValue<int> value = g_ZombieInfestation.OnPreClientTakeDamage(player, info);

	if( value.modified )
	{
		RETURN_META_VALUE_MNEWPARAMS(value.ret, value.typeret, ClientTakeDamage, (info));
	}
	else
	{
		RETURN_META_VALUE(value.ret, value.typeret);
	}

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

static int OnPostClientTakeDamage(CTakeDamageInfo2 &info)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META_VALUE(MRES_IGNORED, 0);
	}

	g_ZombieInfestation.OnPostClientTakeDamage(player, info);

	RETURN_META_VALUE(MRES_IGNORED, 0);
}

static bool OnPreClientWeaponCanUse(BaseWeapon *weaponEnt)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META_VALUE(MRES_IGNORED, true);
	}

	HookReturnValue<bool> value = g_ZombieInfestation.OnPreClientWeaponCanUse(player, weaponEnt);

	if( value.modified )
	{
		RETURN_META_VALUE_MNEWPARAMS(value.ret, value.typeret, ClientWeaponCanUse, (weaponEnt));
	}
	else
	{
		RETURN_META_VALUE(value.ret, value.typeret);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

static void OnPreClientWeaponEquip(BaseWeapon *weaponEnt)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	HookReturn value = g_ZombieInfestation.OnPreClientWeaponEquip(player, weaponEnt);

	if( value.modified )
	{
		RETURN_META_MNEWPARAMS(value.ret, ClientWeaponEquip, (weaponEnt));
	}
	else
	{
		RETURN_META(value.ret);
	}
}

static bool OnPreClientWeaponSwitch(BaseWeapon *weaponEnt, int viewmodelId)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META_VALUE(MRES_IGNORED, true);
	}

	HookReturnValue<bool> value = g_ZombieInfestation.OnPreClientWeaponSwitch(player, weaponEnt, viewmodelId);

	if( value.modified )
	{
		RETURN_META_VALUE_MNEWPARAMS(value.ret, value.typeret, ClientWeaponSwitch, (weaponEnt, viewmodelId));
	}
	else
	{
		RETURN_META_VALUE(value.ret, value.typeret);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

static bool OnPostClientWeaponSwitch(BaseWeapon *weaponEnt, int viewmodelId)
{
	ZIPlayer *player = ZIPlayer::Find((BasePlayer *) META_IFACEPTR(CBaseEntity));

	if( !player )
	{
		RETURN_META_VALUE(MRES_IGNORED, true);
	}

	g_ZombieInfestation.OnPostClientWeaponSwitch(player, weaponEnt, viewmodelId);			

	RETURN_META_VALUE(MRES_IGNORED, true);
}

static void OnPostProjectileSpawn()
{
	BaseGrenade *nadeEnt = (BaseGrenade *) META_IFACEPTR(CBaseEntity);

	HookReturn value;
	value.ret = MRES_IGNORED;
	value.modified = false;

	ZIPlayer *owner = ZIPlayer::Find((BasePlayer *) nadeEnt->GetOwnerEntity());

	if( !owner )
	{
		RETURN_META(MRES_IGNORED);
	}

	const char *classname = nadeEnt->GetClassname();

	if( strncmp(classname, "hegrenade", 9) == 0 )
	{
		ZINades::OnPostHEGrenadeSpawn(nadeEnt, owner);
	}
	else if( strncmp(classname, "flashbang", 9) == 0 )
	{
		ZINades::OnPostFlashbangSpawn(nadeEnt, owner);
	}
	else if( strncmp(classname, "smokegrenade", 12) == 0 )
	{
		//ZINades::OnPostSmokeGrenadeSpawn(nadeEnt, owner);
	}
	else if( strncmp(classname, "molotov", 7) == 0 || strncmp(classname, "incgrenade", 10) == 0 )
	{
		ZINades::OnPostMolotovSpawn(nadeEnt, owner);
	}
	else if( strncmp(classname, "decoy", 5) == 0 )
	{
		ZINades::OnPostDecoySpawn(nadeEnt, owner);
	}
	else if( strncmp(classname, "tagrenade", 9) == 0 )
	{
		ZINades::OnPostTAGrenadeSpawn(nadeEnt, owner);
	}
}

static void OnPreProjectileThink()
{
	BaseGrenade *nadeEnt = (BaseGrenade *) META_IFACEPTR(CBaseEntity);

	HookReturn value;
	value.ret = MRES_IGNORED;
	value.modified = false;

	ZIPlayer *owner = ZIPlayer::Find((BasePlayer *) nadeEnt->GetOwnerEntity());

	// If the owner doesn't exist, block the thinking anyways... this way entities won't explode or do any other stupid thing
	if( !owner )
	{
		RETURN_META(MRES_IGNORED);
	}

	if( ZICore::m_IsRoundEnd )
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	const char *classname = nadeEnt->GetClassname();

	if( strncmp(classname, "flashbang", 9) == 0 )
	{
		value = ZINades::OnPreFlashbangThink(nadeEnt, owner);
	}
	else if( strncmp(classname, "smokegrenade", 12) == 0 )
	{
		//value = ZINades::OnPreSmokeGrenadeThink(nadeEnt, owner);
	}
	else if( strncmp(classname, "decoy", 5) == 0 )
	{
		value = ZINades::OnPreDecoyThink(nadeEnt, owner);
	}
	else if( strncmp(classname, "tagrenade", 9) == 0 )
	{
		value = ZINades::OnPreTAGrenadeThink(nadeEnt, owner);
	}

	RETURN_META(value.ret);
}

static void OnPostProjectileStartTouch(BaseEntity *otherEnt)
{
	BaseGrenade *nadeEnt = (BaseGrenade *) META_IFACEPTR(CBaseEntity);

	HookReturn value;
	value.ret = MRES_IGNORED;
	value.modified = false;

	ZIPlayer *owner = ZIPlayer::Find((BasePlayer *) nadeEnt->GetOwnerEntity());

	// If no owner is available, or is round end, then just keep the entity, don't kill it.. but it'll be removed by round end
	if( !owner )
	{
		RETURN_META(MRES_IGNORED);
	}

	if( ZICore::m_IsRoundEnd )
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	const char *classname = nadeEnt->GetClassname();

	if( strncmp(classname, "hegrenade", 9) == 0 )
	{
		value = ZINades::OnPostHEGrenadeStartTouch(nadeEnt, owner, otherEnt);
	}
	else if( strncmp(classname, "flashbang", 9) == 0 )
	{
		value = ZINades::OnPostFlashbangStartTouch(nadeEnt, owner, otherEnt);
	}
	else if( strncmp(classname, "decoy", 5) == 0 )
	{
		value = ZINades::OnPostDecoyStartTouch(nadeEnt, owner, otherEnt);
	}
	else if( strncmp(classname, "tagrenade", 9) == 0 )
	{
		value = ZINades::OnPostTAGrenadeStartTouch(nadeEnt, owner, otherEnt);
	}

	RETURN_META(value.ret);
}

static void OnPreProjectileDetonate()
{
	BaseGrenade *nadeEnt = (BaseGrenade *) META_IFACEPTR(CBaseEntity);

	HookReturn value;
	value.ret = MRES_IGNORED;
	value.modified = false;

	ZIPlayer *owner = ZIPlayer::Find((BasePlayer *) nadeEnt->GetOwnerEntity());

	if( !owner )
	{
		RETURN_META(MRES_IGNORED);
	}

	const char *classname = nadeEnt->GetClassname();

	if( strncmp(classname, "smokegrenade", 12) == 0 )
	{
		//value = ZINades::OnPreSmokeGrenadeDetonate(nadeEnt, owner);
	}
	else if( strncmp(classname, "decoy", 5) == 0 )
	{
		value = ZINades::OnPreDecoyDetonate(nadeEnt, owner);
	}

	RETURN_META(value.ret);
}

static void OnPreWeaponSetTransmit(CCheckTransmitInfo *info, bool always)
{
	BaseWeapon *weaponEnt = (BaseWeapon *) META_IFACEPTR(CBaseEntity);

	HookReturn value = g_ZombieInfestation.OnPreWeaponSetTransmit(weaponEnt, info, always);

	if( value.modified )
	{
		RETURN_META_MNEWPARAMS(value.ret, WeaponSetTransmit, (info, always));
	}
	else
	{
		RETURN_META(value.ret);
	}
}

static void OnPreEngineLightStyle(int style, const char *lightStyle)
{
	if( lightStyle[0] == 'b' || lightStyle[1] != '\0' )
	{
		RETURN_META(MRES_IGNORED);
	}

	lightStyle = (char *) STRING(LIGHT_STYLE);	

	RETURN_META_NEWPARAMS(MRES_HANDLED, &IVEngineServer::LightStyle, (style, lightStyle));
}

static int OnPreEngineEmitSound(IRecipientFilter& filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, soundlevel_t soundlevel, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown)
{
/*
	ZIPlayer *player = ZIPlayer::Find(entityIndex, false);

	if( player )
	{	
	}

	RETURN_META_VALUE_NEWPARAMS(MRES_IGNORED, -1, static_cast<int (IEngineSound::*)(IRecipientFilter &, int, int, const char *, unsigned int, const char *, float,
	soundlevel_t, int, int, int, const Vector *, const Vector *, CUtlVector<Vector> *, bool, float, int, void *)> (&IEngineSound::EmitSound), (filter, entityIndex,
	channel, sound, GenerateSoundHash(newSample), newSample, volume, soundlevel, seed, flags, pitch, origin, direction, origins, updatePositions, soundtime, speakerEntity, nullptr));
*/
	RETURN_META_VALUE(MRES_IGNORED, -1);
}

static int OnPreEngineEmitSound2(IRecipientFilter &filter, int entityIndex, int channel, const char *sound, unsigned int soundHash, const char *soundFile,
	float volume, float attenuation, int seed, int flags, int pitch, const Vector *origin, const Vector *direction, CUtlVector<Vector> *origins,
	bool updatePositions, float soundtime, int speakerEntity, void *unknown)
{
/*
	ZIPlayer *player = ZIPlayer::Find(entityIndex, false);

	if( player )
	{		
	}


	RETURN_META_VALUE_NEWPARAMS(MRES_IGNORED, -1,static_cast<int (IEngineSound::*)(IRecipientFilter &, int, int, const char *, unsigned int, const char *, float,
	float, int, int, int, const Vector *, const Vector *, CUtlVector<Vector> *, bool, float, int, void *)> (&IEngineSound::EmitSound), (filter, entityIndex,
	channel, sound, soundHash, sample, volume, attenuation, seed, flags, pitch, origin, direction, origins, updatePositions, soundtime, speakerEntity, unknown));
*/
	RETURN_META_VALUE(MRES_IGNORED, -1);
}

static void OnPreSetCommandClient(int clientId)
{
	ZICommands::m_Commander = clientId + 1;
	RETURN_META(MRES_IGNORED);
}

static void OnPreClientFakeCommand(edict_t *client, const char *cmd)
{
	ZIPlayer *player = ZIPlayer::Find(client);

	if( !player )
	{
		return;
	}	

	bool ret = true;

	RETURN_META(ret ? MRES_IGNORED : MRES_SUPERCEDE);
}

static void OnPreConVarChange(ConVar *cvar, const char *oldString, float oldValue)
{
	// cvar holds the new values, if it's equal to the old one then we got no right to be here.
	if( strcmp(cvar->GetString(), oldString) == 0 )
	{
		RETURN_META(MRES_IGNORED);
	}

	const char *cvarName = cvar->GetName();
	float delay = -1.0f;

	if( strcmp(cvarName, "mp_restartgame") == 0 )
	{
		delay = cvar->GetFloat();

		if( delay > 0 )
		{
			CONSOLE_DEBUGGER("Round is going to be restarted in %d secs, clearing everything out...", delay);

			ZICore::m_Score[RoundModeWinner_Humans] = 0;
			ZICore::m_Score[RoundModeWinner_Zombies] = 0;

			// Forgot about the last mode, new game = new life
			ZICore::m_LastMode = nullptr;

			TerminateRound(delay, CSGORoundEnd_Draw);

			// Block the change, otherwise the termination would be registered twice!
			RETURN_META(MRES_SUPERCEDE);
		}
	}
	else if( strcmp(cvarName, "mp_restartround") == 0 )
	{
		delay = cvar->GetFloat();

		if( delay > 0 )
		{
			CONSOLE_DEBUGGER("Round is going to be restarted in %d secs, clearing everything out...", delay);

			TerminateRound(delay, CSGORoundEnd_Draw);

			// Block the change, otherwise the termination would be registered twice!
			RETURN_META(MRES_SUPERCEDE);
		}
	}
}

static void OnPreSayCommmad(const CCommand &cmd)
{
	ZIPlayer *player = ZIPlayer::Find(ZICommands::m_Commander, false);

	if( !player )
	{
		RETURN_META(MRES_IGNORED);
	}

	const char *firstArg = cmd.Arg(1);

	if( !firstArg )
	{
		RETURN_META(MRES_IGNORED);
	}

	// Only allowing '!', '/' and '.' commands
	if( firstArg[0] != '!' && firstArg[0] != '/' && firstArg[0] != '.' )
	{
		RETURN_META(MRES_IGNORED);
	}	

	RETURN_META(ZICommands::OnClientSayCommand(player, (CCommand &) cmd) ? MRES_IGNORED : MRES_SUPERCEDE);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ZIHooks::m_VGUIMenuMsg = -1;

ZIUserMessagesCallback ZIHooks::m_UserMessagesCallback;

void ZIUserMessagesCallback::VGUIMenu::OnUserMessage(int msgId, google::protobuf::Message *msg, IRecipientFilter *filter)
{
}

ResultType ZIUserMessagesCallback::VGUIMenu::InterceptUserMessage(int msgId, google::protobuf::Message *msg, IRecipientFilter *filter)
{
	int *players = nullptr;

	if( _FillInPlayers(&players, filter) < 1 )
	{
		return Pl_Continue;
	}

	ZIPlayer *player = ZIPlayer::Find(players[0], false);

	if( !player )
	{
		return Pl_Continue;
	}
/*
	const google::protobuf::FieldDescriptor *field = msg->GetDescriptor()->FindFieldByName("name");

	if( field )
	{
		char name[32];

		std::string scratch;
		ke::SafeStrcpy(name, sizeof(name), msg->GetReflection()->GetStringReference(*msg, field, &scratch).c_str());

		CONSOLE_DEBUGGER("Sending VGUI UM: %s", name);

		CCSUsrMsg_VGUIMenu *VGUImsg = (CCSUsrMsg_VGUIMenu *) msg;
		
		if( strcmp(name, "team") == 0 || (VGUImsg && VGUImsg->show()) )
		{
			CONSOLE_DEBUGGER("Displaying ZI menu");

			if( player->m_AllowTeamChoosing )
			{
				return Pl_Continue;
			}			

			if( VGUImsg )
			{
				VGUImsg->set_show(false);
				player->ShowMainMenu();

				return Pl_Stop;
			}
		}
	}
*/
	return Pl_Continue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ZIHooks g_Hooks;

void ZIHooks::SetupDetours()
{
	void *address = nullptr;
	
	DETOUR_HOOK_CREATE_MEMBER(TerminateRound, "CCSGameRules::TerminateRound");
	DETOUR_HOOK_CREATE_MEMBER(CanControlBot, "CCSPlayer::CanControlBot");
	DETOUR_HOOK_CREATE_MEMBER(Event_Killed, "CCSPlayer::Event_Killed");
#if defined _WINDOWS
	DETOUR_HOOK_CREATE_MEMBER(Remove, "IServerNetworkable::Remove");
#elif defined _LINUX
	DETOUR_HOOK_CREATE_STATIC(Remove, "IServerNetworkable::Remove");
#endif	
}

void ZIHooks::ReleaseDetours()
{
	DETOUR_HOOK_RELEASE(TerminateRound);
	DETOUR_HOOK_RELEASE(CanControlBot);
	DETOUR_HOOK_RELEASE(Event_Killed);
	DETOUR_HOOK_RELEASE(Remove);	
}

void ZIHooks::GetOffsets()
{
	int offset = 0;

	// Players
	SH_MANUAL_HOOK_GET_OFFSET(ClientSpawn, "Spawn");
	SH_MANUAL_HOOK_GET_OFFSET(ClientPostThink, "PostThink");
	SH_MANUAL_HOOK_GET_OFFSET(ClientRunCommand, "PlayerRunCmd");
	SH_MANUAL_HOOK_GET_OFFSET(ClientTraceAttack, "TraceAttack");
	SH_MANUAL_HOOK_GET_OFFSET(ClientTakeDamage, "OnTakeDamage");
	SH_MANUAL_HOOK_GET_OFFSET(ClientWeaponCanUse, "Weapon_CanUse");
	SH_MANUAL_HOOK_GET_OFFSET(ClientWeaponEquip, "Weapon_Equip");
	SH_MANUAL_HOOK_GET_OFFSET(ClientWeaponSwitch, "Weapon_Switch");	

	// Weapons
	SH_MANUAL_HOOK_GET_OFFSET(WeaponSetTransmit, "SetTransmit");
	
	// Projectiles
	SH_MANUAL_HOOK_GET_OFFSET(ProjectileSpawn, "Spawn");
	SH_MANUAL_HOOK_GET_OFFSET(ProjectileThink, "Think");
	SH_MANUAL_HOOK_GET_OFFSET(ProjectileStartTouch, "StartTouch");
	SH_MANUAL_HOOK_GET_OFFSET(ProjectileDetonate, "CBaseCSGrenadeProjectile::Detonate");	

	SetupDetours();

	// Items
	g_TripmineItem.GetOffsets();
	g_JetpackBazookaItem.GetOffsets();
}

void ZIHooks::AttachToServer()
{
	CDetourManager::Init(g_pSM->GetScriptingEngine(), nullptr);

	SH_ADD_HOOK(IVEngineServer, LightStyle, g_pExtension->m_pEngineServer, SH_STATIC(OnPreEngineLightStyle), false);

	SH_ADD_HOOK(IEngineSound, EmitSound, g_pExtension->m_pEngineSound, SH_STATIC(OnPreEngineEmitSound), false);
	SH_ADD_HOOK(IEngineSound, EmitSound, g_pExtension->m_pEngineSound, SH_STATIC(OnPreEngineEmitSound2), false);

	SH_ADD_HOOK(IServerGameClients, SetCommandClient, g_pExtension->m_pServerGameClients, SH_STATIC(OnPreSetCommandClient), false);
	SH_ADD_HOOK(IServerPluginHelpers, ClientCommand, g_pExtension->m_pServerPluginHelpers, SH_STATIC(OnPreClientFakeCommand), false);
	SH_ADD_HOOK(ICvar, CallGlobalChangeCallbacks, g_pExtension->m_pConsoleVars, SH_STATIC(OnPreConVarChange), false);

	g_pSayCommand = g_pExtension->m_pConsoleVars->FindCommand("say");
	g_pSayTeamCommand = g_pExtension->m_pConsoleVars->FindCommand("say_team");	

	if( g_pSayCommand )
	{
		SH_ADD_HOOK(ConCommand, Dispatch, g_pSayCommand, SH_STATIC(OnPreSayCommmad), false);
	}		

	if( g_pSayTeamCommand )
	{
		SH_ADD_HOOK(ConCommand, Dispatch, g_pSayTeamCommand, SH_STATIC(OnPreSayCommmad), false);
	}	

	// Usermessages
/*	m_VGUIMenuMsg = usermsgs->GetMessageIndex("VGUIMenu");

	if( m_VGUIMenuMsg != -1 && usermsgs->HookUserMessage(m_VGUIMenuMsg, &m_UserMessagesCallback.m_VGUIMenu, true) )
	{
		CONSOLE_DEBUGGER("Hooked VGUIMenu UM");
	}
	else
	{
		CONSOLE_DEBUGGER("Unable to hook VGUIMenu UM");
	}
*/

	GetOffsets();
}

void ZIHooks::AttachToClient(BasePlayer *clientEnt)
{
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PostSpawn, clientEnt, SH_ADD_MANUALVPHOOK(ClientSpawn, clientEnt, SH_STATIC(OnPostClientSpawn), true));
	SH_MANUAL_HOOK_CREATE(CBasePlayer_PostThink, clientEnt, SH_ADD_MANUALVPHOOK(ClientPostThink, clientEnt, SH_STATIC(OnPostClientThink), true));
	SH_MANUAL_HOOK_CREATE(CBasePlayer_RunCommand, clientEnt, SH_ADD_MANUALVPHOOK(ClientRunCommand, clientEnt, SH_STATIC(OnPostClientRunCommand), true));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PreTraceAttack, clientEnt, SH_ADD_MANUALVPHOOK(ClientTraceAttack, clientEnt, SH_STATIC(OnPreClientTraceAttack), false));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PostTraceAttack, clientEnt, SH_ADD_MANUALVPHOOK(ClientTraceAttack, clientEnt, SH_STATIC(OnPostClientTraceAttack), true));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PreTakeDamage, clientEnt, SH_ADD_MANUALVPHOOK(ClientTakeDamage, clientEnt, SH_STATIC(OnPreClientTakeDamage), false));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PostTakeDamage, clientEnt, SH_ADD_MANUALVPHOOK(ClientTakeDamage, clientEnt, SH_STATIC(OnPostClientTakeDamage), true));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PreWeaponCanUse, clientEnt, SH_ADD_MANUALVPHOOK(ClientWeaponCanUse, clientEnt, SH_STATIC(OnPreClientWeaponCanUse), false));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PreWeaponEquip, clientEnt, SH_ADD_MANUALVPHOOK(ClientWeaponEquip, clientEnt, SH_STATIC(OnPreClientWeaponEquip), false));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PreWeaponSwitch, clientEnt, SH_ADD_MANUALVPHOOK(ClientWeaponSwitch, clientEnt, SH_STATIC(OnPreClientWeaponSwitch), false));
	SH_MANUAL_HOOK_CREATE(CBaseCombatCharacter_PostWeaponSwitch, clientEnt, SH_ADD_MANUALVPHOOK(ClientWeaponSwitch, clientEnt, SH_STATIC(OnPostClientWeaponSwitch), true));
}

void ZIHooks::AttachToWeapon(BaseWeapon *weaponEnt)
{
//	SH_MANUAL_HOOK_CREATE(CBaseCombatWeapon_PreSetTransmit, SH_ADD_MANUALVPHOOK(SetTransmit, weaponEnt, SH_STATIC(OnPreWeaponSetTransmit), false));
}

void ZIHooks::OnPostProjectileCreation(BaseGrenade *nadeEnt, const char *classname)
{
	// Breachcharge doesnt agree on some offsets, we'll fix that later
	if( strstr(classname, "breachcharge") )
	{
		return;
	}

	SH_MANUAL_HOOK_CREATE(CBaseCSGrenadeProjectile_PostSpawn, nadeEnt, SH_ADD_MANUALVPHOOK(ProjectileSpawn, nadeEnt, SH_STATIC(OnPostProjectileSpawn), true));
	SH_MANUAL_HOOK_CREATE(CBaseCSGrenadeProjectile_PreThink, nadeEnt, SH_ADD_MANUALVPHOOK(ProjectileThink, nadeEnt, SH_STATIC(OnPreProjectileThink), false));	
	SH_MANUAL_HOOK_CREATE(CBaseCSGrenadeProjectile_PostStartTouch, nadeEnt, SH_ADD_MANUALVPHOOK(ProjectileStartTouch, nadeEnt, SH_STATIC(OnPostProjectileStartTouch), true));
	SH_MANUAL_HOOK_CREATE(CBaseCSGrenadeProjectile_PreDetonate, nadeEnt, SH_ADD_MANUALVPHOOK(ProjectileDetonate, nadeEnt, SH_STATIC(OnPreProjectileDetonate), false));
		
	if( strncmp(classname, "hegrenade", 9) == 0 )
	{
		ZINades::OnPostHEGrenadeCreation(nadeEnt);
	}
	else if( strncmp(classname, "flashbang", 9) == 0 )
	{
		ZINades::OnPostFlashbangCreation(nadeEnt);
	}
	else if( strncmp(classname, "smokegrenade", 12) == 0 )
	{
		//ZINades::OnPostSmokeGrenadeCreation(nadeEnt);
	}
	else if( strncmp(classname, "molotov", 7) == 0 || strncmp(classname, "incgrenade", 10) == 0 )
	{
		ZINades::OnPostMolotovCreation(nadeEnt);
	}
	else if( strncmp(classname, "decoy", 5) == 0 )
	{
		ZINades::OnPostDecoyCreation(nadeEnt);
	}	
	else if( strncmp(classname, "tagrenade", 9) == 0 )
	{
		ZINades::OnPostTAGrenadeCreation(nadeEnt);
	}
}

bool ZIHooks::OnPreProjectileDestruction(BaseGrenade *nadeEnt, const char *classname)
{
	// Breachcharge doesnt agree on some offsets, we'll fix that later
	if( strstr(classname, "breachcharge") )
	{
		return true;
	}

	ZINades::OnPreNadeRemoval(nadeEnt);

	return true;
}

void ZIHooks::Release()
{
	ReleaseDetours();

	SH_REMOVE_HOOK(IVEngineServer, LightStyle, g_pExtension->m_pEngineServer, SH_STATIC(OnPreEngineLightStyle), false);

	SH_REMOVE_HOOK(IEngineSound, EmitSound, g_pExtension->m_pEngineSound, SH_STATIC(OnPreEngineEmitSound), false);
	SH_REMOVE_HOOK(IEngineSound, EmitSound, g_pExtension->m_pEngineSound, SH_STATIC(OnPreEngineEmitSound2), false);

	SH_REMOVE_HOOK(IServerGameClients, SetCommandClient, g_pExtension->m_pServerGameClients, SH_STATIC(OnPreSetCommandClient), false);
	SH_REMOVE_HOOK(IServerPluginHelpers, ClientCommand, g_pExtension->m_pServerPluginHelpers, SH_STATIC(OnPreClientFakeCommand), false);
	SH_REMOVE_HOOK(ICvar, CallGlobalChangeCallbacks, g_pExtension->m_pConsoleVars, SH_STATIC(OnPreConVarChange), false);

	if( g_pSayCommand )
	{
		SH_REMOVE_HOOK(ConCommand, Dispatch, g_pSayCommand, SH_STATIC(OnPreSayCommmad), false);
	}

	if( g_pSayTeamCommand )
	{
		SH_REMOVE_HOOK(ConCommand, Dispatch, g_pSayTeamCommand, SH_STATIC(OnPreSayCommmad), false);
	}	

	RELEASE_POINTERS_ARRAY(EntityHook::totalHooks);

/*
	if( m_VGUIMenuMsg != -1 && usermsgs->UnhookUserMessage(m_VGUIMenuMsg, &m_UserMessagesCallback.m_VGUIMenu, true) )
	{
		CONSOLE_DEBUGGER("Unhooking VGUIMenu UM");
	}
	else
	{
		CONSOLE_DEBUGGER("Unable to unhook VGUIMenu UM");
	}
*/
}
