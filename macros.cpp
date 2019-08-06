#include "macros.h"

/*
* Basically this sends the event to a single client instead of a population
*/
void FireEventToClient(int clientIndex, IGameEvent *event)
{
	IClient *client = g_pExtension->m_pSDKTools->GetIServer()->GetClient(clientIndex - 1);

	if( client )
	{
		IGameEventListener2 *eventListener = (IGameEventListener2 *) ((char *) client - sizeof(void *));

		if( !eventListener )
		{
			CONSOLE_DEBUGGER("Unable to locate the event listener within the client %d", client);
			return;
		}

		eventListener->FireGameEvent(event);
	}
	else
	{
		CONSOLE_DEBUGGER("Failed to find client %d", client);
	}
}

string_t AllocPooledString(const char *str)
{
	// This is admittedly a giant hack, but it's a relatively safe method for
	// inserting a string into the game's string pool that isn't likely to break.
	//
	// We find the first valid ent (should always be worldspawn), save off it's
	// current targetname string_t, set it to our string to insert via SetKeyValue,
	// read back the new targetname value, restore the old value, and return the new one.

	CBaseEntity *entity = ((IServerUnknown *) g_pExtension->m_pServerTools->FirstEntity())->GetBaseEntity();
	auto *datamap = gamehelpers->GetDataMap(entity);
	assert(datamap);

	static int offset = -1;
	if( offset == -1 )
	{
		sm_datatable_info_t info;
		bool found = gamehelpers->FindDataMapInfo(datamap, "m_iName", &info);
		assert(found);
		offset = info.actual_offset;
	}

	string_t *pProp = (string_t *) ((intp) entity + offset);
	string_t backup = *pProp;
	g_pExtension->m_pServerTools->SetKeyValue(entity, "targetname", str);
	string_t newString = *pProp;
	*pProp = backup;

	return newString;
}

int FindCharInString(const char *str, char c, bool reverse)
{
	int len = strlen(str);

	if( !reverse )
	{
		for( int i = 0; i < len; i++ )
		{
			if( str[i] == c )
				return i;
		}
	}
	else
	{
		for( int i = len - 1; i >= 0; i-- )
		{
			if( str[i] == c )
				return i;
		}
	}

	return -1;
}

unsigned int ReplaceStringsInString(char *subject, size_t maxlength, const char *search, const char *replace, bool caseSensitive)
{
	size_t searchLen = strlen(search);
	size_t replaceLen = strlen(replace);

	char *newptr, *ptr = subject;
	unsigned int total = 0;
	while( (newptr = ReplaceStringsInStringEx(ptr, maxlength, search, searchLen, replace, replaceLen, caseSensitive)) != NULL )
	{
		total++;
		maxlength -= newptr - ptr;
		ptr = newptr;
		if( *ptr == '\0' )
		{
			break;
		}
	}

	return total;
}

