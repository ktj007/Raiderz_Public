#include "stdafx.h"
#include "XGameFrame.h"
#include "XProfiler.h"
#include "XBandiCapturer.h"
#include "XGameFrameHelper.h"
#include "XNetwork.h"
#include "XGameState.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// XBaseFrame ///////////////////////////////////////////////////////////////////////////////
bool XBaseFrame::OnEvent(MEvent& e)
{
	if (global.ui && global.ui->OnEvent(e) == true) return true;

	return false;
}

void XBaseFrame::Update(float fDelta)
{
	PFC_THISFUNC;

	PFI_B(101, "XBaseFrame::Update : global.net->Run");
	PFC_B("XBaseFrame::Update : global.net->Run");
	if (global.net) global.net->Update(fDelta);
	PFC_E;
	PFI_E(101);

	PFI_B(102, "XBaseFrame::Update : global.ui->Update");
	PFC_B("XBaseFrame::Update : global.ui->Update");
	if (global.ui) global.ui->Update(fDelta);
	PFC_E;
	PFI_E(102);

	PFI_B(103, "XBaseFrame::Update : global.mint->Run");
	PFC_B("XBaseFrame::Update : global.mint->Run");
	if (global.mint) global.mint->Run();
	PFC_E;
	PFI_E(103);

	PFI_B(105, "XBaseFrame::Update : global.sound->Update");
	PFC_B("XBaseFrame::Update : global.sound->Update");
	if ( global.sound)
	{
		if ( gg.currentgamestate)
		{
			vec3 _pos = vec3::ZERO;
			vec3 _vel = vec3::ZERO;
			vec3 _dir = vec3::ZERO;
			vec3 _up = vec3::AXISZ;
			gg.currentgamestate->GetDataForSoundListener( _pos, _dir);
			global.sound->SetListener( _pos, _vel, _dir, _up);
		}

		global.sound->Update();
	}
	PFC_E;
	PFI_E(105);

	MBitmapManager::Update(fDelta);
}

void XBaseFrame::OnPreRender()
{
	PFC_THISFUNC;

}

void XBaseFrame::OnRender()
{
	PFC_THISFUNC;

	global.ui->Render();

	if (false == XGETCFG_VIDEO_SHOWUIWHENREC)
	{
		if (global.bandi_capturer)
		{
			global.bandi_capturer->DrawCapture(true);
		}
	}

	XGameFrameHelper::RenderMint();
}

void XBaseFrame::OnPreFlip()
{
	PFC_THISFUNC;

	if (XGETCFG_VIDEO_SHOWUIWHENREC)
	{
		if (global.bandi_capturer)
		{
			global.bandi_capturer->DrawCapture(false);
		}
	}
}
