#pragma once

#include "MSender.h"

class XDummyMaster;
class XDummySendListener : public MSendListener
{
public:
	XDummySendListener(XDummyMaster& dummyMaster);
	virtual ~XDummySendListener();

	void OnSend(const MUID& muid,DWORD bytes_transferred) override;

private:
	XDummyMaster& m_dummyMaster;
};

