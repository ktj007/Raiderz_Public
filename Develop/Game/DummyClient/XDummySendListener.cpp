#include "stdafx.h"
#include "XDummySendListener.h"
#include "XDummyMaster.h"

XDummySendListener::XDummySendListener(XDummyMaster& dummyMaster)
: m_dummyMaster(dummyMaster)
{
}

XDummySendListener::~XDummySendListener()
{
}

void XDummySendListener::OnSend(const MUID& muid,DWORD bytes_transferred)
{
	m_dummyMaster.OnSend(muid, bytes_transferred);
}
