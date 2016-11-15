#pragma once

#include "XMotionTrigger_Execute.h"


class XMotionTrigger_Executor
{
private:
	vector<XMotionTrigger_Execute*>		m_vecExecute;
public:
	XMotionTrigger_Executor();
	~XMotionTrigger_Executor();
	void Execute(XMotionTrigger_ThenType nThenType, const wstring& strParam1, const wstring& strParam2, XMotionLogic* pMotionLogic);
};

