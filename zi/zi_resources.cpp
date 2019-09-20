#include "zi_resources.h"
#include "zi_zombies.h"
#include "zi_nades.h"

#define ZOMBIEINFESTATION_SOUNDS_CONFIG "addons/ZombieInfestation/resources/sounds.ini"
#define ZOMBIEINFESTATION_MODELS_CONFIG "addons/ZombieInfestation/resources/models.ini"
#define ZOMBIEINFESTATION_CONFIG "addons/ZombieInfestation/config.cfg"

int ZIResources::m_BloodSprayModelIndex = -1;
int ZIResources::m_BloodDropModelIndex = -1;
int ZIResources::m_LaserModelIndex = -1; 																										  
int ZIResources::m_BeamModelIndex = -1;
int ZIResources::m_HaloModelIndex = -1;
int ZIResources::m_GlowModelIndex = -1;
int ZIResources::m_FireModelIndex = -1;
int ZIResources::m_ExplosionModelIndex = -1;
int ZIResources::m_GlassModelIndex[] = { -1 };

ZIResources g_Resources;

static KeyValues *g_pSoundsKeyValues = nullptr, *g_pModelsKeyValues = nullptr;

static SourceHook::CVector<char *> g_pLoadingSounds;
static SourceHook::CVector<char *> g_pRoundEndSounds[RoundEndSound_Max];

static SourceHook::CVector<ZIModel *> g_pModels[ModelType_Max];

ZIModel::ZIModel()
{
	ke::SafeStrcpy(m_Name, sizeof(m_Name), "");
	ke::SafeStrcpy(m_PlayerModel, MODELS_PATH_MAXLENGTH, "");
	ke::SafeStrcpy(m_ArmsModel, MODELS_PATH_MAXLENGTH, "");

	m_PlayerModelIndex = -1;
	m_ArmsModelIndex = -1;
}

ZIModel::ZIModel(const char *name, const char *playermodel, const char *armsmodel, int playermodelIndex, int armsModelIndex)
{	
	ke::SafeStrcpy(m_Name, sizeof(m_Name), name);
	ke::SafeStrcpy(m_PlayerModel, MODELS_PATH_MAXLENGTH, playermodel);
	ke::SafeStrcpy(m_ArmsModel, MODELS_PATH_MAXLENGTH, armsmodel);

	m_PlayerModelIndex = playermodelIndex;
	m_ArmsModelIndex = armsModelIndex;
}

const char *ZIModel::GetPlayerModel(int *playermodelIndex)
{
	if( playermodelIndex )
	{
		*playermodelIndex = m_PlayerModelIndex;
	}

	return m_PlayerModel;
}

const char *ZIModel::GetArmsModel(int *armsModelIndex)
{
	if( armsModelIndex )
	{
		*armsModelIndex = m_ArmsModelIndex;
	}

	return m_ArmsModel;
}

