#include "takedamageinfo2.h"

CTakeDamageInfo::CTakeDamageInfo(){}

CTakeDamageInfo2::CTakeDamageInfo2( CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType, CBaseEntity *pWeapon, Vector vecDamageForce, Vector vecDamagePosition )
{
	m_hInflictor = pInflictor;
	if ( pAttacker )
	{
		m_hAttacker = pAttacker;
	}
	else
	{
		m_hAttacker = pInflictor;
	}

	m_hWeapon = pWeapon;

	m_flDamage = flDamage;

	m_flBaseDamage = BASEDAMAGE_NOT_SPECIFIED;

	m_bitsDamageType = bitsDamageType;

	m_flMaxDamage = flDamage;
	m_vecDamageForce = vec3_origin;
	m_vecDamagePosition = vec3_origin;
	m_vecReportedPosition = vec3_origin;
	m_iAmmoType = -1;

	m_iDamageCustom = 0;

	m_flRadius = 0.0f;

	m_iDamagedOtherPlayers = 0;
	m_iObjectsPenetrated = 0;
	m_uiBulletID = 0;
	m_uiRecoilIndex = 0;
}