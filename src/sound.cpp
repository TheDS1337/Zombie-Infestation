#include "sound.h"

// This should probably be moved to the gamedata
#define SOUND_ENTRY_HASH_SEED 0x444F5441

uint32 GenerateSoundHash(char const *pSoundEntry)
{
	// First we need to convert the sound entry to lowercase before we calculate the hash
	int nSoundEntryLength = strlen(pSoundEntry);
	char *pSoundEntryLowerCase = (char *) stackalloc(nSoundEntryLength + 1);

	for( int nIndex = 0; nIndex < nSoundEntryLength; nIndex++ )
		pSoundEntryLowerCase[nIndex] = tolower(pSoundEntry[nIndex]);

	// Second we need to calculate the hash using the algorithm reconstructed from CS:GO
	const uint32 nMagicNumber = 0x5bd1e995;

	uint32 nSoundHash = SOUND_ENTRY_HASH_SEED ^ nSoundEntryLength;

	unsigned char *pData = (unsigned char *) pSoundEntryLowerCase;

	while( nSoundEntryLength >= 4 )
	{
		uint32 nLittleDWord = LittleDWord(*(uint32 *) pData);

		nLittleDWord *= nMagicNumber;
		nLittleDWord ^= nLittleDWord >> 24;
		nLittleDWord *= nMagicNumber;

		nSoundHash *= nMagicNumber;
		nSoundHash ^= nLittleDWord;

		pData += 4;
		nSoundEntryLength -= 4;
	}

	switch( nSoundEntryLength )
	{
	case 3: nSoundHash ^= pData[2] << 16;
	case 2: nSoundHash ^= pData[1] << 8;
	case 1: nSoundHash ^= pData[0];
		nSoundHash *= nMagicNumber;
	};

	nSoundHash ^= nSoundHash >> 13;
	nSoundHash *= nMagicNumber;
	nSoundHash ^= nSoundHash >> 15;

	return nSoundHash;
}

cell_t SoundReferenceToIndex(cell_t ref)
{
	if( ref == 0 || ref == -1 || ref == -2 )
	{
		return ref;
	}

	return gamehelpers->ReferenceToIndex(ref);
}