#pragma once

#include "Mint.h"
#include "MWidget.h"
#include "MFrame.h"
#include "MEdit.h"
#include "MButton.h"
#include "MLinePrinter.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;

namespace mint3
{

class MConsole;


/////////////////////////////////////////////////////////////////////////////////
#define CONSOLE_LINES_MAX 300

typedef void (MINT_CONSOLE_CALLBACK) (const char* szCommand);

/// 콘솔창
class MConsole : public MFrame
{
private:
	friend class Mint;

	MLinePrinter*			m_pOutputTextArea;
	MEdit*					m_pInputEdit;
	MButton*				m_pCloseBtn;
	bool					m_bCloseWhenBlankInput;		///< ""로 입력이 들어오면 자동으로 창닫기


	list<string>			m_Commands;
	MINT_CONSOLE_CALLBACK*	m_fnCallback;
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL) override;
	virtual void OnShow( bool bShow) override;
	virtual void OnRun( DWORD dwCurrTime) override;
public:
	MConsole(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MConsole(void);

	void Input(const char* szCommand);
	void OutputMessage(const char* sStr, bool bIsCmdMsg=false);
	void ClearMessage();
	void AddCommand(const char* szCommand);
	void SetCallback(MINT_CONSOLE_CALLBACK* pCallback);
	void SetCloseWhenBlankInput(bool b) { m_bCloseWhenBlankInput = b; }
	bool IsCloseWhenBlankInput() { return m_bCloseWhenBlankInput; }

#define MINT_CONSOLE	"Console"
	virtual const char* GetClassName(void){ return MINT_CONSOLE; }

};

} // namespace mint3