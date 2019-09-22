#include "extension.h"

#if defined(GAMEMOD_GUNGAMESURF)
	#include "ggs/entry.h"
#elif defined (GAMEMOD_ZOMBIEINFESTATION)
	#include "zi/zi_entry.h"
#endif

GET_GAME_MOD();

Evolutionary g_Extension;
Evolutionary *g_pExtension = &g_Extension;		
SDKExtension *g_pExtensionIface = &g_Extension;

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *, const CCommand &);
SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool)

const char *Evolutionary::GetExtensionName()
{
	return g_pGameMod->GetName();
}

const char *Evolutionary::GetExtensionDescription()
{
	return g_pGameMod->GetDescription();
}

const char *Evolutionary::GetExtensionVerString()
{
	return g_pGameMod->GetVersion();
}

const char *Evolutionary::GetExtensionTag()
{
	return g_pGameMod->GetTag();
}

bool Evolutionary::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	m_pEngineServer = engine; 
	m_pServerGameDLL = gamedll; 
	
	GET_V_IFACE_ANY(GetServerFactory, m_pServerTools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION); 
	GET_V_IFACE_ANY(GetServerFactory, m_pServerGameClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS); 
	GET_V_IFACE_ANY(GetServerFactory, m_pServerGameEntities, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS); 
	GET_V_IFACE_CURRENT(GetEngineFactory, m_pServerPluginHelpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
	GET_V_IFACE_ANY(GetEngineFactory, m_pEngineTrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER); 
	GET_V_IFACE_ANY(GetEngineFactory, m_pEngineSound, IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION); 
	GET_V_IFACE_CURRENT(GetEngineFactory, m_pGameEventManager, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2); 
	GET_V_IFACE_CURRENT(GetEngineFactory, m_pConsoleVars, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, m_pNetworkStringTable, INetworkStringTableContainer, INTERFACENAME_NETWORKSTRINGTABLESERVER); 
	GET_V_IFACE_CURRENT(GetFileSystemFactory, m_pFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetPhysicsFactory, m_pPhysics, IPhysics, VPHYSICS_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetPhysicsFactory, m_pPhysicsSurfaceProps, IPhysicsSurfaceProps, VPHYSICS_SURFACEPROPS_INTERFACE_VERSION);

	g_pCVar = m_pConsoleVars;
	ConVar_Register(0, this);

	m_pGlobals = ismm->GetCGlobals();

	SH_ADD_HOOK(IServerGameClients, ClientCommand, m_pServerGameClients, SH_MEMBER(this, &Evolutionary::OnPreClientCommand), false);
	SH_ADD_HOOK(IServerGameClients, ClientCommand, m_pServerGameClients, SH_MEMBER(this, &Evolutionary::OnPostClientCommand), true);
	SH_ADD_HOOK(IGameEventManager2, FireEvent, m_pGameEventManager, SH_MEMBER(this, &Evolutionary::OnPreFireEvent), false);

	// Load the extras from our desired mod
	return g_pGameMod->OnMetamodLoad(ismm, error, maxlen, late);
}

bool Evolutionary::SDK_OnMetamodUnload(char *error, size_t maxlen)
{
	SH_REMOVE_HOOK(IServerGameClients, ClientCommand, m_pServerGameClients, SH_MEMBER(this, &Evolutionary::OnPreClientCommand), false);
	SH_REMOVE_HOOK(IServerGameClients, ClientCommand, m_pServerGameClients, SH_MEMBER(this, &Evolutionary::OnPostClientCommand), true);
	SH_REMOVE_HOOK(IGameEventManager2, FireEvent, m_pGameEventManager, SH_MEMBER(this, &Evolutionary::OnPreFireEvent), false);
	
	return g_pGameMod->OnMetamodUnload(error, maxlen);
}

