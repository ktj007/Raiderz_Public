#include "stdafx.h"
#include "GFieldMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GAppServerFacade.h"
#include "GCommandCenter.h"
#include "GFieldMgr.h"

GFieldMsgHandler::GFieldMsgHandler()
{
	// do nothing
}

GFieldMsgHandler::~GFieldMsgHandler()
{
	// do nothing
}

bool GFieldMsgHandler::OnRequest(const MCommand* pCmd)
{
	if (m_strMsg.empty())	return false;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidCommandSender);
	VALID_RET(pPlayer != NULL, false);

	int nFieldID = pPlayer->GetFieldID();
	if (nFieldID == INVALID_ID)	return false;

	CID nCID = pPlayer->GetCID();
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandFieldReq(m_strMsg, nCID, nFieldID);
	gsys.pAppServerFacade->Route(pNewCmd);

	return true;
}

bool GFieldMsgHandler::OnResponse(const MCommand* pCmd)
{
	// 전달 커맨드 생성
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandFieldRes(m_strMsg, strSenderName);	
	
	// 필드 접속 수신자 추가
	int nFieldID = m_msgHelper.GetReceiverID(pCmd);

	vector<GField*> vecField = gmgr.pFieldMgr->GetFieldList(nFieldID);	
	for each(GField* pField in vecField)
	{		
		GEntityMgr::ENTITY_UID_MAP& mapEntity = pField->GetEntityMap();

		for_each(mapEntity.begin(), mapEntity.end(), 
			tr1::bind((void(MCommand::*)(const MUID&))&MCommand::AddReceiver, pNewCmd, tr1::bind(&GEntityMgr::ENTITY_UID_MAP::value_type::first, tr1::placeholders::_1)));
	}

	// 전달
	gsys.pCommandCenter->PostCommand(pNewCmd);
	
	return true;
}
