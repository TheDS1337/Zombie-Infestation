#ifndef TAKEDAMAGEINFO2_H
#define TAKEDAMAGEINFO2_H
#ifdef _WIN32
#pragma once
#endif

#define GAME_DLL 1

#include <isaverestore.h>

#ifndef _DEBUG
#include <../game/shared/ehandle.h>
#else
#undef _DEBUG
#include <../game/shared/ehandle.h>
#define _DEBUG 1
#endif

#include <server_class.h>

#include <../game/shared/shareddefs.h>
#include <../game/shared/takedamageinfo.h>

class CTakeDamageInfo2 : public CTakeDamageInfo
{
public:
	CTakeDamageInfo2( CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType, CBaseEntity *pWeapon, Vector vecDamageForce, Vector vecDamagePosition );
	
	inline int GetAttacker() const
	{
		return m_hAttacker.IsValid() ? m_hAttacker.GetEntryIndex() : -1;
	}

	inline int GetInflictor() const
	{
		return m_hInflictor.IsValid() ? m_hInflictor.GetEntryIndex() : -1;
	}

	inline int GetWeapon() const
	{
		return m_hWeapon.IsValid() ? m_hWeapon.GetEntryIndex() : -1;
	}

	inline void SetDamageForce(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamageForce.x = x;
		m_vecDamageForce.y = y;
		m_vecDamageForce.z = z;
	}

	inline void SetDamagePosition(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamagePosition.x = x;
		m_vecDamagePosition.y = y;
		m_vecDamagePosition.z = z;
	}
};

#endif