bool Evolutionary::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	g_pShareSys->AddDependency(myself, "bintools.ext", true, true);
	g_pShareSys->AddDependency(myself, "sdkhooks.ext", true, true);

	char buffer[256];

	LOOKUP_FOR_CONFIG(m_pCoreConfig, "core.games");
	LOOKUP_FOR_CONFIG(m_pSDKConfig, "sdktools.games");
	LOOKUP_FOR_CONFIG(m_pHooksConfig, "sdkhooks.games");
	LOOKUP_FOR_CONFIG(m_pCStrikeConfig, "sm-cstrike.games");

	bool ret =  g_pGameMod->OnLoad(error, maxlength, late);

	// Loading here, just after our ZI config is finally loaded (UNTIL I make a global fix with configs being everywhere...)
	m_pEntityList = gamehelpers->GetGlobalEntityList();	

	return ret;
}

bool Evolutionary::QueryRunning(char *error, size_t maxlength)
{
	SM_CHECK_IFACE(BINTOOLS, m_pBinTools);
	SM_CHECK_IFACE(SDKHOOKS, m_pSDKHooks);
	SM_CHECK_IFACE(SDKTOOLS, m_pSDKTools);
	
	if( !m_pMenuStyle )
	{
		// Menu styles can be "default", "valve" or "radio" (use FindStyleByName(style) instead of GetDefaultStyle);
		m_pMenuStyle = menus->GetDefaultStyle();		
	}

	return true;
}

