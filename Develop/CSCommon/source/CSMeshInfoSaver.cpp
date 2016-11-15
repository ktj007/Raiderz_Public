#include "stdafx.h"
#include "CSMeshInfoSaver.h"
#include "MXml.h"
#include "CSStrings.h"
#include "MLocale.h"

#define MESHINFO_XML_TAG_HITINFO				"HitInfo"
#define MESHINFO_XML_TAG_COLINFO				"ColInfo"
#define MESHINFO_XML_TAG_ANIINFO				"AniInfo"

#define MESHINFO_XML_TAG_GROUP					"Group"
#define MESHINFO_XML_TAG_CAPSULE				"Capsule"
#define MESHINFO_XML_TAG_ANIMATION				"Animation"

#define MESHINFO_XML_ATTR_COLINDEX				"ColIndex"
#define MESHINFO_XML_ATTR_NAME					"name"
#define MESHINFO_XML_ATTR_TIME					"time"
#define MESHINFO_XML_ATTR_ACTIVE_TIME_BEGIN		"ActiveTimeBegin"
#define MESHINFO_XML_ATTR_ACTIVE_TIME_END		"ActiveTimeEnd"
#define MESHINFO_XML_ATTR_LINKED_BPART			"LinkedBPart"

bool CSMeshInfoSaver::SaveMeshInfo( CSMeshInfoMgr* pMeshInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	
	MXmlElement* pMaietElement = new MXmlElement(MESHINFO_XML_TAG_MAIET);

	for(map<tstring, CSMeshInfo>::iterator it = pMeshInfoMgr->m_mapMeshInfo.begin(); it != pMeshInfoMgr->m_mapMeshInfo.end(); it++)
	{
		MXmlElement* pMeshElement = new MXmlElement(MESHINFO_XML_TAG_MESHINFO);

		tstring strSaveMeshName = CSStrings::StringToLower((*it).second.m_strMeshName);
		_SetAttribute(pMeshElement, MESHINFO_XML_ATTR_MESHNAME, MLocale::ConvTCHARToAnsi(strSaveMeshName.c_str()));
		
		SaveMeshHitInfo(&((*it).second), pMeshElement);
		SaveMeshColInfo(&((*it).second), pMeshElement);
		SaveMeshAniInfo(&((*it).second), pMeshElement);

		pMaietElement->LinkEndChild(pMeshElement);
	}

	root->LinkEndChild(pMaietElement);

	return xml.SaveFile(MLocale::ConvTCHARToAnsi(szFileName).c_str());
}

void CSMeshInfoSaver::SaveMeshColInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement )
{
	MXmlElement* pColElement = new MXmlElement(MESHINFO_XML_TAG_COLINFO);

	int nCapsuleIndex = 0;
	for(vector<CSColCapsule>::iterator itCapsule = pMeshInfo->m_vecColCapsules.begin(); itCapsule != pMeshInfo->m_vecColCapsules.end(); itCapsule++)
	{
		// Ä¸½¶
		MXmlElement* pCapsule = new MXmlElement(MESHINFO_XML_TAG_CAPSULE);

		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_ID, nCapsuleIndex);
		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_TOP_BONENAME, MLocale::ConvTCHARToAnsi((*itCapsule).m_strBoneTop.c_str()));
		_SetAttribute(pCapsule, MESHINFO_XML_ATTR_BOTTOM_BONENAME, MLocale::ConvTCHARToAnsi((*itCapsule).m_strBoneBottom.c_str()));


		char buf[200] = {0, };
		MCapsule c = (*itCapsule).m_Capsule;
		
		sprintf_s(buf, "%f %f %f %f %f %f %f", c.bottom.x, c.bottom.y, c.bottom.z, c.top.x, c.top.y, c.top.z, c.radius);
		_SetContents(pCapsule, buf);

		pColElement->LinkEndChild(pCapsule);

		nCapsuleIndex++;	
	}

	// ¹Ú½º
	int nBoxIndex = 0;
	for(vector<CSColBox>::iterator itBox = pMeshInfo->m_vecColBox.begin(); itBox != pMeshInfo->m_vecColBox.end(); ++itBox)
	{
		MXmlElement* pBox = new MXmlElement(MESHINFO_XML_TAG_BOX);

		_SetAttribute(pBox, MESHINFO_XML_ATTR_ID, nCapsuleIndex);

		char buf[2048] = {0, };

		sprintf_s(buf, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
			itBox->m_matWorld._11, itBox->m_matWorld._12, itBox->m_matWorld._13,
			itBox->m_matWorld._21, itBox->m_matWorld._22, itBox->m_matWorld._23,
			itBox->m_matWorld._31, itBox->m_matWorld._32, itBox->m_matWorld._33,
			itBox->m_matWorld._41, itBox->m_matWorld._42, itBox->m_matWorld._43,
			itBox->vmax.x, itBox->vmax.y, itBox->vmax.z, 
			itBox->vmin.x, itBox->vmin.y, itBox->vmin.z);

		_SetContents(pBox, buf);

		pColElement->LinkEndChild(pBox);

		nBoxIndex++;	

	}

	pElement->LinkEndChild(pColElement);
}

