#include "stdafx.h"
#include "VSoundInfoLoader.h"
#include "VSoundInfoMgr.h"

VSoundInfoLoader::VSoundInfoLoader()
{
	m_nSoundCount = 0;
}

VSoundInfoLoader::~VSoundInfoLoader()
{

}

void VSoundInfoLoader::Init()
{
	if ( m_xmlSound.LoadFile(V_FILENAME_SOUNDLIST) == false)
	{
		m_nSoundCount = -1;
		return;
	}


	MXmlHandle docHandle = m_xmlSound.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "maiet").FirstChildElement().Element();

	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), "SOUND") == 0)
		{
			++m_nSoundCount;
		}
	}
}

void VSoundInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	pValidatorInfoMgr->m_pSoundInfoMgr->LoadingInfo();

	//////////////////////////////////////////////////////////////////////////
	if(m_nSoundCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_SOUNDLIST, &(pValidatorInfoMgr->m_pSoundInfoMgr->m_ErrorMsg));
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlSound.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "maiet").FirstChildElement().Element();

	float DEFAULT_VOLUME = 1.0f;
	int   DEFAULT_PRIORITY = 128;
	float DEFAULT_MINDISTANCE = 100.0f;
	float DEFAULT_MAXDISTANCE = 1000.0f;
	float DEFAULT_DIFFUSE = 0.7f;
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

			float fDiffuse;
			if ( _Attribute( &fDiffuse, pElement, "diffuse") == true)		DEFAULT_DIFFUSE = fDiffuse;

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

			map< string, RSampleInfo>::iterator itFind = pValidatorInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.find(strName);
			if(itFind != pValidatorInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.end())
			{
				//mlog( "중복된 사운드 이름 : %s\n", _info.strFileName.c_str());
				continue;
			}

			pValidatorInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.insert(map<string, RSampleInfo>::value_type(strName, _info));

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nSoundInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nSoundCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Sound Info Loading... {0}%", nSoundInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Sound Info Complete."));
}
