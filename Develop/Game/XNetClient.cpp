#include "stdafx.h"
#include "XReplay.h"
#include "XCmdHandlerGroup.h"
#include "XNetwork.h"
#include "XConnectionManager.h"
#include "XRequestCommandFilter.h"
#include "XWhoIsRequester.h"
#include "XControlNetCmdState.h"

bool XPostCommand(MCommand* pCmd)
{
	PFI_BLOCK_THISFUNC(24);
	return global.net->Post(pCmd); 
}

MCommand* XNewCmd(int nCmdID)
{
	MUID uidTarget= global.net->GetServerUID();
	return global.net->NewCommand(nCmdID, uidTarget);
}

XNetClient::XNetClient(const MNetClientDesc& desc) : MNetClient(desc), m_pRecorder(NULL), m_pControlNetCmdState(NULL)
{
	m_pCmdHandlerGroup = new XCmdHandlerGroup(this);
	m_pRequestCommandFilter = new XRequestCommandFilter();
	m_pWhoIsRequester = new XWhoIsRequester();

	m_pRequestCommandFilter->AddFilter(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST,		MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST);
	m_pRequestCommandFilter->AddFilter(MC_CHAR_REQUEST_INSERT_CHAR,				MC_CHAR_RESPONSE_INSERT_CHAR);
	m_pRequestCommandFilter->AddFilter(MC_CHAR_REQUEST_DELETE_CHAR,				MC_CHAR_RESPONSE_DELETE_CHAR);
	m_pRequestCommandFilter->AddFilter(MC_CHAR_REQUEST_SEL_MYCHAR,				MC_CHAR_RESPONSE_SEL_MYCHAR);
}

XNetClient::~XNetClient()
{
	SAFE_DELETE(m_pWhoIsRequester);
	SAFE_DELETE(m_pRequestCommandFilter);
	SAFE_DELETE(m_pCmdHandlerGroup);
}

void XNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	MNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	if (global.net)
	{
		global.net->GetConnectionManager()->OnReplyConnect();
	}
}

MCommandResult XNetClient::OnCommand(MCommand* pCommand)
{
	PFI_BLOCK_THISFUNC(21);

	if(m_pControlNetCmdState)
	{
		if(m_pControlNetCmdState->CheckStateUsableCommand(pCommand) == false)
			return CR_FALSE;
	}

	MCommandResult ret = MNetClient::OnCommand(pCommand);
	if (ret != CR_FALSE) return ret;

	return CR_FALSE;
}

void XNetClient::OnPrepareCommand( MCommand* pCommand )
{
	PFI_BLOCK_THISFUNC(22);

	m_pRequestCommandFilter->OnResponse(pCommand->GetID());

	LogCommand(pCommand);
}

bool XNetClient::Post( MCommand* pCommand )
{
	PFI_BLOCK_THISFUNC(23);

	if (m_pRequestCommandFilter && pCommand && m_pRequestCommandFilter->CheckRequestEnable(pCommand->GetID()) == false)
	{
		SAFE_DELETE(pCommand);
		return false;
	}

	if (m_pRecorder && m_pRecorder->IsRecording())
	{
		m_pRecorder->Record(pCommand);
	}

	return MNetClient::Post(pCommand);
}

void XNetClient::SendCommand( MCommand* pCommand )
{
	MNetClient::SendCommand(pCommand);

	LogCommand(pCommand);
}

void XNetClient::LogCommand( MCommand* pCommand )
{
	// madduck [5/8/2006]
#ifndef _PUBLISH
	if (global.ui->GetCommandLog())
	{
		global.ui->GetCommandLog()->AddCommand(pCommand);
	}
#endif
}

void XNetClient::SetControlNetCmdState( XControlNetCmdState* pCmdState )
{
	m_pControlNetCmdState = pCmdState;
}