#include "stdafx.h"
#include "MConsole.h"

namespace mint3 {

#define FRAME_CONSOLE_X 10
#define FRAME_CONSOLE_Y 10
#define FRAME_CONSOLE_WIDTH 400
#define FRAME_CONSOLE_HEIGHT 400

void GetSharedString(string* pShared, list<string>* pList)
{
	string shared;
	for(list<string>::iterator it=pList->begin(); it!=pList->end(); it++){
		if(it==pList->begin()) shared = (*it);
		else{
			string compared = (*it);
			int nLen = (int)shared.length();
			for(int i=0; i<nLen; i++){
				if(toupper(shared[i])!=toupper(compared[i])){
					shared[i] = NULL;
					break;
				}
			}
		}
	}

	*pShared = shared;
}

///////////////////////////////////////////////////////////////////////////////////////////
// MConsole ///////////////////////////////////////////////////////////////////////////////
MConsole::MConsole(const char* szName, MWidget* pParent, MListener* pListener)
: MFrame(szName, pParent, pListener), m_fnCallback(NULL), m_bCloseWhenBlankInput(true)
{
	m_pOutputTextArea = new MLinePrinter( NULL, this, this);
	m_pOutputTextArea->SetRect( 3, 21, m_Rect.w - 6, m_Rect.h - 52);
	m_pOutputTextArea->SetAnchor(MAnchorStyle(true, true, true, true));
	m_pOutputTextArea->ShowBorder( false);

	m_pInputEdit = new MEdit( NULL, this, this);
	m_pInputEdit->EnableHistory( true);
	m_pInputEdit->SetRect( 3, m_Rect.h - 28, m_Rect.w - 6, 25);
	m_pInputEdit->SetAnchor( MAnchorStyle(true, true, false, true));

	m_pCloseBtn = new MButton( NULL, this, this);
	m_pCloseBtn->SetRect( m_Rect.w - 25, 3, 20, 11);
	m_pCloseBtn->SetAnchor( MAnchorStyle( false, true, true, false));
	m_pCloseBtn->SetText( "-");

	SetText( szName);
	SetResizable( true);
}

MConsole::~MConsole()
{
	if (m_pOutputTextArea != NULL) { delete m_pOutputTextArea; m_pOutputTextArea = NULL;	}
	if (m_pInputEdit != NULL) { delete m_pInputEdit; m_pInputEdit = NULL; }
	if ( m_pCloseBtn != NULL) { delete m_pCloseBtn; m_pCloseBtn = NULL; }
}

void MConsole::OnShow( bool bShow)
{
	if ( bShow == true)
	{
		BringToTop();

		m_pInputEdit->SetText( "");
		m_pInputEdit->SetFocus();
	}

	MFrame::OnShow( bShow);
}

bool MConsole::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == m_pCloseBtn  &&  nMsg == MEVENT_CLICK)
	{
		Show( false);
		return true;
	}

	else if (pWidget == m_pInputEdit  &&  nMsg == MEVENT_OK)
	{
		string strText = m_pInputEdit->GetText();
		Input( strText.c_str());
		m_pInputEdit->PushHistory();
		m_pInputEdit->Clear();
		return true;
	}

	return false;
}

void MConsole::Input(const char* szCommand)
{
// 	if (m_bCloseWhenBlankInput)
// 	{
// 		if (strlen(szCommand) <= 0)
// 		{
// 			Show(false);
// 			return;
// 		}
// 	}

	OutputMessage(szCommand, true);

	if (m_fnCallback)
	{
		m_fnCallback(szCommand);
	}
}

void MConsole::OutputMessage(const char* sStr, bool bIsCmdMsg)
{
	if (strlen(sStr) >= 1022) return;

	char szBuf[1024];
	if (bIsCmdMsg)
	{
		strcpy(szBuf, ">");
		strcat(szBuf, sStr);
	}
	else
		{
		strcpy(szBuf, sStr);
	}

	m_pOutputTextArea->AddString(szBuf);
}

void MConsole::ClearMessage()
{
	m_pOutputTextArea->ResetContent();
}


void MConsole::AddCommand(const char* szCommand)
{
	m_Commands.push_back(szCommand);
}

void MConsole::SetCallback(MINT_CONSOLE_CALLBACK* pCallback)
{
	m_fnCallback = pCallback;
}

bool MConsole::OnEvent( MEvent* pEvent, MListener* pListener )
{
// 	if ( MFrame::OnEvent( pEvent, pListener) == true)		return true;
// 
// 	return false;

	return MFrame::OnEvent( pEvent, pListener);
}


void MConsole::OnRun( DWORD dwCurrTime )
{
	MFrame::OnRun(dwCurrTime);

// 	if (m_pInputEdit->IsFocus() == false && IsVisibled())
// 	{
// 		Show(false);
// 	}
}

} // namespace mint3