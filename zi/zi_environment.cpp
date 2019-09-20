#include "zi_environment.h"
#include "zi_round_modes.h"
#include "zi_resources.h"
#include "zi_timers.h"

//#define SKY_NAME "sky_descent"
#define SKY_NAME "blood1_"

string_t LIGHT_STYLE = MAKE_STRING("b");

ZIEnvironment g_Environment;

void ZIEnvironment::Setup()
{
	ChangeSkybox();
	ChangeLight();
	ChangeWeather();
}

void ZIEnvironment::ChangeSkybox()
{
	BaseEntity *worldEnt = BaseEntity::FindEntity(nullptr, "worldspawn");

	if( worldEnt )
	{
		char buffer[128];

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sbk.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sbk.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sdn.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sdn.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sft.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sft.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%slf.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%slf.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%srt.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%srt.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sup.vmt", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		g_pSM->Format(buffer, sizeof(buffer), "materials/skybox/%sup.vtf", SKY_NAME);
		AddFileToDownloadsTable(buffer);

		worldEnt->SetKeyValue("skyname", SKY_NAME);		
	}
}

void ZIEnvironment::ChangeLight()
{
	BaseEntity *lightEnt = nullptr;

	while( (lightEnt = BaseEntity::FindEntity(lightEnt, "env_cascade_light")) )
	{
		CONSOLE_DEBUGGER("Killed light entity: %d", lightEnt);
		lightEnt->AcceptInput("Kill");
	}

	lightEnt = nullptr;

	while( (lightEnt = BaseEntity::FindEntity(lightEnt, "light_environment")) )
	{
		CONSOLE_DEBUGGER("Found enviromental light entity: %d", lightEnt);
		
		lightEnt->SetKeyValue("Brightness", "192 211 222 5");
		lightEnt->SetKeyValue("BrightnessHDR ", "-1 -1 -1 1");
		lightEnt->SetKeyValue("Ambient", "218 208 194 2");
		lightEnt->SetKeyValue("AmbientHDR", "-1 -1 -1 1");
		lightEnt->SetKeyValue("SunSpreadAngle", "180");
		lightEnt->SetKeyValue("spawnflags", "1");
	}

	BaseEntity *shadowEnt = BaseEntity::FindEntity(nullptr, "shadow_control");

	if( shadowEnt )
	{
		CONSOLE_DEBUGGER("Found shadow entity: %d", shadowEnt);
		shadowEnt->SetKeyValue("Shadow Color", "50 50 50");
	}
	
	// TODO: Change light style (Try to fix this shit; its serious)
	if( LIGHT_STYLE != NULL_STRING )
	{
		g_pExtension->m_pEngineServer->LightStyle(0, STRING(LIGHT_STYLE));
	}	
}

void ZIEnvironment::ChangeWeather()
{
	BaseEntity *sunEnt = BaseEntity::FindEntity(nullptr, "env_sun");

	if( sunEnt )
	{
		CONSOLE_DEBUGGER("TurnedOff the sun entity: %d", sunEnt);
		sunEnt->AcceptInput("TurnOff");
	}

	BaseEntity *fogEnt = BaseEntity::FindEntity(nullptr, "env_fog_controller");

	if( fogEnt )
	{
		CONSOLE_DEBUGGER("Fog entity found: %d", fogEnt);
	}
	else if( (fogEnt = BaseEntity::CreateEntity("env_fog_controller")) )
	{
		CONSOLE_DEBUGGER("Fog entity created: %d", fogEnt);
		fogEnt->Spawn();
	}

	// TODO: Fog (Play with settings)
	if( fogEnt )
	{
		fogEnt->AcceptInput("TurnOff");

		fogEnt->SetKeyValue("fogenable", "1");
		fogEnt->SetKeyValue("fogblend", "0");
		fogEnt->SetKeyValue("fogcolor", "0 0 0");
		fogEnt->SetKeyValue("fogcolor2", "0 0 0");
		fogEnt->SetKeyValue("fogstart", 0.0f);
		fogEnt->SetKeyValue("fogend", 200.0f);
		fogEnt->SetKeyValue("fogmaxdensity", 0.95f);
//		fogEnt->SetKeyValue("foglerptime", 0.0f);
//		fogEnt->SetKeyValue("farz", "-1");

		fogEnt->AcceptInput("TurnOn");
	}
}

void ZIEnvironment::RemoveUndesirableEnts(BaseEntity *entity, const char *classname)
{
	if( strstr(classname, "defuse") || strstr(classname, "vip") )
	{
		CONSOLE_DEBUGGER("Suspecious entity: %s", classname);		
	}

	else if( strcmp(classname, "weapon_c4") == 0 || strcmp(classname, "planted_c4") == 0 || strcmp(classname, "planted_c4_training") == 0 || strcmp(classname, "planted_c4_survival") == 0 )
	{
		entity->AcceptInput("Kill");
	}
	else if( strcmp(classname, "hostage_entity") == 0 || strcmp(classname, "hostage_carriable_prop") == 0 || strcmp(classname, "info_hostage_spawn") == 0 || strcmp(classname, "info_hostage_rescue") == 0
		|| strcmp(classname, "func_hostage_rescue") == 0 )
	{
		entity->AcceptInput("Kill");
	}
	else if( strcmp(classname, "func_bomb_target") == 0 || strcmp(classname, "info_bomb_target") == 0 )
	{
		entity->AcceptInput("Kill");
	}
	else if( strcmp(classname, "func_buyzone") == 0 || strstr(classname, "buy") )
	{
		entity->AcceptInput("Kill");		
	}

/*
	if( strcmp(classname, "func_dustcloud") == 0 )
	{
		CONSOLE_DEBUGGER("Dustcloud found!");

		AcceptEntityInput(entity, "TurnOff");

		SetEntityKeyValue(entity, "Color", "115 28 28");
		SetEntityKeyValue(entity, "SpawnRate", "600");
		SetEntityKeyValue(entity, "Alpha", "64");

		AcceptEntityInput(entity, "TurnOn");
	}

	if( strstr(classname, "light") )
	{
		CONSOLE_DEBUGGER("Turning light off.... %d", entity);

		AcceptEntityInput(entity, "TurnOff");
		AcceptEntityInput(entity, "LightOff");
	}
*/
}