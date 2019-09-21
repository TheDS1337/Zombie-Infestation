#include "gameutils.h"

#define SIZEOF_VARIANT_T 20

class VariantWrapper
{
private:
	unsigned char data[SIZEOF_VARIANT_T] = { 0 };

public:
	VariantWrapper()
	{
		memset(this, 0, sizeof(data));
		*(unsigned int *) &data[12] = INVALID_EHANDLE_INDEX;		
	}

	void Reset()
	{
		unsigned char *variant = data;

		*(int *) variant = 0; variant += 3 * sizeof(int);
		*(unsigned long *) variant = INVALID_EHANDLE_INDEX; variant += sizeof(unsigned long);
		*(fieldtype_t *) variant = FIELD_VOID;
	}
};

static VariantWrapper g_Variant;

class CTraceFilterAimTarget final: public CTraceFilterEntitiesOnly
{
public:
	CTraceFilterAimTarget(IHandleEntity *passentity)
	{
		m_pPassEnt = passentity;
	}

	bool ShouldHitEntity(IHandleEntity *serverEntity, int contentsMask) override
	{
		return serverEntity != m_pPassEnt;
	}

private:
	IHandleEntity *m_pPassEnt;
};

class CTraceFilterAimTarget2 final: public CTraceFilterEntitiesOnly
{
public:
	bool ShouldHitEntity(IHandleEntity *serverEntity, int contentsMask) override
	{
		return gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) serverEntity)) > g_pExtension->m_pGlobals->maxClients;
	}	
};

class CTraceFilterVacant final: public CTraceFilter
{
public:
	CTraceFilterVacant(IHandleEntity *passentity)
	{
		m_pPassEnt = passentity;
	}

	bool ShouldHitEntity(IHandleEntity *serverEntity, int contentsMask) override
	{
		return serverEntity != m_pPassEnt;
	}

private:
	IHandleEntity *m_pPassEnt;
};

IChangeInfoAccessor *CBaseEdict::GetChangeAccessor()
{
	return engine->GetChangeAccessor((const edict_t *) this);
}

BaseEntity *BaseEntity::CreateEntity(const char *classname)
{
	if( !g_pSM->IsMapRunning() )
	{
		CONSOLE_DEBUGGER("Attempting to create an entity without map loaded.");
		return nullptr;
	}

	BaseEntity *entity = (BaseEntity *) g_pExtension->m_pServerTools->CreateEntityByName(classname);

	if( !entity )
	{
		entity = (BaseEntity *) g_pExtension->m_pServerTools->CreateItemEntityByName(classname);
	}

	return entity;
}

BaseEntity *BaseEntity::FindEntity(BaseEntity *startingEnt, const char *classname)
{
	BaseEntity *nextEnt = startingEnt ? (BaseEntity *) g_pExtension->m_pServerTools->NextEntity((CBaseEntity *) startingEnt) : (BaseEntity *) g_pExtension->m_pServerTools->FirstEntity();

	while( nextEnt )
	{
		if( strcmp(nextEnt->GetClassname(), classname) == 0 )
		{
			return nextEnt;
		}

		nextEnt = (BaseEntity *) g_pExtension->m_pServerTools->NextEntity(nextEnt);
	}

	return nullptr;	
}

BaseEntity *BaseEntity::FindEntityInSphere(BaseEntity *startingEnt, const char *classname, const Vector &center, float radius)
{
	BaseEntity *nextEnt = startingEnt;

	// VectorLength takes a reference vector, so this is needed or we'll endup with some serious problems
	Vector pos(0.0f, 0.0f, 0.0);

	while( (nextEnt = FindEntity(nextEnt, classname)) )
	{
		pos = nextEnt->GetOrigin() - center;

		if( VectorLength(pos) < radius )
		{
			return nextEnt;
		}
	}

	return nullptr;
}

BaseEntity *BaseEntity::FindEntityByNetClass(int start, const char *classname)
{
	int maxEntities = g_pExtension->m_pGlobals->maxEntities;
	for( int i = start; i < maxEntities; i++ )
	{
		edict_t *current = gamehelpers->EdictOfIndex(i);
		if( current == NULL || current->IsFree() )
			continue;

		IServerNetworkable *network = current->GetNetworkable();
		if( network == NULL )
			continue;

		ServerClass *sClass = network->GetServerClass();
		const char *name = sClass->GetName();

		if( !strcmp(name, classname) )
			return (BaseEntity *) gamehelpers->ReferenceToEntity(gamehelpers->IndexOfEdict(current));
	}

	return NULL;
}

void BaseEntity::SetInputVariant(bool value)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(bool *) variant = value; variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_BOOLEAN;
}

void BaseEntity::SetInputVariant(int value)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(int *) variant = value; variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_INTEGER;
}

void BaseEntity::SetInputVariant(float value)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(float *) variant = value; variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_FLOAT;
}

void BaseEntity::SetInputVariant(const char *value)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(string_t *) variant = MAKE_STRING(value); variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_STRING;
}

void BaseEntity::SetInputVariant(Color value)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(color32 *) variant = value.ToColor32(); variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_COLOR32;
}

void BaseEntity::SetInputVariant(Vector value, bool posVector)
{
	unsigned char *variant = (unsigned char *) &g_Variant;

	*(Vector *) variant = value; variant += 3 * sizeof(int) + sizeof(unsigned long);
	*(fieldtype_t *) variant = posVector ? FIELD_POSITION_VECTOR : FIELD_VECTOR;
}

void BaseEntity::SetInputVariant(BaseEntity *value)
{
	CBaseHandle handle = ((IHandleEntity *) value)->GetRefEHandle();
	unsigned char *variant = (unsigned char *) &g_Variant + 3 * sizeof(int);

	*(unsigned long *) variant = handle.ToInt(); variant += sizeof(unsigned long);
	*(fieldtype_t *) variant = FIELD_EHANDLE;
}

bool BaseEntity::AcceptInput(const char *input, BaseEntity *activator, BaseEntity *caller, int outputId)
{
	static ICallWrapper *callWrapper = nullptr;
	static int variantSize = sizeof(g_Variant);

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("AcceptInput", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("AcceptInput offset is not found!");
			return false;
		}

		PassInfo info[6];

		info[0].type = PassType_Basic;
		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(const char *);
		info[1].type = info[2].type = PassType_Basic;
		info[1].flags = info[2].flags = PASSFLAG_BYVAL;
		info[1].size = info[2].size = sizeof(CBaseEntity *);
		info[3].type = PassType_Object;
		info[3].flags = PASSFLAG_BYVAL | PASSFLAG_OCTOR | PASSFLAG_ODTOR | PASSFLAG_OASSIGNOP;
		info[3].size = variantSize;
		info[4].type = PassType_Basic;
		info[4].flags = PASSFLAG_BYVAL;
		info[4].size = sizeof(int);
		info[5].type = PassType_Basic;
		info[5].flags = PASSFLAG_BYVAL;
		info[5].size = sizeof(bool);

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[5], info, 5);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return false;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char *params = new unsigned char[3 * sizeof(CBaseEntity *) + sizeof(const char *) + variantSize + sizeof(int) + sizeof(bool)];
	unsigned char *vparams = params;
	
	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = input; vparams += sizeof(const char *);
	*(CBaseEntity **) vparams = (CBaseEntity *) activator; vparams += sizeof(CBaseEntity *);
	*(CBaseEntity **) vparams = (CBaseEntity *) caller; vparams += sizeof(CBaseEntity *);	
	memcpy(vparams, &g_Variant, variantSize); vparams += variantSize;
	*(int *) vparams = outputId;

	bool ret;
	callWrapper->Execute(params, &ret);

	// Reset the global variant
	g_Variant.Reset();

	// Release the memory
	delete params;

	return ret;
}

void BaseEntity::Spawn()
{
	g_pExtension->m_pServerTools->DispatchSpawn(this);
}

void BaseEntity::Activate()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("Activate", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("Activate offset is not found!");
			return;
		}

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, 0, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this;

	callWrapper->Execute(params, nullptr);
}

void BaseEntity::Teleport(Vector *origin, QAngle *angles, Vector *velocity)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("Teleport", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("Teleport offset is not found!");
			return;
		}

		PassInfo info[3];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(Vector *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(QAngle *);
		info[1].type = PassType_Basic;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(Vector *);
		info[2].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, info, 3);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + 2 * sizeof(Vector *) + sizeof(QAngle *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(Vector **) vparams = origin; vparams += sizeof(Vector *);
	*(QAngle **) vparams = angles; vparams += sizeof(QAngle *);
	*(Vector **) vparams = velocity;

	callWrapper->Execute(params, nullptr);
}

void BaseEntity::Ignite(float duration, bool NPCOnly, float size, bool called)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("Ignite", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("Ignite offset is not found!");
			return;
		}

		PassInfo info[4];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(float);
		info[0].type = PassType_Float;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(bool);
		info[1].type = PassType_Basic;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(float);
		info[2].type = PassType_Float;
		info[3].flags = PASSFLAG_BYVAL;
		info[3].size = sizeof(bool);
		info[3].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, info, 4);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + 2 * (sizeof(float) + sizeof(bool))];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(float *) vparams = duration; vparams += sizeof(float);
	*(bool *) vparams = NPCOnly; vparams += sizeof(bool);
	*(float *) vparams = size; vparams += sizeof(float);
	*(bool *) vparams = called;

	callWrapper->Execute(params, nullptr);
}

void BaseEntity::Extinguish()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("Extinguish", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("Extinguish offset is not found!");
			return;
		}

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, nullptr, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this;

	callWrapper->Execute(params, nullptr);
}

int BaseEntity::TakeDamage(CTakeDamageInfo2 info)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pHooksConfig->GetOffset("OnTakeDamage", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("OnTakeDamage offset is not found!");
			return -1;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYREF | PASSFLAG_OCTOR;
		info[0].size = sizeof(CTakeDamageInfo2 &);
		info[0].type = PassType_Object;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return -1;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(CTakeDamageInfo2 &)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(CTakeDamageInfo2 *) vparams = info;	

	int ret = -1;
	callWrapper->Execute(params, &ret);

	return ret;
}