void Evolutionary::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(BINTOOLS, m_pBinTools);
	SM_GET_LATE_IFACE(SDKHOOKS, m_pSDKHooks);	
	SM_GET_LATE_IFACE(SDKTOOLS, m_pSDKTools);

	playerhelpers->AddClientListener(this);
	m_pSDKHooks->AddEntityListener(this);

	TempEntityHooker::Load();

	m_pMenuStyle = menus->GetDefaultStyle();	

	if( (m_pChatColors = adtfactory->CreateBasicTrie()) )
	{
		m_pChatColors->Insert("aliceblue", (void *) 0xF0F8FF);
		m_pChatColors->Insert("allies", (void *) 0x4D7942); // same as Allies team in DoD:S
		m_pChatColors->Insert("ancient", (void *) 0xEB4B4B); // same as Ancient item rarity in Dota 2
		m_pChatColors->Insert("antiquewhite", (void *) 0xFAEBD7);
		m_pChatColors->Insert("aqua", (void *) 0x00FFFF);
		m_pChatColors->Insert("aquamarine", (void *) 0x7FFFD4);
		m_pChatColors->Insert("arcana", (void *) 0xADE55C); // same as Arcana item rarity in Dota 2
		m_pChatColors->Insert("axis", (void *) 0xFF4040); // same as Axis team in DoD:S
		m_pChatColors->Insert("azure", (void *) 0x007FFF);
		m_pChatColors->Insert("beige", (void *) 0xF5F5DC);
		m_pChatColors->Insert("bisque", (void *) 0xFFE4C4);
		m_pChatColors->Insert("black", (void *) 0x000000);
		m_pChatColors->Insert("blanchedalmond", (void *) 0xFFEBCD);
		m_pChatColors->Insert("blue", (void *) 0x99CCFF); // same as BLU/Counter-Terrorist team color
		m_pChatColors->Insert("blueviolet", (void *) 0x8A2BE2);
		m_pChatColors->Insert("brown", (void *) 0xA52A2A);
		m_pChatColors->Insert("burlywood", (void *) 0xDEB887);
		m_pChatColors->Insert("cadetblue", (void *) 0x5F9EA0);
		m_pChatColors->Insert("chartreuse", (void *) 0x7FFF00);
		m_pChatColors->Insert("chocolate", (void *) 0xD2691E);
		m_pChatColors->Insert("collectors", (void *) 0xAA0000); // same as Collector's item quality in TF2
		m_pChatColors->Insert("common", (void *) 0xB0C3D9); // same as Common item rarity in Dota 2
		m_pChatColors->Insert("community", (void *) 0x70B04A); // same as Community item quality in TF2
		m_pChatColors->Insert("coral", (void *) 0xFF7F50);
		m_pChatColors->Insert("cornflowerblue", (void *) 0x6495ED);
		m_pChatColors->Insert("cornsilk", (void *) 0xFFF8DC);
		m_pChatColors->Insert("corrupted", (void *) 0xA32C2E); // same as Corrupted item quality in Dota 2
		m_pChatColors->Insert("crimson", (void *) 0xDC143C);
		m_pChatColors->Insert("cyan", (void *) 0x00FFFF);
		m_pChatColors->Insert("darkblue", (void *) 0x00008B);
		m_pChatColors->Insert("darkcyan", (void *) 0x008B8B);
		m_pChatColors->Insert("darkgoldenrod", (void *) 0xB8860B);
		m_pChatColors->Insert("darkgray", (void *) 0xA9A9A9);
		m_pChatColors->Insert("darkgrey", (void *) 0xA9A9A9);
		m_pChatColors->Insert("darkgreen", (void *) 0x006400);
		m_pChatColors->Insert("darkkhaki", (void *) 0xBDB76B);
		m_pChatColors->Insert("darkmagenta", (void *) 0x8B008B);
		m_pChatColors->Insert("darkolivegreen", (void *) 0x556B2F);
		m_pChatColors->Insert("darkorange", (void *) 0xFF8C00);
		m_pChatColors->Insert("darkorchid", (void *) 0x9932CC);
		m_pChatColors->Insert("darkred", (void *) 0x8B0000);
		m_pChatColors->Insert("darksalmon", (void *) 0xE9967A);
		m_pChatColors->Insert("darkseagreen", (void *) 0x8FBC8F);
		m_pChatColors->Insert("darkslateblue", (void *) 0x483D8B);
		m_pChatColors->Insert("darkslategray", (void *) 0x2F4F4F);
		m_pChatColors->Insert("darkslategrey", (void *) 0x2F4F4F);
		m_pChatColors->Insert("darkturquoise", (void *) 0x00CED1);
		m_pChatColors->Insert("darkviolet", (void *) 0x9400D3);
		m_pChatColors->Insert("deeppink", (void *) 0xFF1493);
		m_pChatColors->Insert("deepskyblue", (void *) 0x00BFFF);
		m_pChatColors->Insert("dimgray", (void *) 0x696969);
		m_pChatColors->Insert("dimgrey", (void *) 0x696969);
		m_pChatColors->Insert("dodgerblue", (void *) 0x1E90FF);
		m_pChatColors->Insert("exalted", (void *) 0xCCCCCD); // same as Exalted item quality in Dota 2
		m_pChatColors->Insert("firebrick", (void *) 0xB22222);
		m_pChatColors->Insert("floralwhite", (void *) 0xFFFAF0);
		m_pChatColors->Insert("forestgreen", (void *) 0x228B22);
		m_pChatColors->Insert("frozen", (void *) 0x4983B3); // same as Frozen item quality in Dota 2
		m_pChatColors->Insert("fuchsia", (void *) 0xFF00FF);
		m_pChatColors->Insert("fullblue", (void *) 0x0000FF);
		m_pChatColors->Insert("fullred", (void *) 0xFF0000);
		m_pChatColors->Insert("gainsboro", (void *) 0xDCDCDC);
		m_pChatColors->Insert("genuine", (void *) 0x4D7455); // same as Genuine item quality in TF2
		m_pChatColors->Insert("ghostwhite", (void *) 0xF8F8FF);
		m_pChatColors->Insert("gold", (void *) 0xFFD700);
		m_pChatColors->Insert("goldenrod", (void *) 0xDAA520);
		m_pChatColors->Insert("gray", (void *) 0xCCCCCC); // same as spectator team color
		m_pChatColors->Insert("grey", (void *) 0xCCCCCC);
		m_pChatColors->Insert("green", (void *) 0x3EFF3E);
		m_pChatColors->Insert("greenyellow", (void *) 0xADFF2F);
		m_pChatColors->Insert("haunted", (void *) 0x38F3AB); // same as Haunted item quality in TF2
		m_pChatColors->Insert("honeydew", (void *) 0xF0FFF0);
		m_pChatColors->Insert("hotpink", (void *) 0xFF69B4);
		m_pChatColors->Insert("immortal", (void *) 0xE4AE33); // same as Immortal item rarity in Dota 2
		m_pChatColors->Insert("indianred", (void *) 0xCD5C5C);
		m_pChatColors->Insert("indigo", (void *) 0x4B0082);
		m_pChatColors->Insert("ivory", (void *) 0xFFFFF0);
		m_pChatColors->Insert("khaki", (void *) 0xF0E68C);
		m_pChatColors->Insert("lavender", (void *) 0xE6E6FA);
		m_pChatColors->Insert("lavenderblush", (void *) 0xFFF0F5);
		m_pChatColors->Insert("lawngreen", (void *) 0x7CFC00);
		m_pChatColors->Insert("legendary", (void *) 0xD32CE6); // same as Legendary item rarity in Dota 2
		m_pChatColors->Insert("lemonchiffon", (void *) 0xFFFACD);
		m_pChatColors->Insert("lightblue", (void *) 0xADD8E6);
		m_pChatColors->Insert("lightcoral", (void *) 0xF08080);
		m_pChatColors->Insert("lightcyan", (void *) 0xE0FFFF);
		m_pChatColors->Insert("lightgoldenrodyellow", (void *) 0xFAFAD2);
		m_pChatColors->Insert("lightgray", (void *) 0xD3D3D3);
		m_pChatColors->Insert("lightgrey", (void *) 0xD3D3D3);
		m_pChatColors->Insert("lightgreen", (void *) 0x99FF99);
		m_pChatColors->Insert("lightpink", (void *) 0xFFB6C1);
		m_pChatColors->Insert("lightsalmon", (void *) 0xFFA07A);
		m_pChatColors->Insert("lightseagreen", (void *) 0x20B2AA);
		m_pChatColors->Insert("lightskyblue", (void *) 0x87CEFA);
		m_pChatColors->Insert("lightslategray", (void *) 0x778899);
		m_pChatColors->Insert("lightslategrey", (void *) 0x778899);
		m_pChatColors->Insert("lightsteelblue", (void *) 0xB0C4DE);
		m_pChatColors->Insert("lightyellow", (void *) 0xFFFFE0);
		m_pChatColors->Insert("lime", (void *) 0x00FF00);
		m_pChatColors->Insert("limegreen", (void *) 0x32CD32);
		m_pChatColors->Insert("linen", (void *) 0xFAF0E6);
		m_pChatColors->Insert("magenta", (void *) 0xFF00FF);
		m_pChatColors->Insert("maroon", (void *) 0x800000);
		m_pChatColors->Insert("mediumaquamarine", (void *) 0x66CDAA);
		m_pChatColors->Insert("mediumblue", (void *) 0x0000CD);
		m_pChatColors->Insert("mediumorchid", (void *) 0xBA55D3);
		m_pChatColors->Insert("mediumpurple", (void *) 0x9370D8);
		m_pChatColors->Insert("mediumseagreen", (void *) 0x3CB371);
		m_pChatColors->Insert("mediumslateblue", (void *) 0x7B68EE);
		m_pChatColors->Insert("mediumspringgreen", (void *) 0x00FA9A);
		m_pChatColors->Insert("mediumturquoise", (void *) 0x48D1CC);
		m_pChatColors->Insert("mediumvioletred", (void *) 0xC71585);
		m_pChatColors->Insert("midnightblue", (void *) 0x191970);
		m_pChatColors->Insert("mintcream", (void *) 0xF5FFFA);
		m_pChatColors->Insert("mistyrose", (void *) 0xFFE4E1);
		m_pChatColors->Insert("moccasin", (void *) 0xFFE4B5);
		m_pChatColors->Insert("mythical", (void *) 0x8847FF); // same as Mythical item rarity in Dota 2
		m_pChatColors->Insert("navajowhite", (void *) 0xFFDEAD);
		m_pChatColors->Insert("navy", (void *) 0x000080);
		m_pChatColors->Insert("normal", (void *) 0xB2B2B2); // same as Normal item quality in TF2
		m_pChatColors->Insert("oldlace", (void *) 0xFDF5E6);
		m_pChatColors->Insert("olive", (void *) 0x9EC34F);
		m_pChatColors->Insert("olivedrab", (void *) 0x6B8E23);
		m_pChatColors->Insert("orange", (void *) 0xFFA500);
		m_pChatColors->Insert("orangered", (void *) 0xFF4500);
		m_pChatColors->Insert("orchid", (void *) 0xDA70D6);
		m_pChatColors->Insert("palegoldenrod", (void *) 0xEEE8AA);
		m_pChatColors->Insert("palegreen", (void *) 0x98FB98);
		m_pChatColors->Insert("paleturquoise", (void *) 0xAFEEEE);
		m_pChatColors->Insert("palevioletred", (void *) 0xD87093);
		m_pChatColors->Insert("papayawhip", (void *) 0xFFEFD5);
		m_pChatColors->Insert("peachpuff", (void *) 0xFFDAB9);
		m_pChatColors->Insert("peru", (void *) 0xCD853F);
		m_pChatColors->Insert("pink", (void *) 0xFFC0CB);
		m_pChatColors->Insert("plum", (void *) 0xDDA0DD);
		m_pChatColors->Insert("powderblue", (void *) 0xB0E0E6);
		m_pChatColors->Insert("purple", (void *) 0x800080);
		m_pChatColors->Insert("rare", (void *) 0x4B69FF); // same as Rare item rarity in Dota 2
		m_pChatColors->Insert("red", (void *) 0xFF4040); // same as RED/Terrorist team color
		m_pChatColors->Insert("rosybrown", (void *) 0xBC8F8F);
		m_pChatColors->Insert("royalblue", (void *) 0x4169E1);
		m_pChatColors->Insert("saddlebrown", (void *) 0x8B4513);
		m_pChatColors->Insert("salmon", (void *) 0xFA8072);
		m_pChatColors->Insert("sandybrown", (void *) 0xF4A460);
		m_pChatColors->Insert("seagreen", (void *) 0x2E8B57);
		m_pChatColors->Insert("seashell", (void *) 0xFFF5EE);
		m_pChatColors->Insert("selfmade", (void *) 0x70B04A); // same as Self-Made item quality in TF2
		m_pChatColors->Insert("sienna", (void *) 0xA0522D);
		m_pChatColors->Insert("silver", (void *) 0xC0C0C0);
		m_pChatColors->Insert("skyblue", (void *) 0x87CEEB);
		m_pChatColors->Insert("slateblue", (void *) 0x6A5ACD);
		m_pChatColors->Insert("slategray", (void *) 0x708090);
		m_pChatColors->Insert("slategrey", (void *) 0x708090);
		m_pChatColors->Insert("snow", (void *) 0xFFFAFA);
		m_pChatColors->Insert("springgreen", (void *) 0x00FF7F);
		m_pChatColors->Insert("steelblue", (void *) 0x4682B4);
		m_pChatColors->Insert("strange", (void *) 0xCF6A32); // same as Strange item quality in TF2
		m_pChatColors->Insert("tan", (void *) 0xD2B48C);
		m_pChatColors->Insert("teal", (void *) 0x008080);
		m_pChatColors->Insert("thistle", (void *) 0xD8BFD8);
		m_pChatColors->Insert("tomato", (void *) 0xFF6347);
		m_pChatColors->Insert("turquoise", (void *) 0x40E0D0);
		m_pChatColors->Insert("uncommon", (void *) 0xB0C3D9); // same as Uncommon item rarity in Dota 2
		m_pChatColors->Insert("unique", (void *) 0xFFD700); // same as Unique item quality in TF2
		m_pChatColors->Insert("unusual", (void *) 0x8650AC); // same as Unusual item quality in TF2
		m_pChatColors->Insert("valve", (void *) 0xA50F79); // same as Valve item quality in TF2
		m_pChatColors->Insert("vintage", (void *) 0x476291); // same as Vintage item quality in TF2
		m_pChatColors->Insert("violet", (void *) 0xEE82EE);
		m_pChatColors->Insert("wheat", (void *) 0xF5DEB3);
		m_pChatColors->Insert("white", (void *) 0xFFFFFF);
		m_pChatColors->Insert("whitesmoke", (void *) 0xF5F5F5);
		m_pChatColors->Insert("yellow", (void *) 0xFFFF00);
		m_pChatColors->Insert("yellowgreen", (void *) 0x9ACD32);
	}

	g_pGameMod->OnAllLoaded();
}

