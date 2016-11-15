#pragma once

#include "XModule.h"

class XModuleSound : public XModule
{
	DECLARE_ID(XMID_SOUND);

private:
	bool					m_bModelSoundPlaying;

private:
	void					CheckPlayModelSound(float fDelta);

protected:
	virtual void			OnUpdate(float fDelta);

public:
	XModuleSound(XObject* pOwner=NULL);
	virtual ~XModuleSound(void);
};

