#include "stdafx.h"
#include "GMsgHandler.h"
#include "CSStringFilter.h"
#include "GGlobal.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"


GMsgHandler::GMsgHandler()
{
	// do nothing
}

GMsgHandler::~GMsgHandler()
{
	// do nothing
}

bool GMsgHandler::OnPreRequest(const minet::MCommand* pCmd)
{
	// 길이 제한 - 잘라낸다.
	if (CHATTING_LIMIT_SIZE < m_strMsg.length())
	{
		m_strMsg = m_strMsg.substr(0, CHATTING_LIMIT_SIZE);
	}

	// 욕설필터 처리는 클라이언트에서 합니다. aibeast

	// 시스템 문자 교체
	m_strMsg = CSStringFilter::RemoveGameSystemCharacter(m_strMsg);

	return true;
}