void Evolutionary::SDK_OnUnload()
{
	g_pGameMod->OnUnload();

	playerhelpers->RemoveClientListener(this);
	m_pSDKHooks->RemoveEntityListener(this);

	CLOSE_CONFIG(m_pSDKConfig);
	CLOSE_CONFIG(m_pHooksConfig);
	CLOSE_CONFIG(m_pCStrikeConfig);

	TempEntityHooker::Free();
	
	RELEASE_CALL_WRAPPERS(m_pBinCallWrappers);	
	
	if( m_pChatColors )
	{
		m_pChatColors->Clear();
		m_pChatColors->Destroy();
		m_pChatColors = nullptr;
	}	
 }

void Evolutionary::OnCoreMapStart(edict_t *edictList, int edictCount, int clientMax)
{
	static bool gotOffsets = false;

	if( !gotOffsets )
	{
		gotOffsets = GetSendPropOffsets();
	}

	m_pGameRulesProxyEnt = BaseEntity::FindEntityByNetClass(clientMax, "CCSGameRulesProxy");	

	if( m_pSDKTools->GetGameRules() && m_pGameRulesProxyEnt )
	{
		// Get offsets
		static bool gotGameRulesOffsets = false;

		if( !gotGameRulesOffsets )
		{
			gotGameRulesOffsets = GetGameRulesOffsets();
		}
	}

	FindStringTables();
	
	g_pGameMod->OnCoreMapStart(edictList, edictCount, clientMax);	
}

