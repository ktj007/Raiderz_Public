#include "StdAfx.h"
#include "ZChannelBeholder.h"
#include "MTime.h"
#include "ZFieldManager.h"
#include "ZSharedFIeldMaster.h"
#include "ZSharedFIeld.h"
#include "ZSmartFieldAllocator.h"
#include "ZDef.h"
#include "ZSystem.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerObject.h"
#include "ZConst.h"


ZChannelBeholder::ZChannelBeholder()
{
	m_pBeholderRglt = new MRegulator(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
}

ZChannelBeholder::~ZChannelBeholder()
{
	SAFE_DELETE(m_pBeholderRglt);
}

void ZChannelBeholder::Update(float fDelta)
{
	if (!m_pBeholderRglt->IsReady(fDelta)) return;

	vector<MUID> vecChannelToDispose;

	ZFieldManager::ZSharedFieldMasterMap& refSharedFieldMasterMap = gmgr.pFieldManager->GetSharedFieldMasterMap();
	ZFieldManager::ZSharedFieldMasterMap::iterator itr = refSharedFieldMasterMap.begin();
	for (; itr != refSharedFieldMasterMap.end(); ++itr)
	{
		ZSharedFieldMaster* pSharedFieldMaster = (*itr).second;
		if (IsCrowd(pSharedFieldMaster))
		{
			ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);
			fieldAllocator.AllocNewChannel(pSharedFieldMaster);
		}
		else
		{
			CollectExpiredSilentChannel(pSharedFieldMaster, vecChannelToDispose);
		}
	}

	if (!vecChannelToDispose.empty())
		DestroySilentChannels(vecChannelToDispose);
}

bool ZChannelBeholder::IsCrowd(ZSharedFieldMaster* pSharedFieldMaster)
{
	ZSharedFieldMaster::ZChannelVector& refChannelVec = pSharedFieldMaster->GetChannelVec();
	for (int i = 0; i < (int)refChannelVec.size(); ++i)
	{
		ZSharedField* pSharedFIeld = refChannelVec[i];
		if (false == IsCrowd(pSharedFIeld))
			return false;
	}
	return true;
}

bool ZChannelBeholder::IsCrowd(ZSharedField* pSharedField)
{
	int nCurPlayerCount = pSharedField->GetPlayerCount();
	int nLimitPlayerCount = pSharedField->GetPlayerLimit();
	float fRatio = (float)nCurPlayerCount / (float)nLimitPlayerCount * 100.0f;
	if (fRatio < ZConst::RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL)
		return false;

	return true;
}

void ZChannelBeholder::CollectExpiredSilentChannel(ZSharedFieldMaster* pSharedFieldMaster, vector<MUID>& rvecChannelToDispose)
{
	ZSharedFieldMaster::ZChannelVector& refChannelVec = pSharedFieldMaster->GetChannelVec();
	for (int i = 0; i < (int)refChannelVec.size(); ++i)
	{
		ZSharedField* pSharedField = refChannelVec[i];

		/// Default 채널이면 무시.
		if (pSharedField->IsDefaultChannel()) continue;

		MUID uidChannel = pSharedField->GetUID();
		if (IsSilent(pSharedField))
		{
			if (false == m_mapSilentChannelExpire.IsExist(uidChannel))
				m_mapSilentChannelExpire.Add(uidChannel);

			if (m_mapSilentChannelExpire.CheckExpired(uidChannel))
			{
				m_mapSilentChannelExpire.Del(uidChannel);
				rvecChannelToDispose.push_back(uidChannel);
			}
		}
		else
		{
			if (m_mapSilentChannelExpire.IsExist(uidChannel))
				m_mapSilentChannelExpire.Del(uidChannel);
		}
	}
}

bool ZChannelBeholder::IsSilent(ZSharedField* pSharedField)
{
	int nCurPlayerCount = pSharedField->GetPlayerCount();
	if (0 < nCurPlayerCount)
		return false;

	return true;
}

void ZChannelBeholder::DestroySilentChannels(vector<MUID>& rvecChannelToDispose)
{
	for (int i = 0; i < (int)rvecChannelToDispose.size(); ++i)
	{
		MUID uidChannelToDispose = rvecChannelToDispose[i];
		ZSharedField* pChannel = gmgr.pFieldManager->FindSharedField(uidChannelToDispose);
		if (NULL == pChannel) continue;

		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(pChannel->GetGameServerID());
		if (NULL != pGameServer)
			pGameServer->GetFieldMap().DelSharedField(pChannel->GetUID());

		gmgr.pFieldManager->DestroySharedField(pChannel->GetUID(), true);
	}
}

/// ZChannelBeholder
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// ZChannelBeholder::ZSilentChannelExpireMap

void ZChannelBeholder::ZSilentChannelExpireMap::Add(MUID uidChannel)
{
	VALID(uidChannel.IsValid());

	uint32 nAddTime = gsys.pSystem->GetNowTime();
	this->insert(map<MUID, uint32>::value_type(uidChannel, nAddTime));
}

void ZChannelBeholder::ZSilentChannelExpireMap::Del(MUID uidChannel)
{
	this->erase(uidChannel);
}

bool ZChannelBeholder::ZSilentChannelExpireMap::IsExist(MUID uidChannel)
{
	iterator itr = this->find(uidChannel);
	if (itr == this->end()) return false;

	return true;
}

bool ZChannelBeholder::ZSilentChannelExpireMap::CheckExpired(MUID uidChannel)
{
	iterator itr = this->find(uidChannel);
	if (itr == this->end()) return false;

	uint32 nNowTime = gsys.pSystem->GetNowTime();
	if (nNowTime - (*itr).second < ZConst::TICK_EXPIRE_SILENT_CHANNEL)
		return false;

	return true;
}