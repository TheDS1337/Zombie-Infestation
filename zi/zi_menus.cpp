#include "zi_menus.h"
#include "zi_players.h"
#include "zi_humans.h"
#include "zi_zombies.h"
#include "zi_items.h"
#include "zi_weapons.h"
#include "zi_spawns_manager.h"

namespace ZIMenusCallback
{
	Main m_Main;
	PrimaryWeapons m_PrimaryWeapons;
	SecondaryWeapons m_SecondaryWeapons;
	Items m_Items;
	HumanSelection m_HumanSelection;
	ZombieSelection m_ZombieSelection;

	void Main::OnMenuStart(IBaseMenu *menu)
	{
	}

	void Main::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void Main::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void Main::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		BasePlayer *playerEnt = player->m_pEntity;

		switch( item )
		{
			// Weapons
		case 0:
			if( player->m_CanGetPrimaryWeapon )
			{
				player->ShowWeaponsMenu(CSGO_WEAPON_SLOT_PRIMARY);
			}
			else if( player->m_CanGetSecondaryWeapon )
			{
				player->ShowWeaponsMenu(CSGO_WEAPON_SLOT_SECONDARY);
			}
			else
			{
				UM_SayText(&player->m_Index, 1, 0, true, "You already got your weapons.");
			}

			break;

			// Extra Items
		case 1:
			player->ShowItemsMenu();
			break;

			// Human classes
		case 2:
			player->ShowHumanSelectionMenu();
			break;

			// Zombie classes
		case 3:
			player->ShowZombieSelectionMenu();
			break;

			// Teleport
		case 4:
			if( !player->m_IsAlive )
			{
				UM_SayText(&player->m_Index, 1, 0, true, "You have to be\x04 alive\x01.");
			}
			else if( playerEnt->CanGetInSpace(playerEnt->GetOrigin(), playerEnt->GetFlags() & FL_DUCKING ? true : false) )
			{
				UM_SayText(&player->m_Index, 1, 0, true, "You are not\x04 stuck\x01.");
			}
			else if( ZISpawnsManager::Teleport(player) )
			{
				UM_SayText(&player->m_Index, 1, 0, true, "You have been teleported!");
			}

			break;

			// Join Spectator/Team
		case 5:
			if( playerEnt->GetTeam() == CSGO_TEAM_SPECTATOR )
			{
				player->m_AllowTeamChoosing = true;
				g_pExtension->m_pEngineServer->ClientCommand(player->m_pEdict, "teammenu");
			}
			else
			{
				// We were allowed to choose a team, just because were spectators
				player->m_AllowTeamChoosing = false;

				// Kill the alive before you change his team.
				if( player->m_IsAlive )
				{
					player->m_IsAlive = false;
				}

				playerEnt->SetTeam(CSGO_TEAM_SPECTATOR);
			}

			break;
		}
	}

	void Main::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void Main::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void Main::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void Main::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int Main::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}

	void PrimaryWeapons::OnMenuStart(IBaseMenu *menu)
	{
	}

	void PrimaryWeapons::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void PrimaryWeapons::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void PrimaryWeapons::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		// Infected or not a regular human?
		if( !player->m_IsAlive || player->m_IsInfected || !GET_SOLDIER(player) )
		{
			CLOSE_MENU(player->m_pWeaponsMenu);
			return;
		}

		int weaponId = (int) menu->GetItemInfo(item, nullptr)[0];

		ZIWeapon *weapon = &g_CSGOWeapons[weaponId];

		if( weapon )
		{
			BasePlayer *playerEnt = player->m_pEntity;

			playerEnt->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_PRIMARY);
			playerEnt->GiveItem(weapon->GetClassname());

			player->m_CanGetPrimaryWeapon = false;
		}

		CLOSE_MENU(player->m_pWeaponsMenu);

		// Show secondary weapons menu
		player->ShowWeaponsMenu(CSGO_WEAPON_SLOT_SECONDARY);
	}

	void PrimaryWeapons::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void PrimaryWeapons::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void PrimaryWeapons::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void PrimaryWeapons::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int PrimaryWeapons::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}

	void SecondaryWeapons::OnMenuStart(IBaseMenu *menu)
	{
	}

	void SecondaryWeapons::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void SecondaryWeapons::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void SecondaryWeapons::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		// Infected or not a regular human?
		if( !player->m_IsAlive || player->m_IsInfected || !GET_SOLDIER(player) )
		{
			CLOSE_MENU(player->m_pWeaponsMenu);
			return;
		}

		int weaponId = (int) menu->GetItemInfo(item, nullptr)[0];

		ZIWeapon *weapon = &g_CSGOWeapons[weaponId];

		if( weapon )
		{
			BasePlayer *playerEnt = player->m_pEntity;

			playerEnt->RemoveItemsFromSlot(CSGO_WEAPON_SLOT_SECONDARY);
			playerEnt->GiveItem(weapon->GetClassname());

			player->m_CanGetSecondaryWeapon = false;
		}

		CLOSE_MENU(player->m_pWeaponsMenu);
	}

	void SecondaryWeapons::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void SecondaryWeapons::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void SecondaryWeapons::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void SecondaryWeapons::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int SecondaryWeapons::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}

	void Items::OnMenuStart(IBaseMenu *menu)
	{
	}

	void Items::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void Items::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void Items::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		int *playerIndex = &player->m_Index;

		if( ZICore::m_IsRoundEnd )
		{
			UM_SayText(playerIndex, 1, 0, true, "Items are not available right now.");

			RELEASE_MENU(player->m_pItemsMenu);
			return;
		}

		if( !player->m_IsAlive )
		{
			UM_SayText(playerIndex, 1, 0, true, "You have to be\x04 alive\x01.");

			RELEASE_MENU(player->m_pItemsMenu);
			return;
		}

		if( g_pExtraItems.size() < 1 )
		{
			UM_SayText(playerIndex, 1, 0, true, "This class doesn't have any items.");

			RELEASE_MENU(player->m_pItemsMenu);
			return;
		}

		ZIItem *extraItem = g_pExtraItems.at(item);

		if( extraItem )
		{
			if( extraItem->IsVIP() && !player->m_IsVIP )
			{
				UM_SayText(playerIndex, 1, 0, true, "This item is only available for\x04 VIP\x01 players.");

				RELEASE_MENU(player->m_pItemsMenu);
				return;
			}

			if( extraItem->OnPreSelection(player) == ItemReturn_NotAvailable || ZICore::OnPreItemSelection(extraItem, player) == ItemReturn_NotAvailable )
			{
				//			RELEASE_MENU(player->m_pItemsMenu);
				return;
			}

			int cost = extraItem->GetCost();

			if( cost > player->m_Points )
			{
				UM_SayText(playerIndex, 1, 0, true, "You cannot afford this item.");

				RELEASE_MENU(player->m_pItemsMenu);
				return;
			}

			player->m_Points -= cost;
			extraItem->OnPostSelection(player);

			// Forward it to sub-modules and plugins
			ZICore::OnPostItemSelection(extraItem, player);

			// Reset the item info
			extraItem->AdditionalInfo(nullptr);
		}
		else
		{
			UM_SayText(playerIndex, 1, 0, true, "Invalid item.");
		}

		RELEASE_MENU(player->m_pItemsMenu);
	}

	void Items::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void Items::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void Items::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void Items::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int Items::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}

	void HumanSelection::OnMenuStart(IBaseMenu *menu)
	{
	}

	void HumanSelection::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void HumanSelection::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void HumanSelection::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		int *playerIndex = &player->m_Index;
		ZISoldier *soldier = g_pHumanClasses[item];

		if( soldier )
		{
			if( soldier->IsVIP() && !player->m_IsVIP )
			{
				UM_SayText(playerIndex, 1, 0, true, "This human class is only available for\x04 VIP\x01 players.");
			}
			else
			{
				player->m_pChoosenHumanClass = soldier;

				static char buffer[256];

				if( ZICore::m_IsModeStarted )
				{
					g_pSM->Format(buffer, sizeof(buffer), "Your next human class will be:\x04 %s", soldier->GetName());
				}
				else
				{
					player->Disinfect();

					g_pSM->Format(buffer, sizeof(buffer), "You are playing with:\x04 %s", soldier->GetName());
				}

				UM_SayText(playerIndex, 1, 0, true, buffer);

				g_pSM->Format(buffer, sizeof(buffer), "\x04Health:\x03 %d\x01,\x04 Armor:\x03 %d\x01,\x04 Speed:\x03 %.2f\x01,\x04 Gravity:\x03 %.2f\x01,\x04 Knockback Power:\x03 %.2f", soldier->GetHealth(), soldier->GetArmor(), soldier->GetSpeed(), soldier->GetGravity(), soldier->GetKnockbackPower());
				UM_SayText(playerIndex, 1, 0, true, buffer);
			}
		}
		else
		{
			UM_SayText(playerIndex, 1, 0, true, "Invalid human class.");
		}
	}

	void HumanSelection::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void HumanSelection::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void HumanSelection::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void HumanSelection::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int HumanSelection::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}

	void ZombieSelection::OnMenuStart(IBaseMenu *menu)
	{
	}

	void ZombieSelection::OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display)
	{
	}

	void ZombieSelection::OnMenuSelect(IBaseMenu *menu, int client, unsigned int item)
	{
	}

	void ZombieSelection::OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage)
	{
		ZIPlayer *player = ZIPlayer::Find(client, false);

		if( !player )
		{
			return;
		}

		int *playerIndex = &player->m_Index;
		ZIZombie *zombie = g_pZombieClasses[item];

		if( zombie )
		{
			if( zombie->IsVIP() && !player->m_IsVIP )
			{
				UM_SayText(playerIndex, 1, 0, true, "This zombie class is only available for\x04 VIP\x01 players.");
			}
			else
			{
				player->m_pChoosenZombieClass = zombie;

				static char buffer[256];

				g_pSM->Format(buffer, sizeof(buffer), "Your next zombie class will be:\x04 %s", zombie->GetName());
				UM_SayText(playerIndex, 1, 0, true, buffer);

				g_pSM->Format(buffer, sizeof(buffer), "\x04Health:\x03 %d\x01,\x04 Speed:\x03 %.2f\x01,\x04 Gravity:\x03 %.2f\x01,\x04 Knockback Resistance:\x03 %.2f", zombie->GetHealth(), zombie->GetSpeed(), zombie->GetGravity(), zombie->GetKnockbackResistance());
				UM_SayText(playerIndex, 1, 0, true, buffer);
			}
		}
		else
		{
			UM_SayText(playerIndex, 1, 0, true, "Invalid zombie class.");
		}
	}

	void ZombieSelection::OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason)
	{
	}

	void ZombieSelection::OnMenuEnd(IBaseMenu *menu, MenuEndReason reason)
	{
	}

	void ZombieSelection::OnMenuDestroy(IBaseMenu *menu)
	{
	}

	void ZombieSelection::OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style)
	{
	}

	unsigned int ZombieSelection::OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw)
	{
		return 0;
	}
}