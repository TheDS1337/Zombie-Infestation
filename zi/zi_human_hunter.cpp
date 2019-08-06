#include "zi_human_hunter.h"

HunterHuman g_HunterHuman;

const char *HunterHuman::GetName()
{
	return HUNTER_NAME;
}

const char *HunterHuman::GetDescription()
{
	return HUNTER_DESCRIPTION;
}

ZIModel *HunterHuman::GetModel()
{
	// TODO
	static ZIModel model;

	// Get a Zombie player model
	ZIModel *zombiePlayerModel = ZIResources::RandomModel(ModelType_Zombie);	

	// But give it a Human arms model
	ZIModel *HumanPlayerModel = ZIResources::RandomModel(ModelType_Human);

	
	// Setup our costumized weapon
	model = ZIModel("Hunter", zombiePlayerModel->GetPlayerModel(), HumanPlayerModel->GetArmsModel(), -1, -1);	

	return &model;
}

bool HunterHuman::IsVIP()
{
	return HUNTER_VIP;
}

int HunterHuman::GetHealth()
{
	return HUNTER_HEALTH;
}

int HunterHuman::GetArmor()
{
	return HUNTER_ARMOR;
}

float HunterHuman::GetSpeed()
{
	return HUNTER_SPEED;
}

float HunterHuman::GetGravity()
{
	return HUNTER_GRAVITY;
}

float HunterHuman::GetKnockbackPower()
{
	return HUNTER_KNOCKBACK_POWER;
}

HookReturn HunterHuman::OnPostClientThink(ZIPlayer *player)
{
	if( !player->m_IsAlive || player->m_pHumanLike != this )
	{
		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	player->m_pEntity->SetAddon(CSGO_ADDON_NONE);

	HOOK_RETURN_VOID(MRES_IGNORED, false);
}

bool HunterHuman::OnPreClientDeath(ZIPlayer *player, CTakeDamageInfo2 &info)
{
	if( player->m_pHumanLike != this )
	{
		return true;
	}

	// Change his ragdoll to something human...
	ZIModel *model = ZIResources::RandomModel(ModelType_Human);

	if( model )
	{
		player->m_pEntity->SetModel(model->GetPlayerModel());
	}

	// Allow death
	return true;
}