#include "stdafx.h"
#include "TRealSoundLoader.h"
#include "RSample.h"



// TRealSoundLoader
TRealSoundLoader::TRealSoundLoader()
{
}


// ~TRealSoundLoader
TRealSoundLoader::~TRealSoundLoader()
{
}


// LoadSamples
bool TRealSoundLoader::LoadSamples( const char* szFileName)
{
	MXml xml;
	if ( xml.LoadFile( szFileName) == false)
		return false;


	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "maiet").FirstChildElement().Element();

	float DEFAULT_VOLUME = 1.0f;
	int   DEFAULT_PRIORITY = 128;
	float DEFAULT_MINDISTANCE = 100.0f;
	float DEFAULT_MAXDISTANCE = 1000.0f;
	bool  DEFAULT_UNIQUE = false;

	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), "DEFAULT") == 0)
		{
			float fVolume;
			if ( _Attribute( &fVolume, pElement, "volume") == true)			DEFAULT_VOLUME = fVolume;

			int nPriority;
			if ( _Attribute( &nPriority, pElement, "priority") == true)		DEFAULT_PRIORITY = nPriority;

			float fMinDist;
			if ( _Attribute( &fMinDist, pElement, "mindist") == true)		DEFAULT_MINDISTANCE = fMinDist;

			float fMaxDist;
			if ( _Attribute( &fMaxDist, pElement, "maxdist") == true)		DEFAULT_MAXDISTANCE = fMaxDist;

			bool bUnique;
			if ( _Attribute( &bUnique, pElement, "unique") == true)			DEFAULT_UNIQUE = bUnique;
		}

		else if ( _stricmp( pElement->Value(), "SOUND") == 0)
		{
			RSampleInfo _info;

			// Name
			string strName;
			_Attribute( strName, pElement, "name", "");

			// File name
			_Attribute( _info.strFileName, pElement, "filename", "");

			// Type
			string strType;
			_Attribute( strType, pElement, "type", "generic");
			if ( _stricmp( strType.c_str(), "bgm") == 0)				_info.nType = RSD_SOUNDTYPE_BGM;
			else if ( _stricmp( strType.c_str(), "effect") == 0)		_info.nType = RSD_SOUNDTYPE_EFFECT;
			else														_info.nType = RSD_SOUNDTYPE_GENERIC;

			// Loop
			_Attribute( &_info.bLoop, pElement, "loop", false);

			// Volume
			_Attribute( &_info.fVolume, pElement, "volume", DEFAULT_VOLUME);

			// Priority
			_Attribute( &_info.nPriority, pElement, "priority", DEFAULT_PRIORITY);

			// Min/max distance
			_Attribute( &_info.fMinDist, pElement, "mindist", DEFAULT_MINDISTANCE);
			_Attribute( &_info.fMaxDist, pElement, "maxdist", DEFAULT_MAXDISTANCE);

			// Unique
			_Attribute( &_info.bUnique, pElement, "unique", DEFAULT_UNIQUE);


			// Create sample
			if ( strName.empty() == true  ||  _info.strFileName.empty() == true)		continue;

			RSample* pSample = new RSample( _info);
			if ( g_pMainApp->m_pSoundMgr.AddSample( strName.c_str(), pSample) == false)
			{
				mlog( "중복된 사운드 이름 : %s\n", _info.strFileName.c_str());
				delete pSample;
			}
		}
	}

	return true;
}




