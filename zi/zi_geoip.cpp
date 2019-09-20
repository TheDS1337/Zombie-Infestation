#include "extension.h"
#include "zi_geoip.h"

#define GEOIP_DATABASE_COUNTRY "addons/ZombieInfestation/geoip/GeoLite2-Country.mmdb"
#define GEOIP_DATABASE_CITY "addons/ZombieInfestation/geoip/GeoLite2-City.mmdb"

MMDB_s *ZIGeoIP::m_pDataBase = nullptr;

ZIGeoIP g_GeoIP;

void ZIGeoIP::Load()
{
	m_pDataBase = new MMDB_s();

	char path[256];
	g_pSM->BuildPath(Path_Game, path, sizeof(path), GEOIP_DATABASE_CITY);

	int status = MMDB_open(path, MMDB_MODE_MMAP, m_pDataBase);

	if( status != MMDB_SUCCESS )
	{
		g_pSM->BuildPath(Path_Game, path, sizeof(path), GEOIP_DATABASE_COUNTRY);

		status = MMDB_open(path, MMDB_MODE_MMAP, m_pDataBase);
	}

	if( status != MMDB_SUCCESS )
	{
		CONSOLE_DEBUGGER("Cannot open database file!");
		return;
	}
}

void ZIGeoIP::Free()
{
	MMDB_close(m_pDataBase);
	delete m_pDataBase;
}

bool ZIGeoIP::GetCountry(const char *IP, char *output, int outputLen)
{
	const char *path[] = { "country", "names", "en", nullptr };

	int length = -1;
	const char *country = LookupString(IP, path, &length);

	if( !country || length < 1 )
	{
		return false;
	}

	ke::SafeStrcpy(output, outputLen, country);

	return true;
}

bool ZIGeoIP::GetCity(const char *IP, char *output, int outputLen)
{
	const char *path[] = { "city", "names", "en", nullptr };

	int length = -1;
	const char *city = LookupString(IP, path, &length);

	if( !city || length < 1 )
	{
		return false;
	}

	ke::SafeStrcpy(output, outputLen, city);

	return true;
}

MMDB_entry_data_s *ZIGeoIP::LookupByIP(const char *IP, const char **path)
{
	int gai_error = 0, mmdb_error = 0;
	MMDB_lookup_result_s lookup = MMDB_lookup_string(m_pDataBase, IP, &gai_error, &mmdb_error);

	if( gai_error != 0 || mmdb_error != MMDB_SUCCESS || !lookup.found_entry )
	{
		return nullptr;
	}

	static MMDB_entry_data_s entry_data;
	MMDB_aget_value(&lookup.entry, &entry_data, path);

	if( !entry_data.has_data )
	{
		size_t i = 0;

		// Dirty fall back to default language ("en") in case provided user's language is not localized.

		// Searh "names" position.
		while( path[i] && strcmp(path[i++], "names") );

		// No localized entry or we use already default language.
		if( !path[i] || !strcmp(path[i], "en") )
		{
			return false;
		}

		// Overwrite user's language.
		path[i] = "en";

		// Try again.
		gai_error = mmdb_error = 0;
		MMDB_aget_value(&lookup.entry, &entry_data, path);

		if( !entry_data.has_data )
		{
			return nullptr;
		}
	}

	return &entry_data;
}

const char *ZIGeoIP::LookupString(const char *IP, const char **path, int *length)
{
	static char buffer[256]; // This should be large enough for long name in UTF-8.
	MMDB_entry_data_s *data = LookupByIP(IP, path);

	if( !data )
	{
		if( length )
		{
			*length = 0;
		}

		return nullptr;
	}

	// Let's avoid a crash in case we go over the buffer size.
	size_t maxLength = min((size_t) data->data_size, sizeof(buffer) - 1);

	// Strings from database are not null terminated.
	memcpy(buffer, data->utf8_string, maxLength);
	buffer[maxLength] = '\0';

	if( length )
	{
		*length = maxLength;
	}

	return buffer;
}

double ZIGeoIP::LookupDouble(const char *IP, const char **path)
{
	MMDB_entry_data_s *data = LookupByIP(IP, path);

	if( !data )
	{
		return 0.0f;
	}

	return data->double_value;
}