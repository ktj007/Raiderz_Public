#include "stdafx.h"
#include "XDummyRecvListener.h"
#include "XDummyMaster.h"

XDummyRecvListener::XDummyRecvListener(XDummyMaster& dummyMaster)
: m_dummyMaster(dummyMaster)
{
}

XDummyRecvListener::~XDummyRecvListener()
{
}

void XDummyRecvListener::OnDisconnect(MUID uidLink)
{
	assert(uidLink!=MUID::ZERO);
	m_dummyMaster.OnDisconnect(uidLink);
}

void XDummyRecvListener::OnHardDisconnect(MUID uidLink)
{
	assert(uidLink!=MUID::ZERO);
	m_dummyMaster.OnDisconnect(uidLink);
}

void XDummyRecvListener::OnRecv(MServerTcpSocket& tcpsocket,MUID uidLink,BYTE* pData,DWORD dwPacketLen)
{
	assert(NULL!=pData);
	m_dummyMaster.OnRecv(uidLink, pData, dwPacketLen);
}

