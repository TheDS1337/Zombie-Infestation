#include "zi_core.h"
#include "zi_weapons.h"
#include "zi_menus.h"

ZIWeapon g_CSGOWeapons[CSGO_WEAPONS] =
{
	// Name								Classname					Slot						Clip		Ammo		Knockback Power

	// Pistols
	ZIWeapon("Glock-18",				"weapon_glock",				CSGO_WEAPON_SLOT_SECONDARY, 20,			120,		2.0f),
	ZIWeapon("P2000",					"weapon_hkp2000",			CSGO_WEAPON_SLOT_SECONDARY,	13,			52,			2.2f),
	ZIWeapon("USP-S",					"weapon_usp_silencer",		CSGO_WEAPON_SLOT_SECONDARY,	12,			24,			2.5f),
	ZIWeapon("P250",					"weapon_p250",				CSGO_WEAPON_SLOT_SECONDARY,	13,			26,			2.5f),
	ZIWeapon("Tec-9",					"weapon_tec9",				CSGO_WEAPON_SLOT_SECONDARY,	18,			90,			2.3f),
	ZIWeapon("CZ75-Auto",				"weapon_cz75a",				CSGO_WEAPON_SLOT_SECONDARY,	12,			12,			5.0f),		// see about this 
	ZIWeapon("Fiveseven",				"weapon_fiveseven",			CSGO_WEAPON_SLOT_SECONDARY,	20,			100,		2.5f),
	ZIWeapon("Desert Eagle",			"weapon_deagle",			CSGO_WEAPON_SLOT_SECONDARY,	7,			35,			5.3f),
	ZIWeapon("R8 Revolver",				"weapon_revolver",			CSGO_WEAPON_SLOT_SECONDARY,	8,			8,			7.0f),		// see about this
	ZIWeapon("Dual Berettas Elite",		"weapon_elite",				CSGO_WEAPON_SLOT_SECONDARY,	30,			120,		2.0f),

	// Rifles
	ZIWeapon("Galil",					"weapon_galilar",			CSGO_WEAPON_SLOT_PRIMARY,	35,			90,			5.5f),
	ZIWeapon("Famas",					"weapon_famas",				CSGO_WEAPON_SLOT_PRIMARY,	25,			75,			5.5f),
	ZIWeapon("AK-47",					"weapon_ak47",				CSGO_WEAPON_SLOT_PRIMARY,	30,			90,			6.2f),
	ZIWeapon("M4A4",					"weapon_m4a1",				CSGO_WEAPON_SLOT_PRIMARY,	30,			90,			5.9f),
	ZIWeapon("M4A1-S",					"weapon_m4a1_silencer",		CSGO_WEAPON_SLOT_PRIMARY,	25,			75,			5.7f),
	ZIWeapon("SG556",					"weapon_sg556",				CSGO_WEAPON_SLOT_PRIMARY,	30,			90,			5.3f),
	ZIWeapon("AUG",						"weapon_aug",				CSGO_WEAPON_SLOT_PRIMARY,	30,			90,			5.3f),

	// Shotguns
	ZIWeapon("Nova",					"weapon_nova",				CSGO_WEAPON_SLOT_PRIMARY,	8,			32,			8.0f),
	ZIWeapon("XM1014",					"weapon_xm1014",			CSGO_WEAPON_SLOT_PRIMARY,	7,			32,			7.5f),
	ZIWeapon("Sawed-Off",				"weapon_sawedoff",			CSGO_WEAPON_SLOT_PRIMARY,	7,			32,			8.0f),
	ZIWeapon("MAG7",					"weapon_mag7",				CSGO_WEAPON_SLOT_PRIMARY,	8,			32,			8.0f),

	// SMGs
	ZIWeapon("MAC10",					"weapon_mac10",				CSGO_WEAPON_SLOT_PRIMARY,	30,			100,		2.3f),
	ZIWeapon("MP9",						"weapon_mp9",				CSGO_WEAPON_SLOT_PRIMARY,	30,			120,		2.3f),
	ZIWeapon("UMP45",					"weapon_ump45",				CSGO_WEAPON_SLOT_PRIMARY,	25,			100,		2.7f),
	ZIWeapon("MP5 Navy",				"weapon_mp5sd",				CSGO_WEAPON_SLOT_PRIMARY,	30,			120,		2.4f),
	ZIWeapon("MP7",						"weapon_mp7",				CSGO_WEAPON_SLOT_PRIMARY,	30,			120,		2.5f),
	ZIWeapon("PP Bizon",				"weapon_bizon",				CSGO_WEAPON_SLOT_PRIMARY,	64,			120,		1.8f),
	ZIWeapon("P90",						"weapon_p90",				CSGO_WEAPON_SLOT_PRIMARY,	50,			200,		2.0f),

	// Machine guns
	ZIWeapon("Negev",					"weapon_negev",				CSGO_WEAPON_SLOT_PRIMARY,	150,		300,		5.0f),
	ZIWeapon("M249",					"weapon_m249",				CSGO_WEAPON_SLOT_PRIMARY,	100,		200,		5.4f),

	// Snipers
	ZIWeapon("SSG 08 Scout",			"weapon_ssg08",				CSGO_WEAPON_SLOT_PRIMARY,	10,			90,			7.5f),
	ZIWeapon("AWP",						"weapon_awp",				CSGO_WEAPON_SLOT_PRIMARY,	10,			30,			10.0f),
	ZIWeapon("G3SG1",					"weapon_g3sg1",				CSGO_WEAPON_SLOT_PRIMARY,	20,			90,			6.2f),
	ZIWeapon("SCAR-20",					"weapon_scar20",			CSGO_WEAPON_SLOT_PRIMARY,	20,			90,			6.5f)
};

