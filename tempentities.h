#ifndef _INCLUDE_TEMPENTITIES_PROPER_H_
#define _INCLUDE_TEMPENTITIES_PROPER_H_
#pragma once

#include "macros.h"

class TempEntity
{
private:
	const char *m_pName;
	void *m_pAddress;
	ServerClass *m_pServerClass;

	TempEntity(const char *name, void *address);	
	
public:
	~TempEntity();
	const char *GetName();	
	
	// Get/Set data
	template<class T> T GetProperty(const char *propname)
	{
		sm_sendprop_info_t sendprop;

		if( !gamehelpers->FindSendPropInfo(m_pServerClass->GetName(), propname, &sendprop) )
		{
			CONSOLE_DEBUGGER("Couldn't find entity property: %s", propname);
			return (T) 0;
		}

		return *(T *) ((unsigned char *) m_pAddress + sendprop.actual_offset);
	}

	template<class T> void SetProperty(const char *propname, T value)
	{
		sm_sendprop_info_t sendprop;

		if( !gamehelpers->FindSendPropInfo(m_pServerClass->GetName(), propname, &sendprop) )
		{
			CONSOLE_DEBUGGER("Couldn't find entity property: %s", propname);
			return;
		}

		*(T *) ((unsigned char *) m_pAddress + sendprop.actual_offset) = value;
	}
	
	void Fire(IRecipientFilter &filter, float delay);
	
	static bool GetFirstTempEntity();
	static TempEntity *FindTempEntity(const char *name);	
};

typedef ResultType (*TempEntPreHook_t) (TempEntity *, IRecipientFilter &, float &);
typedef void (*TempEntPostHook_t) (TempEntity *, IRecipientFilter &, float &);

class TempEntityHooker
{
private:
	TempEntity *m_pEntity;
	void *m_pCallback;
	bool m_IsPost;

public:
	TempEntityHooker(TempEntity *entity, void *callback, bool post);
	~TempEntityHooker();	

	TempEntity *GetEntity();
	void *GetCallback();
	bool IsPostHook();

	static void Load();
	static void Free();

	static bool AddHook(TempEntity *tempEnt, void *callback, bool post = false);
	static bool AddHook(const char *name, void *callback, bool post = false);

	static void RemoveHook(TempEntity *tempEnt, bool post = false);
	static void RemoveHook(const char *name, bool post = false);
};

// Useful ready setups
extern void TE_Sprite(IRecipientFilter &filter, float delay, Vector origin, int modelIndex, float scale, int brightness);
extern void TE_WorldDecal(IRecipientFilter &filter, float delay, Vector origin, int decalIndex);
extern void TE_BeamFollow(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, BaseEntity *entity);
extern void TE_BeamRingPoint(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, Vector center, float startRadius, float endRadius);
extern void TE_BeamPoints(IRecipientFilter &filter, float delay, int beamModelIndex, int haloModelIndex, int startFrame, int frameRate, float life, float width, float endWidth, int fadeLength,
	float amplitude, int speed, Color color, int flags, Vector startPos, Vector endPos);
extern void TE_Sparks(IRecipientFilter &filter, float delay, Vector origin, int magnitude, int trailLength, Vector direction);
extern void TE_BreakModel(IRecipientFilter &filter, float delay, Vector origin, QAngle rotation, Vector size, Vector velocity, int modelIndex, int randomization, int count, float time, int flags);
extern void TE_GlowSprite(IRecipientFilter &filter, float delay, Vector origin, int glowModelIndex, float scale, float life, int brightness);
extern void TE_BloodSprite(IRecipientFilter &filter, float delay, Vector origin, Vector direction, Color color, int sprayModelIndex, int dropModelIndex, int size);
extern void TE_EnergySplash(IRecipientFilter &filter, float delay, Vector origin, Vector direction, bool explosive);
extern void TE_Explosion(IRecipientFilter &filter, float delay, Vector origin, int modelIndex, float scale, int frameRate, int flags, Vector normal, int materialType, int radius, int magnitude);

#endif