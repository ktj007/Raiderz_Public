#ifndef _XENGINE_DEBUG_FRAME_H
#define _XENGINE_DEBUG_FRAME_H

#include "XPlayFrame.h"

class XEngineDebugger;
class XEngineDebugFrame : public XPlayFrame
{

protected:

	// Open ½Ã¿¡
	virtual bool					OnOpenWorldLoadingInit();
	virtual bool					OnOpenControllerInit();
	virtual bool					OnOpenCameraInit();
	virtual bool					OnOpenGameInit();

	XEngineDebugger*				m_pEngineDebugger;
	bool							m_bInitAfterLoading;

public:
	XEngineDebugFrame();
	virtual ~XEngineDebugFrame();
	virtual void					Update(float fDelta);
	virtual void					OnRender();
	virtual bool					OnEvent(MEvent& e);
	virtual bool					Open( int nPrevFrameID, const wchar_t* szArgs);
	virtual void					Close( int nNextFrameID);
	virtual int						GetID()	{ return ENGINE_DEBUG_FRAME_ID; }
	virtual const wchar_t*			GetName() { return L"engine_debug"; }
};

#endif // _XENGINE_DEBUG_FRAME_H