const char *BaseEntity::GetClassname()
{
	if( g_pExtension->m_iClassname < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}
	
	return STRING(*(string_t *) ((unsigned char *) this + g_pExtension->m_iClassname));
}

void BaseEntity::SetModel(const char *model)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("SetEntityModel", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("SetEntityModel offset is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(const char *);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = model;

	callWrapper->Execute(params, nullptr);
}

void BaseEntity::SetSize(const Vector &min, const Vector &max)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("UTIL_SetSize", &address) || !address )
		{
			CONSOLE_DEBUGGER("UTIL_SetSize address is not found!");
			return;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYREF | PASSFLAG_OCTOR | PASSFLAG_OASSIGNOP;
		info[0].size = sizeof(Vector &);
		info[0].type = PassType_Object;
		info[1].flags = PASSFLAG_BYREF | PASSFLAG_OCTOR | PASSFLAG_OASSIGNOP;
		info[1].size = sizeof(Vector &);
		info[1].type = PassType_Object;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, info, 2);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + 2 * sizeof(Vector &)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(Vector *) vparams = min; vparams += sizeof(Vector *);
	*(Vector *) vparams = max;

	callWrapper->Execute(params, nullptr);
}

int BaseEntity::GetHealth()
{
	if( g_pExtension->m_iHealth < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iHealth);
}

void BaseEntity::SetHealth(int value)
{
	if( g_pExtension->m_iHealth < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iHealth) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iHealth);
	}
}

float BaseEntity::GetGravity()
{
	if( g_pExtension->m_flGravity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flGravity) * 800.0f;
}

void BaseEntity::SetGravity(float value)
{
	if( g_pExtension->m_flGravity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flGravity) = value / 800.0f;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flGravity);
	}
}

int BaseEntity::GetModelIndex()
{
	if( g_pExtension->m_nModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(short *) ((unsigned char *) this + g_pExtension->m_nModelIndex);
}

void BaseEntity::SetModelIndex(int modelIndex)
{
	if( g_pExtension->m_nModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(short *) ((unsigned char *) this + g_pExtension->m_nModelIndex) = modelIndex;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nModelIndex);
	}
}