void Evolutionary::OnCoreMapEnd()
{
	g_pGameMod->OnCoreMapEnd();		
}

void Evolutionary::OnPreClientCommand(edict_t *client, const CCommand &args)
{
	if( strncmp(args[0], "name", 4) == 0 )
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(g_pGameMod->OnPreClientCommand(client, args) ? MRES_IGNORED : MRES_SUPERCEDE);
}

void Evolutionary::OnPostClientCommand(edict_t *client, const CCommand &args)
{
	g_pGameMod->OnPostClientCommand(client, args);
}

void Evolutionary::OnClientConnected(int client)
{
	// Setting up the hud messages...
	HudManager::ResetClientHud(client);

	g_pGameMod->OnClientConnected(client);
}

void Evolutionary::OnClientPutInServer(int client)
{
	IGamePlayer *player = playerhelpers->GetGamePlayer(client);

	if( !player )
	{
		return;
	}

	g_pGameMod->OnClientPutInServer(player);
}

void Evolutionary::OnClientDisconnected(int client)
{
	g_pGameMod->OnClientDisconnected(client);
}

void Evolutionary::OnClientSettingsChanged(int client)
{
	g_pGameMod->OnClientSettingsChanged(client);
}

void Evolutionary::OnEntityCreated(CBaseEntity *entity, const char *classname)
{
	if( !entity )
	{
		return;
	}

	// Prefer this to be done here for all entities, rather than OnClientPutInServer (involves the players entity only)
	static bool gotOffsets = false;

	if( !gotOffsets )
	{
		gotOffsets = GetDataMapOffsets((BaseEntity *) entity);		
	}	

	g_pGameMod->OnEntityCreated((BaseEntity *) entity, classname);
}

