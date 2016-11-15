#include "StdAfx.h"
#include "XCommandLogFrm.h"
#include "MLookManager.h"
#include "XPath.h"

#define FRAME_CONSOLE_X			10
#define FRAME_CONSOLE_Y			10
#define FRAME_CONSOLE_WIDTH		400
#define FRAME_CONSOLE_HEIGHT	400

#define CONSOLE_VIEW_HEIGHT		200

XCommandLogFrm::XCommandLogFrm(const char* szName, MWidget* pParent, MListener* pListener)
: mint3::MTextFrame(szName, pParent, pListener), m_bLogging(true)
{
	AddControls();

	SetRect(MRECT(FRAME_CONSOLE_X, FRAME_CONSOLE_Y, FRAME_CONSOLE_WIDTH, FRAME_CONSOLE_HEIGHT));
	SetOpacity(0.8f);
	SetText("CommandLog");

	unsigned long int nOutputFlags = MLOG_PROG;
	if (XCONST::DEBUG_LOG_COMMAND) 
	{
		nOutputFlags |= MLOG_FILE;
		mlog("Start logging Command\n");
	}

	m_CmdLogger.Init(MLOG_LEVEL_DEBUG, nOutputFlags, "mlog_command.txt",NULL, MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str()).c_str());
}

void XCommandLogFrm::AddControls()
{
	m_pView = new MPanel(NULL, this, this);
	m_pView->SetRect(1, 1, 100, CONSOLE_VIEW_HEIGHT);

	m_pSendButton = new MButton(NULL, m_pView, this);
	//Mint* mint = global.ui->GetMint();
	//map<wstring, MButtonLook*>::iterator itor =
	//	mint->GetLookManager()->m_ButtonLookMap.find("look_DefaultCheckBox");
	//m_pSendButton->ChangeLook((MButtonLook*)(*itor).second);
	m_pSendButton->SetType(MBT_CHECKBTN);
	m_pSendButton->SetText("Send만 보기");
	m_pSendButton->SetRect(1, 1, 100, 15);

	m_pRecvButton = new MButton(NULL, m_pView, this);
	//map<wstring, MButtonLook*>::iterator itor =
	//	mint->GetLookManager()->m_ButtonLookMap.find("look_DefaultCheckBox");
	//m_pRecvButton->ChangeLook((MButtonLook*)(*itor).second);
	m_pRecvButton->SetType(MBT_CHECKBTN);
	m_pRecvButton->SetText("Recv만 보기");
	m_pRecvButton->SetRect(1, 20, 100, 15);

// 	m_pIgnoreCmdTextArea = new MTextArea(120, "IgnoreCmd", m_pView, this);
// 	m_pIgnoreCmdTextArea->SetEditable(false);
// 	m_pIgnoreCmdTextArea->SetBounds(1, 40, 200, 143);
}

void XCommandLogFrm::OnSize(int w, int h)
{
	MRECT r = GetClientRect();
	m_pView->SetRect( w+6-CONSOLE_VIEW_HEIGHT, 22, CONSOLE_VIEW_HEIGHT-9, r.h - 25);
}

//bool XCommandLogFrm::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
//{
	//if (pWidget == this)
	//{
	//	switch(nMsg)
	//	{
	//	case MEVENT_CLICK:
	//		{
	//			return true;
	//		}
	//	case MEVENT_SHOW:
	//		{
	//			return true;
	//		}
	//		break;
	//	}
	//} 
	//else 
//	if (pWidget == m_pListBox)
//	{
//		switch(nMsg)
//		{
//		case MEVENT_CLICK:
//			{
//				return true;
//			}
//// 		case MEVENT_LB_ITEM_DEL:
//// 			{
//// 				char* item = (char*)m_pListBox->glueGetSelUserData();
//// 				int nID = atoi(item);
//// 				IgnoreCommand(nID, true);
//
////			char text[512]="";
////			GetIgnoredCmdListString(text, 512);
//			//	m_pIgnoreCmdTextArea->SetText(text);
//
//// 				return true;
//// 			}
//		}
//	}
	//else if (pWidget == m_pSendButton)
	//{
	//	switch(nMsg)
	//	{
	//	case MEVENT_CLICK:
	//		{
	//			return true;
	//		}
	//	}
	//}

//	return false;
//}

