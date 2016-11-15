#include "stdafx.h"
#include "XMintDesignFrame.h"
#include "XPost_Local.h"
#include "XGameFrameUILoader.h"
#include "XGameFrameHelper.h"
#include "XBaseApplication.h"
#include "XWidgetNameDef.h"

XMintDesignFrame::XMintDesignFrame()
{

}

XMintDesignFrame::~XMintDesignFrame()
{

}

void XMintDesignFrame::Update(float fDelta)
{
	PFC_THISFUNC;
	XBaseFrame::Update(fDelta);
}

void XMintDesignFrame::OnRender()
{
	PFC_THISFUNC;
	XBaseFrame::OnRender();
}

bool XMintDesignFrame::OnEvent(MEvent& e)
{
	if (XBaseFrame::OnEvent(e)) return true;
	if (XGameFrameHelper::OnDebugEvent(e)) return true;

	switch(e.nMessage)
	{
	case MWM_KEYDOWN:
		{
			if (e.bCtrl == false && e.bAlt == false)
			{
				if (e.nKey == VK_F5)
				{
					XPostLocal_ReloadUI();

					return true;
				}
			}
		}
		break;
	};

	return false;
}

bool XMintDesignFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
	XGameFrameUILoader ui_loader;
	ui_loader.Load(L"mint");

	if (global.ui)
	{
		global.ui->ShowSheet(UI_GAME_SHEET_NAME);
		global.ui->ShowMouse(true);
	}

	if (global.app)
		global.app->SetClearColor(0xFF000000);

	return true;
}
void XMintDesignFrame::Close( int nNextFrameID)
{

}