const char *BaseEntity::GetModelName()
{
	if( g_pExtension->m_ModelName < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	return STRING(*(string_t *) ((unsigned char *) this + g_pExtension->m_ModelName));
}

int BaseEntity::GetMoveType()
{
	if( g_pExtension->m_MoveType < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *((unsigned char *) this + g_pExtension->m_MoveType);
}

void BaseEntity::SetMoveType(int value)
{
	if( g_pExtension->m_MoveType < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*((unsigned char *) this + g_pExtension->m_MoveType) = value;
}

int BaseEntity::GetSolidType()
{
	if( g_pExtension->m_nSolidType < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *((unsigned char *) this + g_pExtension->m_nSolidType);
}

void BaseEntity::SetSolidType(int value)
{
	if( g_pExtension->m_nSolidType < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*((unsigned char *) this + g_pExtension->m_nSolidType) = value;
}

int BaseEntity::GetSolidFlags()
{
	if( g_pExtension->m_usSolidFlags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(unsigned short *) ((unsigned char *) this + g_pExtension->m_usSolidFlags);
}

void BaseEntity::SetSolidFlags(int value)
{
	if( g_pExtension->m_usSolidFlags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(unsigned short *) ((unsigned char *) this + g_pExtension->m_usSolidFlags) = value;
}

int BaseEntity::GetCollisionGroup()
{
	if( g_pExtension->m_CollisionGroup < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(unsigned int *) ((unsigned char *) this + g_pExtension->m_CollisionGroup);
}

void BaseEntity::SetCollisionGroup(int value)
{
	if( g_pExtension->m_CollisionGroup < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(unsigned int *) ((unsigned char *) this + g_pExtension->m_CollisionGroup) = value;
}

int BaseEntity::GetFlags()
{
	if( g_pExtension->m_fFlags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_fFlags);
}

void BaseEntity::SetFlags(int value)
{
	if( g_pExtension->m_fFlags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_fFlags) = value;
}

int BaseEntity::GetSpawnFlags()
{
	if( g_pExtension->m_spawnflags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_spawnflags);
}

void BaseEntity::SetSpawnFlags(int value)
{
	if( g_pExtension->m_spawnflags < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_spawnflags) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_spawnflags);
	}
}

int BaseEntity::GetEffects()
{
	if( g_pExtension->m_fEffects < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(size_t *) ((unsigned char *) this + g_pExtension->m_fEffects);
}

void BaseEntity::SetEffects(int value)
{
	if( g_pExtension->m_fEffects < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(size_t *) ((unsigned char *) this + g_pExtension->m_fEffects) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_fEffects);
	}
}

BaseEntity *BaseEntity::GetOwner()
{
	if( g_pExtension->m_hOwner < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *ownerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hOwner);

	if( !ownerHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *ownerEnt = gamehelpers->ReferenceToEntity(ownerHandle->GetEntryIndex());

	if( !ownerEnt || *ownerHandle != ((IHandleEntity *) ownerEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseEntity *) ownerEnt;
}

void BaseEntity::SetOwner(BaseEntity *ownerEnt)
{
	if( g_pExtension->m_hOwner < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *ownerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hOwner);
	ownerHandle->Set((IHandleEntity *) ownerEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hOwner);
	}
}

BaseEntity *BaseEntity::GetOwnerEntity()
{
	if( g_pExtension->m_hOwnerEntity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *ownerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hOwnerEntity);

	if( !ownerHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *ownerEnt = gamehelpers->ReferenceToEntity(ownerHandle->GetEntryIndex());

	if( !ownerEnt || *ownerHandle != ((IHandleEntity *) ownerEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseEntity *) ownerEnt;
}

void BaseEntity::SetOwnerEntity(BaseEntity *ownerEnt)
{
	if( g_pExtension->m_hOwnerEntity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *ownerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hOwnerEntity);
	ownerHandle->Set((IHandleEntity *) ownerEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hOwnerEntity);
	}
}

BaseEntity *BaseEntity::GetParent()
{
	if( g_pExtension->m_pParent < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	return *(BaseEntity **) ((unsigned char *) this + g_pExtension->m_pParent);
}

void BaseEntity::SetParent(BaseEntity *parentEnt)
{
	if( g_pExtension->m_pParent < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(CBaseEntity **) ((unsigned char *) this + g_pExtension->m_pParent) = (CBaseEntity *) parentEnt;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_pParent);
	}
}

Vector BaseEntity::GetOrigin()
{
	if( g_pExtension->m_vecOrigin < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return *(Vector *) ((unsigned char *) this + g_pExtension->m_vecOrigin);
}

void BaseEntity::SetOrigin(Vector value)
{
	if( g_pExtension->m_vecOrigin < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(Vector *) ((unsigned char *) this + g_pExtension->m_vecOrigin) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_vecOrigin);
	}
}

Vector BaseEntity::GetVelocity()
{
	if( g_pExtension->m_vecVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return *(Vector *) ((unsigned char *) this + g_pExtension->m_vecVelocity);
}

Vector BaseEntity::GetVelocity(AngularImpulse &angVelocity)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("GetVelocity", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("GetVelocity offset is not found!");
			return Vector(0.0f, 0.0f, 0.0f);
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(Vector *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(AngularImpulse *);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, info, 2);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return Vector(0.0f, 0.0f, 0.0f);
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(Vector *) + sizeof(AngularImpulse *)];
	unsigned char *vparams = params;

	static Vector velocity;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(Vector **) vparams = &velocity; vparams += sizeof(Vector *);
	*(AngularImpulse **) vparams = &angVelocity;

	callWrapper->Execute(params, nullptr);

	return velocity;
}

void BaseEntity::SetVelocity(Vector value)
{
	if( g_pExtension->m_vecVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(Vector *) ((unsigned char *) this + g_pExtension->m_vecVelocity) = value;
}

Vector BaseEntity::GetAbsVelocity()
{
	if( g_pExtension->m_vecAbsVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return *(Vector *) ((unsigned char *) this + g_pExtension->m_vecAbsVelocity);
}

void BaseEntity::SetAbsVelocity(Vector value)
{
	if( g_pExtension->m_vecAbsVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(Vector *) ((unsigned char *) this + g_pExtension->m_vecAbsVelocity) = value;
}

CUtlVector<ThinkFuncData> *BaseEntity::GetThinkFunctions()
{
	if( g_pExtension->m_aThinkFunctions < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	return (CUtlVector<ThinkFuncData> *) ((unsigned char *) this + g_pExtension->m_aThinkFunctions);
}

TF BaseEntity::GetThink()
{
	if( g_pExtension->m_pfnThink < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	return *(TF *) ((unsigned char *) this + g_pExtension->m_pfnThink);
}

void BaseEntity::SetThink(TF func)
{
	if( g_pExtension->m_pfnThink < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(TF *) ((unsigned char *) this + g_pExtension->m_pfnThink) = func;
}

int BaseEntity::GetThinkFuncId(const char *context)
{
	CUtlVector<ThinkFuncData> *thinkFunctions = GetThinkFunctions();

	for( int i = 0; i < thinkFunctions->Count(); i++ )
	{
		if( !Q_strncmp(STRING(thinkFunctions->Element(i).m_iszContext), context, MAX_CONTEXT_LENGTH) )
		{
			return i;
		}
	}

	return NO_THINK_CONTEXT;
}

int BaseEntity::RegisterThinkFuncId(const char *context)
{
	int funcId = GetThinkFuncId(context);

	if( funcId != NO_THINK_CONTEXT )
	{
		return funcId;
	}

	// Make a new think func
	ThinkFuncData newFunc;
	Q_memset(&newFunc, 0, sizeof(newFunc));

	newFunc.m_pfnThink = nullptr;
	newFunc.m_nNextThinkTick = 0;
	newFunc.m_iszContext = AllocPooledString(context);

	CUtlVector<ThinkFuncData> *thinkFunctions = GetThinkFunctions();

	if( thinkFunctions )
	{
		// Insert it into our list
		return thinkFunctions->AddToTail(newFunc);
	}

	return NO_THINK_CONTEXT;
}

bool BaseEntity::IsWillingToThink()
{
	if( GetNextThinkTick() > 0 )
	{
		return true;
	}

	CUtlVector<ThinkFuncData> *thinkFunctions = GetThinkFunctions();

	if( thinkFunctions )
	{
		for( int i = 0; i < thinkFunctions->Count(); i++ )
		{
			if( thinkFunctions->Element(i).m_nNextThinkTick > 0 )
			{
				return true;
			}
		}
	}

	return false;
}

TF BaseEntity::SetThinkFunc(TF func, float thinkTime, const char *context)
{
	// Old system?
	if( !context )
	{
		SetThink(func);
		return func;
	}

	// Find the think function in our list, and if we couldn't find it, register it
	int funcId = GetThinkFuncId(context);

	if( funcId == NO_THINK_CONTEXT )
	{
		funcId = RegisterThinkFuncId(context);
	}

	CUtlVector<ThinkFuncData> *thinkFunctions = GetThinkFunctions();

	if( thinkFunctions )
	{
		thinkFunctions->Element(funcId).m_pfnThink = func;

		if( thinkTime != 0 )
		{
			int thinkTick = (thinkTime == TICK_NEVER_THINK) ? TICK_NEVER_THINK : TIME_TO_TICKS2(thinkTime);
			thinkFunctions->Element(funcId).m_nNextThinkTick = thinkTick;

			bool thinking = thinkTick == TICK_NEVER_THINK ? false : true;

			int flags = GetFlags();

			if( thinking && flags & EFL_NO_THINK_FUNCTION )
			{
				flags &= ~EFL_NO_THINK_FUNCTION;
			}
			else if( !thinking && !(flags & EFL_NO_THINK_FUNCTION) && !IsWillingToThink() )
			{
				flags |= EFL_NO_THINK_FUNCTION;
			}

			SetFlags(flags);
		}
	}

	return func;
}

int BaseEntity::GetNextThinkTick()
{
	if( g_pExtension->m_nNextThinkTick < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nNextThinkTick);
}

void BaseEntity::SetNextThinkTick(int value)
{
	if( g_pExtension->m_nNextThinkTick < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nNextThinkTick) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nNextThinkTick);
	}
}

int BaseEntity::GetTakeDamage()
{
	if( g_pExtension->m_takedamage < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_takedamage);
}

void BaseEntity::SetTakeDamage(int value)
{
	if( g_pExtension->m_takedamage < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_takedamage) = value;
}

Color BaseEntity::GetRenderColor()
{
	if( g_pExtension->m_clrRender < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return Color(0, 0, 0, 0);
	}

	color32 color = *(color32 *) ((unsigned char *) this + g_pExtension->m_clrRender);

	return Color(color.r, color.g, color.b, color.a);
}

void BaseEntity::SetRenderColor(Color color)
{
	if( g_pExtension->m_clrRender < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(color32 *) ((unsigned char *) this + g_pExtension->m_clrRender) = color.ToColor32();

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_clrRender);
	}
}

RenderFx_t BaseEntity::GetRenderFx()
{
	if( g_pExtension->m_nRenderFX < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return kRenderFxNone;
	}

	return (RenderFx_t) *((unsigned char *) this + g_pExtension->m_nRenderFX);
}

void BaseEntity::SetRenderFx(RenderFx_t value)
{
	if( g_pExtension->m_nRenderFX < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*((unsigned char *) this + g_pExtension->m_nRenderFX) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nRenderFX);
	}
}

RenderMode_t BaseEntity::GetRenderMode()
{
	if( g_pExtension->m_nRenderMode < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return kRenderNone;
	}

	return (RenderMode_t) *((unsigned char *) this + g_pExtension->m_nRenderMode);
}

void BaseEntity::SetRenderMode(RenderMode_t value)
{
	if( g_pExtension->m_nRenderMode < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*((unsigned char *) this + g_pExtension->m_nRenderMode) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nRenderMode);
	}
}

bool BaseEntity::GetKeyValue(const char *key, char *value, int valueLen)
{
	return g_pExtension->m_pServerTools->GetKeyValue(this, key, value, valueLen);
}

bool BaseEntity::SetKeyValue(const char *key, char *value)
{
	return g_pExtension->m_pServerTools->SetKeyValue(this, key, value);
}

bool BaseEntity::GetKeyValue(const char *key, bool *value)
{
	static char strValue[32];

	if( GetKeyValue(key, strValue, sizeof(strValue)) )
	{
		*value = strtoul(strValue, nullptr, 10) > 0;
		return true;
	}

	return false;
}

bool BaseEntity::SetKeyValue(const char *key, bool value)
{
	static char strValue[2];
	ke::SafeSprintf(strValue, sizeof(strValue), "%d", value ? 1 : 0);

	return g_pExtension->m_pServerTools->SetKeyValue(this, key, strValue);
}

bool BaseEntity::GetKeyValue(const char *key, int &value)
{
	static char strValue[32];

	if( GetKeyValue(key, strValue, sizeof(strValue)) )
	{
		value = strtoul(strValue, nullptr, 10);
		return true;
	}

	return false;
}

bool BaseEntity::SetKeyValue(const char *key, int value)
{
	static char strValue[12];
	ke::SafeSprintf(strValue, sizeof(strValue), "%d", value);

	return g_pExtension->m_pServerTools->SetKeyValue(this, key, strValue);
}

bool BaseEntity::GetKeyValue(const char *key, float &value)
{
	static char strValue[32];

	if( GetKeyValue(key, strValue, sizeof(strValue)) )
	{
		value = strtod(strValue, nullptr);
		return true;
	}

	return false;
}

bool BaseEntity::SetKeyValue(const char *key, float value)
{
	return g_pExtension->m_pServerTools->SetKeyValue(this, key, value);
}

bool BaseEntity::GetKeyValue(const char *key, Vector &value)
{
	static char strValue[64];

	if( GetKeyValue(key, strValue, sizeof(strValue)) )
	{
		int spacePos = 0, spaces[3], spacesCount = 0;

		while( (spacePos = FindCharInString(&strValue[spacePos], ' ')) )
		{
			spaces[spacesCount++] = spacePos;
		}

		if( spacesCount != 2 )
		{
			return false;
		}

		value.x = strtod(strValue, nullptr);
		value.y = strtod(strValue + spaces[1], nullptr);
		value.z = strtod(strValue + spaces[2], nullptr);

		return true;
	}

	return false;
}

bool BaseEntity::SetKeyValue(const char *key, Vector value)
{
	return g_pExtension->m_pServerTools->SetKeyValue(this, key, value);
}

bool BaseEntity::GetKeyValue(const char *key, QAngle &value)
{
	static char strValue[64];

	if( GetKeyValue(key, strValue, sizeof(strValue)) )
	{
		int spacePos = 0, spaces[3], spacesCount = 0;

		while( (spacePos = FindCharInString(&strValue[spacePos], ' ')) )
		{
			spaces[spacesCount++] = spacePos;
		}

		if( spacesCount != 2 )
		{
			return false;
		}

		value.x = strtod(strValue, nullptr);
		value.y = strtod(strValue + spaces[1], nullptr);
		value.z = strtod(strValue + spaces[2], nullptr);

		return true;
	}

	return false;
}

bool BaseEntity::SetKeyValue(const char *key, QAngle value)
{
	return g_pExtension->m_pServerTools->SetKeyValue(this, key, Vector(value.x, value.y, value.z));
}

int BaseAnimating::GetSkin()
{
	if( g_pExtension->m_nSkin < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nSkin);
}

void BaseAnimating::SetSkin(int skin)
{
	if( g_pExtension->m_nSkin < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nSkin) = skin;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nSkin);
	}
}

int BaseAnimating::GetBody()
{
	if( g_pExtension->m_nBody < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nBody);
}

void BaseAnimating::SetBody(int body)
{
	if( g_pExtension->m_nBody < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nBody) = body;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nBody);
	}
}

int BaseAnimating::GetSequence()
{
	if( g_pExtension->m_nSequence < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nSequence);
}

void BaseAnimating::SetSequence(int value)
{
	if( g_pExtension->m_nSequence < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nSequence) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nSequence);
	}
}

float BaseAnimating::GetPlaybackRate()
{
	if( g_pExtension->m_flPlaybackRate < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flPlaybackRate);
}

void BaseAnimating::SetPlaybackRate(float value)
{
	if( g_pExtension->m_flPlaybackRate < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flPlaybackRate) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flPlaybackRate);
	}
}

int BaseAnimating::SelectWeightedSequence(Activity activity)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CBaseAnimating::SelectWeightedSequence", &address) || !address )
		{
			CONSOLE_DEBUGGER("CBaseAnimating::SelectWeightedSequence address is not found!");
			return -1;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(Activity);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return -1;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(Activity)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(Activity *) vparams = activity;

	int ret = -1;
	callWrapper->Execute(params, &ret);

	return ret;
}

int BaseAnimating::LookupAttachment(const char *name)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CBaseAnimating::LookupAttachment", &address) || !address )
		{
			CONSOLE_DEBUGGER("CBaseAnimating::LookupAttachment address is not found!");
			return -1;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(const char *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return -1;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = name;

	int ret = -1;
	callWrapper->Execute(params, &ret);

	return ret;
}

bool BaseAnimating::GetAttachment(const char *name, Vector &origin, QAngle &angles)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CBaseAnimating::GetAttachment", &address) || !address )
		{
			CONSOLE_DEBUGGER("CBaseAnimating::GetAttachment address is not found!");
			return false;
		}

		PassInfo info[4];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(const char *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(Vector *);
		info[1].type = PassType_Basic;		
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(QAngle *);
		info[2].type = PassType_Basic;	
		info[3].flags = PASSFLAG_BYVAL;
		info[3].size = sizeof(bool);
		info[3].type = PassType_Basic;		

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, &info[3], info, 3);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return false;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *) + sizeof(Vector *) + sizeof(QAngle *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = name; vparams += sizeof(const char *);
	*(Vector **) vparams = &origin; vparams += sizeof(Vector);
	*(QAngle **) vparams = &angles; 

	bool ret = true;
	callWrapper->Execute(params, &ret);

	return ret;
}

bool BaseProp::IsGlowing()
{
	if( g_pExtension->m_bShouldGlow < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	return *(bool *) ((unsigned char *) this + g_pExtension->m_bShouldGlow);
}

void BaseProp::SetGlow(bool on)
{
	if( g_pExtension->m_bShouldGlow < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(bool *) ((unsigned char *) this + g_pExtension->m_bShouldGlow) = on;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bShouldGlow);
	}
}

int BaseProp::GetGlowStyle()
{
	if( g_pExtension->m_nGlowStyle < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nGlowStyle);
}

void BaseProp::SetGlowStyle(int value)
{
	if( g_pExtension->m_nGlowStyle < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nGlowStyle) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nGlowStyle);
	}
}

