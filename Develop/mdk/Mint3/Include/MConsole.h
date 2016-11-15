#pragma once

#include "Mint.h"
#include "MWidget.h"
#include "MFrame.h"
#include "MEdit.h"
//#include "MListBox.h"
#include "MTextArea.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;

namespace mint3 {

class MConsole;

/////////////////////////////////////////////////////////////////////////////////
#define CONSOLE_LINES_MAX 300

typedef void (MINT_CONSOLE_CALLBACK) (const char* szCommand);

/// ÄÜ¼ÖÃ¢
class MConsole : public MFrame
{
private:
	friend class Mint;

	//MListBox*				m_pListBox;
	MTextArea*				m_pOutputTextArea;

	MEdit*					m_pInputEdit;
	list<string>			m_Commands;
	MINT_CONSOLE_CALLBACK*	m_fnCallback;
protected:
	virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	virtual void OnShow(void);

	MConsole(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
public:
	virtual ~MConsole(void);

	void Input(const char* szCommand);
	void OutputMessage(const char* sStr, bool bIsCmdMsg=false);
	void ClearMessage();
	void SetBounds(MRECT& r);
	void SetBounds(int x, int y, int w, int h);
	void AddCommand(const char* szCommand);
	void SetCallback(MINT_CONSOLE_CALLBACK* pCallback);

#define MINT_CONSOLE	"Console"
	virtual const char* GetClassName(void){ return MINT_CONSOLE; }

};

} // namespace mint3