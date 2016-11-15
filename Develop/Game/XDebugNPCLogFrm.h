#pragma once

#include "MTextFrame.h"

class XDebugNPCLogFrm : public MTextFrame
{
private:
	friend class Mint4Game;

	MButton*				m_pCommonBT;
	MButton*				m_pUseTalentBT;
	MButton*				m_pMoveBT;
	MButton*				m_pColtBT;

	vector<wstring>			m_VecNPCLog;

protected:
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	virtual void OnShow( bool bShow);

	void AddControls();
	void AddLog(const wchar_t* sLog);
	void PrintLog(FILE* fp);

	XDebugNPCLogFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:
	void ClearMessage();
	void SetNPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog);
	void SaveLog();

#define MINT_DEBUGNPCLOG "debugnpclog"
	virtual const char* GetClassName(void){ return MINT_DEBUGNPCLOG; }
};
