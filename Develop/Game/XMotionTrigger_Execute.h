#pragma once

#include "XMotionTrigger.h"

class XMotionLogic;

class XMotionTrigger_Execute
{
public:
	virtual void Execute(XMotionLogic* pMotionLogic, const wstring& strParam1, const wstring& strParam2) {}
};

class XMotionTrigger_ExecuteChangeMotion : public XMotionTrigger_Execute
{
public:
	virtual void Execute(XMotionLogic* pMotionLogic, const wstring& strParam1, const wstring& strParam2);
};

class XMotionTrigger_ExecuteHold : public XMotionTrigger_Execute
{
public:
	virtual void Execute(XMotionLogic* pMotionLogic, const wstring& strParam1, const wstring& strParam2);
};

