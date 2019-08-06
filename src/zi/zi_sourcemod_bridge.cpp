#include "zi_sourcemod_bridge.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"
#include "zi_round_modes.h"
#include "zi_items.h"

///// WRAPPERS OVER OUR DEFAULT CLASSES
// SM plugins need to dynamically allocate memory at initiation, these classes are prepared for dynamic allocation
// The classes will be freed automatically when the extension is unloaded

// TODO: Reconsider using the member variables in parent classes so I won't have to make these here

class ZISMHuman final: public ZISoldier
{
private:
	char *m_Name;
	char *m_Description;
	char *m_Model;

	bool m_IsVIP;

	int m_Health;
	int m_Armor;
	float m_Speed;
	float m_Gravity;
	float m_KnockbackPower;

public:
	ZISMHuman(const char *name, const char *description, const char *model, bool vip, int health, int armor, float speed, float gravity, float knockbackPower)
	{
		m_Name = (char *) name;
		m_Description = (char *) description;
		m_Model = (char *) model;

		m_IsVIP = vip;

		m_Health = health;
		m_Armor = armor;
		m_Speed = speed;
		m_Gravity = gravity;
		m_KnockbackPower = knockbackPower;
	};

	~ZISMHuman()
	{
		m_Name = nullptr;
		m_Description = nullptr;
		m_Model = nullptr;
	}

	const char *GetName() override
	{
		return m_Name;
	};

	const char *GetDescription() override
	{
		return m_Description;
	};

	ZIModel *GetModel() override
	{
		return ZIResources::GetModel(m_Model, ModelType_Human);
	}

	bool IsVIP() override
	{
		return m_IsVIP;
	}

	int GetHealth() override
	{
		return m_Health;
	};

	int GetArmor() override
	{
		return m_Armor;
	};

	float GetSpeed() override
	{
		return m_Speed;
	};

	float GetGravity() override
	{
		return m_Gravity;
	};

	float GetKnockbackPower() override
	{
		return m_KnockbackPower;
	};
};

class ZISMZombie final: public ZIZombie
{
private:
	char *m_Name;
	char *m_Description;
	char *m_Model;

	bool m_IsVIP;

	int m_Health;
	float m_Speed;
	float m_Gravity;
	float m_KnockbackResistance;

public:
	ZISMZombie(const char *name, const char *description, const char *model, bool vip, int health, float speed, float gravity, float knockbackResistance)
	{
		m_Name = (char *) name;
		m_Description = (char *) description;
		m_Model = (char *) model;

		m_IsVIP = vip;

		m_Health = health;
		m_Speed = speed;
		m_Gravity = gravity;
		m_KnockbackResistance = knockbackResistance;
	};

	~ZISMZombie()
	{
		m_Name = nullptr;
		m_Description = nullptr;
		m_Model = nullptr;
	}

	const char *GetName() override
	{
		return m_Name;
	};

	const char *GetDescription() override
	{
		return m_Description;
	};

	ZIModel *GetModel() override
	{
		return ZIResources::GetModel(m_Model, ModelType_Zombie);
	}

	bool IsVIP() override
	{
		return m_IsVIP;
	}

	int GetHealth() override
	{
		return m_Health;
	};

	float GetSpeed() override
	{
		return m_Speed;
	};

	float GetGravity() override
	{
		return m_Gravity;
	};

	float GetKnockbackResistance() override
	{
		return m_KnockbackResistance;
	};
};

class ZISMItem final: public ZIItem
{
private:
	char *m_Name;
	bool m_IsVIP;
	int m_Cost;

public:
	ZISMItem(const char *name, bool vip, int cost)
	{
		m_Name = (char *) name;
		m_IsVIP = vip;
		m_Cost = cost;
	};

	~ZISMItem()
	{
		m_Name = nullptr;
		m_IsVIP = false;
		m_Cost = 0;
	}

	const char *GetName() override
	{
		return m_Name;
	};

	bool IsVIP() override
	{
		return m_IsVIP;
	}

	int GetCost() override
	{
		return m_Cost;
	};

	void OnPostSelection(ZIPlayer *player) override
	{
		// Do nothing here, our ZICore::OnPostItemSelection will do the job
	}
};