Color BaseProp::GetGlowColor()
{
	if( g_pExtension->m_clrGlow < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return { 0, 0, 0, 0 };
	}

	color32 color = *(color32 *) ((unsigned char *) this + g_pExtension->m_clrGlow);

	return Color(color.r, color.g, color.b, color.a);
}

void BaseProp::SetGlowColor(Color color)
{
	if( g_pExtension->m_clrGlow < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(color32 *) ((unsigned char *) this + g_pExtension->m_clrGlow) = color.ToColor32();

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_clrGlow);
	}
}

float BaseProp::GetGlowMaxDist()
{
	if( g_pExtension->m_flGlowMaxDist < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flGlowMaxDist);
}

void BaseProp::SetGlowMaxDist(float value)
{
	if( g_pExtension->m_flGlowMaxDist < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flGlowMaxDist) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flGlowMaxDist);
	}
}

int BaseWeapon::GetViewModel()
{
	if( g_pExtension->m_nViewModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nViewModelIndex);
}

void BaseWeapon::SetViewModel(int value)
{
	if( g_pExtension->m_nViewModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nViewModelIndex) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nViewModelIndex);
	}
}

int BaseWeapon::GetViewModelIndex()
{
	if( g_pExtension->m_iViewModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iViewModelIndex);
}

void BaseWeapon::SetViewModelIndex(int value)
{
	if( g_pExtension->m_iViewModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iViewModelIndex) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iViewModelIndex);
	}
}

int BaseWeapon::GetWorldModelIndex()
{
	if( g_pExtension->m_iWorldModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iWorldModelIndex);
}

void BaseWeapon::SetWorldModelIndex(int value)
{
	if( g_pExtension->m_iWorldModelIndex < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iWorldModelIndex) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iWorldModelIndex);
	}
}

bool BaseWeapon::IsUsingClip1()
{
	if( g_pExtension->m_iClip1 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iClip1) != -1;
}

bool BaseWeapon::IsUsingClip2()
{
	if( g_pExtension->m_iClip2 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iClip2) != -1;
}

int BaseWeapon::GetClip()
{
	if( g_pExtension->m_iClip1 < 1 || g_pExtension->m_iClip2 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	if( IsUsingClip1() )
	{
		return *(int *) ((unsigned char *) this + g_pExtension->m_iClip1);
	}
	else if( IsUsingClip2() )
	{
		return *(int *) ((unsigned char *) this + g_pExtension->m_iClip2);
	}

	return -1;
}

void BaseWeapon::SetClip(int value)
{
	if( g_pExtension->m_iClip1 < 1 || g_pExtension->m_iClip2 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	int offset = 0;

	if( IsUsingClip1() )
	{
		offset = g_pExtension->m_iClip1;
	}
	else if( IsUsingClip2() )
	{
		offset = g_pExtension->m_iClip2;
	}
	else
	{
		// We can do nothing about it if it doesnt support neither of the clip types.
		return;
	}

	*(int *) ((unsigned char *) this + offset) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, offset);
	}
}

void BaseGrenade::Detonate()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pGameMod->GetConfig()->GetOffset("CBaseCSGrenadeProjectile::Detonate", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("CBaseCSGrenadeProjectile::Detonate offset is not found!");
			return;
		}

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, 0, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this;

	callWrapper->Execute(params, nullptr);
}

/* Doesnt work properly */
Vector BaseGrenade::GetInitialVelocity()
{
	if( g_pExtension->m_vInitialVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return *(Vector *) ((unsigned char *) this + g_pExtension->m_vInitialVelocity);
}

void BaseGrenade::SetInitialVelocity(Vector value)
{

	if( g_pExtension->m_vInitialVelocity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(Vector *) ((unsigned char *) this + g_pExtension->m_vInitialVelocity) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_vInitialVelocity);
	}
}

BasePlayer *BaseGrenade::GetThrower()
{
	if( g_pExtension->m_hThrower < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *throwerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hThrower);

	if( !throwerHandle->IsValid() )
	{
		CONSOLE_DEBUGGER("Invalid handle");
		return nullptr;
	}

	CBaseEntity *throwerEnt = gamehelpers->ReferenceToEntity(throwerHandle->GetEntryIndex());

	if( !throwerEnt || *throwerHandle != ((IHandleEntity *) throwerEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BasePlayer *) throwerEnt;
}

void BaseGrenade::SetThrower(BasePlayer *throwerEnt)
{
	if( g_pExtension->m_hThrower < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *throwerHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hThrower);
	throwerHandle->Set((IHandleEntity *) throwerEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hThrower);
	}
}

float BaseGrenade::GetDamage()
{
	if( g_pExtension->m_flDamage < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0.0f;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flDamage);
}

void BaseGrenade::SetDamage(float value)
{
	if( g_pExtension->m_flDamage < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flDamage) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flDamage);
	}
}

float BaseGrenade::GetDamageRadius()
{
	if( g_pExtension->m_DmgRadius < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_DmgRadius);
}

void BaseGrenade::SetDamageRadius(float value)
{
	if( g_pExtension->m_DmgRadius < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_DmgRadius) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_DmgRadius);
	}
}

bool BaseGrenade::IsLive()
{
	if( g_pExtension->m_bIsLive < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	return *(bool *) ((unsigned char *) this + g_pExtension->m_bIsLive);
}

void BaseGrenade::SetLive(bool on)
{
	if( g_pExtension->m_bIsLive < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(bool *) ((unsigned char *) this + g_pExtension->m_bIsLive) = on;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bIsLive);
	}
}

int BaseViewModel::GetViewModelIndex()
{
	if( g_pExtension->m_nViewModelIndex2 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nViewModelIndex2);
}

void BaseViewModel::SetViewModelIndex(int value)
{
	if( g_pExtension->m_nViewModelIndex2 < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_nViewModelIndex2) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_nViewModelIndex2);
	}
}

int BaseViewModel::UpdateTransmitState()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pGameMod->GetConfig()->GetOffset("CBaseViewModel::UpdateTransmitState", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("CBaseViewModel::UpdateTransmitState offset is not found!");
			return 0;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(int);
		info.type = PassType_Basic;		

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info, nullptr, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return 0;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; 

	int ret;
	callWrapper->Execute(params, &ret);

	return ret;
}

void BasePlayer::Kill(bool explode, bool force)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("CommitSuicide", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("CommitSuicide offset is not found!");
			return;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(bool);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(bool);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, info, 2);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + 2 * sizeof(bool)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(bool *) vparams = explode; vparams += sizeof(bool);
	*(bool *) vparams = force;

	callWrapper->Execute(params, nullptr);
}

void BasePlayer::Respawn()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pExtension->m_pCStrikeConfig->GetMemSig("RoundRespawn", &address) || !address )
		{
			CONSOLE_DEBUGGER("RoundRespawn address is not found!");
			return;
		}

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, nullptr, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];	
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this;	

	callWrapper->Execute(params, nullptr);
}

