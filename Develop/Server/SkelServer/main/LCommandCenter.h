#pragma once

#include "MCommand.h"
#include "LNetServer.h"
#include "SCommandCenter.h"

/// 커맨드 흐름을 관리하는 클래스
class LCommandCenter : public SCommandCenter
{
protected:
public:
	LCommandCenter(minet::MNetServer* netserver, SCommObjectManager* pCommObjectManager);
	virtual ~LCommandCenter() {}

};