void ZIResources::Load()
{
//////////////////////////
//// SOUNDS
//////////////////////////
	// THE NEST IS REAL!
	KeyValues *subKv = nullptr, *subSubKv = nullptr, *subSubSubKv = nullptr;

	g_pSoundsKeyValues = new KeyValues("Sounds");

	if( g_pSoundsKeyValues->LoadFromFile(g_pExtension->m_pFileSystem, ZOMBIEINFESTATION_SOUNDS_CONFIG) )
	{
		char *section = nullptr, *name = nullptr, *type = nullptr, *soundfile = nullptr;
		float duration = 0.0f;

		for( subKv = g_pSoundsKeyValues->GetFirstTrueSubKey(); subKv; subKv = subKv->GetNextTrueSubKey() )
		{
			section = (char *) subKv->GetName();

			if( strcmp(section, "Loading") == 0 )
			{
				for( subSubKv = subKv->GetFirstSubKey(); subSubKv; subSubKv = subSubKv->GetNextKey() )
				{
					soundfile = (char *) subSubKv->GetName();
					
					// Ignore non MP3 files
					if( !strstr(soundfile, "mp3") )
					{
						continue;
					}

//					if( !g_pExtension->m_pEngineSound->IsSoundPrecached(&soundfile[STRLEN_SOUND]) )
//					{
						AddFileToDownloadsTable(soundfile);

						soundfile = &soundfile[STRLEN_SOUND];

						g_pExtension->m_pEngineSound->PrecacheSound(soundfile, true);
//					}	

					// TODO: Add check if sounds with same name already exists
					g_pLoadingSounds.push_back(soundfile);
				}
			}
			else if( strcmp(section, "Round End") == 0 )
			{
				for( subSubKv = subKv->GetFirstTrueSubKey(); subSubKv; subSubKv = subSubKv->GetNextTrueSubKey() )
				{
					name = (char *) subSubKv->GetName();
					type = (char *) subSubKv->GetString("Type");
					soundfile = (char *) subSubKv->GetString("Sound");

					// Ignore non MP3 files
					if( !strstr(soundfile, "mp3") )
					{
						continue;
					}

//					if( !g_pExtension->m_pEngineSound->IsSoundPrecached(&soundfile[STRLEN_SOUND]) )
//					{
						AddFileToDownloadsTable(soundfile);

						soundfile = &soundfile[STRLEN_SOUND];

						g_pExtension->m_pEngineSound->PrecacheSound(soundfile, true);
//					}	

					// TODO: Add check if sounds with same name already exists					
					if( strcmp(type, "Human") == 0 )
					{
						g_pRoundEndSounds[RoundEndSound_Humans].push_back(soundfile);
					}
					else if( strcmp(type, "Zombie") == 0 )
					{
						g_pRoundEndSounds[RoundEndSound_Zombies].push_back(soundfile);
					}
				}
			}						
		}		
	}
	else
	{
		CONSOLE_DEBUGGER("%s failed to load", ZOMBIEINFESTATION_SOUNDS_CONFIG);
	}	

//// EXTRAS: TODO (DO ALL SOUNDS LIKE THIS, FUCK CONFIG FILES, THEY'RE TOO COMPLEXE)
	int i = 0;

	// Misc
	PRECACHE_SOUND("ZombieInfestation/warning_biohazard.mp3");	
	PRECACHE_SOUND("ZombieInfestation/warning_death.mp3");	
	PRECACHE_SOUND("ZombieInfestation/round_start_countdown.mp3");
	
	// Players
	g_pExtension->m_pEngineSound->PrecacheSound("items/flashlight1.wav", true);
	PRECACHE_SOUND("ZombieInfestation/human_armor_damage.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_infection1.mp3");	
	PRECACHE_SOUND("ZombieInfestation/human_infection2.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_infection3.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_infection4.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_infection5.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_disinfection1.mp3");
	PRECACHE_SOUND("ZombieInfestation/human_disinfection2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_growl1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_growl2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_growl3.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_growl4.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_growl5.mp3");	
	PRECACHE_SOUND("ZombieInfestation/zombie_infection1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_infection2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_infection3.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_infection4.mp3");	
	PRECACHE_SOUND("ZombieInfestation/zombie_pain1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_pain2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_pain3.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_pain4.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_pain5.mp3");	
	PRECACHE_SOUND("ZombieInfestation/zombie_fall1.mp3");	
	PRECACHE_SOUND("ZombieInfestation/zombie_burn1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_burn2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_frozen1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_unfrozen1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_death1.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_death2.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_death3.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_death4.mp3");
	PRECACHE_SOUND("ZombieInfestation/zombie_death5.mp3");	
	PRECACHE_SOUND("ZombieInfestation/nemesis_pain1.mp3");
	PRECACHE_SOUND("ZombieInfestation/nemesis_pain2.mp3");
	PRECACHE_SOUND("ZombieInfestation/nemesis_pain3.mp3");
	PRECACHE_SOUND("ZombieInfestation/player_gibbed.mp3");

	// Ambient
	PRECACHE_SOUND("ZombieInfestation/round_ambient_infection1.mp3");
	PRECACHE_SOUND("ZombieInfestation/round_ambient_infection2.mp3");

	// Modes
	PRECACHE_SOUND("ZombieInfestation/round_mode_multiple_infection1.mp3");	
	PRECACHE_SOUND("ZombieInfestation/round_mode_survivor1.mp3");
	PRECACHE_SOUND("ZombieInfestation/round_mode_survivor2.mp3");
	PRECACHE_SOUND("ZombieInfestation/round_mode_nemesis1.mp3");
	PRECACHE_SOUND("ZombieInfestation/round_mode_nemesis2.mp3");
	
	// Nades
	PRECACHE_SOUND("ZombieInfestation/grenade_frost_explode.mp3");
	PRECACHE_SOUND("ZombieInfestation/grenade_flare_on.mp3");
	PRECACHE_SOUND("ZombieInfestation/grenade_infection_explode.mp3");
	PRECACHE_SOUND("ZombieInfestation/grenade_infection_scream.mp3");

	// Explosions
	PRECACHE_SOUND("ZombieInfestation/explosion_energy.mp3");
	PRECACHE_SOUND("ZombieInfestation/explosion1.mp3");
	PRECACHE_SOUND("ZombieInfestation/explosion2.mp3");
	PRECACHE_SOUND("ZombieInfestation/explosion3.mp3");
	PRECACHE_SOUND("ZombieInfestation/flatline.mp3");

	// Bullet-Time
	PRECACHE_SOUND("ZombieInfestation/bullettime_start.mp3");
	PRECACHE_SOUND("ZombieInfestation/bullettime_end.mp3");

//////////////////////////
//// MODELS
//////////////////////////

	g_pModelsKeyValues = new KeyValues("Models");

	if( g_pModelsKeyValues->LoadFromFile(g_pExtension->m_pFileSystem, ZOMBIEINFESTATION_MODELS_CONFIG) )
	{
		char *name = nullptr, *type = nullptr, *playermodel = nullptr, *armsmodel = nullptr; 
		int playermodelIndex = -1, armsmodelIndex = -1;

		bool precacheMaterials = true;

		for( subKv = g_pModelsKeyValues->GetFirstTrueSubKey(); subKv; subKv = subKv->GetNextTrueSubKey() )
		{
			name = (char *) subKv->GetName();
			type = (char *) subKv->GetString("Type");
			playermodel = (char *) subKv->GetString("PlayerModel");
			armsmodel = (char *) subKv->GetString("ArmsModel");
			
			// Ignore non MDL files
			if( !strstr(playermodel, "mdl") )
			{
				continue;
			}

			if( !g_pExtension->m_pEngineServer->IsModelPrecached(playermodel) )
			{
				playermodelIndex = g_pExtension->m_pEngineServer->PrecacheModel(playermodel, true);
				AddFileToDownloadsTable(playermodel);

				precacheMaterials = false;
			}

			if( !g_pExtension->m_pEngineServer->IsModelPrecached(armsmodel) )
			{
				armsmodelIndex = g_pExtension->m_pEngineServer->PrecacheModel(armsmodel, true);
				AddFileToDownloadsTable(armsmodel);

				precacheMaterials = false;
			}

			ZIModel *model = new ZIModel(name, playermodel, armsmodel, playermodelIndex, armsmodelIndex);	// TODO: Add check if model with same name already exists

			if( !model )
			{
				continue;
			}

			if( strcmp(type, "Human") == 0 )
			{
				g_pModels[ModelType_Human].push_back(model);
			}
			else if( strcmp(type, "Zombie") == 0 )
			{
				g_pModels[ModelType_Zombie].push_back(model);
			}
			else if( strcmp(type, "Survivor") == 0 )
			{
				g_pModels[ModelType_Survivor].push_back(model);
			}
			else if( strcmp(type, "Sniper") == 0 )
			{
				g_pModels[ModelType_Sniper].push_back(model);
			}
			else if( strcmp(type, "Nemesis") == 0 )
			{
				g_pModels[ModelType_Nemesis].push_back(model);
			}
			else if( strcmp(type, "Assassin") == 0 )
			{
				g_pModels[ModelType_Assassin].push_back(model);
			}
			
			if( precacheMaterials )
			{
				subSubKv = subKv->FindKey("Materials");

				for( subSubKv = subSubKv->GetFirstSubKey(); subSubKv; subSubKv = subSubKv->GetNextKey() )
				{
					AddFileToDownloadsTable(subSubKv->GetName());
				}
			}
		}			
	}
	else
	{
		CONSOLE_DEBUGGER("%s failed to load", ZOMBIEINFESTATION_MODELS_CONFIG);
	}	

	// GIBS
	PRECACHE_MODEL("models/gibs/pgib_p1.mdl");
	PRECACHE_MODEL("models/gibs/pgib_p2.mdl");
	PRECACHE_MODEL("models/gibs/pgib_p3.mdl");
	PRECACHE_MODEL("models/gibs/pgib_p4.mdl");
	PRECACHE_MODEL("models/gibs/pgib_p5.mdl");
	PRECACHE_MODEL("models/gibs/hgibs_jaw.mdl");
	PRECACHE_MODEL("models/gibs/hgibs_scapula.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p1.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p2.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p3.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p4.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p5.mdl");
	PRECACHE_MODEL("models/gibs/rgib_p6.mdl");
	PRECACHE_MODEL("models/gibs/gibhead.mdl");	



//////////////////////////
//// MATERIALS
//////////////////////////

	AddFileToDownloadsTable("materials/ZombieInfestation/effects/zombie_vision.vtf");
	AddFileToDownloadsTable("materials/ZombieInfestation/effects/zombie_vision.vmt");

	m_BloodSprayModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/bloodspray.vmt", true);
	m_BloodDropModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/effects/blood_drop.vmt", true);

	m_LaserModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/laserbeam.vmt", true); // dronegun_laser.vmt or purplelaser1.vmt are good too
//	m_BeamModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/laserbeam.vmt", true);
	m_BeamModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/physbeam.vmt", true);
	m_HaloModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/glow01.vmt", true);
	m_GlowModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/ledglow.vmt", true);

	m_FireModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/xfireball3.vmt", true);
	m_ExplosionModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/zerogxplode.vmt", true);

	m_GlassModelIndex[0] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard01.mdl", true);
	m_GlassModelIndex[1] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard02.mdl", true);
	m_GlassModelIndex[2] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard03.mdl", true);
	m_GlassModelIndex[3] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard04.mdl", true);
	m_GlassModelIndex[4] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard05.mdl", true);
	m_GlassModelIndex[5] = g_pExtension->m_pEngineServer->PrecacheModel("models/gibs/glass_shard06.mdl", true);	

//////////////////////////
//// CONFIG
//////////////////////////
	char config[256];
	g_pSM->BuildPath(Path_Game, config, sizeof(config), ZOMBIEINFESTATION_CONFIG);
	
	if( !libsys->PathExists(config) || !libsys->IsPathFile(config) )
	{
		CONSOLE_DEBUGGER("The config file was not found!");
		return;
	}

	FILE *file = fopen(config, "rt");

	if( file )
	{
		char line[512];
		int lineLength = 0;

		while( !feof(file) && fgets(line, sizeof(line), file) )
		{
			if( line[0] == '\0' || !isalpha(line[0]) )
			{
				continue;
			}

			lineLength = strlen(line);

			line[lineLength] = '\n';
			line[lineLength + 1] = '\0';

			gamehelpers->ServerCommand(line);
		}

		fclose(file);
	}
}

