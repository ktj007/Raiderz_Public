#include "StdAfx.h"
#include "ZLoginFieldGuide.h"
#include "ZGameServerObjectManager.h"
#include "ZDynamicFieldMaster.h"
#include "ZDynamicField.h"
#include "ZFieldManager.h"
#include "ZFieldInfoManager.h"
#include "ZFieldFinder.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"


ZLoginFieldGuide::ZLoginFieldGuide(CID nCID, PLAYER_FIELD_INFO& refPlayerFieldInfo)
: m_nCID(nCID)
, m_PlayerFieldInfo(refPlayerFieldInfo)
{
}

ZLoginFieldGuide::~ZLoginFieldGuide()
{
}

bool ZLoginFieldGuide::Search(ZGameServerObject*& poutDestServer, ZField*& poutDestField)
{
	/// 게임서버가 하나라도 있는지 체크
	if (!CheckExistGameServer())
		return false;

	/// 1. 처음 로그인이면 입장하는 필드를 찾는다.
	if (SearchFirstLoginField(poutDestServer, poutDestField))
		return true;

	/// 2. 동적필드를 찾는다.
	if (SearchDynamicField(poutDestServer, poutDestField))
		return true;

	/// 3. 공유필드를 찾는다.
	if (SearchSharedField(poutDestServer, poutDestField))
		return true;

	/// 4. 예외상황 발생, 입장 시킬 필드를 찾는다.	
	if (SearchExceptionField(poutDestServer, poutDestField))
		return true;

	return false;
}

bool ZLoginFieldGuide::CheckExistGameServer()
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount())
	{
		return false;
	}
	return true;
}

bool ZLoginFieldGuide::SearchFirstLoginField(ZGameServerObject*& poutDestServer, ZField*& poutDestField)
{
	/// SharedFieldID 가 0보다 크면 유효한 공유필드 입장으로 처리돼야 합니다.
	if (m_PlayerFieldInfo.nSharedFieldID > 0)
		return false;

	ZSharedFieldMaster* pFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(FIRST_ENTER_FIELD_ID);
	if (NULL == pFieldMaster)
	{
		mlog3("Error! ZLoginFieldGuide::SearchFirstLoginField(), FIRST_ENTER_FIELD_ID is Not Exist! (CID=%d)\n", m_nCID);
		return false;
	}

	poutDestField = pFieldMaster->GetEnterableChannel();
	if (NULL == poutDestField)
		return true;	///< 재시도 응답

	poutDestServer = gmgr.pGameServerObjectManager->FindGameServer(poutDestField->GetGameServerID());
	if (NULL == poutDestServer)
		return false;

	return true;
}

bool ZLoginFieldGuide::SearchDynamicField(ZGameServerObject*& poutDestServer, ZField*& poutDestField)
{
	if (m_PlayerFieldInfo.uidDynamicFieldGroup.IsInvalid())
		return false;

	ZDynamicFieldMaster* pFieldMaster = gmgr.pFieldManager->FindDynamicFieldMaster(m_PlayerFieldInfo.uidDynamicFieldGroup);
	if (NULL == pFieldMaster)
		return false;

	poutDestField = pFieldMaster->FindField(m_PlayerFieldInfo.nDynamicFieldID);
	if (NULL == poutDestField)
		return false;
	
	poutDestServer = gmgr.pGameServerObjectManager->FindGameServer(poutDestField->GetGameServerID());
	if (NULL == poutDestServer)
	{
		mlog3("Error! ZLoginFieldGuide::SearchDynamicField(), DestGameServer is NULL! (GameServerID=%d, CID=%d, FieldGroupUID=%I64u)\n"
			, poutDestField->GetGameServerID(), m_nCID, m_PlayerFieldInfo.uidDynamicFieldGroup.Value);
		return false;
	}

	return true;
}

bool ZLoginFieldGuide::SearchSharedField(ZGameServerObject*& poutDestServer, ZField*& poutDestField)
{
	/// 필드 정보 검사
	ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(m_PlayerFieldInfo.nSharedFieldID);
	if (NULL == pFieldInfo)
	{
		mlog3("Error! ZLoginFieldGuide::SearchSharedField(), FieldInfo is NULL! (CID=%d, FieldID=%d)\n"
			, m_nCID, m_PlayerFieldInfo.nSharedFieldID);
		return false;
	}

	/// 입장 가능한 공유 필드 찾는다.
	ZSharedField* poutSharedField = NULL;

	ZFieldFinder fieldFinder;
	if (false == fieldFinder.FindEnterableSharedField(poutSharedField, m_PlayerFieldInfo.nSharedFieldID))
	{
		mlog3("Error! ZLoginFieldGuide::SearchSharedField(), Failed Find Enterable SharedField(id=%d)\n", m_PlayerFieldInfo.nSharedFieldID);
		return false;
	}

	if (NULL == poutSharedField)
		return true;	///< 재시도 응답

	poutDestField = poutSharedField;

	poutDestServer = gmgr.pGameServerObjectManager->FindGameServer(poutDestField->GetGameServerID());
	if (NULL == poutDestServer)
	{
		mlog3("Error! ZLoginFieldGuide::SearchSharedField(), DestGameServer is NULL! (CID=%d, FieldID=%d, ChannelID=%d)\n"
			, poutDestField->GetGameServerID(), m_nCID, poutDestField->GetID(), poutSharedField->GetChannelID());
		return false;
	}

	return true;
}

bool ZLoginFieldGuide::SearchExceptionField(ZGameServerObject*& poutDestServer, ZField*& poutDestField)
{
	// 일단 시작 필드로 입장 시킨다.

	ZSharedFieldMaster* pFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(FIRST_ENTER_FIELD_ID);
	if (NULL == pFieldMaster)
		return false;

	poutDestField = pFieldMaster->GetEnterableChannel();
	if (NULL == poutDestField)
		return true;	///< 재시도 응답

	poutDestServer = gmgr.pGameServerObjectManager->FindGameServer(poutDestField->GetGameServerID());
	if (NULL == poutDestServer)
		return false;

	return true;
}
