/**
* vim: set ts=4 :
* =============================================================================
* SourceMod SDKTools Extension
* Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, AlliedModders LLC gives you permission to link the
* code of this program (as well as its derivative works) to "Half-Life 2," the
* "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, AlliedModders LLC grants
* this exception to all derivative works.  AlliedModders LLC defines further
* exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
* or <http://www.sourcemod.net/license.php>.
*
* Version: $Id$
*/

#ifndef _INCLUDE_SOURCEMOD_CELLRECIPIENTFILTER_H_
#define _INCLUDE_SOURCEMOD_CELLRECIPIENTFILTER_H_

#include "extension.h"

class CellRecipientFilter: public IRecipientFilter
{
public:
	CellRecipientFilter();	
	CellRecipientFilter(int client);
	
public: //IRecipientFilter
	bool IsReliable() const;
	bool IsInitMessage() const;
	int GetRecipientCount() const;
	int GetRecipientIndex(int slot) const;
public:
	void Initialize(int *ptr, size_t count);
	void SetToReliable(bool isreliable);
	void SetToInit(bool isinitmsg);
	void Reset();
private:
	int m_Players[255];
	bool m_IsReliable;
	bool m_IsInitMessage;
	size_t m_Size;
};

extern size_t _FillInPlayers(int **pl_array, IRecipientFilter *pFilter);
extern size_t _OnlinePlayers(int **pl_array);

#endif //_INCLUDE_SOURCEMOD_CELLRECIPIENTFILTER_H_