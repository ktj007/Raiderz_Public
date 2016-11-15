#pragma once

#include "MCommand.h"
#include "PNetServer.h"
#include "SCommandCenter.h"
#include "STypes.h"

class SCommObjectManager;
class PChatChannel;


/// 커맨드 흐름을 관리하는 클래스
class PCommandCenter : public SCommandCenter
{
public:
	PCommandCenter(minet::MNetServer* netserver);
	virtual ~PCommandCenter() {}

	void RouteToAll(MCommand* pCommand);
	void RouteToChannel(PChatChannel* pChannel, minet::MCommand* pCommand, CID cidIgnorePlayer = 0);

};



