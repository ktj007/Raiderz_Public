#pragma once

#include "MReceiver.h"

class XDummyMaster;
class XDummyRecvListener : public MRecvListener
{
public:
	XDummyRecvListener(XDummyMaster& dummyMaster);
	virtual ~XDummyRecvListener();

	void OnDisconnect(MUID uidLink) override;
	void OnHardDisconnect(MUID uidLink) override;
	void OnRecv(MServerTcpSocket& tcpsocket, MUID uidLink, BYTE* pData, DWORD dwPacketLen) override;

private:
	XDummyMaster& m_dummyMaster;
};

