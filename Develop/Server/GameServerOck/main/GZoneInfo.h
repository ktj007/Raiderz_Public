#pragma once

class GZoneInfo
{
public:
	vec3		m_vMapSize;
	vec3		m_vTerrainSize;

	GZoneInfo() : m_vMapSize(vec3::ZERO), m_vTerrainSize(vec3::ZERO) {}
};

class GZoneInfoFileLoader
{
private:
public:
	bool Load( const wchar_t* szZoneFileName, GZoneInfo& outZoneInfo );
};


#define SERVERZONE_XML_TAG_ZONE				"ZONE"
#define SERVERZONE_XML_TAG_MAP_SIZE			"MAP_SIZE"
#define SERVERZONE_XML_TAG_TERRAIN_SIZE		"TERRAIN_SIZE"