void BasePlayer::SetName(const char *name)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("SetClientName", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("SetClientName offset is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(const char *);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);
	
	unsigned char params[sizeof(void *) + sizeof(const char *)];
	unsigned char *vparams = params;

	*(void **) vparams = (void *) ((int) g_pExtension->m_pSDKTools->GetIServer()->GetClient(gamehelpers->IndexOfEdict(client) - 1) - sizeof(void *)); vparams += sizeof(void *);
	*(const char **) vparams = name;

	callWrapper->Execute(params, nullptr);

	// Alert the server of the change
	g_pExtension->m_pServerGameClients->ClientSettingsChanged(client);
}

void BasePlayer::SetProgressBarTime(int time)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CCSPlayer::SetProgressBarTime", &address) || !address )
		{
			CONSOLE_DEBUGGER("CCSPlayer::SetProgressBarTime address is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(int);
		info.type = PassType_Basic;		

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(int)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(int *) vparams = time;

	callWrapper->Execute(params, nullptr);
}

// See: https://developer.valvesoftware.com/wiki/Dimensions for more info, the values here are calculated using Pythagoras's theorem
// More about traces: https://developer.valvesoftware.com/wiki/UTIL_TraceLine
#define PLAYER_WIDTH 32.0f
#define PLAYER_HEIGHT 72.0f

bool BasePlayer::CanGetInSpace(Vector &origin, bool ducking)
{
	// Origin fix up, usually it starts from the feet but let's keep it at the middle
	origin.z += PLAYER_HEIGHT / 2;

	float radius = sqrt(2 * (PLAYER_WIDTH * PLAYER_WIDTH) + PLAYER_HEIGHT * PLAYER_HEIGHT) / 2;

	Vector heads[4];

	heads[0].x = origin.x + PLAYER_WIDTH / 2;
	heads[0].y = origin.y + PLAYER_WIDTH / 2;
	heads[0].z = origin.z + PLAYER_HEIGHT / 2;

	heads[1].x = origin.x - PLAYER_WIDTH / 2;
	heads[1].y = origin.y - PLAYER_WIDTH / 2;
	heads[1].z = origin.z - PLAYER_HEIGHT / 2;

	heads[2].x = origin.x + PLAYER_WIDTH / 2;
	heads[2].y = origin.y - PLAYER_WIDTH / 2;
	heads[2].z = origin.z - PLAYER_HEIGHT / 2;

	heads[3].x = origin.x - PLAYER_WIDTH / 2;
	heads[3].y = origin.y + PLAYER_WIDTH / 2;
	heads[3].z = origin.z + PLAYER_HEIGHT / 2;

	// Accept collision with all other entities, but not ME
	CTraceFilterVacant filter((IHandleEntity *) this);

	// We do it first in the X-Y plane	
	Vector dir = heads[1] - heads[0];
	VectorNormalize(dir);

	Ray_t ray;
	ray.Init(origin - radius * dir, origin + radius * dir);

	trace_t trace;
	g_pExtension->m_pEngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	if( trace.DidHit() )
	{
		return VectorLength(trace.endpos - trace.startpos) > radius;
	}

	return true;
}

class CEconItemView;
BaseWeapon *BasePlayer::GiveItem(const char *item, int subtype, bool removeIfNotCarried)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("GiveNamedItem", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("GiveNamedItem offset is not found!");
			return nullptr;
		}

		PassInfo info[6];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(const char *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(CEconItemView *);
		info[2].type = PassType_Basic;
		info[3].flags = PASSFLAG_BYVAL;
		info[3].size = sizeof(bool);
		info[3].type = PassType_Basic;
		info[4].flags = PASSFLAG_BYVAL;
		info[4].size = sizeof(CBaseEntity *);
		info[4].type = PassType_Basic;
		info[5].flags = PASSFLAG_BYVAL;
		info[5].size = sizeof(void *);
		info[5].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[5], info, 5);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return nullptr;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *) + sizeof(int) + sizeof(CEconItemView *) + sizeof(bool) + sizeof(void *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = item; vparams += sizeof(const char *);
	*(int *) vparams = subtype; vparams += sizeof(int);
	*(CEconItemView **) vparams = nullptr; vparams += sizeof(CEconItemView *);
	*(bool *) vparams = removeIfNotCarried; vparams += sizeof(bool);
	*(void **) vparams = nullptr;

	BaseWeapon *itemEnt;
	callWrapper->Execute(params, &itemEnt);

	return itemEnt;
}

int BasePlayer::GiveAmmo(int amount, int ammotype, bool suppressSound)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("GiveAmmo", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("GiveAmmo offset is not found!");
			return -1;
		}

		PassInfo info[4];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(int);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(bool);
		info[2].type = PassType_Basic;
		info[3].flags = PASSFLAG_BYVAL;
		info[3].size = sizeof(int);
		info[3].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[3], info, 3);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return 0;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + 2 * sizeof(int) + sizeof(bool)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(int *) vparams = amount; vparams += sizeof(int);
	*(int *) vparams = ammotype; vparams += sizeof(int);
	*(bool *) vparams = suppressSound;

	int ret;
	callWrapper->Execute(params, &ret);

	return ret;
}

void BasePlayer::EquipItem(BaseEntity *itemEntity)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("WeaponEquip", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("WeaponEquip offset is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(CBaseEntity *);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[2 * sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(CBaseEntity **) vparams = (CBaseEntity *) itemEntity;

	callWrapper->Execute(params, nullptr);
}

bool BasePlayer::RemoveItem(BaseEntity *itemEntity)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("RemovePlayerItem", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("RemovePlayerItem offset is not found!");
			return false;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(CBaseEntity *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(bool);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return false;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[2 * sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(CBaseEntity **) vparams = (CBaseEntity *) itemEntity;

	bool ret;
	callWrapper->Execute(params, &ret);

	return ret;
}

BaseWeapon *BasePlayer::GetItemFromSlot(int slot)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("Weapon_GetSlot", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("Weapon_GetSlot offset is not found!");
			return nullptr;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(int);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(CBaseEntity *);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return nullptr;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(int)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(int *) vparams = slot;

	BaseWeapon *itemEnt;
	callWrapper->Execute(params, &itemEnt);

	return itemEnt;
}

void BasePlayer::SelectItem(const char *classname, int subType)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CBasePlayer::SelectItem", &address) || !address )
		{
			CONSOLE_DEBUGGER("CBasePlayer::SelectItem address is not found!");
			return;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(const char *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(int);
		info[1].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, info, 2);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *) + sizeof(int)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = classname; vparams += sizeof(const char *);
	*(int *) vparams = subType;

	callWrapper->Execute(params, nullptr);
}

int BasePlayer::GetItemsCountFromSlot(int slot)
{
	int itemsCount = 0;

	BaseWeapon *weaponEnt = nullptr;

	for( int weapon = 0; weapon < CSGO_MAX_WEAPONS; weapon++ )
	{
		if( !(weaponEnt = GetWeapons(weapon)) )
		{
			continue;
		}		

		if( GetItemSlot(weaponEnt->GetClassname()) == slot )
		{
			itemsCount++;
		}
	}

	return itemsCount;
}

int BasePlayer::GetItemsFromSlot(int slot, SourceHook::CVector<WeaponData> *items)
{
	int weaponSlot = -1, itemsCount = 0;

	BaseWeapon *weaponEnt = nullptr;

	for( int weapon = 0; weapon < CSGO_MAX_WEAPONS; weapon++ )
	{
		if( !(weaponEnt = GetWeapons(weapon)) )
		{
			continue;
		}

		weaponSlot = /**(int *) ((unsigned char *) weaponEnt + OFFSET_WEAPON_SLOT);*/ GetItemSlot(weaponEnt->GetClassname());

		if( weaponSlot != slot )
		{
			continue;
		}	

		if( itemsCount >= CSGO_MAX_WEAPON_IN_SLOT )
		{
			break;
		}

		if( items )
		{
			if( (int) items->size() > itemsCount )
			{
				items->at(itemsCount).Push(weaponEnt, GetIndexInMyWeapons(weaponEnt));
			}
			else
			{
				items->push_back(WeaponData(weaponEnt, GetIndexInMyWeapons(weaponEnt)));
			}
		}

		itemsCount++;
	}	

	return itemsCount;
}

void BasePlayer::RemoveItemFromSlot(int slot, const char *cls)
{
	const char *classname = nullptr;
	BaseWeapon *weaponEnt = nullptr;

	while( (weaponEnt = GetItemFromSlot(slot)) )
	{
		classname = weaponEnt->GetClassname();

		if( strcmp(classname, cls) != 0 )
		{
			continue;
		}

		if( RemoveItem(weaponEnt) )
		{
			weaponEnt->AcceptInput("Kill");
		}
		else
		{
			CONSOLE_DEBUGGER("Item isn't removable: %s", classname);
		}
	}
}

void BasePlayer::RemoveItemsFromSlot(int slot)
{
	BaseWeapon *weaponEnt = nullptr;

	while( (weaponEnt = GetItemFromSlot(slot)) )
	{
		if( RemoveItem(weaponEnt) )
		{
			weaponEnt->AcceptInput("Kill");
		}
		else
		{
			CONSOLE_DEBUGGER("Item isn't removable: %s", weaponEnt->GetClassname());
		}
	}	
}

int GetItemSlot(const char *classname)
{
	if( strcmp(classname, "weapon_nova") == 0 || strcmp(classname, "weapon_xm1014") == 0 || strcmp(classname, "weapon_sawedoff") == 0 || strcmp(classname, "weapon_mag7") == 0 || strcmp(classname, "weapon_mac10") == 0
		|| strcmp(classname, "weapon_mp9") == 0 || strcmp(classname, "weapon_ump45") == 0 || strcmp(classname, "weapon_mp5sd") == 0 || strcmp(classname, "weapon_mp7") == 0 || strcmp(classname, "weapon_bizon") == 0
		|| strcmp(classname, "weapon_p90") == 0 || strcmp(classname, "weapon_galilar") == 0 || strcmp(classname, "weapon_famas") == 0 || strcmp(classname, "weapon_ak47") == 0 || strcmp(classname, "weapon_m4a1") == 0
		|| strcmp(classname, "weapon_m4a1_silencer") == 0 || strcmp(classname, "weapon_sg556") == 0 || strcmp(classname, "weapon_aug") == 0 || strcmp(classname, "weapon_negev") == 0 || strcmp(classname, "weapon_m249") == 0
		|| strcmp(classname, "weapon_ssg08") == 0 || strcmp(classname, "weapon_awp") == 0 || strcmp(classname, "weapon_g3sg1") == 0 || strcmp(classname, "weapon_scar20") == 0 )
	{
		return CSGO_WEAPON_SLOT_PRIMARY;
	}
	else if( strcmp(classname, "weapon_glock") == 0 || strcmp(classname, "weapon_hkp2000") == 0 || strcmp(classname, "weapon_usp_silencer") == 0 || strcmp(classname, "weapon_p250") == 0
		|| strcmp(classname, "weapon_tec9") == 0 || strcmp(classname, "weapon_cz75a") == 0 || strcmp(classname, "weapon_fiveseven") == 0 || strcmp(classname, "weapon_deagle") == 0
		|| strcmp(classname, "weapon_revolver") == 0 || strcmp(classname, "weapon_elite") == 0 )
	{
		return CSGO_WEAPON_SLOT_SECONDARY;
	}
	else if( strcmp(classname, "weapon_hegrenade") == 0 || strcmp(classname, "weapon_flashbang") == 0 || strcmp(classname, "weapon_smokegrenade") == 0 || strcmp(classname, "weapon_molotov") == 0
		|| strcmp(classname, "weapon_incgrenade") == 0 || strcmp(classname, "weapon_decoy") == 0 || strcmp(classname, "weapon_tegrenade") == 0 )
	{
		return CSGO_WEAPON_SLOT_NADES;
	}
	else if( strstr(classname, "knife") || strcmp(classname, "weapon_fists") == 0 || strcmp(classname, "weapon_axe") == 0 || strcmp(classname, "weapon_hammer") == 0
		|| strcmp(classname, "weapon_spanner") == 0 )
	{
		return CSGO_WEAPON_SLOT_MELEE;
	}
	else if( strcmp(classname, "weapon_c4") == 0 || strcmp(classname, "weapon_breachcharge") == 0 )
	{
		return CSGO_WEAPON_SLOT_C4;
	}

	return -1;
}

Vector BasePlayer::GetOrigin()
{
	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);
	IGamePlayer *player = playerhelpers->GetGamePlayer(client);

	if( !player )
	{
		CONSOLE_DEBUGGER("CS:GO is not supported!");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	IPlayerInfo *info = player->GetPlayerInfo();

	if( !info )
	{
		CONSOLE_DEBUGGER("Could not find player info");
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return info->GetAbsOrigin();
}

QAngle BasePlayer::GetAngles()
{
	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);
	IGamePlayer *player = playerhelpers->GetGamePlayer(client);

	if( !player )
	{
		CONSOLE_DEBUGGER("CS:GO is not supported!");
		return QAngle(0.0f, 0.0f, 0.0f);
	}

	IPlayerInfo *info = player->GetPlayerInfo();

	if( !info )
	{
		CONSOLE_DEBUGGER("Could not find player info");
		return QAngle(0.0f, 0.0f, 0.0f);
	}

	return info->GetAbsAngles();
}

Vector BasePlayer::GetEyePosition()
{
	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( !client )
	{
		return Vector(0.0f, 0.0f, 0.0f);
	}

	Vector dir;
	AngleVectors(GetEyeAngles(), &dir);

	Vector eyePos;
	g_pExtension->m_pServerGameClients->ClientEarPosition(client, &eyePos);

	return eyePos;
}

QAngle BasePlayer::GetEyeAngles()
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pExtension->m_pSDKConfig->GetOffset("EyeAngles", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("EyeAngles offset is not found!");
			return QAngle(0.0f, 0.0f, 0.0f);
		}

		PassInfo info;
		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(QAngle *);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info, nullptr, 0);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return QAngle(0.0f, 0.0f, 0.0f);
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this;

	QAngle *angles;
	callWrapper->Execute(vparams, &angles);

	return angles ? *angles : QAngle(0.0f, 0.0f, 0.0f);
}

