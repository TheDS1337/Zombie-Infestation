#ifndef _INCLUDE_ENTITY_HOOKER_PROPER_H_
#define _INCLUDE_ENTITY_HOOKER_PROPER_H_
#pragma once

#include "extension.h"

class EntityHook
{
private:
	int hookId;
	char classname[32];
	char name[64];
	int hookedEntitiesCount;

public:
	EntityHook(int hookId, const char *name, const char *classname);
	~EntityHook();

	int GetHookId();
	const char *GetClassname();
	int GetHookedEntitiesCount();

	static EntityHook *CreateHookIfNotFound(BaseEntity *entity, const char *name, int hookId);
	static SourceHook::List<EntityHook *> totalHooks;
};

#endif 