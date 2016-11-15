#pragma once

class XActor;

class XDeathController
{
private:
public:
	XDeathController() {}
	~XDeathController() {}
	bool IsDeadMotionImmediately(MF_STATE nMFState);
	bool IsDeadActionImmediatelyOnHit(XActor* pActor, MF_STATE nMFState);
};