BaseEntity *BasePlayer::GetAimTarget()
{
	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( !client )
	{
		return nullptr;
	}

	Vector dir;
	AngleVectors(GetEyeAngles(), &dir);

	Vector eyePos;
	g_pExtension->m_pServerGameClients->ClientEarPosition(client, &eyePos);

	Ray_t ray;
	ray.Init(eyePos, eyePos + dir * 8000.0f);

	trace_t trace;
	CTraceFilterAimTarget target(client->GetIServerEntity());

	g_pExtension->m_pEngineTrace->TraceRay(ray, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, &target, &trace);

	if( trace.DidHit() )
	{
		return (BaseEntity *) trace.m_pEnt;
	}

	return nullptr;
}

Vector BasePlayer::GetAimTarget2()
{
	edict_t *client = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( !client )
	{
		return Vector(0.0f, 0.0f, 0.0f);
	}

	Vector dir;
	AngleVectors(GetEyeAngles(), &dir);

	Vector eyePos;
	g_pExtension->m_pServerGameClients->ClientEarPosition(client, &eyePos);

	Ray_t ray;
	ray.Init(eyePos, eyePos + dir * 8000.0f);

	trace_t trace;
	CTraceFilterAimTarget2 target;

	g_pExtension->m_pEngineTrace->TraceRay(ray, MASK_SHOT, &target, &trace);

	if( trace.DidHit() )
	{
		return trace.endpos;
	}

	return Vector(0.0f, 0.0f, 0.0f);
}

int BasePlayer::GetArmor()
{
	if( g_pExtension->m_ArmorValue < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(uint8 *) ((unsigned char *) this + g_pExtension->m_ArmorValue);
}

void BasePlayer::SetArmor(int value, bool helmet)
{
	if( g_pExtension->m_ArmorValue < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(uint8 *) ((unsigned char *) this + g_pExtension->m_ArmorValue) = value;

	if( value > 0 )
	{
		*(bool *) ((unsigned char *) this + g_pExtension->m_bHasHeavyArmor) = false;
		*(bool *) ((unsigned char *) this + g_pExtension->m_bHasHelmet) = helmet;
	}
	else
	{
		*(bool *) ((unsigned char *) this + g_pExtension->m_bHasHeavyArmor) = false;
		*(bool *) ((unsigned char *) this + g_pExtension->m_bHasHelmet) = false;
	}

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_ArmorValue);
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bHasHeavyArmor);
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bHasHelmet);
	}
}

float BasePlayer::GetSpeed()
{
	if( g_pExtension->m_flLaggedMovementValue < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flLaggedMovementValue) * 250.0f;
}

void BasePlayer::SetSpeed(float value)
{
	if( g_pExtension->m_flLaggedMovementValue < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flLaggedMovementValue) = value / 250.0f;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flLaggedMovementValue);
	}
}

float BasePlayer::GetVelocityModifier()
{
	if( g_pExtension->m_flVelocityModifier < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) this + g_pExtension->m_flVelocityModifier);
}

void BasePlayer::SetVelocityModifier(float value)
{
	if( g_pExtension->m_flVelocityModifier < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) this + g_pExtension->m_flVelocityModifier) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flVelocityModifier);
	}
}

bool BasePlayer::IsScoped()
{
	if( g_pExtension->m_bIsScoped < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	return *(bool *) ((unsigned char *) this + g_pExtension->m_bIsScoped);
}

void BasePlayer::SetScoped(bool value)
{
	if( g_pExtension->m_bIsScoped < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(bool *) ((unsigned char *) this + g_pExtension->m_bIsScoped) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bIsScoped);
	}
}

int BasePlayer::GetFOV()
{
	if( g_pExtension->m_iFOV < 1 || g_pExtension->m_iDefaultFOV < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 90.0f;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iFOV);
}

void BasePlayer::SetFOV(int value)
{
	if( g_pExtension->m_iFOV < 1 || g_pExtension->m_iDefaultFOV < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iFOV) = value;
	*(int *) ((unsigned char *) this + g_pExtension->m_iDefaultFOV) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iFOV);
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iDefaultFOV);
	}
}

bool BasePlayer::IsFlashLightOn()
{
	return GetEffects() & EF_DIMLIGHT ? true : false;
}

void BasePlayer::SetFlashLight(bool on)
{
	if( on )
	{
		SetEffects(GetEffects() | EF_DIMLIGHT);
	}
	else
	{
		SetEffects(GetEffects() & ~EF_DIMLIGHT);
	}
}

bool BasePlayer::ToggleFlashLight()
{
	bool value = !IsFlashLightOn();
	SetFlashLight(value);

	return value;
}

bool BasePlayer::IsNightVisionOn()
{
	if( g_pExtension->m_bHasNightVision < 1 || g_pExtension->m_bNightVisionOn < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return false;
	}

	if( !*(bool *) ((unsigned char *) this + g_pExtension->m_bHasNightVision) )
	{
		return false;
	}

	return *(bool *) ((unsigned char *) this + g_pExtension->m_bNightVisionOn);
}

void BasePlayer::SetNightVision(bool on, const char *overlay)
{
	if( g_pExtension->m_bHasNightVision < 1 || g_pExtension->m_bNightVisionOn < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

//	*(bool *) ((unsigned char *) this + g_pExtension->m_bHasNightVision) = on;
//	*(bool *) ((unsigned char *) this + g_pExtension->m_bNightVisionOn) = on;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
//		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bHasNightVision);
//		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_bNightVisionOn);

//		static ConCommandBase *r_screenoverlay = g_pExtension->m_pConsoleVars->FindCommandBase("r_screenoverlay");
		static ConVarRef r_screenoverlay("r_screenoverlay");

		if( r_screenoverlay.IsValid() )
		{
			if( r_screenoverlay.IsFlagSet(FCVAR_CHEAT) )
			{
				CONSOLE_DEBUGGER("r_screenoverlay is cheaty!");
			}
			else
			{
				CONSOLE_DEBUGGER("r_screenoverlay is not cheaty!");
			}

		}
		else
		{
			CONSOLE_DEBUGGER("Could not find r_screenoverlay!");
		}

		if( on && overlay && *overlay )
		{
			g_pExtension->m_pEngineServer->ClientCommand(edict, "r_screenoverlay %s", overlay);
		}
		else
		{
			g_pExtension->m_pEngineServer->ClientCommand(edict, "r_screenoverlay \"\"");
		}
	}
}

bool BasePlayer::ToggleNightVision()
{
	bool value = !IsNightVisionOn();
	SetNightVision(value);

	return value;
}

const char *BasePlayer::GetArmsmodel()
{
	if( g_pExtension->m_szArmsModel < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	return STRING(*(string_t *) ((unsigned char *) this + g_pExtension->m_szArmsModel));
}

void BasePlayer::SetArmsmodel(const char *model)
{
	if( g_pExtension->m_szArmsModel < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	ke::SafeStrcpy((char *) this + g_pExtension->m_szArmsModel, DT_MAX_STRING_BUFFERSIZE, model);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_szArmsModel);		
	}
}

BaseViewModel *BasePlayer::CreateViewModel(int id)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		int offset = 0;
		if( !g_pGameMod->GetConfig()->GetOffset("CCSPlayer::CreateViewModel", &offset) || offset < 1 )
		{
			CONSOLE_DEBUGGER("CCSPlayer::CreateViewModel offset is not found!");
			return nullptr;
		}

		PassInfo info[2];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(int);
		info[0].type = PassType_Basic;	
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(CBaseEntity *);
		info[1].type = PassType_Basic;		

		callWrapper = g_pExtension->m_pBinTools->CreateVCall(offset, 0, 0, &info[1], info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return nullptr;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(int)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(int *) vparams = id;	

	BaseViewModel *viewmodelEnt;
	callWrapper->Execute(params, &viewmodelEnt);

	return viewmodelEnt;
}

