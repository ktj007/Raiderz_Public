#include "stdafx.h"
#include "XLoadingFrame.h"
#include "XLoadingMgr.h"
#include "XLoadingChecker.h"




// XLoadingFrame
XLoadingFrame::XLoadingFrame( XBaseFrame* pFrame)
{
	m_pFrame = pFrame;
	m_bRun = false;
	m_bPause = false;
	m_bPauseOnComplete = false;
	m_bCompleteLoading = false;
	m_bCompleteResLoading = false;
	m_fElapsedTime = 10.0f;
}


// ~XLoadingFrame
XLoadingFrame::~XLoadingFrame()
{
	m_pFrame = NULL;
	m_LoadingMgr.ClearCheckers();
}


// AddLoadingChecker
void XLoadingFrame::AddLoadingChecker( XLoadingChecker* pChecker)
{
	if ( pChecker != NULL)		m_LoadingMgr.AddChecker( pChecker);
}


// ClearLoadingChecker
void XLoadingFrame::ClearLoadingChecker()
{
	m_LoadingMgr.ClearCheckers();
}


// Start
void XLoadingFrame::Start()
{
	m_bCallPreLoading = false;
	m_bCallLoading = false;
	m_bCallPaused = false;
	m_bCallLoaded = false;
	m_bRun = true;
	m_bPause = m_bPauseOnComplete;
	m_bCompleteLoading = false;
	m_bCompleteResLoading = false;
	m_fElapsedTime = 0.0f;

	m_LoadingMgr.Start();
}


// Continue
void XLoadingFrame::Continue()
{
	m_bPause = false;
}


// Stop
void XLoadingFrame::Stop()
{
	m_bRun = false;
	m_bPause = false;
	m_bCompleteLoading = false;
	m_bCompleteResLoading = false;
	m_fElapsedTime = 0.0f;
}


// IsLoading
bool XLoadingFrame::IsLoading()
{
	if ( m_bRun == false)				return false;
	return !m_bCompleteLoading;
}


// Update
void XLoadingFrame::Update( float fDelta)
{
	m_fElapsedTime += fDelta;
	m_fElapsedTime = min( 10.0f, m_fElapsedTime);


	if ( m_bRun == false)		return;


	// Call functions
	if ( m_bCompleteResLoading == true  &&  m_bCallPreLoading == true  &&  m_bCallLoading == true  &&  m_bCallLoaded == false  &&  m_bPause == false)
	{
		if ( m_pFrame != NULL)		m_pFrame->OnLoaded();
		m_bCallLoaded = true;
		m_bCompleteLoading = true;
		m_fElapsedTime = 0.0f;
	}

	else if ( m_bCompleteResLoading == true  &&  m_bCallPreLoading == true  &&  m_bCallLoading == true  &&  m_bCallLoaded == false  &&  m_bPause == true  &&  m_bCallPaused == false)
	{
		if ( m_pFrame != NULL)		m_pFrame->OnPaused();
		m_bCallPaused = true;
	}

	else if ( m_bCompleteResLoading == false  &&  m_bCallPreLoading == true  &&  m_bCallLoading == false)
	{
		if ( m_pFrame != NULL)		m_pFrame->OnLoading();
		m_bCallLoading = true;
	}

	else if ( m_bCompleteResLoading == false  &&  m_bCallPreLoading == false)
	{
		if ( m_pFrame != NULL)		m_pFrame->OnPreLoading();
		m_bCallPreLoading = true;
	}


	// Check resource loading complete
	if ( m_bCompleteResLoading == false  &&  m_bCallPreLoading == true  &&  m_bCallLoading == true  &&  m_LoadingMgr.CheckCompleted() == true)
	{
		m_bCompleteResLoading = true;
		m_fElapsedTime = 0.0f;
	}
}


// Render
void XLoadingFrame::Render( MDrawContext* pDC)
{
	if ( pDC == NULL)			return;

	float fRatio = 0;
	if ( m_bRun == true)		fRatio = m_bCompleteLoading  ?  1.0f : 0.0f;
	else						fRatio = max( 0.0f, min( 1.0f, -m_fElapsedTime + 1.0f));

	if ( fRatio > 0.0f)
	{
		int width = global.ui->GetMint()->GetWorkspaceWidth();
		int height = global.ui->GetMint()->GetWorkspaceHeight();
		unsigned char alpha = (unsigned char)( fRatio * 255.0);

		pDC->BeginDraw();
		pDC->SetColor( MCOLOR( 0, 0, 0, alpha));
		pDC->FillRectangle( 0, 0, width, height);
		pDC->EndDraw();
	}
}