static SourceHook::CVector<ZISoldier *> g_pSMHumanClasses;
static SourceHook::CVector<ZIZombie *> g_pSMZombieClasses;
static SourceHook::CVector<ZIItem *> g_pSMItems;

static cell_t IsPlayerVIP(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_IsVIP ? 1 : 0;
}

static cell_t IsPlayerInfected(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_IsInfected ? 1 : 0;
}

static cell_t IsPlayerLastHuman(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_IsLastHuman ? 1 : 0;
}

static cell_t IsPlayerLastZombie(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_IsLastZombie ? 1 : 0;
}

static cell_t IsPlayerSurvivor(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return !player->m_IsInfected && GET_SURVIVOR(player) ? 1 : 0;
}

static cell_t IsPlayerSniper(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return !player->m_IsInfected && GET_SNIPER(player) ? 1 : 0;
}

static cell_t IsPlayerNemesis(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1]; 

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false); 

	if( !player ) 
	{ 
	   context->ReportError("Player %d must be disconnected.", playerIndex); 
	   return 0; 
	} 

	return player->m_IsInfected && GET_NEMESIS(player) ? 1 : 0; 
} 

static cell_t IsPlayerAssassin(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1]; 

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false); 

	if( !player ) 
	{ 
	   context->ReportError("Player %d must be disconnected.", playerIndex); 
	   return 0; 
	} 

	return player->m_IsInfected && GET_ASSASSIN(player) ? 1 : 0; 
} 

static cell_t IsPlayerFrozen(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_IsFrozen ? 1 : 0;
}

static cell_t GetPlayerPoints(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_Points;
}

static cell_t SetPlayerPoints(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	player->m_Points = params[2];

	return 1;
}

static cell_t GetLastPlayerInfector(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_pInfector ? player->m_pInfector->m_Index : 0;
}

static cell_t GetLastPlayerDisinfector(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_pDisinfector ? player->m_pDisinfector->m_Index : 0;
}

static cell_t GetCurrentHumanClass(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	ZISoldier *soldier = GET_SOLDIER(player);

	return !player->m_IsInfected && soldier ? soldier->GetIndex() : -1;
}

static cell_t GetChosenHumanClass(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_pChoosenHumanClass ? player->m_pChoosenHumanClass->GetIndex() : -1;
}

static cell_t GetCurrentZombieClass(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	ZIZombie *zombie = GET_ZOMBIE(player);

	return player->m_IsInfected && zombie ? zombie->GetIndex() : -1;
}

static cell_t GetChosenZombieClass(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->m_pChoosenZombieClass ? player->m_pChoosenZombieClass->GetIndex() : -1;
}

static cell_t InfectPlayer(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	ZIInfected *infected = nullptr;

	// Nemesis
	if( params[3] )
	{
		infected = &g_Nemesis;
	}
	// Assassin
	else if( params[4] )
	{
		infected = &g_Assassin;
	}

	return player->Infect(ZIPlayer::Find(params[2], false), infected) ? 1 : 0;
}

static cell_t DisinfectPlayer(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	ZIHumanoid *humanoid = nullptr;

	// Survivor
	if( params[3] )
	{
		humanoid = &g_Survivor;
	}
	// Sniper
	else if( params[4] )
	{
		humanoid = &g_Sniper;
	}

	return player->Disinfect(ZIPlayer::Find(params[2], false), humanoid) ? 1 : 0;
}

static cell_t FreezePlayer(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	return player->Freeze(sp_ctof(params[2])) ? 1 : 0;
}

static cell_t UnfreezePlayer(IPluginContext *context, const cell_t *params)
{
	int playerIndex = params[1];

	ZIPlayer *player = ZIPlayer::Find(playerIndex, false);

	if( !player )
	{
		context->ReportError("Player %d must be disconnected.", playerIndex);
		return 0;
	}

	player->Unfreeze();

	return 1;
}

static cell_t GetHumansCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::HumansCount();
}

static cell_t GetSurvivorsCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::SurvivorsCount();
}

static cell_t GetSnipersCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::SnipersCount();
}

static cell_t GetZombiesCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::ZombiesCount();
}

static cell_t GetNemesisCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::NemesisCount();
}

static cell_t GetAssassinsCount(IPluginContext *context, const cell_t *params)
{
	return ZIPlayer::AssassinsCount();
}

