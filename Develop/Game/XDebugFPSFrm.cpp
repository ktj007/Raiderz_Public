#include "StdAfx.h"
#include "XDebugFPSFrm.h"
#include "XMyPlayer.h"
#include "XModuleMotion.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XGameTransformControllerManager.h"
#include "XNetwork.h"
#include "XNetClient.h"
#include "XCameraManager.h"

XDebugFPSFrm::XDebugFPSFrm( const char* szName, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	SetRect(10, 0, 700, 150);
//	SetOpacity(0.6f);
	SetText("FPS");
	SetResizable(false);
}

void XDebugFPSFrm::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	int fps = m_FrameCounter.Update();
	uint32 globaltime = global.system->GetTimer().GetNowGlobalTime();
	globaltime = globaltime - (globaltime % 100);

	int LINE_PIXEL = 20;
	int nLineCount = 0;

	wchar_t szBuf[512];
	MRECT r = GetInitialClientRect();
	r.x += 10;
	r.y += 10;
	pDC->SetColor(255, 255, 255);

	uint32 nPing = 0;
	uint32 nRecvBPS = 0;
	uint32 nSendBPS = 0;
	int64 nTotalRecvBPS = 0;
	int64 nTotalSendBPS = 0;
	uint32 nAvgRecvBPS = 0;
	uint32 nAvgSendBPS = 0;

	if (global.net && global.net->GetClient())
	{
		nPing = global.net->GetPing();
		global.net->GetClient()->GetSecondTraffic(nRecvBPS, nSendBPS);
		global.net->GetClient()->GetTotalTraffic(nTotalRecvBPS, nTotalSendBPS);

		uint32 elapsed_sec = global.system->GetTimer().GetNowTime() / 1000;

		nAvgRecvBPS = (uint32)nTotalRecvBPS / elapsed_sec;
		nAvgSendBPS = (uint32)nTotalSendBPS / elapsed_sec;

	}

	swprintf_s(szBuf, L"FPS=%d , globaltime=%u , ping=%u , Recv BPS = %u(%u), Send BPS = %u(%u)", fps, globaltime, nPing, nRecvBPS, nAvgRecvBPS, nSendBPS, nAvgSendBPS);
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());


	if (gg.omgr == NULL) return;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	if (pMyPlayer == NULL) return;
	XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
	XModuleMotion* pModuleMotion = pMyPlayer->GetModuleMotion();
	XModuleEntity* pModuleEntity = pMyPlayer->GetModuleEntity();

	XMyActionState* pState = static_cast<XMyActionState*>(pModuleMyControl->GetCurrentActionState());
	if (pState)
	{
		swprintf_s(szBuf, 512, L"MyActionID = %s", pState->GetDebugName());
		pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	}
	swprintf_s(szBuf, 512, L"Motion = %s", pModuleMotion->GetCurrMotion().c_str());
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	swprintf_s(szBuf, 512, L"ani = %s , (%3.2f / %3.2f)", pModuleEntity->GetAnimationController()->GetAnimationName().c_str(),
		pModuleEntity->GetAnimationController()->GetCurFrameToSecond(),
		pModuleEntity->GetAnimationController()->GetMaxFrameToSecond());
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	XModuleMovable* pModuleMovable = pMyPlayer->GetModuleMovable();
	const XMovableFactor::SVelocity& VEL = pModuleMovable->GetMovableFactor().VELOCITY;

	
	swprintf_s(szBuf, L"%s (%I64u) pos = (%.1f %.1f %.1f) , dir = (%.1f %.1f %.1f) , v = (%.1f %.1f %.1f) , (a = %.1f %.1f %.1f)", 
		gvar.MyInfo.ChrInfo.szName, XGetMyUID(),
		pMyPlayer->GetPosition().x, pMyPlayer->GetPosition().y, pMyPlayer->GetPosition().z + 0.001f,
		pMyPlayer->GetDirection().x, pMyPlayer->GetDirection().y, pMyPlayer->GetDirection().z,
		VEL.m_vVelocity.x, VEL.m_vVelocity.y, VEL.m_vVelocity.z,
		VEL.m_vAccel.x, VEL.m_vAccel.y, VEL.m_vAccel.z);
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	swprintf_s(szBuf, L"MUID : (%u:%u) , UIID : %d", XGetMyUID().High, XGetMyUID().Low, XGetMyUIID());
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	swprintf_s(szBuf, L"Camera pos = (%.1f %.1f %.1f) , dir = (%.1f %.1f %.1f) , Dist = %.1f", 
		global.camera->GetCamera()->GetPosition().x,
		global.camera->GetCamera()->GetPosition().y,
		global.camera->GetCamera()->GetPosition().z,
		global.camera->GetCamera()->GetDirection().x,
		global.camera->GetCamera()->GetDirection().y,
		global.camera->GetCamera()->GetDirection().z,
		global.camera->GetCamera()->GetDist());
	pDC->Text(r.x, r.y+(LINE_PIXEL*nLineCount++), MLocale::ConvUTF16ToAnsi(szBuf).c_str());
}