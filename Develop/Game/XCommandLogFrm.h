#ifndef _XCOMMANDCONSOLE_H_
#define _XCOMMANDCONSOLE_H_

#include "Mint4Game.h"
#include "MTextFrame.h"
#include "MEdit.h"
#include "MButton.h"
#include "MPanel.h"
#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

using namespace std;
using namespace mint3;

#define MAX_COMMAND_LOG		300

class XCommandLogFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

	MPanel*					m_pView;
	MButton*				m_pSendButton;
	MButton*				m_pRecvButton;
//	MTextArea*				m_pIgnoreCmdTextArea;
	//MButton*				m_pCloseButton;

	set<int>				m_IgnoredCmdSet;
	bool					m_bLogging;
	MLogger					m_CmdLogger;

protected:
	void OnSize(int w, int h);
	//virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);

	XCommandLogFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	void AddControls();
public:
	void OutputMessage(const wchar_t* sStr, bool bIsCmdMsg=true);
	void ClearMessage();
	void AddCommand(MCommand* pCmd);

	void IgnoreCommand(int nID, bool bIgnore=true);	// 해당 ID의 커맨드 로그를 찍을지 여부 설정
	void IgnoreCommandAll(bool bIgnore=true);		// 모든 커맨드 로그를 찍을지 여부 설정
	void Start();									// 로그 재시작
	void Stop();									// 로그 스톱
	void GetIgnoredCmdListString(wchar_t* outStr, int nStrLen);

#define MINT_COMMANDLOGFRM		"commandlogfrm"
	virtual const char* GetClassName(void){ return MINT_COMMANDLOGFRM; }
};


#endif