static cell_t GetRandomAlivePlayer(IPluginContext *context, const cell_t *params)
{
	ZIPlayer *player = ZIPlayer::RandomAlive();

	if( !player )
	{
		context->ReportError("No player was found");
		return 0;
	}

	return player->m_Index;
}

static cell_t RegisterHumanClass(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZISoldier *soldier = ZISoldier::Find(name);

	if( soldier )
	{
		return soldier->GetIndex();
	}

	char *description = nullptr;
	context->LocalToStringNULL(params[2], &description);

	char *model = nullptr;
	context->LocalToStringNULL(params[3], &model);

	soldier = new ZISMHuman(name, description, model, params[4] > 0 ? true : false, params[5], params[6], sp_ctof(params[7]), sp_ctof(params[8]), sp_ctof(params[9]));

	// Releasing it later...
	g_pSMHumanClasses.push_back(soldier);

	return ZISoldier::Register(soldier);
}

static cell_t GetHumanClass(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZISoldier *soldier = ZISoldier::Find(name);

	if( soldier )
	{
		return soldier->GetIndex();
	}

	return -1;
}

static cell_t GetHumanClassHealth(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return g_pHumanClasses[hclass]->GetHealth();
}

static cell_t GetHumanClassArmor(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return g_pHumanClasses[hclass]->GetArmor();
}

static cell_t GetHumanClassSpeed(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pHumanClasses[hclass]->GetSpeed());
}

static cell_t GetHumanClassGravity(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pHumanClasses[hclass]->GetGravity());
}

static cell_t GetHumanClassKnockbackPower(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pHumanClasses[hclass]->GetKnockbackPower());
}

