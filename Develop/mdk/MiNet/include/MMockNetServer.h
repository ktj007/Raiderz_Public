#pragma once

#include "MiNetLib.h"
#include "MNetServer.h"

namespace minet {

class MMockLink;

class MINET_API MMockNetServer : public MNetServer
{
protected:
	virtual bool SendCommandToLink(MUID uidReceiver, MCommand* pCommand, MCommandMsg* pMsg, int nPacketSize) override;
	virtual MMockLink* NewMockLink(MUID uidLink) = 0;
public:
	using MNetServer::OnCommand;
	MMockNetServer(const MNetServerDesc& desc) : MNetServer(desc) {}
	virtual ~MMockNetServer() {}

	virtual bool Create(int nPort, const bool bReuse=false, MUID uidSeed=MUID::ZERO, int nSocketPoolSize=2000, int nSendPendingLimitCount = INT_MAX, const char* szMyNetworkCardIP="") override;
	virtual void Destroy();

	MMockLink* AddMockLink(MUID uidLink);
	void DelMockLink(MUID uidLink);
	void ClearMockLinks();
};

} // namespace minet {