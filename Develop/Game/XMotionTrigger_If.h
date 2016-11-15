#pragma once

#include "XMotionTrigger.h"

class XActor;

class XMotionTrigger_If
{
public:
	virtual bool Check(XActor* pActor) { return false; }
};

class XMotionTrigger_IfTrue : public XMotionTrigger_If
{
public:
	virtual bool Check(XActor* pActor) { return true; }
};

class XMotionTrigger_IfDead : public XMotionTrigger_If
{
public:
	virtual bool Check(XActor* pActor);
};