void ZIResources::Free()
{

//// SOUNDS
//	RELEASE_POINTERS_ARRAY_ARRAY(g_pLoadingSounds);
	
	if( g_pSoundsKeyValues )
	{
		g_pSoundsKeyValues->deleteThis(); g_pSoundsKeyValues = nullptr;
	}
/*
	for( file = 0; file < RoundEndSound_Max; file++ )
	{
		RELEASE_POINTERS_ARRAY_ARRAY(char, g_pRoundEndSounds[file]);
	}

	for( file = 0; file < HumanSound_Max; file++ )
	{
		RELEASE_POINTERS_ARRAY_ARRAY(char, g_pHumanSounds[file]);
	}

	for( file = 0; file < ZombieSound_Max; file++ )
	{
		RELEASE_POINTERS_ARRAY_ARRAY(char, g_pZombieSounds[file]);
	}
*/
///// MODELS

	for( int modelType = 0; modelType < ModelType_Max; modelType++ )
	{
		RELEASE_POINTERS_ARRAY(g_pModels[modelType]);
	}	

	if( g_pModelsKeyValues )
	{
		g_pModelsKeyValues->deleteThis(); g_pModelsKeyValues = nullptr;
	}
}

char *ZIResources::RandomLoadingSound()
{
	int soundsCount = g_pLoadingSounds.size();

	if( soundsCount < 1 )
	{
		CONSOLE_DEBUGGER("Sound files are not precached yet!");
		return nullptr;
	}

	return g_pLoadingSounds[soundsCount > 1 ? RandomInt(0, soundsCount - 1) : 0];
}

