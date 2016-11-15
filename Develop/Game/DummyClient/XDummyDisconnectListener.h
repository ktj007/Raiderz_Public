#pragma once

#include "MDisconnector.h"

class XDummyMaster;
class XDummyDisconnectListener : public MDisconnectListener
{
public:
	XDummyDisconnectListener(XDummyMaster& dummyMaster);
	virtual ~XDummyDisconnectListener();

	void OnDisconnect(MUID uidLink) override;

private:
	XDummyMaster& m_dummyMaster;
};

