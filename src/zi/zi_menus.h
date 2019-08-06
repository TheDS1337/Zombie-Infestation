#ifndef _INCLUDE_ZI_MENUS_PROPER_H_
#define _INCLUDE_ZI_MENUS_PROPER_H_
#pragma once

#include "zi_core.h"

class ZIMenusCallback
{
public:
	class Main final: public IMenuHandler 
	{ 
	public:
		void OnMenuStart(IBaseMenu *menu) override; 
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override; 
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override; 
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override; 
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override; 
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override; 
		void OnMenuDestroy(IBaseMenu *menu) override; 
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override; 
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static Main m_Main;

	class PrimaryWeapons final: public IMenuHandler
	{
	public:
		void OnMenuStart(IBaseMenu *menu) override;
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override;
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override;
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override;
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override;
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override;
		void OnMenuDestroy(IBaseMenu *menu) override;
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override;
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static PrimaryWeapons m_PrimaryWeapons;

	class SecondaryWeapons final: public IMenuHandler
	{
	public:
		void OnMenuStart(IBaseMenu *menu) override;
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override;
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override;
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override;
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override;
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override;
		void OnMenuDestroy(IBaseMenu *menu) override;
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override;
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static SecondaryWeapons m_SecondaryWeapons;

	class Items final: public IMenuHandler
	{
	public:
		void OnMenuStart(IBaseMenu *menu) override;
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override;
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override;
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override;
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override;
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override;
		void OnMenuDestroy(IBaseMenu *menu) override;
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override;
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static Items m_Items;

	class HumanSelection final: public IMenuHandler
	{
	public:
		void OnMenuStart(IBaseMenu *menu) override;
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override;
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override;
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override;
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override;
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override;
		void OnMenuDestroy(IBaseMenu *menu) override;
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override;
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static HumanSelection m_HumanSelection;

	class ZombieSelection final: public IMenuHandler
	{
	public:
		void OnMenuStart(IBaseMenu *menu) override;
		void OnMenuDisplay(IBaseMenu *menu, int client, IMenuPanel *display) override;
		void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override;
		void OnMenuSelect2(IBaseMenu *menu, int client, unsigned int item, unsigned int itemOnPage) override;
		void OnMenuCancel(IBaseMenu *menu, int client, MenuCancelReason reason) override;
		void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override;
		void OnMenuDestroy(IBaseMenu *menu) override;
		void OnMenuDrawItem(IBaseMenu *menu, int client, unsigned int item, unsigned int &style) override;
		unsigned int OnMenuDisplayItem(IBaseMenu *menu, int client, IMenuPanel *panel, unsigned int item, const ItemDrawInfo &itemDraw) override;
	};

	static ZombieSelection m_ZombieSelection;	
};

#endif 