char *ZIResources::RandomRoundEndSound(RoundEndSound sound)
{
	if( sound <= RoundEndSound_None || sound >= RoundEndSound_Max )
	{
		CONSOLE_DEBUGGER("Invalid access, carefull!");
		return nullptr;
	}

	int soundsCount = g_pRoundEndSounds[sound].size();

	if( soundsCount < 1 )
	{
		CONSOLE_DEBUGGER("Sound files are not precached yet!");
		return nullptr;
	}

	return g_pRoundEndSounds[sound][soundsCount > 1 ? RandomInt(0, soundsCount - 1) : 0];
}

const char *ZIResources::GetRandomExplosionSound()
{
	switch( RandomInt(1, 3) )
	{
	case 1:
		return "ZombieInfestation/explosion1.mp3";

	case 2:
		return "ZombieInfestation/explosion2.mp3";

	case 3:
		return "ZombieInfestation/explosion3.mp3";
	}

	return nullptr;
}

ZIModel *ZIResources::GetModel(const char *name, ModelType type)
{
	SourceHook::CVector<ZIModel *> *models = &g_pModels[type];

	int modelsCount = models->size();

	if( modelsCount < 1 )
	{
		CONSOLE_DEBUGGER("Zombie model files are not precached yet!");
		return nullptr;
	}

	ZIModel *model = nullptr;
	
	for( auto iterator = models->begin(); iterator != models->end(); iterator++ )
	{
		model = *iterator;

		if( !model )
		{
			continue;
		}

		if( strcmp(model->m_Name, name) == 0 )
		{
			return model;
		}
	}

	return nullptr;
}

ZIModel *ZIResources::RandomModel(ModelType type)
{
	if( type < ModelType_Human || type >= ModelType_Max )
	{
		CONSOLE_DEBUGGER("Invalid access, carefull!");
		return nullptr;
	}

	int modelsCount = g_pModels[type].size();

	if( modelsCount < 1 )
	{
		CONSOLE_DEBUGGER("Model files are not precached yet!");
		return nullptr;
	}

	return g_pModels[type][modelsCount > 1 ? RandomInt(0, modelsCount - 1) : 0];
}