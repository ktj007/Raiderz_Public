#include "stdafx.h"
#include "CSMsgCommandFacade.h"
#include "CSMsgHandler.h"

CSMsgCommandFacade::CSMsgCommandFacade()
{
	// do nothing
}

CSMsgCommandFacade::~CSMsgCommandFacade()
{
	for(handler_iterator it = m_mapHandler.begin(); it != m_mapHandler.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_mapHandler.clear();
}

CSMsgHandler* CSMsgCommandFacade::SetHandler(CSMsgType eType, CSMsgHandler* pNewHandler)
{
	if (pNewHandler == NULL)	return NULL;

	CSMsgHandler* pOldHandler = NULL;

	handler_iterator it = m_mapHandler.find(eType);
	if (it != m_mapHandler.end())
	{
		pOldHandler = it->second;
	}

	m_mapHandler[eType] = pNewHandler;
	return pOldHandler;
}

bool CSMsgCommandFacade::RequestMessage(const minet::MCommand* pCmd)
{
	if (pCmd == NULL)	return false;

	// 타입 확인 및 담당 클래스 확인
	int nType;
	if (!pCmd->GetParameter(&nType, 0, minet::MPT_INT))	return false;
	if (!IsExistHandler((CSMsgType)nType))				return false;

	// 처리
	return m_mapHandler[(CSMsgType)nType]->Request(pCmd);
}

bool CSMsgCommandFacade::ResponseMessage(const minet::MCommand* pCmd)
{
	if (pCmd == NULL)	return false;

	// 타입 확인 및 담당 클래스 확인
	int nType;
	if (!pCmd->GetParameter(&nType, 0, minet::MPT_INT))	return false;
	if (!IsExistHandler((CSMsgType)nType))				return false;

	// 처리
	return m_mapHandler[(CSMsgType)nType]->Response(pCmd);
}

bool CSMsgCommandFacade::IsExistHandler(CSMsgType eType)
{
	return m_mapHandler.find(eType) != m_mapHandler.end();
}