BaseViewModel *BasePlayer::GetViewModel(int id)
{
	if( g_pExtension->m_hViewModel < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *viewModelHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hViewModel + id * sizeof(CBaseHandle *));

	if( !viewModelHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *viewModelEnt = gamehelpers->ReferenceToEntity(viewModelHandle->GetEntryIndex());

	if( !viewModelEnt || *viewModelHandle != ((IHandleEntity *) viewModelEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseViewModel *) viewModelEnt;
}

void BasePlayer::SetViewModel(int id, BaseViewModel *viewModelEnt)
{
	if( g_pExtension->m_hViewModel < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *viewModelHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hViewModel + id * sizeof(CBaseHandle *));
	viewModelHandle->Set((IHandleEntity *) viewModelEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hViewModel);
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hViewModel + 4);
	}	
}

BaseEntity *BasePlayer::GetRagdoll()
{
	if( g_pExtension->m_hRagdoll < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *ragdollHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hRagdoll);
/*
	if( !ragdollHandle->IsValid() )
	{
		return nullptr;
	}
*/
	CONSOLE_DEBUGGER("ragdollHandle->GetEntryIndex(): %d", ragdollHandle->GetEntryIndex());

	CBaseEntity *ragdollEnt = gamehelpers->ReferenceToEntity(ragdollHandle->GetEntryIndex());

	if( !ragdollEnt || *ragdollHandle != ((IHandleEntity *) ragdollEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseWeapon *) ragdollEnt;
}

BaseWeapon *BasePlayer::GetActiveWeapon()
{
	if( g_pExtension->m_hActiveWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hActiveWeapon);

	if( !weaponHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *weaponEnt = gamehelpers->ReferenceToEntity(weaponHandle->GetEntryIndex());

	if( !weaponEnt || *weaponHandle != ((IHandleEntity *) weaponEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseWeapon *) weaponEnt;
}

void BasePlayer::SetActiveWeapon(BaseWeapon *weaponEnt)
{
	if( g_pExtension->m_hActiveWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hActiveWeapon);
	weaponHandle->Set((IHandleEntity *) weaponEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hActiveWeapon);
	}
}

BaseWeapon *BasePlayer::GetLastWeapon()
{
	if( g_pExtension->m_hLastWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hLastWeapon);

	if( !weaponHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *weaponEnt = gamehelpers->ReferenceToEntity(weaponHandle->GetEntryIndex());

	if( !weaponEnt || *weaponHandle != ((IHandleEntity *) weaponEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseWeapon *) weaponEnt;
}

void BasePlayer::SetLastWeapon(BaseWeapon *weaponEnt)
{
	if( g_pExtension->m_hLastWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hLastWeapon);
	weaponHandle->Set((IHandleEntity *) weaponEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hLastWeapon);
	}
}

BaseWeapon *BasePlayer::GetWeapons(int id)
{
	if( g_pExtension->m_hMyWeapons < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hMyWeapons + (id * sizeof(CBaseHandle *)));

	if( !weaponHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *weaponEnt = gamehelpers->ReferenceToEntity(weaponHandle->GetEntryIndex());

	if( !weaponEnt || *weaponHandle != ((IHandleEntity *) weaponEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseWeapon *) weaponEnt;
}

void BasePlayer::SetWeapons(int id, BaseWeapon *weaponEnt)
{
	if( g_pExtension->m_hMyWeapons < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	CBaseHandle *weaponHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hMyWeapons + (id * sizeof(CBaseHandle *)));
	weaponHandle->Set((IHandleEntity *) weaponEnt);

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_hMyWeapons);
	}
}

int BasePlayer::GetIndexInMyWeapons(BaseWeapon *weapon)
{
	BaseWeapon *weaponEnt = nullptr;

	for( int i = 0; i < CSGO_MAX_WEAPONS; i++ )
	{
		weaponEnt = GetWeapons(i);

		if( !weaponEnt )
		{
			continue;
		}

		if( weaponEnt == weapon )
		{
			return i;
		}
	}

	return -1;
}

int BasePlayer::GetIndexInMyWeapons(const char *classname)
{
	BaseWeapon *weaponEnt = nullptr;

	for( int i = 0; i < CSGO_MAX_WEAPONS; i++ )
	{
		weaponEnt = GetWeapons(i);

		if( !weaponEnt )
		{
			continue;
		}

		if( strcmp(GetClassname(), classname) == 0 )
		{
			return i;
		}
	}

	return -1;
}

int BasePlayer::GetTeam()
{
	IGamePlayer *player = playerhelpers->GetGamePlayer(g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this));

	if( !player )
	{
		CONSOLE_DEBUGGER("CS:GO is not supported!");
		return CSGO_TEAM_NONE;
	}

	IPlayerInfo *info = player->GetPlayerInfo();

	if( !info )
	{
		CONSOLE_DEBUGGER("Could not find player info");
		return CSGO_TEAM_NONE;
	}

	return info->GetTeamIndex();
}

void BasePlayer::SetTeam(int team)
{
	// Using any other way will crash the game, good luck with that
	if( team == CSGO_TEAM_SPECTATOR )
	{
		IGamePlayer *player = playerhelpers->GetGamePlayer(g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this));

		if( !player )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		IPlayerInfo *info = player->GetPlayerInfo();

		if( !info )
		{
			CONSOLE_DEBUGGER("Could not find player info");
			return;
		}

		info->ChangeTeam(CSGO_TEAM_SPECTATOR);
		return;
	}

#if defined _WINDOWS
	static void *address = nullptr;
	if( !g_pExtension->m_pCStrikeConfig->GetMemSig("SwitchTeam", &address) || !address )
	{
		CONSOLE_DEBUGGER("SwitchTeam address is not found!");
		return;
	}

	void *thisEntity = this;
	void *gamerules = g_pExtension->m_pSDKTools->GetGameRules();

	if( !gamerules )
	{
		CONSOLE_DEBUGGER("Couldn't find gamerules pointer");
		return;
	}

	__asm
	{
		push team
		mov ecx, thisEntity
		mov ebx, gamerules

		call address
	}
#elif defined _LINUX	
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pExtension->m_pCStrikeConfig->GetMemSig("SwitchTeam", &address) || !address )
		{
			CONSOLE_DEBUGGER("SwitchTeam address is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(int);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(int)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) this; vparams += sizeof(CBaseEntity *);
	*(int *) vparams = team;

	callWrapper->Execute(params, nullptr);
#endif
}

// TODO: OFFSET MAY BE WRONG
int BasePlayer::GetMVPs()
{
	if( g_pExtension->m_bIsHoldingLookAtWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_bIsHoldingLookAtWeapon + 15);
}
// TODO: OFFSET MAY BE WRONG
void BasePlayer::SetMVPs(int value)
{
	if( g_pExtension->m_bIsHoldingLookAtWeapon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	int offset = g_pExtension->m_bIsHoldingLookAtWeapon + 15;

	*(int *) ((unsigned char *) this + offset) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, offset);
	}
}

int BasePlayer::GetHideHUD()
{
	if( g_pExtension->m_iHideHUD < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iHideHUD);
}

void BasePlayer::SetHideHUD(int value)
{
	if( g_pExtension->m_iHideHUD < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iHideHUD) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iHideHUD);
	}
}

int BasePlayer::GetAddon()
{
	if( g_pExtension->m_iAddonBits < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iAddonBits);
}

void BasePlayer::SetAddon(int value)
{
	if( g_pExtension->m_iAddonBits < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iAddonBits) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAddonBits);
	}
}

int BasePlayer::GetPrimaryAddon()
{
	if( g_pExtension->m_iPrimaryAddon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iPrimaryAddon);
}

void BasePlayer::SetPrimaryAddon(int value)
{
	if( g_pExtension->m_iPrimaryAddon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iPrimaryAddon) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iPrimaryAddon);
	}
}

int BasePlayer::GetSecondaryAddon()
{
	if( g_pExtension->m_iSecondaryAddon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iSecondaryAddon);
}

void BasePlayer::SetSecondaryAddon(int value)
{
	if( g_pExtension->m_iSecondaryAddon < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(int *) ((unsigned char *) this + g_pExtension->m_iSecondaryAddon) = value;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iSecondaryAddon);
	}
}

/*
* Slot is complicated than it looks like.... https://forums.alliedmods.net/showthread.php?t=81546&page=2
*
* Check these: https://forums.alliedmods.net/showthread.php?t=271090
*/
int BasePlayer::GetWeaponAmmo(BaseWeapon *weaponEnt)
{
	if( g_pExtension->m_iAmmo < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	const char *classname = weaponEnt->GetClassname();

	// TODO: Exclude knives, c4 and other bullshits...

	if( strcmp(classname, "weapon_hegrenade") == 0 )
	{
		return ((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_HEGRENADE];
	}
	else if( strcmp(classname, "weapon_flashbang") == 0 )
	{
		return ((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_FLASHBANG];
	}
	else if( strcmp(classname, "weapon_smokegrenade") == 0 )
	{
		return ((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_SMOKEGRENADE];
	}
	else if( strcmp(classname, "weapon_molotov") == 0 || strcmp(classname, "weapon_incgrenade") == 0 )
	{
		return ((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_INFERNO];
	}
	else if( strcmp(classname, "weapon_decoy") == 0 )
	{
		return ((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_DECOY];
	}

	return *(int *) ((unsigned char *) weaponEnt + g_pExtension->m_iPrimaryReserveAmmoCount);
}

/*
* Slot is complicated than it looks like.... https://forums.alliedmods.net/showthread.php?t=81546&page=2
*
* Check these: https://forums.alliedmods.net/showthread.php?t=271090
*/
void BasePlayer::SetWeaponAmmo(BaseWeapon *weaponEnt, int value)
{
	if( g_pExtension->m_iAmmo < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	const char *classname = weaponEnt->GetClassname();

	// TODO: Exclude knives, c4 and other bullshits...

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) this);

	if( !edict || strstr(classname, "knife") )
	{
		return;
	}

	if( strcmp(classname, "weapon_hegrenade") == 0 )
	{
		((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_HEGRENADE] = value;
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAmmo + WEAPONAMMO_HEGRENADE);

		return;
	}
	else if( strcmp(classname, "weapon_flashbang") == 0 )
	{
		((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_FLASHBANG] = value;
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAmmo + WEAPONAMMO_FLASHBANG);

		return;
	}
	else if( strcmp(classname, "weapon_smokegrenade") == 0 )
	{
		((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_SMOKEGRENADE] = value;
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAmmo + WEAPONAMMO_SMOKEGRENADE);

		return;
	}
	else if( strcmp(classname, "weapon_molotov") == 0 || strcmp(classname, "weapon_incgrenade") == 0 )
	{
		((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_INFERNO] = value;
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAmmo + WEAPONAMMO_INFERNO);

		return;
	}
	else if( strcmp(classname, "weapon_decoy") == 0 )
	{
		((int *) ((unsigned char *) this + g_pExtension->m_iAmmo))[WEAPONAMMO_DECOY] = value;
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iAmmo + WEAPONAMMO_DECOY);

		return;
	}

	*(int *) ((unsigned char *) weaponEnt + g_pExtension->m_iPrimaryReserveAmmoCount) = value;

	edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) weaponEnt);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_iPrimaryReserveAmmoCount);
	}
}

