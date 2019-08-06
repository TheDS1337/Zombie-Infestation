#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_

/* Basic information exposed publicly */
//#define SMEXT_CONF_NAME			g_pGameMod->GetName()
//#define SMEXT_CONF_DESCRIPTION	"Evolutionary extension"
//#define SMEXT_CONF_VERSION		g_pGameMod->GetVersion()
#define SMEXT_CONF_AUTHOR		"Amine \"DS\" BOUSSOUBEL"
#define SMEXT_CONF_URL			"Steam Profile: http://steamcommunity.com/profiles/76561198146350143"
//#define SMEXT_CONF_LOGTAG		g_pGameMod->GetTag()
#define SMEXT_CONF_LICENSE		"GPL"
#define SMEXT_CONF_DATESTRING	__DATE__

#define SMEXT_CONF_METAMOD		

/** Enable interfaces you want to use here by uncommenting lines */
#define SMEXT_ENABLE_FORWARDSYS
#define SMEXT_ENABLE_HANDLESYS
#define SMEXT_ENABLE_PLAYERHELPERS
//#define SMEXT_ENABLE_DBMANAGER
#define SMEXT_ENABLE_GAMECONF
//#define SMEXT_ENABLE_MEMUTILS
#define SMEXT_ENABLE_GAMEHELPERS
#define SMEXT_ENABLE_TIMERSYS
//#define SMEXT_ENABLE_THREADER
#define SMEXT_ENABLE_LIBSYS
#define SMEXT_ENABLE_MENUS
#define SMEXT_ENABLE_ADTFACTORY
//#define SMEXT_ENABLE_PLUGINSYS
#define SMEXT_ENABLE_ADMINSYS
//#define SMEXT_ENABLE_TEXTPARSERS
#define SMEXT_ENABLE_USERMSGS
//#define SMEXT_ENABLE_TRANSLATOR
#define SMEXT_ENABLE_ROOTCONSOLEMENU

#endif