#ifndef _INCLUDE_ZI_RESOURCES_PROPER_H_
#define _INCLUDE_ZI_RESOURCES_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_humans.h"

#define SOUNDS_PATH_MAXLENGTH 128
#define MODELS_PATH_MAXLENGTH 128
#define MATERIALS_PATH_MAXLENGTH 128

#define STRLEN_SOUND 6	// Length of "sound/"

enum RoundEndSound
{
	RoundEndSound_None = -1,
	RoundEndSound_Humans,
	RoundEndSound_Zombies,
	RoundEndSound_Max
};

enum ModelType
{
	ModelType_Human = 0,
	ModelType_Zombie,
	ModelType_Survivor,
	ModelType_Sniper,
	ModelType_Nemesis,
	ModelType_Assassin,
	ModelType_Max
};

class ZIModel
{
public:
	char m_Name[32];	
	char m_PlayerModel[MODELS_PATH_MAXLENGTH];
	char m_ArmsModel[MODELS_PATH_MAXLENGTH];
	int m_PlayerModelIndex;
	int m_ArmsModelIndex;

	ZIModel();
	ZIModel(const char *name, const char *playermodel, const char *armsmodel, int playermodelIndex, int armsModelIndex);

	const char *GetPlayerModel(int *playermodelIndex = 0);
	const char *GetArmsModel(int *armsModelIndex = 0);
};

namespace ZIResources
{
	extern int m_BloodSprayModelIndex;
	extern int m_BloodDropModelIndex;

	extern int m_LaserModelIndex;
	extern int m_BeamModelIndex;
	extern int m_HaloModelIndex;
	extern int m_GlowModelIndex;

	extern int m_FireModelIndex;
	extern int m_ExplosionModelIndex;

	extern int m_GlassModelIndex[6];

	void Load();
	void Free();

	char *RandomLoadingSound();
	char *RandomRoundEndSound(RoundEndSound sound);
	const char *GetRandomExplosionSound();

	ZIModel *GetModel(const char *name, ModelType type);
	ZIModel *RandomModel(ModelType type);
};

#endif 