#include "tempentities.h"

static ICallWrapper *g_pCallWrapper = nullptr;
static void *g_pFirstTempEntity = nullptr;

static SourceHook::List<TempEntity *> g_pTempEntities;
static SourceHook::List<TempEntityHooker *> g_pTempEntityHooks;

static int g_NameOffset, g_NextTempEntityOffset, g_ServerClassOffset;

// Just to skip all that search again....
// Basically saves us a loop, we do one single loop in the pre-hook and then we just save the address of our hook builder
// An other way would be to do 2 loops, instead of just 1, which is.... kinda idiotic
static TempEntityHooker *g_LastKnownHook = nullptr;

static void	OnPreEnginePlaybackTempEntity(IRecipientFilter &filter, float delay, const void *sender, const SendTable *sendTable, int classId)
{
	// Reset this every time we look into the hook
	g_LastKnownHook = nullptr;

	TempEntity *tempEnt = TempEntity::FindTempEntity(*(char **) ((unsigned char *) sender + g_NameOffset));	

	if( !tempEnt )
	{
		RETURN_META(MRES_IGNORED);
	}
	
	TempEntityHooker *hook = nullptr, *preHook = nullptr;

	for( auto iterator = g_pTempEntityHooks.begin(); iterator != g_pTempEntityHooks.end(); iterator++ )
	{
		hook = *iterator;

		if( !hook || hook->GetEntity() != tempEnt )
		{
			hook = nullptr;
			continue;
		}

		if( hook->IsPostHook() )
		{
			g_LastKnownHook = hook;
			hook = nullptr;

			// Found the pre counterpart? destroy the loop, otherwise just keep searching if it exists. until the end.
			if( preHook )
			{
				break;
			}
		}	
		else
		{
			preHook = hook;

			// Found the post counterpart? destroy the loop, otherwise just keep searching if it exists. until the end.
			if( g_LastKnownHook )
			{
				break;
			}
		}
	}

	if( preHook )
	{
		TempEntPreHook_t callback = (TempEntPreHook_t) preHook->GetCallback();

		if( callback )
		{
			ResultType result = callback(tempEnt, filter, delay);

			switch( result )
			{
			case Pl_Continue:
				RETURN_META(MRES_IGNORED);

			case Pl_Changed:
				RETURN_META_NEWPARAMS(MRES_HANDLED, &IVEngineServer::PlaybackTempEntity, (filter, delay, sender, sendTable, classId));

			default:
				RETURN_META(MRES_SUPERCEDE);
			}
		}
	}
}

static void	OnPostEnginePlaybackTempEntity(IRecipientFilter &filter, float delay, const void *sender, const SendTable *sendTable, int classId)
{
	if( !g_LastKnownHook )
	{
		RETURN_META(MRES_IGNORED);
	}

	TempEntPostHook_t callback = (TempEntPostHook_t) g_LastKnownHook->GetCallback();

	if( callback )
	{
		callback(g_LastKnownHook->GetEntity(), filter, delay);
	}
}

SH_DECL_HOOK5_void(IVEngineServer, PlaybackTempEntity, SH_NOATTRIB, 0, IRecipientFilter &, float, const void *, const SendTable *, int)

TempEntity::TempEntity(const char *name, void *address)
{
	m_pName = name;
	m_pAddress = address;

	g_pCallWrapper->Execute(&address, &m_pServerClass);
}

TempEntity::~TempEntity()
{
	m_pName = nullptr;
	m_pAddress = nullptr;
	m_pServerClass = nullptr;
}

const char *TempEntity::GetName()
{
	return m_pName;
}

void TempEntity::Fire(IRecipientFilter &filter, float delay)
{
	g_pExtension->m_pEngineServer->PlaybackTempEntity(filter, delay, m_pAddress, m_pServerClass->m_pTable, m_pServerClass->m_ClassID);
}