void Evolutionary::OnEntityDestroyed(CBaseEntity *entity)
{
	if( !entity )
	{
		return;
	}

	g_pGameMod->OnEntityDestroyed((BaseEntity *) entity);
}

void Evolutionary::FireGameEvent(IGameEvent *event)
{
	if( !event )
	{
		return;
	}

	g_pGameMod->OnPostFireEvent(event);
}

int Evolutionary::GetEventDebugID()
{
	return EVENT_DEBUG_ID_INIT;
}

bool Evolutionary::OnPreFireEvent(IGameEvent *event, bool dontBroadcast)
{
	if( !event )
	{
		RETURN_META_VALUE(MRES_IGNORED, false);
	}

	bool returnValue = true; META_RES metaResult = MRES_IGNORED;

	// Check if this actually works
	g_pGameMod->OnPreFireEvent(event, returnValue, metaResult);

	// TODO: change this, make it like the zi_hooks system...
	if( metaResult == MRES_HANDLED )
	{
		RETURN_META_VALUE_NEWPARAMS(metaResult, returnValue, &IGameEventManager2::FireEvent, (event, dontBroadcast));
	}
	else
	{
		RETURN_META_VALUE(metaResult, returnValue);
	}
}

bool Evolutionary::RegisterConCommandBase(ConCommandBase *cvar)
{
	return META_REGCVAR(cvar);
}