ZIWeapon::ZIWeapon(const char *name, const char *classname, int slot, int clip, int ammo, float knockback_power)
{
	m_pName = name;
	m_pClassname = classname;
	m_Slot = slot;
	m_Clip = clip;
	m_Ammo = ammo;
	m_KnockbackPower = knockback_power;
}

ZIWeapon *ZIWeapon::Find(const char *classname)
{
	ZIWeapon *weapon = nullptr;

	for( int iterator = 0; iterator < CSGO_WEAPONS; iterator++ )
	{
		weapon = &g_CSGOWeapons[iterator];

		if( !weapon || strcmp(weapon->m_pClassname, classname) != 0 )
		{
			continue;
		}

		return weapon;
	}

	if( !strstr(classname, "knife") )
	{
		CONSOLE_DEBUGGER("Warning: weapon isn't registred: %s", classname);
	}
	return nullptr;
}

ZIWeapon *ZIWeapon::Find(BaseWeapon *weaponEnt)
{
	const char *classname = weaponEnt->GetClassname();

	if( classname && *classname )
	{
		return Find(classname);
	}

	return nullptr;
}

ZIWeapon *ZIWeapon::FindActive(ZIPlayer *player)
{
	BaseWeapon *weaponEnt = player->m_pEntity->GetActiveWeapon();

	if( weaponEnt )
	{
		return Find(weaponEnt);
	}

	return nullptr;
}

const char *ZIWeapon::GetName()
{
	return m_pName;
}

const char *ZIWeapon::GetClassname()
{
	return m_pClassname;
}

int ZIWeapon::GetSlot()
{
	return m_Slot;
}

int ZIWeapon::GetClip()
{
	return m_Clip;
}

int ZIWeapon::GetAmmo()
{
	return m_Ammo;
}

float ZIWeapon::GetKnockbackPower()
{
	return m_KnockbackPower;
}

IBaseMenu *ZIWeapon::BuildPrimaryWeaponsMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		return nullptr;
	}

	static IBaseMenu *menu = nullptr;

	if( !menu && (menu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_PrimaryWeapons)) )
	{	
		menu->SetDefaultTitle("Primary Weapons");

		static char itemInfo[2];

		ZIWeapon *weapon = nullptr;

		for( int iterator = 0; iterator < CSGO_WEAPONS; iterator++ )
		{
			weapon = &g_CSGOWeapons[iterator];

			if( !weapon || weapon->GetSlot() != CSGO_WEAPON_SLOT_PRIMARY )
			{
				continue;
			}

			itemInfo[0] = iterator;
			itemInfo[1] = '\0';

			menu->AppendItem(itemInfo, ItemDrawInfo(weapon->GetName(), ITEMDRAW_DEFAULT));
		}

		menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT | MENUFLAG_BUTTON_EXITBACK);
	}

	return menu;
}

IBaseMenu *ZIWeapon::BuildSecondaryWeaponsMenu()
{
	if( !g_pExtension->m_pMenuStyle )
	{
		return nullptr;
	}

	static IBaseMenu *menu = nullptr;

	if( !menu && (menu = g_pExtension->m_pMenuStyle->CreateMenu(&ZICore::m_MenusCallback.m_SecondaryWeapons)) )
	{
		menu->SetDefaultTitle("Secondary Weapons");

		static char itemInfo[2];

		ZIWeapon *weapon = nullptr;

		for( int iterator = 0; iterator < CSGO_WEAPONS; iterator++ )
		{
			weapon = &g_CSGOWeapons[iterator];

			if( !weapon || weapon->GetSlot() != CSGO_WEAPON_SLOT_SECONDARY )
			{
				continue;
			}

			itemInfo[0] = iterator;
			itemInfo[1] = '\0';

			menu->AppendItem(itemInfo, ItemDrawInfo(weapon->GetName(), ITEMDRAW_DEFAULT));
		}
		
		menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT | MENUFLAG_BUTTON_EXITBACK);
	}

	return menu;
}