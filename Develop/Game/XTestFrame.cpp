#include "stdafx.h"
#include "XTestFrame.h"
#include "RSceneManager.h"
#include "XGameFrameUILoader.h"
#include "XGameFrameHelper.h"

XTestFrame::XTestFrame() : XBaseFrame()
{
	
}

XTestFrame::~XTestFrame()
{

}

void XTestFrame::Update(float fDelta)
{
	PFC_THISFUNC;
	XBaseFrame::Update(fDelta);
}

void XTestFrame::OnRender()
{
	PFC_THISFUNC;
	XBaseFrame::OnRender();
}

bool XTestFrame::OnEvent(MEvent& e)
{
	if (XBaseFrame::OnEvent(e)) return true;
	if (XGameFrameHelper::OnDebugEvent(e)) return true;

	return false;
}

bool XTestFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
	XGameFrameUILoader ui_loader;
	ui_loader.Load(L"game");

	return true;
}

void XTestFrame::Close( int nNextFrameID)
{

}





