#include "stdafx.h"
#include "XCutSceneObjectControllerBGM.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerBGM::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	std::tstring strFileName;
	bool bLoop;
	float fVolume;
	int nKeyUID;

	MXmlElement* pBgmKeyElement = pControllerElement->FirstChildElement("bgmkey");
	while (pBgmKeyElement)
	{
		_Attribute(fTime, pBgmKeyElement, "time");
		_Attribute(nKeyUID, pBgmKeyElement, "uid");

		MXmlElement* pSoundElement = pBgmKeyElement->FirstChildElement("SOUND");
		_Attribute(strFileName, pSoundElement, "filename");
		_Attribute(bLoop, pSoundElement, "loop");
		_Attribute(fVolume, pSoundElement, "volume");

		AddKey(fTime, strFileName, bLoop, fVolume, &nKeyUID);
		pBgmKeyElement = pBgmKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerBGM::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pBgmElement = new MXmlElement("bgm");
	_SetAttribute(pBgmElement, "uid", m_nUID);

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pBgmKey = new MXmlElement("bgmkey");
		_SetAttribute(pBgmKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pBgmKey, "uid", m_vKeyInfos[i].nUid);
		pBgmElement->LinkEndChild(pBgmKey);

		// sound element
		MXmlElement* pSoundElement = new MXmlElement("SOUND");
		char buf[256];
		sprintf_s( buf, "_bgm_%d_%d", m_nUID, m_vKeyInfos[i].nUid);
		_SetAttribute(pSoundElement, "name", buf);
		_SetAttribute(pSoundElement, "filename", MLocale::ConvTCHARToAnsi(m_vKeyInfos[i].strFileName.c_str()).c_str());
		_SetAttribute(pSoundElement, "BG", "false");
		_SetAttribute(pSoundElement, "loop", m_vKeyInfos[i].bLoop);
		_SetAttribute(pSoundElement, "volume", m_vKeyInfos[i].fVolume);
		pBgmKey->LinkEndChild(pSoundElement);
	}

	pObjectElement->LinkEndChild(pBgmElement);
}

void XCutSceneObjectControllerBGM::GetKeyInfo(int nIdx, float& _rOutTime, std::tstring& _rFileName, bool& _rbLoop, float& _rfVolume)
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime = rKeyInfo.fTime;
	_rFileName = rKeyInfo.strFileName;
	_rbLoop = rKeyInfo.bLoop;
	_rfVolume = rKeyInfo.fVolume;
}

void XCutSceneObjectControllerBGM::SetKeyInfo(int nIdx, float _fTime, const std::tstring& _rFileName, bool _bLoop, float _fVolume)
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.strFileName = _rFileName;
	rKeyInfo.bLoop = _bLoop;
	rKeyInfo.fVolume = _fVolume;
}

void XCutSceneObjectControllerBGM::AddKey( float _fTime, const std::tstring& _rFileName, bool _bLoop, float _fVolume, int* _pCustomUID )
{
	int newUID = -1;
	if(_pCustomUID)
	{
		newUID = *_pCustomUID;
	}
	else
	{
		std::set<int> uids;
		for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
		{
			_ASSERT( uids.find(itr->nUid) == uids.end() );
			uids.insert(itr->nUid);
		}

		if (uids.empty())
		{
			newUID = 0;
		}
		else
		{
			int i = 0;
			while(true)
			{
				if (uids.find(i) == uids.end())
				{
					newUID = i;
					break;
				}
				++i;
			}
		}
	}

	_ASSERT( newUID != -1);
	m_vKeyInfos.push_back( SKeyInfo(_fTime, _rFileName, _bLoop, _fVolume, newUID) );
}

void XCutSceneObjectControllerBGM::DeleteKey(int nIdx)
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerBGM::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if (pCurrent->bUpdated)
		return;

	pCurrent->bUpdated = true;

	TCHAR buf[256];
	_stprintf_s( buf, _T("_bgm_%d_%d"), m_nUID, pCurrent->nUid);
	m_pOwner->GetEntity()->PlaySound(buf);
}

void XCutSceneObjectControllerBGM::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (pProcKey->bUpdated)
		return;

	pProcKey->bUpdated = true;

	TCHAR buf[256];
	_stprintf_s( buf, _T("_bgm_%d_%d"), m_nUID, pProcKey->nUid);
	m_pOwner->GetEntity()->PlaySound(buf);
}

void XCutSceneObjectControllerBGM::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
	{
		itr->bUpdated = false;
	}
}
