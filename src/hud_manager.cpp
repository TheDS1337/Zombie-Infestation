#include "hud_manager.h"

client_chaninfo_t HudManager::m_PlayerHuds[256 + 1];

HudManager g_HudManager;

int HudManager::AutoSelectChannel(unsigned int client)
{
	client_chaninfo_t *player = &m_PlayerHuds[client];

	int last_channel = 0;

	for( unsigned int i = 1; i < MAX_HUD_CHANNELS; i++ )
	{
		if( player->chan_times[i] < player->chan_times[last_channel] )
		{
			last_channel = i;
		}
	}

	ManualSelectChannel(client, last_channel);
	return last_channel;
}

int HudManager::AutoSelectChannel(unsigned int client, hud_obj_t *obj)
{
	// First, see if we can re-use the previous channel. 
	int last_channel = obj->player_channels[client];

	client_chaninfo_t *player = &m_PlayerHuds[client];

	if( player->chan_syncobjs[last_channel] == obj )
	{
		player->chan_times[last_channel] = timersys->GetTickedTime();
		return last_channel;
	}

	// If not, start over
	last_channel = 0;

	for( unsigned int i = 1; i < MAX_HUD_CHANNELS; i++ )
	{
		if( player->chan_times[i] < player->chan_times[last_channel] )
		{
			last_channel = i;
		}
	}

	obj->player_channels[client] = last_channel;
	player->chan_syncobjs[last_channel] = obj;
	player->chan_times[last_channel] = timersys->GetTickedTime();

	return last_channel;
}

int HudManager::TryReuseLastChannel(unsigned int client, hud_obj_t *obj)
{
	// First, see if we can re-use the previous channel. 
	int last_channel = obj->player_channels[client];

	client_chaninfo_t *player = &m_PlayerHuds[client];

	if( player->chan_syncobjs[last_channel] == obj )
	{
		player->chan_times[last_channel] = timersys->GetTickedTime();
		return last_channel;
	}

	return -1;
}

int HudManager::ManualSelectChannel(unsigned int client, int channel)
{
	client_chaninfo_t *player = &m_PlayerHuds[client];

	player->chan_times[channel] = timersys->GetTickedTime();
	player->chan_syncobjs[channel] = nullptr;

	return channel;
}

void HudManager::ResetClientHud(int client)
{
	client_chaninfo_t *player = &m_PlayerHuds[client];

	memset(player->chan_syncobjs, 0, sizeof(player->chan_syncobjs));
	memset(player->chan_times, 0, sizeof(player->chan_times));
}