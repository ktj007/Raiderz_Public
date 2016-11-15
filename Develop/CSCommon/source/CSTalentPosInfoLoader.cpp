#include "stdafx.h"
#include "CSTalentPosInfoLoader.h"
#include "CSTalentInfoDef.h"
#include "CSTalentInfoMgr.h"
#include "MMath.h"
#include "MDebug.h"
#include "MLocale.h"


bool CSTalentPosInfoLoader::Load( CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) 
	{
		mlog("Failed To Load %s\n", MLocale::ConvTCHARToAnsi(szFileName).c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_INFO))
		{
			Parse(pTalentInfoMgr, &xml, pElement);
		}
	}

	return true;
}

void CSTalentPosInfoLoader::Parse( CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pTalentInfoElement )
{
	int id = -1;
	CSTalentInfo* pNewInfo = NULL;

	_Attribute(&id, pTalentInfoElement, TALENT_XML_ATTR_ID);

	pNewInfo = pTalentInfoMgr->Find(id);

	if (pNewInfo == NULL)
	{
		mlog("talent_pos_info.xml : 존재하지 않는 탤런트 ID입니다. (TalentID = %d)\n", id );
		return;
	}


	int nLastTimeIndex = -1;
	MXmlElement* pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_MOVE_SEGMENT);

	for( pMoveSegElement; pMoveSegElement != NULL; pMoveSegElement=pMoveSegElement->NextSiblingElement() )
	{
		if (!_stricmp(pMoveSegElement->Value(), TALENT_XML_TAG_MOVE_SEGMENT))
		{
			int nTimeIndex = -1;
			if (_Attribute(&nTimeIndex, pMoveSegElement, TALENT_XML_ATTR_TIME_INDEX))
			{
				if (nTimeIndex == nLastTimeIndex+1)
				{
					nLastTimeIndex++;

					char szBuffer[512] = "";
					CSTalentInfo::_TALENT_MOVE_INFO	new_move_info;
					new_move_info.vPos = vec3::ZERO;
					new_move_info.fRot = 0.0f;

					MXmlElement* pPosElement = pMoveSegElement->FirstChildElement(TALENT_XML_TAG_POS);
					if (pPosElement)
					{
						if (_Contents(szBuffer, pPosElement))
						{
							sscanf_s(szBuffer,"%f %f %f", &new_move_info.vPos.x, &new_move_info.vPos.y, &new_move_info.vPos.z);
						}
					}
					MXmlElement* pDirElement = pMoveSegElement->FirstChildElement(TALENT_XML_TAG_ROT);
					if (pDirElement)
					{
						if (_Contents(szBuffer, pDirElement))
						{
							sscanf_s(szBuffer,"%f", &new_move_info.fRot);
							//new_move_info.fRot += MMath::PI;
						}
					}

					pNewInfo->m_MovingInfoList.push_back(new_move_info);
				}
				else
				{
					// TimeIndex가 잘못되었다.
					_ASSERT(0);
				}
			}
		}
	}


	pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_CASTING_MOVE_SEGMENT);
	ParseExtraMoveSegment(pMoveSegElement, pNewInfo->m_vCastingPos, pNewInfo->m_fCastingRot, MLocale::ConvAnsiToTCHAR(TALENT_XML_TAG_CASTING_MOVE_SEGMENT));
	pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_CASTINGLOOP_MOVE_SEGMENT);
	ParseExtraMoveSegment(pMoveSegElement, pNewInfo->m_vCastingLoopPos, pNewInfo->m_fCastingLoopRot, MLocale::ConvAnsiToTCHAR(TALENT_XML_TAG_CASTINGLOOP_MOVE_SEGMENT));

	pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_EXTRA_MOVE_SEGMENT);
	ParseExtraMoveSegment(pMoveSegElement, pNewInfo->m_vExtraPos, pNewInfo->m_fExtraRot, MLocale::ConvAnsiToTCHAR(TALENT_XML_TAG_EXTRA_MOVE_SEGMENT));
	pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_EXTRA2_MOVE_SEGMENT);
	ParseExtraMoveSegment(pMoveSegElement, pNewInfo->m_vExtra2Pos, pNewInfo->m_fExtra2Rot, MLocale::ConvAnsiToTCHAR(TALENT_XML_TAG_EXTRA2_MOVE_SEGMENT));
	pMoveSegElement = pTalentInfoElement->FirstChildElement(TALENT_XML_TAG_EXTRA3_MOVE_SEGMENT);
	ParseExtraMoveSegment(pMoveSegElement, pNewInfo->m_vExtra3Pos, pNewInfo->m_fExtra3Rot, MLocale::ConvAnsiToTCHAR(TALENT_XML_TAG_EXTRA3_MOVE_SEGMENT));
}

void CSTalentPosInfoLoader::ParseExtraMoveSegment( MXmlElement* pMoveSegElement, vec3& vExtraPos, float& fExtraRot, tstring strMoveSegElement )
{
	for( pMoveSegElement; pMoveSegElement != NULL; pMoveSegElement=pMoveSegElement->NextSiblingElement() )
	{
		if (!_tcsicmp(MLocale::ConvAnsiToTCHAR(pMoveSegElement->Value()).c_str(), strMoveSegElement.c_str()))
		{
			char szBuffer[512] = "";
			vec3 vPos = vec3::ZERO;
			float fRot = 0.0f;

			MXmlElement* pPosElement = pMoveSegElement->FirstChildElement(TALENT_XML_TAG_POS);
			if (pPosElement)
			{
				if (_Contents(szBuffer, pPosElement))
				{
					sscanf_s(szBuffer,"%f %f %f", &vPos.x, &vPos.y, &vPos.z);
					vExtraPos = vPos;
				}
			}
			MXmlElement* pDirElement = pMoveSegElement->FirstChildElement(TALENT_XML_TAG_ROT);
			if (pDirElement)
			{
				if (_Contents(szBuffer, pDirElement))
				{
					sscanf_s(szBuffer,"%f", &fRot);
					fExtraRot = fRot;
				}
			}
		}
	}
}