bool Evolutionary::GetDataMapOffsets(BaseEntity *entity)
{
	datamap_t *datamap = gamehelpers->GetDataMap((CBaseEntity *) entity);

	if( !datamap )
	{
		return false;
	}

	bool gotOffsets = true;
	sm_datatable_info_t info;

	GET_DATAMAP_OFFSET(m_hOwner);
	GET_DATAMAP_OFFSET(m_pParent);
	GET_DATAMAP_OFFSET(m_flDissolveStartTime);
	GET_DATAMAP_OFFSET(m_iClassname);
	GET_DATAMAP_OFFSET(m_ModelName);
	GET_DATAMAP_OFFSET(m_vecVelocity);
	GET_DATAMAP_OFFSET(m_vecAbsVelocity);
	GET_DATAMAP_OFFSET(m_flGravity);
	GET_DATAMAP_OFFSET(m_nButtons);
	GET_DATAMAP_OFFSET(m_MoveType);	
	GET_DATAMAP_OFFSET(m_nSolidType);
	GET_DATAMAP_OFFSET(m_usSolidFlags);
	GET_DATAMAP_OFFSET(m_CollisionGroup);
	GET_DATAMAP_OFFSET(m_fFlags);
	GET_DATAMAP_OFFSET(m_aThinkFunctions);
	GET_DATAMAP_OFFSET(m_pfnThink);
	GET_DATAMAP_OFFSET(m_nNextThinkTick);
	GET_DATAMAP_OFFSET(m_takedamage);

	// Used for storing infos
	GET_DATAMAP_OFFSET(m_OnUser1);
	GET_DATAMAP_OFFSET(m_OnUser2);
	GET_DATAMAP_OFFSET(m_OnUser3);
	GET_DATAMAP_OFFSET(m_OnUser4);	

	return gotOffsets;
}