char *ReplaceStringsInStringEx(char *subject, size_t maxLen, const char *search, size_t searchLen, const char *replace, size_t replaceLen, bool caseSensitive)
{
	char *ptr = subject;
	size_t browsed = 0;
	size_t textLen = strlen(subject);

	/* It's not possible to search or replace */
	if( searchLen > textLen )
	{
		return NULL;
	}

	/* Handle the case of one byte replacement.
	* It's only valid in one case.
	*/
	if( maxLen == 1 )
	{
		/* If the search matches and the replace length is 0,
		* we can just terminate the string and be done.
		*/
		if( (caseSensitive ? strcmp(subject, search) : strcasecmp(subject, search)) == 0 && replaceLen == 0 )
		{
			*subject = '\0';
			return subject;
		}
		else
		{
			return NULL;
		}
	}

	/* Subtract one off the maxlength so we can include the null terminator */
	maxLen--;

	while( *ptr != '\0' && (browsed <= textLen - searchLen) )
	{
		/* See if we get a comparison */
		if( (caseSensitive ? strncmp(ptr, search, searchLen) : strncasecmp(ptr, search, searchLen)) == 0 )
		{
			if( replaceLen > searchLen )
			{
				/* First, see if we have enough space to do this operation */
				if( maxLen - textLen < replaceLen - searchLen )
				{
					/* First, see if the replacement length goes out of bounds. */
					if( browsed + replaceLen >= maxLen )
					{
						/* EXAMPLE CASE:
						* Subject: AABBBCCC
						* Buffer : 12 bytes
						* Search : BBB
						* Replace: DDDDDDDDDD
						* OUTPUT : AADDDDDDDDD
						* POSITION:           ^
						*/
						/* If it does, we'll just bound the length and do a strcpy. */
						replaceLen = maxLen - browsed;
						/* Note, we add one to the final result for the null terminator */
						ke::SafeStrcpy(ptr, replaceLen + 1, replace);
					}
					else
					{
						/* EXAMPLE CASE:
						* Subject: AABBBCCC
						* Buffer : 12 bytes
						* Search : BBB
						* Replace: DDDDDDD
						* OUTPUT : AADDDDDDDCC
						* POSITION:         ^
						*/
						/* We're going to have some bytes left over... */
						size_t origBytesToCopy = (textLen - (browsed + searchLen)) + 1;
						size_t realBytesToCopy = (maxLen - (browsed + replaceLen)) + 1;
						char *moveFrom = ptr + searchLen + (origBytesToCopy - realBytesToCopy);
						char *moveTo = ptr + replaceLen;

						/* First, move our old data out of the way. */
						memmove(moveTo, moveFrom, realBytesToCopy);

						/* Now, do our replacement. */
						memcpy(ptr, replace, replaceLen);
					}
				}
				else
				{
					/* EXAMPLE CASE:
					* Subject: AABBBCCC
					* Buffer : 12 bytes
					* Search : BBB
					* Replace: DDDD
					* OUTPUT : AADDDDCCC
					* POSITION:      ^
					*/
					/* Yes, we have enough space.  Do a normal move operation. */
					char *moveFrom = ptr + searchLen;
					char *moveTo = ptr + replaceLen;

					/* First move our old data out of the way. */
					size_t bytesToCopy = (textLen - (browsed + searchLen)) + 1;
					memmove(moveTo, moveFrom, bytesToCopy);

					/* Now do our replacement. */
					memcpy(ptr, replace, replaceLen);
				}
			}
			else if( replaceLen < searchLen )
			{
				/* EXAMPLE CASE:
				* Subject: AABBBCCC
				* Buffer : 12 bytes
				* Search : BBB
				* Replace: D
				* OUTPUT : AADCCC
				* POSITION:   ^
				*/
				/* If the replacement does not grow the string length, we do not
				* need to do any fancy checking at all.  Yay!
				*/
				char *moveFrom = ptr + searchLen;		/* Start after the search pointer */
				char *moveTo = ptr + replaceLen;		/* Copy to where the replacement ends */

														/* Copy our replacement in, if any */
				if( replaceLen )
				{
					memcpy(ptr, replace, replaceLen);
				}

				/* Figure out how many bytes to move down, including null terminator */
				size_t bytesToCopy = (textLen - (browsed + searchLen)) + 1;

				/* Move the rest of the string down */
				memmove(moveTo, moveFrom, bytesToCopy);
			}
			else
			{
				/* EXAMPLE CASE:
				* Subject: AABBBCCC
				* Buffer : 12 bytes
				* Search : BBB
				* Replace: DDD
				* OUTPUT : AADDDCCC
				* POSITION:     ^
				*/
				/* We don't have to move anything around, just do a straight copy */
				memcpy(ptr, replace, replaceLen);
			}

			return ptr + replaceLen;
		}
		ptr++;
		browsed++;
	}

	return NULL;
}

int AddCommas(int iNum, char *szOutput, int iLen)
{
	char szTmp[15];
	int iOutputPos = 0, iNumPos = 0, iNumLen = 0;

	if( iNum < 0 )
	{
		szOutput[iOutputPos++] = '-';
		iNum *= -1;
	}

	iNumLen = ke::SafeSprintf(szTmp, sizeof(szTmp), "%d", iNum);

	if( iNumLen <= 3 )
	{
		iOutputPos += ke::SafeStrcpy(&szOutput[iOutputPos], iLen, szTmp);
	}
	else
	{
		while( (iNumPos < iNumLen) && (iOutputPos < iLen) )
		{
			szOutput[iOutputPos++] = szTmp[iNumPos++];

			if( (iNumLen - iNumPos) && !((iNumLen - iNumPos) % 3) )
				szOutput[iOutputPos++] = ',';
		}

		szOutput[iOutputPos] = '\0';
	}

	return iOutputPos;
}

int AddCommas(float num, char *output, int len)
{
	char temp[32];
	ke::SafeSprintf(temp, sizeof(temp), "%.2f", num);

	int fraction = FindCharInString(temp, '.', true);
	int written = AddCommas((int) num, output, len);

	return ke::SafeStrcpy(&output[written], len - written, &temp[fraction]);
}