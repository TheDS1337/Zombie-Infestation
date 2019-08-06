#ifndef _INCLUDE_HUD_MANAGER_PROPER_H_
#define _INCLUDE_HUD_MANAGER_PROPER_H_
#pragma once

#include "extension.h"

#define MAX_HUD_CHANNELS		6

struct hud_obj_t
{
	int player_channels[256 + 1];
};

struct client_chaninfo_t
{
	double chan_times[MAX_HUD_CHANNELS];
	hud_obj_t *chan_syncobjs[MAX_HUD_CHANNELS];
};

class HudManager
{
private:
	static client_chaninfo_t m_PlayerHuds[256 + 1];

public:
	static int AutoSelectChannel(unsigned int client);
	static int AutoSelectChannel(unsigned int client, hud_obj_t *obj);
	static int TryReuseLastChannel(unsigned int client, hud_obj_t *obj);
	static int ManualSelectChannel(unsigned int client, int channel);

	static void ResetClientHud(int client);	
};

extern HudManager g_HudManager;

#endif 