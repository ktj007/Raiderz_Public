#pragma once

#include "XMotionTrigger_If.h"

class XMotionTrigger_Checker
{
private:
	vector<XMotionTrigger_If*>		m_vecIf;
public:
	XMotionTrigger_Checker();
	~XMotionTrigger_Checker();
	bool Check(XMotionTrigger_IfType nIfType, XActor* pActor);
};
