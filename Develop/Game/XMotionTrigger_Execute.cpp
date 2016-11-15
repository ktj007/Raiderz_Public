#include "stdafx.h"
#include "XMotionTrigger_Execute.h"
#include "XMotionLogic.h"

void XMotionTrigger_ExecuteHold::Execute( XMotionLogic* pMotionLogic, const wstring& strParam1, const wstring& strParam2 )
{
	pMotionLogic->HoldSequence();
}

void XMotionTrigger_ExecuteChangeMotion::Execute( XMotionLogic* pMotionLogic, const wstring& strParam1, const wstring& strParam2 )
{
	pMotionLogic->ChangeMotion(strParam1.c_str());
}