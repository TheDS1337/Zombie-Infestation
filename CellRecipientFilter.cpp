#include "CellRecipientFilter.h"

CellRecipientFilter::CellRecipientFilter()
{
	m_IsReliable = false;
	m_IsInitMessage = false;

	int *players = nullptr;
	int playersCount = _OnlinePlayers(&players);

	Initialize(players, playersCount);
}

CellRecipientFilter::CellRecipientFilter(int client)
{
	m_IsReliable = false;
	m_IsInitMessage = false;

	static int player;	// needs to be static since we're passing the address outside of the function
	player = client;

	Initialize(&player, 1);
}

void CellRecipientFilter::Reset()
{
	m_IsReliable = false;
	m_IsInitMessage = false;
	m_Size = 0;
}

bool CellRecipientFilter::IsReliable() const
{
	return m_IsReliable;
}

bool CellRecipientFilter::IsInitMessage() const
{
	return m_IsInitMessage;
}

int CellRecipientFilter::GetRecipientCount() const
{
	return m_Size;
}

int CellRecipientFilter::GetRecipientIndex(int slot) const
{
	if( (slot < 0) || (slot >= GetRecipientCount()) )
	{
		return -1;
	}	

	return m_Players[slot];
}

void CellRecipientFilter::SetToInit(bool isinitmsg)
{
	m_IsInitMessage = isinitmsg;
}

void CellRecipientFilter::SetToReliable(bool isreliable)
{
	m_IsReliable = isreliable;
}

void CellRecipientFilter::Initialize(int *ptr, size_t count)
{
	memcpy(m_Players, ptr, count * sizeof(int));
	m_Size = count;
}

size_t _FillInPlayers(int **pl_array, IRecipientFilter *pFilter)
{
	static int players[SM_MAXPLAYERS];

	size_t size = static_cast<size_t>(pFilter->GetRecipientCount());

	for( size_t i = 0; i<size; i++ )
	{
		players[i] = pFilter->GetRecipientIndex(i);
	}

	*pl_array = players;

	return size;
}

size_t _OnlinePlayers(int **pl_array)
{
	static int players[SM_MAXPLAYERS];

	size_t count = 0, maxClients = g_pExtension->m_pGlobals->maxClients;

	for( size_t i = 1; i <= maxClients; i++ )
	{
		IGamePlayer *player = playerhelpers->GetGamePlayer(i);

		if( !player || !player->IsInGame() )
		{
			continue;
		}

		players[count++] = i;
	}

	*pl_array = players;

	return count;
}