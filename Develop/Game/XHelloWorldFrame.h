#ifndef _XHELLO_WORLD_FRAME_H
#define _XHELLO_WORLD_FRAME_H

#include "XConst.h"
#include "XGameFrame.h"
#include "XGameFrameID.h"

using namespace rs3;

class XHelloWorldFrame : public XBaseFrame
{
protected:
public:
	XHelloWorldFrame();
	virtual ~XHelloWorldFrame();
	virtual void Update(float fDelta);
	virtual void OnRender();
	virtual bool OnEvent(MEvent& e);
	virtual void OnActive(bool bActive)	{}
	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs);
	virtual void Close( int nNextFrameID);
	virtual int GetID()	{ return HELLOWORLD_FRAME_ID; }
	virtual const wchar_t* GetName() { return L"hello_world"; }
};

#endif // _XHELLO_WORLD_FRAME_H