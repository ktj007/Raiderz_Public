#include "stdafx.h"
#include "XDummyBot_Chat.h"
#include "XDummyMaster.h"
#include "XPost_GM.h"
#include "XDummyCollision.h"
#include "CSMsgCommandHelper.h"

XDummyBot_Chat::XDummyBot_Chat(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo) 
: XDummyBot(pAgent, pAgentInfo), m_nLastChatPost(0), m_vCenter(pAgent->GetCenter()), m_fRadius(1000.0f)
{
	USES_CONVERSION_EX;
}

XDummyBot_Chat::~XDummyBot_Chat()
{
	
}

void XDummyBot_Chat::OnRun(float fDelta)
{
	uint32 nNowTime = XGetNowTime();
	if (nNowTime - m_nLastChatPost <= 500) return;

	m_nLastChatPost = nNowTime;

	PostChat();
}

minet::MCommandResult XDummyBot_Chat::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	XDummyBot::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
		}
		break;
	default:
		{
		}
		break;
	}

	return CR_FALSE;
}

void XDummyBot_Chat::PostChat()
{
	CSMsgCommandHelper	m_msgHelper;

	MCommand* pNewCmd = NULL;

	int n = XMath::RandomNumber(0, 3);
	wstring strMsg = MakeRandomMsg();

	if (n == 0) pNewCmd = m_msgHelper.MakeNewClientCommandFieldReq(strMsg);
	else if (n == 1) pNewCmd = m_msgHelper.MakeNewClientCommandSayReq(strMsg);
	else if (n == 2) pNewCmd = m_msgHelper.MakeNewClientCommandGlobalReq(strMsg);
	else pNewCmd = m_msgHelper.MakeNewClientCommandPartyReq(strMsg);

	pNewCmd->SetReceiverUID(m_pAgent->GetClient()->GetServerUID());
	pNewCmd->SetSenderUID(m_pAgent->GetClient()->GetUID());

	XBirdPostCommand(m_pAgent->GetClient(), pNewCmd);
}

std::wstring XDummyBot_Chat::MakeRandomMsg()
{
	wstring strMsg;

	int nSize = XMath::RandomNumber(10, 20);

	for (int i = 0; i < nSize; i++)
	{
		int n = XMath::RandomNumber(0, 20);
		if (n == 0) strMsg += L"가";
		else if (n == 1) strMsg += L"나";
		else if (n == 2) strMsg += L"다";
		else if (n == 3) strMsg += L"라";
		else if (n == 4) strMsg += L"마";
		else if (n == 5) strMsg += L"바";
		else if (n == 6) strMsg += L"사";
		else if (n == 7) strMsg += L"아";
		else if (n == 8) strMsg += L"자";
		else if (n == 9) strMsg += L"차";
		else if (n == 10) strMsg += L"카";
		else if (n == 11) strMsg += L"타";
		else if (n == 12) strMsg += L"파";
		else if (n == 13) strMsg += L"하";
		else if (n == 14) strMsg += L"규";
		else if (n == 15) strMsg += L"뉴";
		else if (n == 16) strMsg += L"듀";
		else if (n == 17) strMsg += L"랄";
		else if (n == 18) strMsg += L"라";
		else if (n == 19) strMsg += L"합";
		else strMsg += L"ㅋ";
	}

	return strMsg;
}