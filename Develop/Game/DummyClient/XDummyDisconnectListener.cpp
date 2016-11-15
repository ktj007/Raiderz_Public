#include "stdafx.h"
#include "XDummyDisconnectListener.h"
#include "XDummyMaster.h"

XDummyDisconnectListener::XDummyDisconnectListener(XDummyMaster& dummyMaster)
: m_dummyMaster(dummyMaster)
{
}

XDummyDisconnectListener::~XDummyDisconnectListener()
{
}

void XDummyDisconnectListener::OnDisconnect(MUID uidLink)
{
	assert(uidLink!=MUID::ZERO);
	m_dummyMaster.OnDisconnect(uidLink);
}
