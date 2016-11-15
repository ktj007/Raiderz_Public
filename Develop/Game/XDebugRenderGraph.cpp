#include "StdAfx.h"
#include "XDebugRenderGraph.h"
#include "XEffectManager.h"
#include "RCameraSceneNode.h"
#include "XCameraManager.h"

XDebugRenderGraph::XDebugRenderGraph( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	ShowTitleBar( false);
	SetRect(10, 360, 800, 130);
//	SetOpacity(0.6f);
	SetText("RenderGraph");
}

void XDebugRenderGraph::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	MRECT r = GetRect();
	MFont* pFont = MFontManager::Get( NULL);
	pDC->SetFont( pFont);

	// TODO: downcasting -_-; rs쪽에 기본 폰트를 두는게 나을듯
	MFontR3* pFontR3 = (MFontR3*)pFont;
	if(IsShowTitleBar() == true)	pFontR3->GetFont()->EndFont();
	rs3::REngine::GetResourceProfiler().RenderTimeGraph(pFontR3->GetFont(), r.x+40, r.y+10);

	if(IsShowTitleBar() == true)	pFontR3->GetFont()->BeginFont();

	const rs3::RDeviceProfileInfo& deviceProfile = global.device->GetDeviceProfileInfo();
	int nDeviceTotal = deviceProfile.nDP + deviceProfile.nDIP + deviceProfile.nDPUP + deviceProfile.nDIPUP;

	char szBuf[512];
	r = GetInitialClientRect();
	pDC->SetColor(255, 255, 255);

	int nLinecount = 7;

	sprintf_s(szBuf, 512, "shader=%s , ColorBits=%d , DepthBits=%d", XGETCFG_VIDEO_SHADER ? "TRUE" : "FALSE", XGETCFG_VIDEO_COLORBITS, XGETCFG_VIDEO_DEPTHBITS);
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	// device 전체 폴리곤
	sprintf_s(szBuf, 512, "Total Polygons = %d", nDeviceTotal);
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	unsigned int texMemory = global.device->GetTextureMemoryUsed()/(1024*1024);
	sprintf_s(szBuf, 512, "Texture memory(MB) = %d", texMemory);
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	unsigned int vbMemory = global.device->GetVertexBufferMemoryUsed()/(1024*1024);
	sprintf_s(szBuf, 512, "Vertex memory(MB) = %d", vbMemory);
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	nLinecount++;
	for ( unsigned int i=0; i< rs3::REngine::GetResourceProfiler().GetTypeCount(); ++i )
	{
			sprintf_s(szBuf, 512, "%s = %d", 
			rs3::REngine::GetResourceProfiler().GetTypeName((PROFILE_POLYGON_TYPE)i), 
			rs3::REngine::GetResourceProfiler().GetPolygonCount((PROFILE_POLYGON_TYPE)i) );
			pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);
	}

	// device - 위의 합
	sprintf_s(szBuf, 512, "etc = %d", nDeviceTotal-rs3::REngine::GetResourceProfiler().GetTotalPolygons());
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	sprintf_s(szBuf, 512, "----------------------------");
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	sprintf_s(szBuf, 512, "call dp = %d  dpup = %d  dip = %d  dipup = %d ", 
		deviceProfile.nDPCall, deviceProfile.nDPUPCall, deviceProfile.nDIPCall, deviceProfile.nDIPUPCall );
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	// 락이 없어서 위험해보이기는 하나, 디버그용 코드라 남겨둠
	RBackgroundWorker* pWorker = &REngine::GetBackgroundWorker();
	sprintf_s(szBuf, 512, "Total Background Work = %d", pWorker->GetRemainedWorkableTicketSize() );
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	sprintf_s(szBuf, 512, "Total Effects = %d", global.emgr->GetCount() );
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	sprintf_s(szBuf, 512, "Total Objects = %d", (int)gg.omgr->GetCount());
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	sprintf_s(szBuf, 512, "Players = %d , NPCs = %d", gg.omgr->GetCount(ETID_PLAYER), gg.omgr->GetCount(ETID_NPC));
	pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);

	// camera
	if (global.smgr->GetPrimaryCamera())
	{
		vec3 pos = global.smgr->GetPrimaryCamera()->GetPosition();
		vec3 dir = global.smgr->GetPrimaryCamera()->GetDirection();

		char szCameraType[256] = "none";
		if (global.camera->IsPrimaryCameraType(CAMERA_MAIN_BACKVIEW)) strcpy_s(szCameraType, "main_backview");
		else if (global.camera->IsPrimaryCameraType(CAMERA_FREE_LOOK)) strcpy_s(szCameraType, "free_look");
		else if (global.camera->IsPrimaryCameraType(CAMERA_GRAPPLED)) strcpy_s(szCameraType, "grappled");
		else if (global.camera->IsPrimaryCameraType(CAMERA_REPLAY_BACKVIEW)) strcpy_s(szCameraType, "replay_backview");

		sprintf_s(szBuf, 512, "Camera pos(%.2f, %.2f, %.2f) , dir(%.2f, %.2f, %.2f), type=%s", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, szCameraType);
		pDC->Text(r.x, r.y+(20*nLinecount++), szBuf);
	}
}

bool XDebugRenderGraph::OnEvent( MEvent* pEvent, MListener* pListener )
{
	MTextFrame::OnEvent(pEvent, pListener);

	switch(pEvent->nMessage)
	{
	case MWM_KEYUP:
		{
			REngine::GetResourceProfiler().ProcessDebugKey(pEvent->nKey);
		}
		break;
	}

	return false;
}