int BasePlayer::GetSpecMode()
{
	if( g_pExtension->m_iObserverMode < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_iObserverMode);
}

BasePlayer *BasePlayer::GetSpecTarget()
{
	if( g_pExtension->m_hObserverTarget < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *targetHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hObserverTarget);

	if( !targetHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *targetEnt = gamehelpers->ReferenceToEntity(targetHandle->GetEntryIndex());

	if( !targetEnt || *targetHandle != ((IHandleEntity *) targetEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BasePlayer *) targetEnt;
}

BaseEntity *BasePlayer::GetGroundEntity()
{
	if( g_pExtension->m_hGroundEntity < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return nullptr;
	}

	CBaseHandle *groundEntityHandle = (CBaseHandle *) ((unsigned char *) this + g_pExtension->m_hGroundEntity);

	if( !groundEntityHandle->IsValid() )
	{
		return nullptr;
	}

	CBaseEntity *groundEnt = gamehelpers->ReferenceToEntity(groundEntityHandle->GetEntryIndex());

	if( !groundEnt || *groundEntityHandle != ((IHandleEntity *) groundEnt)->GetRefEHandle() )
	{
		return nullptr;
	}

	return (BaseEntity *) groundEnt;
}

int BasePlayer::GetButtons()
{
	if( g_pExtension->m_nButtons < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return 0;
	}

	return *(int *) ((unsigned char *) this + g_pExtension->m_nButtons);
}

float GetFlameEntityLifeTime(BaseEntity *entity)
{
	if( g_pExtension->m_flLifetime < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) entity + g_pExtension->m_flLifetime);
}

void SetFlameEntityLifeTime(BaseEntity *entity, float time)
{
	if( g_pExtension->m_flLifetime < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) entity + g_pExtension->m_flLifetime) = time;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) entity);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flLifetime);
	}
}

float GetEntityDissolveTime(BaseEntity *entity)
{
	if( g_pExtension->m_flDissolveStartTime < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return -1.0f;
	}

	return *(float *) ((unsigned char *) entity + g_pExtension->m_flDissolveStartTime);
}

void SetEntityDissolveTime(BaseEntity *entity, float time)
{
	if( g_pExtension->m_flDissolveStartTime < 1 )
	{
		CONSOLE_DEBUGGER("Offset not found yet.");
		return;
	}

	*(float *) ((unsigned char *) entity + g_pExtension->m_flDissolveStartTime) = time;

	edict_t *edict = g_pExtension->m_pServerGameEntities->BaseEntityToEdict((CBaseEntity *) entity);

	if( edict )
	{
		gamehelpers->SetEdictStateChanged(edict, g_pExtension->m_flDissolveStartTime);
	}
}

void SpawnGib(BaseEntity *gibEnt, const char *model)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CGib::Spawn", &address) || !address )
		{
			CONSOLE_DEBUGGER("CGib::Spawn address is not found!");
			return;
		}

		PassInfo info;

		info.flags = PASSFLAG_BYVAL;
		info.size = sizeof(const char *);
		info.type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, &info, 1);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(CBaseEntity *) + sizeof(const char *)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) gibEnt; vparams += sizeof(CBaseEntity *);
	*(const char **) vparams = model;

	callWrapper->Execute(params, nullptr);
}

void InitGib(BaseEntity *gibEnt, BasePlayer *victimEnt, float minSpeed, float maxSpeed)
{
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CGib::InitGib", &address) || !address )
		{
			CONSOLE_DEBUGGER("CGib::InitGib address is not found!");
			return;
		}

		PassInfo info[3];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(CBaseEntity *);
		info[0].type = PassType_Basic;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(float);
		info[1].type = PassType_Float;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(float);
		info[2].type = PassType_Float;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, info, 3);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[2 * sizeof(CBaseEntity *) + 2 * sizeof(float)];
	unsigned char *vparams = params;

	*(CBaseEntity **) vparams = (CBaseEntity *) gibEnt; vparams += sizeof(CBaseEntity *);
	*(CBaseEntity **) vparams = (CBaseEntity *) victimEnt; vparams += sizeof(CBaseEntity *);
	*(float *) vparams = maxSpeed; vparams += sizeof(float);									
	*(float *) vparams = minSpeed;

	callWrapper->Execute(params, nullptr);
}

void SpawnSpecificGibs(BasePlayer *victimEnt, int gibs, float minSpeed, float maxSpeed, const char *model, float lifeTime)
{
	float currentTime = g_pExtension->m_pGlobals->curtime;

	for( int i = 0; i < gibs; i++ )
	{
		BaseAnimating *gibEnt = (BaseAnimating *) BaseEntity::CreateEntity("gib");

		if( !gibEnt )
		{
			continue;
		}

		SpawnGib(gibEnt, model);

		gibEnt->SetBody(i);

		InitGib(gibEnt, victimEnt, minSpeed, maxSpeed);

		// Check if this actually works
		SetEntityDissolveTime(gibEnt, currentTime + RandomFloat(15.0f, 45.0f));

//		pGib->m_lifeTime = lifeTime;

		if( victimEnt )
		{
			if( victimEnt->GetFlags() & FL_ONFIRE )
			{
				float flameLifeTime = 0.0f;

				// TODO: Get duration from the corpse's Ignition entity and make this work perfectly
				BaseEntity *flameEnt = victimEnt;

				while( (flameEnt = BaseEntity::FindEntity(flameEnt, "entityflame")) )
				{
					// Making sure we get the latest flame entity...
					flameLifeTime = GetFlameEntityLifeTime(flameEnt);
				}

				// Add some random time
				flameLifeTime -= currentTime - RandomFloat(3.0f, 5.0f);

				// For now it'll be a randomization of 10 - 15 secs
				gibEnt->Ignite(flameLifeTime);
			}
		}		
	}
}

BaseProp *CreateEntityGlow(BaseEntity *entity, int style, Color color, const char *attachement)
{
	BaseProp *glowEnt = (BaseProp *) BaseEntity::CreateEntity("prop_dynamic_glow");

	if( !glowEnt )
	{
		return nullptr;
	}

//	glowEnt->SetKeyValue("model", model);
	g_pExtension->m_pServerTools->SetKeyValue(glowEnt, "model", entity->GetModelName());
	glowEnt->SetKeyValue("disablereceiveshadows", 1);
	glowEnt->SetKeyValue("disableshadows", 1);
	glowEnt->SetKeyValue("solid", SOLID_NONE);
	glowEnt->SetKeyValue("spawnflags", 256);

	glowEnt->SetCollisionGroup(COLLISION_GROUP_NONE);
	glowEnt->Spawn();

	glowEnt->SetRenderMode(kRenderGlow);
	glowEnt->SetRenderColor(Color(0, 0, 0, 0));

	glowEnt->SetEffects(EF_BONEMERGE);

	BaseEntity::SetInputVariant("!activator");
	glowEnt->AcceptInput("SetParent", entity, glowEnt);

	if( attachement && *attachement )
	{
		BaseEntity::SetInputVariant(attachement);
		glowEnt->AcceptInput("SetParentAttachment", glowEnt, glowEnt);
	}

	glowEnt->SetGlow(true);
	glowEnt->SetGlowStyle(style);
	glowEnt->SetGlowColor(color);
	glowEnt->SetGlowMaxDist(100000.0f);

	return glowEnt;
}

BaseEntity *CreateParticleSystem(Vector pos, QAngle angles, const char *effect, BaseEntity *parentEnt, const char *attachement, float killDelay)
{
	BaseEntity *particlesEnt = BaseEntity::CreateEntity("info_particle_system");

	if( !particlesEnt )
	{
		return nullptr;
	}

	particlesEnt->SetKeyValue("origin", pos);
	particlesEnt->SetKeyValue("angles", angles);
	particlesEnt->SetKeyValue("start_active", "1");
//	particlesEnt->SetKeyValue("effect_name", effect);
	g_pExtension->m_pServerTools->SetKeyValue(particlesEnt, "effect_name", effect);

	particlesEnt->Spawn();

	if( parentEnt )
	{
		BaseEntity::SetInputVariant("!activator");
		particlesEnt->AcceptInput("SetParent", parentEnt, particlesEnt);

		if( attachement && *attachement )
		{
			BaseEntity::SetInputVariant(attachement);
			particlesEnt->AcceptInput("SetParentAttachment", parentEnt, particlesEnt);
		}
	}

	particlesEnt->Activate();
	particlesEnt->AcceptInput("Start");

	if( killDelay > 0.0f )
	{
		static char buffer[64];
		ke::SafeSprintf(buffer, sizeof(buffer), "OnUser1 !self:Kill::%f:1", killDelay);

		BaseEntity::SetInputVariant(buffer);
		particlesEnt->AcceptInput("AddOutput");
		particlesEnt->AcceptInput("FireUser1");
	}

	return particlesEnt;
}