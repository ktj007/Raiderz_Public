#ifndef _XMODULE_BACKGROUNDWORK_H
#define _XMODULE_BACKGROUNDWORK_H

#include "XModule.h"

class XBackgroundWorkUnit;
class XModuleBackgroundWork : public XModule, public MMemPool<XModuleBackgroundWork>
{
public:
	DECLARE_ID(XMID_BGW);
									XModuleBackgroundWork(XObject* pOwner=NULL);
	virtual							~XModuleBackgroundWork(void);

protected:
	virtual void					OnUpdate(float fDelta);
	virtual XEventResult			OnEvent(XEvent& msg);
	virtual void					OnSubscribeEvent();

	std::list<XBackgroundWorkUnit*>	m_BackgroundWorkUnits;
	std::list<XBackgroundWorkUnit*>	m_BackgroundWorkUnitsChecked;
};

#endif // #ifndef _XMODULE_BACKGROUNDWORK_H
