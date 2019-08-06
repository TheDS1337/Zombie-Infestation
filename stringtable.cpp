#include "stringtable.h"

static INetworkStringTable *g_pDownloadablesTable;
static INetworkStringTable *g_pSoundPrecacheTable;
static INetworkStringTable *g_pEffectDispatchTable;
static INetworkStringTable *g_pParticleEffectNamesTable;

void FindStringTables()
{
	g_pDownloadablesTable = g_pExtension->m_pNetworkStringTable->FindTable("downloadables");
	g_pSoundPrecacheTable = g_pExtension->m_pNetworkStringTable->FindTable("soundprecache");
	g_pEffectDispatchTable = g_pExtension->m_pNetworkStringTable->FindTable("EffectDispatch");
	g_pParticleEffectNamesTable = g_pExtension->m_pNetworkStringTable->FindTable("ParticleEffectNames");
}

void AddFileToDownloadsTable(const char *file)
{
	if( !g_pExtension->m_pNetworkStringTable )
	{
		CONSOLE_DEBUGGER("NetworkStringTable interface is not loaded yet.");
		return;
	}

	if( !g_pDownloadablesTable )
	{
		CONSOLE_DEBUGGER("Could not find 'downloadables' table");
		return;
	}

	bool save = g_pExtension->m_pEngineServer->LockNetworkStringTables(false);
	g_pDownloadablesTable->AddString(true, file);
	g_pExtension->m_pEngineServer->LockNetworkStringTables(save);	
}

void AddSoundToPrecacheTable(const char *sound)
{
	if( !g_pExtension->m_pNetworkStringTable )
	{
		CONSOLE_DEBUGGER("NetworkStringTable interface is not loaded yet.");
		return;
	}

	if( !g_pSoundPrecacheTable )
	{
		CONSOLE_DEBUGGER("Could not find 'soundprecache' table");
		return;
	}

	bool save = g_pExtension->m_pEngineServer->LockNetworkStringTables(false);

	g_pSoundPrecacheTable->AddString(true, sound);
	g_pExtension->m_pEngineServer->LockNetworkStringTables(save);
}

void AddEffectToParticleEffectNamesTable(const char *effect)
{
	if( !g_pExtension->m_pNetworkStringTable )
	{
		CONSOLE_DEBUGGER("NetworkStringTable interface is not loaded yet.");
		return;
	}

	if( !g_pParticleEffectNamesTable )
	{
		CONSOLE_DEBUGGER("Could not find 'ParticleEffectNames' table");
		return;
	}

	bool save = g_pExtension->m_pEngineServer->LockNetworkStringTables(false);

	g_pParticleEffectNamesTable->AddString(true, effect);
	g_pExtension->m_pEngineServer->LockNetworkStringTables(save);
}

const char *GetStringInTable(const char *tablename, int index)
{
	if( !g_pExtension->m_pNetworkStringTable )
	{
		CONSOLE_DEBUGGER("NetworkStringTable interface is not loaded yet.");
		return nullptr;
	}

	INetworkStringTable *table = g_pExtension->m_pNetworkStringTable->FindTable(tablename);

	if( table )
	{
		return table->GetString(index);
	}

	return nullptr;
}

int PrecacheParticleSystem(const char *name)
{
	AddEffectToParticleEffectNamesTable(name);

	static char path[128];
	ke::SafeSprintf(path, sizeof(path), "particles/%s.pcf", name);

	return g_pExtension->m_pEngineServer->PrecacheGeneric(path, true);
}

int PrecacheEffect(const char *name)
{
	if( !g_pExtension->m_pNetworkStringTable )
	{
		CONSOLE_DEBUGGER("NetworkStringTable interface is not loaded yet.");
		return -1;
	}

	if( !g_pEffectDispatchTable )
	{
		CONSOLE_DEBUGGER("Could not find 'EffectDispatch' table");
		return -1;
	}

	bool save = g_pExtension->m_pEngineServer->LockNetworkStringTables(false);

	int pos = g_pEffectDispatchTable->AddString(true, name);
	g_pExtension->m_pEngineServer->LockNetworkStringTables(save);

	return pos;
}