bool Evolutionary::GetSendPropOffsets()
{
	bool gotOffsets = true;
	sm_sendprop_info_t info;

	// Players only
	GET_SENDPROP_OFFSET(CBasePlayer, m_iHealth);
	GET_SENDPROP_OFFSET(CCSPlayer, m_ArmorValue);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bHasHeavyArmor);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bHasHelmet);
	GET_SENDPROP_OFFSET(CBasePlayer, m_flLaggedMovementValue);	// m_flLaggedMovementValue "CBasePlayer" instead of "m_flVelocityModifier" "CCSPlayer" (console spams)
	GET_SENDPROP_OFFSET(CCSPlayer, m_iFOV);
	GET_SENDPROP_OFFSET(CCSPlayer, m_iDefaultFOV);	
	GET_SENDPROP_OFFSET(CCSPlayer, m_bHasNightVision);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bNightVisionOn);
	GET_SENDPROP_OFFSET(CCSPlayer, m_szArmsModel); 
	GET_SENDPROP_OFFSET(CBasePlayer, m_hViewModel);
	GET_SENDPROP_OFFSET(CCSPlayer, m_hRagdoll);
	GET_SENDPROP_OFFSET(CBasePlayer, m_hActiveWeapon);	
	GET_SENDPROP_OFFSET(CBasePlayer, m_hLastWeapon);
	GET_SENDPROP_OFFSET(CBasePlayer, m_hMyWeapons);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bHasDefuser);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bInBuyZone);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bSpotted);
	GET_SENDPROP_OFFSET(CBasePlayer, m_iObserverMode);
	GET_SENDPROP_OFFSET(CBasePlayer, m_hObserverTarget);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bIsHoldingLookAtWeapon);
	GET_SENDPROP_OFFSET(CCSPlayer, m_iAmmo);
	GET_SENDPROP_OFFSET(CCSPlayer, m_hGroundEntity);
	GET_SENDPROP_OFFSET(CBasePlayer, m_iHideHUD);
	GET_SENDPROP_OFFSET(CCSPlayer, m_iAddonBits);
	GET_SENDPROP_OFFSET(CCSPlayer, m_iPrimaryAddon);
	GET_SENDPROP_OFFSET(CCSPlayer, m_iSecondaryAddon);
	GET_SENDPROP_OFFSET(CCSPlayer, m_flVelocityModifier);
	GET_SENDPROP_OFFSET(CCSPlayer, m_bIsScoped);

	// Weapons only
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_nViewModelIndex);
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_iViewModelIndex);
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_iWorldModelIndex);
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_iClip1);
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_iClip2);
	GET_SENDPROP_OFFSET(CBaseCombatWeapon, m_iPrimaryReserveAmmoCount);	

	// Grenades
	GET_SENDPROP_OFFSET(CBaseCSGrenadeProjectile, m_hThrower);
	GET_SENDPROP_OFFSET(CBaseCSGrenadeProjectile, m_flDamage);
	GET_SENDPROP_OFFSET(CBaseCSGrenadeProjectile, m_DmgRadius);
	GET_SENDPROP_OFFSET(CBaseCSGrenadeProjectile, m_bIsLive);
	GET_SENDPROP_OFFSET(CBaseCSGrenadeProjectile, m_vInitialVelocity);

	// Dynamic props
	GET_SENDPROP_OFFSET(CDynamicProp, m_bShouldGlow);
	GET_SENDPROP_OFFSET(CDynamicProp, m_nGlowStyle);
	GET_SENDPROP_OFFSET(CDynamicProp, m_clrGlow);
	GET_SENDPROP_OFFSET(CDynamicProp, m_flGlowMaxDist);

	// View models
	GET_SENDPROP_OFFSET(CBaseViewModel, m_nViewModelIndex2);	
	
	// Involves all entities
	GET_SENDPROP_OFFSET(CBaseEntity, m_hOwnerEntity);
	GET_SENDPROP_OFFSET(CEntityFlame, m_flLifetime);
	GET_SENDPROP_OFFSET(CBaseEntity, m_spawnflags);
	GET_SENDPROP_OFFSET(CBaseEntity, m_clrRender);
	GET_SENDPROP_OFFSET(CBaseEntity, m_nRenderFX);
	GET_SENDPROP_OFFSET(CBaseEntity, m_nRenderMode);
	GET_SENDPROP_OFFSET(CBaseEntity, m_fEffects);
	GET_SENDPROP_OFFSET(CBaseEntity, m_vecOrigin);
	GET_SENDPROP_OFFSET(CBaseEntity, m_nModelIndex);	
	GET_SENDPROP_OFFSET(CBaseAnimating, m_nSkin);
	GET_SENDPROP_OFFSET(CBaseAnimating, m_nBody);
	GET_SENDPROP_OFFSET(CBaseAnimating, m_nSequence);
	GET_SENDPROP_OFFSET(CBaseAnimating, m_flPlaybackRate);
		
	return gotOffsets;
}

bool Evolutionary::GetGameRulesOffsets()
{
	bool gotOffsets = true;
	sm_sendprop_info_t info;

	GET_GAMERULES_OFFSET(m_iRoundTime);
	GET_GAMERULES_OFFSET(m_fRoundStartTime);
	GET_GAMERULES_OFFSET(m_bCTCantBuy);

	return gotOffsets;
}