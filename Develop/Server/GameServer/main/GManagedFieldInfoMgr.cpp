#include "stdafx.h"
#include "GManagedFieldInfoMgr.h"
#include "GGlobal.h"
#include "GFieldInfoMgr.h"
#include "GServer.h"

bool GManagedSharedFieldInfo::ExistChannel( int nChannelID )
{
	set<int>::iterator itor = m_setChannelID.find(nChannelID);
	if (itor == m_setChannelID.end()) return false;

	return true;
}

void GManagedSharedFieldInfo::AddChannel( int nChannelID, MUID uidField )
{
	m_setChannelID.insert(nChannelID);
	_ChannelInfo channelInfo;
	channelInfo.nChannelID = nChannelID;
	channelInfo.uidField = uidField;

	m_vecChannelInfo.push_back(channelInfo);
}

void GManagedSharedFieldInfo::DelChannel(MUID uidField)
{
	vector<_ChannelInfo>::iterator itr = m_vecChannelInfo.begin();
	for (; itr != m_vecChannelInfo.end(); ++itr)
	{
		_ChannelInfo& refChannelInfo = (*itr);
		if (refChannelInfo.uidField != uidField)
			continue;

		m_setChannelID.erase(refChannelInfo.nChannelID);
		m_vecChannelInfo.erase(itr);
		break;
	}
}

MUID GManagedSharedFieldInfo::GetChannelFieldUID(int nChannelID)
{
	for (size_t i = 0; i < m_vecChannelInfo.size(); ++i)
	{
		if (nChannelID == m_vecChannelInfo[i].nChannelID)
			return m_vecChannelInfo[i].uidField;
	}

	return MUID::ZERO;
}

GManagedFieldInfoMgr::GManagedFieldInfoMgr()
{

}

GManagedFieldInfoMgr::~GManagedFieldInfoMgr()
{
	Clear();
}

void GManagedFieldInfoMgr::Insert( GManagedSharedFieldInfo* pInfo )
{
	insert(value_type(pInfo->m_nFieldID, pInfo));
}

void GManagedFieldInfoMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		delete (*itor).second;
	}
	clear();
}

GManagedSharedFieldInfo* GManagedFieldInfoMgr::Find( int nFieldID )
{
	iterator itor = find(nFieldID);
	if (itor == end()) return NULL;

	return (*itor).second;
}

GManagedSharedFieldInfo* GManagedFieldInfoMgr::FindByUID(MUID uidField)
{
	iterator itor = begin();
	for (itor; itor != end(); ++itor)
	{
		GManagedSharedFieldInfo* pInfo = (*itor).second;
		for (size_t i = 0; i < pInfo->m_vecChannelInfo.size(); ++i)
		{
			if (pInfo->m_vecChannelInfo[i].uidField == uidField)
				return pInfo;
		}
	}
	return NULL;
}

void GManagedFieldInfoMgr::AssignSharedField( TDMG_MANAGED_SHARED_FIELD* pFieldParam, int nCount )
{
	for (int i = 0; i < nCount; i++)
	{
		GManagedSharedFieldInfo* pManagedFieldInfo = new GManagedSharedFieldInfo();
		pManagedFieldInfo->m_nFieldID = pFieldParam[i].nFieldID;

		for (int channel_index = 0; channel_index < pFieldParam[i].nChannelCount; channel_index++)
		{
			int nChannelID = pFieldParam[i].nChannelID[channel_index];
			MUID uidField = pFieldParam[i].uidField[channel_index];

			pManagedFieldInfo->AddChannel(nChannelID, uidField);
		}

		Insert(pManagedFieldInfo);
	}

}

void GManagedFieldInfoMgr::AssignAllForStandAlone( GFieldInfoMgr* pFieldInfoMgr )
{
	for (GFieldInfoMgr::iterator itor = pFieldInfoMgr->begin(); itor != pFieldInfoMgr->end(); ++itor)
	{
		GFieldInfo* pFieldInfo = (*itor).second;

		if (pFieldInfo->m_bDynamic) continue;

		GManagedSharedFieldInfo* pManagedFieldInfo = new GManagedSharedFieldInfo();
		pManagedFieldInfo->m_nFieldID = pFieldInfo->m_nFieldID;

		for (int channel_id = 1; channel_id <= pFieldInfo->m_nDefaultChannelCount; channel_id++)
		{
			int nChannelID = channel_id;
			MUID uidField = gsys.pServer->NewUID();
			pManagedFieldInfo->AddChannel(nChannelID, uidField);
		}

		Insert(pManagedFieldInfo);
	}
}

bool GManagedFieldInfoMgr::Exist( int nFieldID, int nChannelID )
{
	GManagedSharedFieldInfo* pInfo = Find(nFieldID);
	if (pInfo == NULL) return false;

	if (pInfo->ExistChannel(nChannelID) == false)
	{
		return false;
	}

	return true;
}

bool GManagedFieldInfoMgr::Exist( int nFieldID )
{
	GManagedSharedFieldInfo* pInfo = Find(nFieldID);
	if (pInfo == NULL) return false;

	return true;
}

void GManagedFieldInfoMgr::AddChannel(MUID uidField, int nFieldID, int nChannelID)
{
	GManagedSharedFieldInfo* pInfo = Find(nFieldID);

	// 없는 필드 추가
	if (pInfo == NULL)
	{
		pInfo = new GManagedSharedFieldInfo;		
		pInfo->m_nFieldID = nFieldID;
		Insert(pInfo);
	}

	// 없는 채널 추가
	if (nChannelID != 0 && !pInfo->ExistChannel(nChannelID))
	{
		pInfo->AddChannel(nChannelID, uidField);
	}
}

void GManagedFieldInfoMgr::DelChannel(MUID uidField)
{
	GManagedSharedFieldInfo* pInfo = FindByUID(uidField);
	if (NULL == pInfo) return;

	pInfo->DelChannel(uidField);
}

MUID GManagedFieldInfoMgr::GetFieldUID(int nFieldID, int nChannelID)
{
	GManagedSharedFieldInfo* pInfo = Find(nFieldID);
	if (pInfo == NULL) return MUID::ZERO;

	return pInfo->GetChannelFieldUID(nChannelID);
}

int GManagedFieldInfoMgr::GetAllChannelCount()
{
	int nAllChannelCount = 0;
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		GManagedSharedFieldInfo* pInfo = (*itor).second;

		nAllChannelCount += pInfo->GetChannelCount();
	}
	return nAllChannelCount;
}
