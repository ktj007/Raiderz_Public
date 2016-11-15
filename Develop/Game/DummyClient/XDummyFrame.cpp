#include "stdafx.h"
#include "XDummyFrame.h"
#include "XNetClient.h"
#include "XDummyMaster.h"
#include "XDummyFileLoader.h"
#include "XGameFrameHelper.h"
#include "XDummyUI.h"
#include "XBaseApplication.h"

XDummyFrame::XDummyFrame() : XBaseFrame()
{
	m_pDummyMaster = new XDummyMaster();
	m_pUI = new XDummyUI();
}

XDummyFrame::~XDummyFrame()
{
	SAFE_DELETE(m_pUI);
	SAFE_DELETE(m_pDummyMaster);
	Close(0);
}

bool XDummyFrame::Open( int nPrevFrameID, const wchar_t* args )
{
	XCONST::AUTO_REPORT_CRASHDUMP = true;

	global.app->SetClearColor(0xFF000000);

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnDebugOnGameStarted();
	}

	wchar_t szDummyFileName[_MAX_PATH];
	swprintf_s(szDummyFileName, args);

	m_pDummyMaster->Init(szDummyFileName);

	m_pUI->Create(m_pDummyMaster->GetData());


	m_pDummyMaster->Start();


	return true;
}

void XDummyFrame::Close(int nNextFrameID)
{
	m_pUI->Destroy();
	m_pDummyMaster->End();
}

void XDummyFrame::Update( float fDelta )
{
	XBaseFrame::Update(fDelta);
	m_pDummyMaster->Run(fDelta);
}

bool XDummyFrame::OnEvent( MEvent& e )
{
	if (XBaseFrame::OnEvent(e)) return true;

	if (e.nMessage == MWM_KEYDOWN)
	{
		if (global.ui)
		{
			if (e.nKey == VK_F5 && e.bCtrl == true)
			{
				m_pUI->Reload();
				return true;
			}

			if (e.nKey == 192 && e.bCtrl == true)
			{
				global.ui->ToggleConsole();
				return true;
			}
		}
	}

	return false;
}

void XDummyFrame::OnRender()
{
	m_pUI->Render();
	XGameFrameHelper::RenderMint();

}


