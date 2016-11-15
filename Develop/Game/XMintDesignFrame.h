#ifndef _XMINT_DESIGN_FRAME_H
#define _XMINT_DESIGN_FRAME_H

#include "XConst.h"
#include "XGlobal.h"
#include "XGameFrame.h"
#include "XGameFrameID.h"

using namespace rs3;

class XMintDesignFrame : public XBaseFrame
{
protected:

public:
	XMintDesignFrame();
	virtual ~XMintDesignFrame();
	virtual void Update(float fDelta);
	virtual void OnRender();
	virtual bool OnEvent(MEvent& e);
	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs);
	virtual void Close( int nNextFrameID);
	virtual int GetID()	{ return MINT_DESIGN_FRAME_ID; }
	virtual const wchar_t* GetName() { return L"mint"; }
};









#endif