void CSMeshInfoSaver::SaveMeshHitInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement )
{
	MXmlElement* pHitElement = new MXmlElement(MESHINFO_XML_TAG_HITINFO);
	
	for(map<int, CSHitGroup>::iterator itGroup = pMeshInfo->m_mapHitGroupInfo.begin(); itGroup != pMeshInfo->m_mapHitGroupInfo.end(); itGroup++)
	{
		// ±×·ì ID
		MXmlElement * pPartsGroup = new MXmlElement(MESHINFO_XML_TAG_GROUP);
		_SetAttribute(pPartsGroup, MESHINFO_XML_ATTR_ID, itGroup->first);

		int nCapsuleIndex = 0;
		for(vector<CSHitCapsule>::iterator itCapsule = itGroup->second.m_vecHitCapsules.begin();
			itCapsule != itGroup->second.m_vecHitCapsules.end(); itCapsule++)
		{
			// Ä¸½¶ ID
			MXmlElement* pCapsule = new MXmlElement(MESHINFO_XML_TAG_CAPSULE);

			_SetAttribute(pCapsule, MESHINFO_XML_ATTR_ID, nCapsuleIndex);
			_SetAttribute(pCapsule, MESHINFO_XML_ATTR_COLINDEX, (*itCapsule).m_nHitEffectCapsuleID);
			_SetAttribute(pCapsule, MESHINFO_XML_ATTR_LINKED_BPART, (*itCapsule).nLinkedBPart);

			if ((*itCapsule).GetActiveTimeBegin() > 0.001f)
				_SetAttribute(pCapsule, MESHINFO_XML_ATTR_ACTIVE_TIME_BEGIN, (*itCapsule).GetActiveTimeBegin());

			if ((*itCapsule).GetActiveTimeEnd() > 0.001f)
				_SetAttribute(pCapsule, MESHINFO_XML_ATTR_ACTIVE_TIME_END, (*itCapsule).GetActiveTimeEnd());

			
			char buf[200] = {0, };
			MCapsule c = (*itCapsule).m_Capsule;

			sprintf_s(buf, "%f %f %f %f %f %f %f", c.bottom.x, c.bottom.y, c.bottom.z, c.top.x, c.top.y, c.top.z, c.radius);
			_SetContents(pCapsule, buf);

			pPartsGroup->LinkEndChild(pCapsule);

			nCapsuleIndex++;
		}

		if(itGroup->second.m_vecHitCapsules.size() > 0)
		{
			pHitElement->LinkEndChild(pPartsGroup);
		}
		else
		{
			SAFE_DELETE(pPartsGroup);
		}
	}

	pElement->LinkEndChild(pHitElement);
}

void CSMeshInfoSaver::SaveMeshAniInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement )
{
	MXmlElement* pAniElement = new MXmlElement(MESHINFO_XML_TAG_ANIINFO);

	for(vector<CSAniInfo>::iterator itAni = pMeshInfo->m_vecAniInfo.begin(); itAni != pMeshInfo->m_vecAniInfo.end(); itAni++)
	{
		if((*itAni).m_fAnimationTime == 0.0f)
			continue;

		MXmlElement * pAnimation = new MXmlElement(MESHINFO_XML_TAG_ANIMATION);

		_SetAttribute(pAnimation, MESHINFO_XML_ATTR_NAME, MLocale::ConvTCHARToAnsi((*itAni).m_strAnimationName.c_str()));
		_SetAttribute(pAnimation, MESHINFO_XML_ATTR_TIME, (*itAni).m_fAnimationTime);

		pAniElement->LinkEndChild(pAnimation);
	}

	pElement->LinkEndChild(pAniElement);

}

void CSMeshInfoSaver::SaveMeshBPartsInfo( CSMeshInfo* pMeshInfo, MXmlElement* pElement )
{
	MXmlElement* pBPartsElement = NULL;

	for (int i = 0; i < 4; ++i)
	{
		MXmlElement* pPartsNodeElement = NULL;

		if (!pMeshInfo->m_strBPartsStartSound[i].empty())
		{
			if (NULL == pPartsNodeElement)
			{
				pPartsNodeElement = new MXmlElement("bpart");
				_SetAttribute(pPartsNodeElement, "id", i+1);
			}

			_SetAttribute(pPartsNodeElement, "StartSound", MLocale::ConvTCHARToAnsi(pMeshInfo->m_strBPartsStartSound[i].c_str()));
		}

		if (!pMeshInfo->m_strBPartsCollisionSound[i].empty())
		{
			if (NULL == pPartsNodeElement)
			{
				pPartsNodeElement = new MXmlElement("bpart");
				_SetAttribute(pPartsNodeElement, "id", i+1);
			}

			_SetAttribute(pPartsNodeElement, "CollisionSound", MLocale::ConvTCHARToAnsi(pMeshInfo->m_strBPartsCollisionSound[i].c_str()));
		}

		if (pPartsNodeElement)
		{
			if (NULL == pBPartsElement)
			{
				pBPartsElement = new MXmlElement("BPartsInfo");
			}

			pBPartsElement->LinkEndChild(pPartsNodeElement);
		}
	}

	if (pBPartsElement)
	{
		pElement->LinkEndChild(pBPartsElement);
	}
}
