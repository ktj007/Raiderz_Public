#include "stdafx.h"
#include "XHelloWorldFrame.h"
#include "RSceneManager.h"
#include "XGameFrameHelper.h"

XHelloWorldFrame::XHelloWorldFrame() : XBaseFrame()
{

}

XHelloWorldFrame::~XHelloWorldFrame()
{

}

void XHelloWorldFrame::Update(float fDelta)
{
	PFC_THISFUNC;
	XBaseFrame::Update(fDelta);
}

void XHelloWorldFrame::OnRender()
{
	PFC_THISFUNC;
	XBaseFrame::OnRender();
}

bool XHelloWorldFrame::OnEvent(MEvent& e)
{
	if (XBaseFrame::OnEvent(e)) return true;
	if (XGameFrameHelper::OnDebugEvent(e)) return true;

	return false;
}

bool XHelloWorldFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{


	return true;
}

void XHelloWorldFrame::Close( int nNextFrameID)
{

}