bool TempEntity::GetFirstTempEntity()
{
	void *address = nullptr;

	if( g_pExtension->m_pSDKConfig->GetMemSig("s_pTempEntities", &address) && address )
	{
		g_pFirstTempEntity = *(void **) address;
	}

	if( !g_pFirstTempEntity )
	{
		if( g_pExtension->m_pSDKConfig->GetMemSig("CBaseTempEntity", &address) && address )
		{
			int offset = 0;

			if( !g_pExtension->m_pSDKConfig->GetOffset("s_pTempEntities", &offset) || offset < 1 )
			{
				g_pSM->LogError(myself, "s_pTempEntities offset is not found!");
				return false;
			}

#ifdef PLATFORM_X86
			g_pFirstTempEntity = **(void ***) ((unsigned char *) address + offset);
#else
			int32_t varOffset = *(int32_t *) ((unsigned char *) address + offset);
			g_pFirstTempEntity = **(void ***) ((unsigned char *) address + offset + sizeof(int32_t) + varOffset);
#endif
			if( !g_pFirstTempEntity )
			{
				return false;
			}
		}
		else
		{
			g_pSM->LogError(myself, "CBaseTempEntity address is not found!");
		}
	}

	if( !g_pExtension->m_pSDKConfig->GetOffset("GetTEName", &g_NameOffset) )
	{
		g_pSM->LogError(myself, "s_pTempEntities offset is not found!");
		return false;
	}

	if( !g_pExtension->m_pSDKConfig->GetOffset("GetTENext", &g_NextTempEntityOffset) )
	{
		g_pSM->LogError(myself, "GetTENext offset is not found!");
		return false;
	}

	if( !g_pExtension->m_pSDKConfig->GetOffset("TE_GetServerClass", &g_ServerClassOffset) )
	{
		g_pSM->LogError(myself, "TE_GetServerClass offset is not found!");
		return false;
	}
	
	PassInfo info;

	info.flags = PASSFLAG_BYVAL;
	info.type = PassType_Basic;
	info.size = sizeof(ServerClass *);

	g_pCallWrapper = g_pExtension->m_pBinTools->CreateVCall(g_ServerClassOffset, 0, 0, &info, nullptr, 0);

	if( !g_pCallWrapper )
	{
		return false;
	}

	g_pExtension->m_pBinCallWrappers.push_back(g_pCallWrapper);

	return true;
}

TempEntity *TempEntity::FindTempEntity(const char *name)
{
	void *address = g_pFirstTempEntity;
		
	if( !address )
	{
		return nullptr;
	}

	TempEntity *tempEnt = nullptr;

	// First check if its one of ours
	for( auto iterator = g_pTempEntities.begin(); iterator != g_pTempEntities.end(); iterator++ )
	{
		tempEnt = *iterator;

		if( !tempEnt || strcmp(name, tempEnt->GetName()) != 0 )
		{
			tempEnt = nullptr;
			continue;
		}

		// Temp ent found!
		break;
	}

	// If not, search deeply in the engine and make sure to push it into our TempEntities list so it'll be easier to retreive it next time
	if( !tempEnt )
	{
		char *tempEntName = nullptr;

		do
		{
			tempEntName = *(char **) ((unsigned char *) address + g_NameOffset);

			if( !tempEntName || strcmp(tempEntName, name) != 0 )
			{
				continue;
			}

			tempEnt = new TempEntity(tempEntName, address);
			g_pTempEntities.push_back(tempEnt);
		} while( (address = *(void **) ((unsigned char *) address + g_NextTempEntityOffset)) );
	}

	return tempEnt;
}

TempEntityHooker::TempEntityHooker(TempEntity *entity, void *callback, bool post)
{
	m_pEntity = entity;
	m_pCallback = callback;
	m_IsPost = post;
}

TempEntityHooker::~TempEntityHooker()
{
	m_pEntity = nullptr;
	m_pCallback = nullptr;
	m_IsPost = false;
}

TempEntity *TempEntityHooker::GetEntity()
{
	return m_pEntity;
}

void *TempEntityHooker::GetCallback()
{
	return m_pCallback;
}

bool TempEntityHooker::IsPostHook()
{
	return m_IsPost;
}

void TempEntityHooker::Load()
{
	if( !TempEntity::GetFirstTempEntity() )
	{
		return;
	}

	// Invoking our hooks
	SH_ADD_HOOK(IVEngineServer, PlaybackTempEntity, engine, SH_STATIC(OnPreEnginePlaybackTempEntity), false);
	SH_ADD_HOOK(IVEngineServer, PlaybackTempEntity, engine, SH_STATIC(OnPostEnginePlaybackTempEntity), true);
}

void TempEntityHooker::Free()
{
	// Making sure we're not corrupting stuff
	if( !g_pFirstTempEntity )
	{
		return;
	}

	// Releasing our hooks
	SH_REMOVE_HOOK(IVEngineServer, PlaybackTempEntity, engine, SH_STATIC(OnPreEnginePlaybackTempEntity), false);
	SH_REMOVE_HOOK(IVEngineServer, PlaybackTempEntity, engine, SH_STATIC(OnPostEnginePlaybackTempEntity), true);

	// Release the memory
	RELEASE_POINTERS_ARRAY(g_pTempEntities);
}