static cell_t IsHumanClassVIP(IPluginContext *context, const cell_t *params)
{
	size_t hclass = params[1];

	if( hclass >= g_pHumanClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return g_pHumanClasses[hclass]->IsVIP() ? 1 : 0;
}

static cell_t RegisterZombieClass(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZIZombie *zombie = ZIZombie::Find(name);

	if( zombie )
	{
		return zombie->GetIndex();
	}

	char *description = nullptr;
	context->LocalToStringNULL(params[2], &description);

	char *model = nullptr;
	context->LocalToStringNULL(params[3], &model);

	zombie = new ZISMZombie(name, description, model, params[4] > 0 ? true : false, params[5], sp_ctof(params[6]), sp_ctof(params[7]), sp_ctof(params[8]));

	// Releasing it later...
	g_pSMZombieClasses.push_back(zombie);

	return ZIZombie::Register(zombie);
}

static cell_t GetZombieClass(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZIZombie *zombie = ZIZombie::Find(name);

	if( zombie )
	{
		return zombie->GetIndex();
	}

	return -1;
}

static cell_t GetZombieClassHealth(IPluginContext *context, const cell_t *params)
{
	size_t zclass = params[1];

	if( zclass >= g_pZombieClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return g_pZombieClasses[zclass]->GetHealth();
}

static cell_t GetZombieClassSpeed(IPluginContext *context, const cell_t *params)
{
	size_t zclass = params[1];

	if( zclass >= g_pZombieClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pZombieClasses[zclass]->GetSpeed());
}

static cell_t GetZombieClassGravity(IPluginContext *context, const cell_t *params)
{
	size_t zclass = params[1];

	if( zclass >= g_pZombieClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pZombieClasses[zclass]->GetGravity());
}

static cell_t GetZombieClassKnockbackResistance(IPluginContext *context, const cell_t *params)
{
	size_t zclass = params[1];

	if( zclass >= g_pZombieClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return sp_ftoc(g_pZombieClasses[zclass]->GetKnockbackResistance());
}

static cell_t IsZombieClassVIP(IPluginContext *context, const cell_t *params)
{
	size_t zclass = params[1];

	if( zclass >= g_pZombieClasses.size() )
	{
		context->ReportError("No class was found");
		return 0;
	}

	return g_pZombieClasses[zclass]->IsVIP() ? 1 : 0;
}

static cell_t RegisterItem(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ItemTeam team = (ItemTeam) params[2];
	ZIItem *item = ZIItem::Find(name, team);

	if( item )
	{
		return item->GetIndex();
	}

	item = new ZISMItem(name, params[3] > 0 ? true : false, params[4]);

	// Releasing it later...
	g_pSMItems.push_back(item);

	return ZIItem::Register(item, team);
}

static cell_t GetItem(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZIItem *item = ZIItem::Find(name, (ItemTeam) params[2]);

	if( item )
	{
		return item->GetIndex();
	}

	return -1;
}

static cell_t GetItemCost(IPluginContext *context, const cell_t *params)
{
	size_t item = params[1];
	ItemTeam team = (ItemTeam) params[2];

	SourceHook::CVector<ZIItem *> *items = nullptr;

	switch( team )
	{
	case ItemTeam_Humans:
		items = &g_pHumansItems;

	case ItemTeam_Zombies:
		items = &g_pZombiesItems;
	}

	if( items )
	{
		if( item >= items->size() )
		{
			context->ReportError("No item was found");
			return 0;
		}

		return items->at(item)->GetCost();
	}

	context->ReportError("No item was found");
	return 0;
}

static cell_t IsItemVIP(IPluginContext *context, const cell_t *params)
{
	size_t item = params[1];
	ItemTeam team = (ItemTeam) params[2];

	SourceHook::CVector<ZIItem *> *items = nullptr;

	switch( team )
	{
	case ItemTeam_Humans:
		items = &g_pHumansItems;
		break;

	case ItemTeam_Zombies:
		items = &g_pZombiesItems;
		break;
	}

	if( items )
	{
		if( item >= items->size() )
		{
			context->ReportError("No item was found");
			return 0;
		}

		return items->at(item)->IsVIP() ? 1 : 0;
	}

	context->ReportError("No item was found");
	return 0;
}

static cell_t ZI_AdditionalItemInfo(IPluginContext *context, const cell_t *params)
{
	size_t item = params[1];
	ItemTeam team = (ItemTeam) params[2];

	SourceHook::CVector<ZIItem *> *items = nullptr;

	switch( team )
	{
	case ItemTeam_Humans:
		items = &g_pHumansItems;

	case ItemTeam_Zombies:
		items = &g_pZombiesItems;
	}

	if( items )
	{
		if( item >= items->size() )
		{
			context->ReportError("No item was found");
			return 0;
		}

		char *info = nullptr;
		context->LocalToString(params[3], &info);

		ZISMItem *extraItem = dynamic_cast<ZISMItem *> (items->at(item));

		if( extraItem )
		{
			extraItem->AdditionalInfo(info);
			return 1;
		}
	}

	context->ReportError("No item was found");
	return 0;
}

static cell_t GetRoundMode(IPluginContext *context, const cell_t *params)
{
	char *name = nullptr;
	context->LocalToString(params[1], &name);

	ZIRoundMode *mode = ZIRoundMode::Find(name);

	if( mode )
	{
		return mode->GetIndex();
	}

	return -1;
}

static cell_t GetRoundModeChance(IPluginContext *context, const cell_t *params)
{
	size_t mode = params[1];

	if( mode >= g_pRoundModes.size() )
	{
		context->ReportError("No mode was found");
		return 0;
	}

	return sp_ftoc(g_pRoundModes[mode]->GetChance() / ZIRoundMode::m_TotalChances);
}

static cell_t GetRoundTarget(IPluginContext *context, const cell_t *params)
{
	return ZIRoundMode::m_RoundTarget ? ZIRoundMode::m_RoundTarget->m_Index : 0;
}

static cell_t IsInfectionAllowed(IPluginContext *context, const cell_t *params)
{
	if( ZICore::m_CurrentMode && ZICore::m_IsModeStarted && !ZICore::m_IsRoundEnd )
	{
		return ZICore::m_CurrentMode->IsInfectionAllowed() ? 1 : 0;
	}

	return false;
}

static cell_t RechooseRoundMode(IPluginContext *context, const cell_t *params)
{
	ZIRoundMode::Choose();
	return 1;
}

static cell_t StartRoundMode(IPluginContext *context, const cell_t *params)
{
	size_t mode = params[1];

	if( mode >= g_pRoundModes.size() )
	{
		context->ReportError("No mode was found");
		return 0;
	}

	int targetIndex = params[2];

	ZIPlayer *target = ZIPlayer::Find(targetIndex, false);

	if( !target )
	{
		context->ReportError("Targeted player %d must be disconnected.", targetIndex);
		return 0;
	}

	ZIRoundMode::Start(g_pRoundModes[mode], target);
	return 1;
}

static const sp_nativeinfo_t g_SourceModNatives[] =
{	
	// Player
	{ "ZI_IsPlayerVIP", IsPlayerVIP },
	{ "ZI_IsPlayerInfected", IsPlayerInfected },
	{ "ZI_IsPlayerLastHuman", IsPlayerLastHuman },
	{ "ZI_IsPlayerLastZombie", IsPlayerLastZombie },
	{ "ZI_IsPlayerSurvivor", IsPlayerSurvivor },
	{ "ZI_IsPlayerSniper", IsPlayerSniper },
	{ "ZI_IsPlayerNemesis", IsPlayerNemesis },
	{ "ZI_IsPlayerAssassin", IsPlayerAssassin },
	{ "ZI_IsPlayerFrozen", IsPlayerFrozen },
	{ "ZI_GetPlayerPoints", GetPlayerPoints },
	{ "ZI_SetPlayerPoints", SetPlayerPoints },
	{ "ZI_GetLastPlayerInfector", GetLastPlayerInfector },
	{ "ZI_GetLastPlayerDisinfector", GetLastPlayerDisinfector },
	{ "ZI_GetCurrentHumanClass", GetCurrentHumanClass },
	{ "ZI_GetChosenHumanClass", GetChosenHumanClass },
	{ "ZI_GetCurrentZombieClass", GetCurrentZombieClass },
	{ "ZI_GetChosenZombieClass", GetChosenZombieClass },
	{ "ZI_InfectPlayer", InfectPlayer },
	{ "ZI_DisinfectPlayer", DisinfectPlayer },
	{ "ZI_FreezePlayer", FreezePlayer },
	{ "ZI_UnfreezePlayer", UnfreezePlayer },
	{ "ZI_GetHumansCount", GetHumansCount },
	{ "ZI_GetSurvivorsCount", GetSurvivorsCount },
	{ "ZI_GetSnipersCount", GetSnipersCount },
	{ "ZI_GetZombiesCount", GetZombiesCount },
	{ "ZI_GetNemesisCount", GetNemesisCount },
	{ "ZI_GetAssassinsCount", GetAssassinsCount },
	{ "ZI_GetRandomAlivePlayer", GetRandomAlivePlayer },

	// Human classes
	{ "ZI_RegisterHumanClass", RegisterHumanClass }, 
	{ "ZI_GetHumanClass", GetHumanClass }, 
	{ "ZI_GetHumanClassHealth", GetHumanClassHealth },
	{ "ZI_GetHumanClassArmor", GetHumanClassArmor },
	{ "ZI_GetHumanClassSpeed", GetHumanClassSpeed },
	{ "ZI_GetHumanClassGravity", GetHumanClassGravity },
	{ "ZI_GetHumanClassKnockbackPower", GetHumanClassKnockbackPower },
	{ "ZI_IsHumanClassVIP", IsHumanClassVIP },

	// Zombie classes
	{ "ZI_RegisterZombieClass", RegisterZombieClass },
	{ "ZI_GetZombieClass", GetZombieClass },
	{ "ZI_GetZombieClassHealth", GetZombieClassHealth },
	{ "ZI_GetZombieClassSpeed", GetZombieClassSpeed },
	{ "ZI_GetZombieClassGravity", GetZombieClassGravity },
	{ "ZI_GetZombieClassKnockbackResistance", GetZombieClassKnockbackResistance },
	{ "ZI_IsZombieClassVIP", IsZombieClassVIP },

	// Items	
	{ "ZI_RegisterItem", RegisterItem },
	{ "ZI_GetItem", GetItem },
	{ "ZI_GetItemCost", GetItemCost },
	{ "ZI_IsItemVIP", IsItemVIP },
	{ "ZI_AdditionalItemInfo", ZI_AdditionalItemInfo },	

	// Round modes
	{ "ZI_GetRoundMode", GetRoundMode },
	{ "ZI_GetRoundModeChance", GetRoundModeChance },
	{ "ZI_GetRoundTarget", GetRoundTarget },
	{ "ZI_IsInfectionAllowed", IsInfectionAllowed },
	{ "ZI_RechooseRoundMode", RechooseRoundMode },
	{ "ZI_StartRoundMode", StartRoundMode },

	{ NULL, NULL }
};

IForward *ZISourceModBridge::m_pPrePlayerInfection = nullptr;
IForward *ZISourceModBridge::m_pPostPlayerInfection = nullptr;
IForward *ZISourceModBridge::m_pPrePlayerDisinfection = nullptr;
IForward *ZISourceModBridge::m_pPostPlayerDisinfection = nullptr;
IForward *ZISourceModBridge::m_pPlayerLastHuman = nullptr;
IForward *ZISourceModBridge::m_pPlayerLastZombie = nullptr;
IForward *ZISourceModBridge::m_pPreItemSelection = nullptr;
IForward *ZISourceModBridge::m_pPostItemSelection = nullptr;
IForward *ZISourceModBridge::m_pRoundModeStart = nullptr;
IForward *ZISourceModBridge::m_pRoundModeEnd = nullptr;

ZISourceModBridge g_SourceModBridge;

void ZISourceModBridge::Load()
{
	// Register our library
	g_pShareSys->RegisterLibrary(myself, "ZombieInfestation");

	// Natives
	g_pShareSys->AddNatives(myself, g_SourceModNatives);

	// Forwards
	m_pPrePlayerInfection = g_pForwards->CreateForward("ZI_OnPrePlayerInfection", ET_Event, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
	m_pPostPlayerInfection = g_pForwards->CreateForward("ZI_OnPostPlayerInfection", ET_Event, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
	m_pPrePlayerDisinfection = g_pForwards->CreateForward("ZI_OnPrePlayerDisinfection", ET_Event, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
	m_pPostPlayerDisinfection = g_pForwards->CreateForward("ZI_OnPostPlayerDisinfection", ET_Event, 4, NULL, Param_Cell, Param_Cell, Param_Cell, Param_Cell);
	m_pPlayerLastHuman = g_pForwards->CreateForward("ZI_OnPlayerLastHuman", ET_Event, 1, NULL, Param_Cell);
	m_pPlayerLastZombie = g_pForwards->CreateForward("ZI_OnPlayerLastZombie", ET_Event, 1, NULL, Param_Cell);
	m_pPreItemSelection = g_pForwards->CreateForward("ZI_OnPreItemSelection", ET_Event, 3, NULL, Param_Cell, Param_Cell, Param_Cell);
	m_pPostItemSelection = g_pForwards->CreateForward("ZI_OnPostItemSelection", ET_Event, 3, NULL, Param_Cell, Param_Cell, Param_Cell);
	m_pRoundModeStart = g_pForwards->CreateForward("ZI_OnRoundModeStart", ET_Event, 2, NULL, Param_Cell, Param_Cell);
	m_pRoundModeEnd = g_pForwards->CreateForward("ZI_OnRoundModeEnd", ET_Event, 2, NULL, Param_Cell, Param_Cell);	
}

void ZISourceModBridge::Free()
{
	RELEASE_POINTERS_ARRAY(g_pSMHumanClasses);
	RELEASE_POINTERS_ARRAY(g_pSMZombieClasses);
	RELEASE_POINTERS_ARRAY(g_pSMItems);

	g_pForwards->ReleaseForward(m_pPrePlayerInfection); m_pPrePlayerInfection = nullptr;
	g_pForwards->ReleaseForward(m_pPostPlayerInfection); m_pPostPlayerInfection = nullptr;
	g_pForwards->ReleaseForward(m_pPrePlayerDisinfection); m_pPrePlayerDisinfection = nullptr;
	g_pForwards->ReleaseForward(m_pPostPlayerDisinfection); m_pPostPlayerDisinfection = nullptr;
	g_pForwards->ReleaseForward(m_pPlayerLastHuman); m_pPlayerLastHuman = nullptr;
	g_pForwards->ReleaseForward(m_pPlayerLastZombie); m_pPlayerLastZombie = nullptr;
	g_pForwards->ReleaseForward(m_pPreItemSelection); m_pPreItemSelection = nullptr;
	g_pForwards->ReleaseForward(m_pPostItemSelection); m_pPostItemSelection = nullptr;
	g_pForwards->ReleaseForward(m_pRoundModeStart); m_pRoundModeStart = nullptr;
	g_pForwards->ReleaseForward(m_pRoundModeEnd); m_pRoundModeEnd = nullptr;
}
