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
	: MFrame(szName, pParent, pListener), m_fnCallback(NULL)
{
	m_pOutputTextArea = new MTextArea(200, NULL, this, this);
	m_pOutputTextArea->SetBounds(MRECT(3, 22, 100, 100));
	m_pOutputTextArea->SetAnchor(MAnchorStyle(true, true, true, true));
	m_pOutputTextArea->SetEditable(false);
	m_pOutputTextArea->SetWordWrap(false);

	m_pInputEdit = new MEdit(NULL, this, this);
	m_pInputEdit->m_bSupportHistory = true;
	m_pInputEdit->SetAnchor(MAnchorStyle(true, true, false, true));

	SetBounds(MRECT(FRAME_CONSOLE_X, FRAME_CONSOLE_Y, FRAME_CONSOLE_WIDTH, FRAME_CONSOLE_HEIGHT));

	SetText("Console");
}

MConsole::~MConsole()
{
	ReleaseExclusive();

	if (m_pOutputTextArea != NULL) { delete m_pOutputTextArea; m_pOutputTextArea = NULL;	}
	if (m_pInputEdit != NULL) { delete m_pInputEdit; m_pInputEdit = NULL; }
}

void MConsole::OnShow(void)
{
	MFrame::OnShow();

	SetZOrder(MZ_TOP);

	m_pInputEdit->SetText("");
	m_pInputEdit->SetFocus();
}

void MConsole::OnDraw(MDrawContext* pDC)
{
	MFrame::OnDraw(pDC);

}
bool MConsole::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if (pWidget == this)
	{
		switch(nMsg)
		{
		case MEVENT_CLICK:
			{
				m_pInputEdit->SetFocus();
				return true;
			}
		case MEVENT_SHOW:
			{
				m_pInputEdit->SetFocus();
				return true;
			}
			break;
		}
	}
	else if (pWidget == m_pInputEdit)
	{
		switch(nMsg)
		{
		case MEVENT_EDIT_ENTER:
			{
				Input(m_pInputEdit->GetText());
				m_pInputEdit->SetText("");
				return true;
			}
			break;
		case MEVENT_EDIT_CHAR:
			{
				if (this->IsVisible() && strlen(m_pInputEdit->GetText()) <= 0)
				{
					
				}
			}
			break;
		}
	}
	else if (pWidget == m_pOutputTextArea)
	{
		switch(nMsg)
		{
		case MEVENT_CLICK:
			{
				m_pInputEdit->SetFocus();
				return true;
			}
		}
	}

	return false;
}

void MConsole::Input(const char* szCommand)
{
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

	m_pOutputTextArea->AddText(szBuf);
}

void MConsole::ClearMessage()
{
	m_pOutputTextArea->Clear();
}

void MConsole::SetBounds(MRECT& r)
{
	SetBounds(r.x, r.y, r.w, r.h);
}

void MConsole::SetBounds(int x, int y, int w, int h)
{
	MFrame::SetBounds(x, y, w, h);
	
	m_pOutputTextArea->SetBounds(MRECT(3, 22, w-6, h - 50));
	m_pInputEdit->SetBounds(MRECT(3, h - 24, w - 6, 20));
	
}

void MConsole::AddCommand(const char* szCommand)
{
	m_Commands.push_back(szCommand);
}

void MConsole::SetCallback(MINT_CONSOLE_CALLBACK* pCallback)
{
	m_fnCallback = pCallback;
}

} // namespace mint3