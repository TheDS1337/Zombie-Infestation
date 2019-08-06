#include "entity_hooker.h"

EntityHook::EntityHook(int hookId, const char *name, const char *serverclass)
{
	this->hookId = hookId;
	this->hookedEntitiesCount = 1;

	ke::SafeStrcpy(this->classname, sizeof(this->classname), serverclass);
	ke::SafeStrcpy(this->name, sizeof(this->name), name);
}

EntityHook::~EntityHook()
{
	CONSOLE_DEBUGGER("Releasing hook: %d, class: %s", hookId, classname);
	SH_REMOVE_HOOK_ID(hookId); 
	
	hookId = 0;
}

int EntityHook::GetHookId()
{
	return hookId;
}

const char *EntityHook::GetClassname()
{
	return classname;
}

int EntityHook::GetHookedEntitiesCount()
{
	return hookedEntitiesCount;
}

EntityHook *EntityHook::CreateHookIfNotFound(BaseEntity *entity, const char *name, int hookId)
{
/*
	IServerNetworkable *networkable = ((IServerUnknown *) entity)->GetNetworkable();

	if( !networkable )
	{
		return nullptr;
	}

	ServerClass *server = networkable->GetServerClass();

	if( !server )
	{
		return nullptr;
	}

	const char *srvclass = server->GetName();
*/
	const char *cls = entity->GetClassname();

	EntityHook *entHook = nullptr;

	// Look if a similar entity is already hooked
	for( auto iterator = totalHooks.begin(); iterator != totalHooks.end(); iterator++ )
	{
		entHook = *iterator;

		if( !entHook )
		{
			continue;
		}

		if( strcmp(entHook->classname, cls) == 0 && strcmp(entHook->name, name) == 0 )
		{	
			// Increase how many entities we've got tied to this hook
			entHook->hookedEntitiesCount++;

			// if it was found, we gonna make sure we delete the damage we just cause...
			SH_REMOVE_HOOK_ID(hookId);

			// Return the hook we just found.
			return entHook;
		}
	}

	// If a hook is not found, then create a new one linked to this specific classname
	entHook = new EntityHook(hookId, name, cls);
	
	// Add to the list in order to fucking wipe it later, son of a bitch crashed my game!
	totalHooks.push_back(entHook);
	return entHook;
}

SourceHook::List<EntityHook *> EntityHook::totalHooks;