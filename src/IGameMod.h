#ifndef _INCLUDE_IGAMEMOD_PROPER_H_
#define _INCLUDE_IGAMEMOD_PROPER_H_
#pragma once

#include "extension.h"

class BaseEntity;

class IGameMod
{
public:
	virtual const char *GetName() const = 0; 
	virtual const char *GetDescription() const = 0;
	virtual const char *GetVersion() const = 0;
	virtual const char *GetTag() const = 0;
	virtual IGameConfig *GetConfig() const = 0;

	virtual bool OnMetamodLoad(ISmmAPI *ismm, char *error, unsigned int maxlength, bool late) = 0;
	virtual bool OnMetamodUnload(char *error, unsigned int maxlength) = 0;

	virtual bool OnLoad(char *error, unsigned int maxlength, bool late) = 0;
	virtual void OnAllLoaded() = 0;
	virtual void OnUnload() = 0;

	virtual void OnCoreMapStart(edict_t *edictList, int edictCount, int clientMax) = 0;
	virtual void OnCoreMapEnd() = 0;

	virtual void OnClientConnected(int client) = 0;
	virtual void OnClientPutInServer(IGamePlayer *gameplayer) = 0;
	virtual void OnClientSettingsChanged(int client) = 0;
	virtual void OnClientDisconnected(int client) = 0;

	virtual void OnEntityCreated(BaseEntity *entity, const char *classname) = 0;
	virtual void OnEntityDestroyed(BaseEntity *entity) = 0;

	virtual void OnPreFireEvent(IGameEvent *event, bool &returnValue, META_RES &metaResult) = 0;
	virtual void OnPostFireEvent(IGameEvent *event) = 0;	

	virtual bool OnPreClientCommand(edict_t *client, const CCommand &args) = 0;
	virtual void OnPostClientCommand(edict_t *client, const CCommand &args) = 0;
};

extern IGameMod *g_pGameMod;

#endif 