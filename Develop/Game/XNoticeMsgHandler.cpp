#include "stdafx.h"
#include "XNoticeMsgHandler.h"
#include "XController.h"
#include "XStrings.h"

XNoticeMsgHandler::XNoticeMsgHandler()
{
	// do nothing
}

XNoticeMsgHandler::~XNoticeMsgHandler()
{
	// do nothing
}

bool XNoticeMsgHandler::OnResponse(const minet::MCommand* pCmd)
{	
	if (gg.controller && !m_strMsg.empty())
	{
		wstring strMsg = XStringResMgr::GetInstance()->TransKeyStrWithParam(m_strMsg);

		wstringstream strStream;

		strStream	<< L"[Notice]: "
					<< strMsg;

		// 문자열 테이블 키 전달 시 변환 - 나레이션, 노티피 등에 이용
		gg.controller->OnChat(MT_NOTICE, strStream.str());
		global.ui->OnPresentationUpper(strStream.str());
	}

	return true;
}