void XCommandLogFrm::OutputMessage(const wchar_t* sStr, bool bIsCmdMsg)
{
	wchar_t szBuf[1024];
	if (bIsCmdMsg)
	{
		wcscpy_s(szBuf, L">");
		wcscat(szBuf, sStr);
	}
	else
	{
		wcscpy_s(szBuf, sStr);
	}

	AddString(MLocale::ConvUTF16ToAnsi(szBuf).c_str());
}

void XCommandLogFrm::ClearMessage()
{
	ResetContent();
}

void XCommandLogFrm::AddCommand(MCommand* pCmd)
{
	if (!m_bLogging) return;
	if (m_IgnoredCmdSet.find(pCmd->GetID()) != m_IgnoredCmdSet.end()) return;


	char temp[1024] = {0,};
	char szParam[256] = {0,};
	MUID uidReceiver, uidSender;

	//시간
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	sprintf_s(temp, "[%02d:%02d:%02d.%03d]:"
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		, stCurTime.wMilliseconds);


	int nCmdSize = pCmd->GetSize();

	//보내는지 받는지
	char trans[] = "Recv";
	uidReceiver = pCmd->GetReceiverUID();
	uidSender = pCmd->GetSenderUID();

	if (uidReceiver == uidSender)
		strcpy_s( trans, "Locl" );
	else if( uidReceiver != gvar.MyInfo.MyUID )
	{
		if (m_pRecvButton->GetCheck()) return;

		strcpy_s( trans, "Send" );
	}
	else
	{
		if (m_pSendButton->GetCheck()) return;
	}


	sprintf_s(temp, "%s [%s,%4d] ", temp, trans, nCmdSize);


	//패킷내용.
	sprintf_s(temp, "%s (%d)%s ", temp, pCmd->m_pCommandDesc->GetID(), pCmd->m_pCommandDesc->GetName());
	for(int i=0; i<pCmd->GetParameterCount(); i++)
	{
		if (i == 0)
		{
			strcat_s(temp, "- ");
		}

		pCmd->GetParameter(i)->GetString(szParam,sizeof(szParam));
		MUID uid = pCmd->GetSenderUID();
		sprintf_s(temp, "%s %s(%s)", temp, pCmd->GetParameter(i)->GetClassName(), szParam);

	}

	AddString(temp);

//	char tempID[1024] = {0,};
//	sprintf_s(tempID, "%d", pCmd->m_pCommandDesc->GetID());
////	m_pListBox->SetUserData(m_pListBox->GetCount()-1, tempID);
//
//	if (m_pListBox->GetCount() > MAX_COMMAND_LOG)
//	{
//		m_pListBox->DeleteString(0);
//	}
//
//// 	int t = m_pListBox->GetCount() - m_pListBox->GetShowItemCount();
//// 	if(t < 0) return;
//// 	m_pListBox->SetStartItem(t);

	m_CmdLogger.Output(temp);
	m_CmdLogger.Output("\n");
}

void XCommandLogFrm::Start()
{
	m_bLogging = true;
}

void XCommandLogFrm::Stop()
{
	m_bLogging = false;
}

void XCommandLogFrm::IgnoreCommand(int nID, bool bIgnore)
{
	if (bIgnore == true)
	{
		if ( MCommandDescMap::GetInstance()->GetCommandDescByID(nID))
			m_IgnoredCmdSet.insert(nID);
	}
	else
	{
		m_IgnoredCmdSet.erase(nID);
	}
}

void XCommandLogFrm::IgnoreCommandAll(bool bIgnore)
{
	if (bIgnore == true)
	{
		for (int i = MC_COMM_REQUEST_ACCOUNT_CHAR_LIST; i < MC_TIME_SYNCH+1; i++)
		{
			if ( MCommandDescMap::GetInstance()->GetCommandDescByID(i))
			{
				m_IgnoredCmdSet.insert(i);
			}
		}
	}
	else
	{
		m_IgnoredCmdSet.clear();
	}
}

void XCommandLogFrm::GetIgnoredCmdListString(wchar_t* outStr, int nStrLen)
{
	int j = 0;
	wcscpy_s(outStr, 16, L"igored cmd: ");
	for (set<int>::iterator itor = m_IgnoredCmdSet.begin(); itor != m_IgnoredCmdSet.end(); ++itor)
	{
		int i = swprintf_s(outStr+j, nStrLen-j, L"%s%d ", outStr+j, (*itor));
		j += i;
		if (j >= nStrLen-i)
		{
			return;
		}
	}
}
