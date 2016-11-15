#ifndef _XLUALOG_H_
#define _XLUALOG_H_

#include "Mint4Game.h"
#include "MTextFrame.h"
#include "MFrame.h"
#include "MListBox.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

using namespace std;
using namespace mint3;

#define MAX_LUA_LOG		300

class XLuaLogFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

	MLogger					m_CmdLogger;

protected:
	XLuaLogFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:
	void ClearMessage();
	void AddLog(const char* sLog);

#define MINT_LUALOGFRM		"lualogfrm"
	virtual const char* GetClassName(void){ return MINT_LUALOGFRM; }

};


#endif