bool TempEntityHooker::AddHook(TempEntity *tempEnt, void *callback, bool post)
{
	if( !tempEnt || !callback )
	{
		return false;
	}

	TempEntityHooker *hook = new TempEntityHooker(tempEnt, callback, post);

	if( hook )
	{
		g_pTempEntityHooks.push_back(hook);
		return true;
	}

	return false;
}

bool TempEntityHooker::AddHook(const char *name, void *callback, bool post)
{
	return AddHook(TempEntity::FindTempEntity(name), callback, post);
}

void TempEntityHooker::RemoveHook(TempEntity *tempEnt, bool post)
{
	TempEntityHooker *hook = nullptr;

	// Check for any hook linked to this entity
	for( auto iterator = g_pTempEntityHooks.begin(); iterator != g_pTempEntityHooks.end(); iterator++ )
	{
		hook = *iterator;

		if( hook->GetEntity() != tempEnt || hook->IsPostHook() != post )
		{
			hook = nullptr;
			continue;
		}

		// Make sure it's removed from our hook list
		g_pTempEntityHooks.remove(hook);

		// Freeing memory, just as important as breathing
		delete hook;
	}	
}

void TempEntityHooker::RemoveHook(const char *name, bool post)
{
	RemoveHook(TempEntity::FindTempEntity(name), post);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////			SOME USEFUL SETUPS		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TE_Sprite(IRecipientFilter &filter, float delay, Vector origin, int modelIndex, float scale, int brightness)
{
	static TempEntity *spriteEnt = TempEntity::FindTempEntity("Sprite");

	if( !spriteEnt )
	{
		return;
	}

	spriteEnt->SetProperty<Vector>("m_vecOrigin", origin);
	spriteEnt->SetProperty<int>("m_nModelIndex", modelIndex);
	spriteEnt->SetProperty<float>("m_fScale", scale);
	spriteEnt->SetProperty<int>("m_nBrightness", brightness);

	// Fire after carefull NASA preparation
	spriteEnt->Fire(filter, delay);
}

void TE_WorldDecal(IRecipientFilter &filter, float delay, Vector origin, int decalIndex)
{
	static TempEntity *decalEnt = TempEntity::FindTempEntity("World Decal");

	if( !decalEnt )
	{
		return;
	}

	decalEnt->SetProperty<Vector>("m_vecOrigin", origin);
	decalEnt->SetProperty<int>("m_nIndex", decalIndex);

	// Fire after carefull NASA preparation
	decalEnt->Fire(filter, delay);
}

void TE_BeamFollow(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, BaseEntity *entity)
{
	if( !entity )
	{
		return;
	}

	static TempEntity *beamEnt = TempEntity::FindTempEntity("BeamFollow");

	if( !beamEnt )
	{
		return;
	}

	beamEnt->SetProperty<int>("m_nModelIndex", beamModelIndex);
	beamEnt->SetProperty<int>("m_nHaloIndex", haloModelIndex);
	beamEnt->SetProperty<int>("m_nStartFrame", startFrame);	// look for more info about this
	beamEnt->SetProperty<int>("m_nFrameRate", frameRate);	// look for more info about this
	beamEnt->SetProperty<float>("m_fLife", life);
	beamEnt->SetProperty<float>("m_fWidth", width);
	beamEnt->SetProperty<float>("m_fEndWidth", endWidth);		// look for more info about this
	beamEnt->SetProperty<int>("m_nFadeLength", fadeLength);
	beamEnt->SetProperty<float>("m_fAmplitude", amplitude);		// look for more info about this
	beamEnt->SetProperty<int>("m_nSpeed", speed);				// look for more info about this
	beamEnt->SetProperty<int>("r", color.r());
	beamEnt->SetProperty<int>("g", color.g());
	beamEnt->SetProperty<int>("b", color.b());
	beamEnt->SetProperty<int>("a", color.a());
	beamEnt->SetProperty<int>("m_nFlags", flags);				// look for more info about this
	beamEnt->SetProperty<int>("m_iEntIndex", gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) entity)));

	// Fire after carefull NASA preparation
	beamEnt->Fire(filter, delay);
}

