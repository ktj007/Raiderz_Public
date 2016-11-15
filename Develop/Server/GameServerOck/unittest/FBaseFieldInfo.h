#pragma once

#include "GTestWrapper.h"
#include "GFieldInfoMgr.h"
#include "SUnitTestUtil.h"

struct FBaseFieldInfo
{
	FBaseFieldInfo()
	{

	}
	virtual ~FBaseFieldInfo()
	{

	}

	GFieldInfo* AddFieldInfo(int nFieldID, float fMaxX=50000.0f, float fMaxY=50000.0f)
	{
		if (gmgr.pFieldInfoMgr->Find(nFieldID) != NULL)
		{
			return gmgr.pFieldInfoMgr->Find(nFieldID);
		}

		GFieldInfo* pNewFieldInfo = new GFieldInfo();
		pNewFieldInfo->m_nFieldID = nFieldID;
		pNewFieldInfo->SetFieldMaxXY_ForTest(fMaxX, fMaxY);

		MARKER_INFO marker;
		marker.nID = 1;
		marker.nFieldID = nFieldID;
		marker.vPoint = vec3(1,1,1);
		marker.vDir = vec3(1,0,0);

		pNewFieldInfo->InsertMarkerInfo(marker);

		gmgr.pFieldInfoMgr->Insert(pNewFieldInfo);

		return pNewFieldInfo;
	}

	GFieldInfo* AddFieldInfo()
	{
		int nNewID = SUnitTestUtil::NewID();
		return AddFieldInfo(nNewID);
	}

	FIELD_GROUP_INFO* AddFieldGroupInfo(int nFieldGroupID, int nFirstFieldID)
	{
		if (gmgr.pFieldInfoMgr->FindFieldGroup(nFieldGroupID) != NULL)
		{
			return gmgr.pFieldInfoMgr->FindFieldGroup(nFieldGroupID);
		}

		GFieldInfo* pFirstFieldInfo = gmgr.pFieldInfoMgr->Find(nFirstFieldID);
		if (NULL == pFirstFieldInfo)
		{
			pFirstFieldInfo = AddFieldInfo(nFirstFieldID);
			pFirstFieldInfo->m_bDynamic = true;
		}

		FIELD_GROUP_INFO* pNewFieldGroupInfo = new FIELD_GROUP_INFO();
		pNewFieldGroupInfo->nID = nFieldGroupID;
		pNewFieldGroupInfo->pStartFieldInfo = pFirstFieldInfo;
		pNewFieldGroupInfo->pStartMarkerInfo = pFirstFieldInfo->FindMarker(1);
		pNewFieldGroupInfo->vecFieldInfo.push_back(pFirstFieldInfo);

		gmgr.pFieldInfoMgr->InsertGroup(pNewFieldGroupInfo);

		return pNewFieldGroupInfo;
	}

	GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgrWrapper;
};