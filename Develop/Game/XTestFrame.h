#ifndef _XTEST_FRAME_H
#define _XTEST_FRAME_H

#include "XGameFrame.h"
#include "XGameFrameID.h"

using namespace rs3;

class XGame;
class XController;

class XTestFrame : public XBaseFrame
{
protected:
public:
	XTestFrame();
	virtual ~XTestFrame();
	virtual void Update(float fDelta);
	virtual void OnRender();
	virtual bool OnEvent(MEvent& e);
	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs);
	virtual void Close( int nNextFrameID);
	virtual int GetID()	{ return TEST_FRAME_ID; }
	virtual const wchar_t* GetName() { return L"test"; }
};

#endif // _XTEST_FRAME_H