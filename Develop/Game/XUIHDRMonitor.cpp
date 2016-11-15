#include "stdafx.h"
#include "XUIHDRMonitor.h"
#include "RCameraSceneNode.h"
#include "RPostEffectHDR.h"
#include "XCameraManager.h"

XUIHDRMonitor::XUIHDRMonitor( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_fBucketSize		= 38;
	m_fBucketBlankSize	= 5;
}

XUIHDRMonitor::~XUIHDRMonitor()
{
}

void XUIHDRMonitor::OnDraw(MDrawContext* pDC)
{
	/*
	RPostEffectHDR* pHDRController = (RPostEffectHDR*)global.camera->GetCamera()->GetCameraController( RCCT_POSTRENDER, "0_HDR" );
	if (!pHDRController)
		return;
	
	MRECT rect = GetClientRect();
	
	float fTotal	= 0.0f;
	float fMaxPixel = 0.0f;

	for( int i = 0; i < NUM_MAX_BUCKETS; ++i )
		fMaxPixel += pHDRController->m_arrBucket[i].m_nCountPixels;

	// Bucket
	pDC->SetColor( 255, 0, 0, 255 );
	for( int i = 0; i < NUM_MAX_BUCKETS; ++i )
	{
		int nStartPos = m_fBucketSize * i + m_fBucketBlankSize * i;
		pDC->FillRectangle( nStartPos, rect.h - ( ( pHDRController->m_arrBucket[i].m_nCountPixels / fMaxPixel ) * rect.h ), m_fBucketSize, rect.h);
	}

	// Barriers
	RenderBarrier(pDC, rect, MCOLOR(0, 255, 255, 255), pHDRController->m_nDarkBucketFrame, pHDRController->m_fTonemapMin);
	RenderBarrier(pDC, rect, MCOLOR(0, 255, 0, 255), pHDRController->m_nBrightBucketFrame, pHDRController->m_fTonemapMax);

	rs3::RCameraSceneNode* pCamera = (rs3::RCameraSceneNode*)global.camera->GetCamera();
	if( !pCamera ) 
		return;

	RPostEffectHDR* pController = (RPostEffectHDR*)pCamera->GetCameraController( RCCT_POSTRENDER, "0_HDR" );
	if( !pController ) 
		return;

	char szBuf[512];
	MRECT r = GetInitialClientRect();

	sprintf_s(szBuf, 512, "Tonemap Min = %.2f", 0.0f);
	pDC->Text(r.w - 200, r.y+20*0, szBuf);

	sprintf_s(szBuf, 512, "Tonemap Max = %.2f", MAX_LUMINANCE);
	pDC->Text(r.w - 200, r.y+20*1, szBuf);

	sprintf_s(szBuf, 512, "DarkBarrier(Blue)	= %.2f(%.2f%%)", pController->m_fTonemapMin, pController->m_fDarkBarrierRatio);
	pDC->Text(r.w - 200, r.y+20*3, szBuf);

	sprintf_s(szBuf, 512, "BrightBarrier(Green) = %.2f(%.2f%%)", pController->m_fTonemapMax, pController->m_fBrightBarrierRatio);
	pDC->Text(r.w - 200, r.y+20*4, szBuf);

	sprintf_s(szBuf, 512, "Limit Min(Pink) = %.2f(%.2f%%)", pController->m_fLimitMin, pController->m_fLimitMin / MAX_LUMINANCE );
	pDC->Text(r.w - 200, r.y+20*6, szBuf);

	sprintf_s(szBuf, 512, "Limit Max(Yellow) = %.2f(%.2f%%)", pController->m_fLimitMax, pController->m_fLimitMax / MAX_LUMINANCE );
	pDC->Text(r.w - 200, r.y+20*7, szBuf);
	*/
}

void XUIHDRMonitor::RenderBarrier(MDrawContext* pDC, MRECT& rect, MCOLOR color, int nBucketFrame, float fCurrentPos)
{
	//if( nBucketFrame < 0 )
	//	return;

	//float fRatio	= fCurrentPos / MAX_LUMINANCE;
	//float fSize		= m_fBucketSize * NUM_MAX_BUCKETS + m_fBucketBlankSize * NUM_MAX_BUCKETS;
	//int x = (int)(fSize * fRatio);

	//pDC->SetColor(color);
	//pDC->Line( x, 0, x, rect.h);
}

bool XUIHDRMonitor::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;

	return false;
}