void TE_BeamRingPoint(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, Vector center, float startRadius, float endRadius)
{
	static TempEntity *beamRingPoint = TempEntity::FindTempEntity("BeamRingPoint");

	if( !beamRingPoint )
	{
		return;
	}

	beamRingPoint->SetProperty<int>("m_nModelIndex", beamModelIndex);
	beamRingPoint->SetProperty<int>("m_nHaloIndex", haloModelIndex);
	beamRingPoint->SetProperty<int>("m_nStartFrame", startFrame);	// look for more info about this
	beamRingPoint->SetProperty<int>("m_nFrameRate", frameRate);	// look for more info about this
	beamRingPoint->SetProperty<float>("m_fLife", life);
	beamRingPoint->SetProperty<float>("m_fWidth", width);
	beamRingPoint->SetProperty<float>("m_fEndWidth", endWidth);		// look for more info about this
	beamRingPoint->SetProperty<int>("m_nFadeLength", fadeLength);
	beamRingPoint->SetProperty<float>("m_fAmplitude", amplitude);		// look for more info about this
	beamRingPoint->SetProperty<int>("m_nSpeed", speed);				// look for more info about this
	beamRingPoint->SetProperty<int>("r", color.r());
	beamRingPoint->SetProperty<int>("g", color.g());
	beamRingPoint->SetProperty<int>("b", color.b());
	beamRingPoint->SetProperty<int>("a", color.a());
	beamRingPoint->SetProperty<int>("m_nFlags", flags);				// look for more info about this
	beamRingPoint->SetProperty<Vector>("m_vecCenter", center);
	beamRingPoint->SetProperty<float>("m_flStartRadius", startRadius);
	beamRingPoint->SetProperty<float>("m_flEndRadius", endRadius);

	// Fire after carefull NASA preparation
	beamRingPoint->Fire(filter, delay);
}

void TE_BeamPoints(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, Vector startPos, Vector endPos)
{
	static TempEntity *beamPoints = TempEntity::FindTempEntity("BeamPoints");

	if( !beamPoints )
	{
		return;
	}

	beamPoints->SetProperty<int>("m_nModelIndex", beamModelIndex);
	beamPoints->SetProperty<int>("m_nHaloIndex", haloModelIndex);
	beamPoints->SetProperty<int>("m_nStartFrame", startFrame);	// look for more info about this
	beamPoints->SetProperty<int>("m_nFrameRate", frameRate);	// look for more info about this
	beamPoints->SetProperty<float>("m_fLife", life);
	beamPoints->SetProperty<float>("m_fWidth", width);
	beamPoints->SetProperty<float>("m_fEndWidth", endWidth);		// look for more info about this
	beamPoints->SetProperty<int>("m_nFadeLength", fadeLength);
	beamPoints->SetProperty<float>("m_fAmplitude", amplitude);		// look for more info about this
	beamPoints->SetProperty<int>("m_nSpeed", speed);				// look for more info about this
	beamPoints->SetProperty<int>("r", color.r());
	beamPoints->SetProperty<int>("g", color.g());
	beamPoints->SetProperty<int>("b", color.b());
	beamPoints->SetProperty<int>("a", color.a());
	beamPoints->SetProperty<int>("m_nFlags", flags);				// look for more info about this
	beamPoints->SetProperty<Vector>("m_vecStartPoint", startPos);
	beamPoints->SetProperty<Vector>("m_vecEndPoint", endPos);

	// Fire after carefull NASA preparation
	beamPoints->Fire(filter, delay);
}

void TE_Sparks(IRecipientFilter &filter, float delay, Vector origin, int magnitude, int trailLength, Vector direction)
{
	static TempEntity *sparksEnt = TempEntity::FindTempEntity("Sparks");

	if( !sparksEnt )
	{
		return;
	}

	sparksEnt->SetProperty<Vector>("m_vecOrigin[0]", origin);
	sparksEnt->SetProperty<int>("m_nMagnitude", magnitude);
	sparksEnt->SetProperty<int>("m_nTrailLength", trailLength);
	sparksEnt->SetProperty<Vector>("m_vecDir", direction);

	// Fire after carefull NASA preparation
	sparksEnt->Fire(filter, delay);
}

