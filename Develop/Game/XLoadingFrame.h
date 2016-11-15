#pragma once

#include "XGameFrame.h"
#include "XLoadingMgr.h"
#include "XLoadingChecker.h"

using namespace rs3;
using namespace mint3;



// XLoadingFrame
class XLoadingFrame
{
	typedef void (*_callback)( XBaseFrame* pFrame);

	
/// Member variables
private:
	XBaseFrame*		m_pFrame;
	XLoadingMgr		m_LoadingMgr;
	bool			m_bCallPreLoading;
	bool			m_bCallLoading;
	bool			m_bCallPaused;
	bool			m_bCallLoaded;
	bool			m_bRun;
	bool			m_bPause;
	bool			m_bPauseOnComplete;
	bool			m_bCompleteLoading;
	bool			m_bCompleteResLoading;
	float			m_fElapsedTime;



/// Member functions
public:
	XLoadingFrame( XBaseFrame* pFrame);
	virtual ~XLoadingFrame();

	void AddLoadingChecker( XLoadingChecker* pChecker);
	void ClearLoadingChecker();

	void Start();
	void Continue();
	void Stop();

	bool IsLoading();

	void PauseOnLoadingComplete( bool bPause)				{ m_bPauseOnComplete = bPause;		}

	void Update( float fDelta);
	void Render( MDrawContext* pDC);
};