void TE_BreakModel(IRecipientFilter &filter, float delay, Vector origin, QAngle rotation, Vector size, Vector velocity, int modelIndex, int randomization, int count, float time, int flags)
{
	static TempEntity *breakModelEnt = TempEntity::FindTempEntity("breakmodel");

	if( !breakModelEnt )
	{
		return;
	}

	breakModelEnt->SetProperty<Vector>("m_vecOrigin", origin);
	breakModelEnt->SetProperty<QAngle>("m_angRotation[0]", rotation);
	breakModelEnt->SetProperty<Vector>("m_vecSize", size);
	breakModelEnt->SetProperty<Vector>("m_vecVelocity", velocity);
	breakModelEnt->SetProperty<int>("m_nModelIndex", modelIndex);
	breakModelEnt->SetProperty<int>("m_nRandomization", randomization);
	breakModelEnt->SetProperty<int>("m_nCount", count);
	breakModelEnt->SetProperty<float>("m_fTime", time);
	breakModelEnt->SetProperty<int>("m_nFlags", flags);

	// Fire after carefull NASA preparation
	breakModelEnt->Fire(filter, delay);
}

void TE_GlowSprite(IRecipientFilter &filter, float delay, Vector origin, int glowModelIndex, float scale, float life, int brightness)
{
	static TempEntity *glowSpriteEnt = TempEntity::FindTempEntity("GlowSprite");

	if( !glowSpriteEnt )
	{
		return;
	}

	glowSpriteEnt->SetProperty<Vector>("m_vecOrigin", origin);
	glowSpriteEnt->SetProperty<int>("m_nModelIndex", glowModelIndex);
	glowSpriteEnt->SetProperty<float>("m_fScale", scale);
	glowSpriteEnt->SetProperty<float>("m_fLife", life);
	glowSpriteEnt->SetProperty<int>("m_nBrightness", brightness);

	// Fire after carefull NASA preparation
	glowSpriteEnt->Fire(filter, delay);
}

void TE_BloodSprite(IRecipientFilter &filter, float delay, Vector origin, Vector direction, Color color, int sprayModelIndex, int dropModelIndex, int size)
{
	static TempEntity *bloodSpriteEnt = TempEntity::FindTempEntity("Blood Sprite");

	if( !bloodSpriteEnt )
	{
		return;
	}

	bloodSpriteEnt->SetProperty<Vector>("m_vecOrigin", origin);
	bloodSpriteEnt->SetProperty<Vector>("m_vecDirection", direction);
	bloodSpriteEnt->SetProperty<int>("r", color.r());
	bloodSpriteEnt->SetProperty<int>("g", color.g());
	bloodSpriteEnt->SetProperty<int>("b", color.b());
	bloodSpriteEnt->SetProperty<int>("a", color.a());
	bloodSpriteEnt->SetProperty<int>("m_nSprayModel", sprayModelIndex);
	bloodSpriteEnt->SetProperty<int>("m_nDropModel", dropModelIndex);
	bloodSpriteEnt->SetProperty<int>("m_nSize", size);

	// Fire after carefull NASA preparation
	bloodSpriteEnt->Fire(filter, delay);
}

void TE_EnergySplash(IRecipientFilter &filter, float delay, Vector origin, Vector direction, bool explosive)
{
	static TempEntity *energySplashEnt = TempEntity::FindTempEntity("Energy Splash");

	if( !energySplashEnt )
	{
		return;
	}

	energySplashEnt->SetProperty<Vector>("m_vecPos", origin);
	energySplashEnt->SetProperty<Vector>("m_vecDir", direction);
	energySplashEnt->SetProperty<bool>("m_bExplosive", explosive);

	// Fire after carefull NASA preparation
	energySplashEnt->Fire(filter, delay);
}

void TE_Explosion(IRecipientFilter &filter, float delay, Vector origin, int modelIndex, float scale, int frameRate, int flags, Vector normal, int materialType, int radius, int magnitude)
{
	static TempEntity *explosionEnt = TempEntity::FindTempEntity("Explosion");

	if( !explosionEnt )
	{
		return;
	}	

	explosionEnt->SetProperty<Vector>("m_vecOrigin[0]", origin);
	explosionEnt->SetProperty<int>("m_nModelIndex", modelIndex);
	explosionEnt->SetProperty<float>("m_fScale", scale);
	explosionEnt->SetProperty<int>("m_nFrameRate", frameRate);
	explosionEnt->SetProperty<int>("m_nFlags", flags);
	explosionEnt->SetProperty<Vector>("m_vecNormal", normal);
	explosionEnt->SetProperty<int>("m_chMaterialType", materialType);
	explosionEnt->SetProperty<int>("m_nRadius", radius);
	explosionEnt->SetProperty<int>("m_nMagnitude", magnitude);

	// Fire after carefull NASA preparation
	explosionEnt->Fire(filter, delay);
}