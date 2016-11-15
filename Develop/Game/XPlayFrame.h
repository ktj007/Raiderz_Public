
#pragma once

#include "XGameFrame.h"
#include "XGameFrameID.h"
#include "XLoadingFrame.h"
#include "XCursorSmartShower.h"
#include "XPlayFrame_Debugger.h"

#ifndef _PUBLISH
#include "XDebugDashBoard.h"
#endif



using namespace rs3;

class XGame;
class XController;
class XMovableDebugger;





/// class : XPlayFrame
class XPlayFrame : public XBaseFrame
{
protected:
	XGame*					m_pGame;				// Game, Controller
	XLoadingFrame*			m_pLoadingFrame;
	XCursorSmartShower		m_CursorSmartShower;
	int						m_nLoadingType;

	XPlayFrame_Debugger		m_Debugger;

#ifndef _PUBLISH
	// Debugging and test
	XMovableDebugger*		m_pMovableDebugger;
	XDebugDashBoard			m_DebugDashBoard;
#endif



/// Member functions
private:
	bool CheckCameraStateClear(MEvent& e);

public:
	XPlayFrame();
	virtual ~XPlayFrame();

	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs) override;
	virtual void Close( int nNextFrameID) override;

	virtual int GetID() override				{ return PLAY_FRAME_ID;					}

	bool DispatchEvent( MEvent& e);

	bool InitWorld();
	bool InitCamera();
	bool InitGame();

	bool IsLoadingFrame();

/// Event handler
public:
	virtual bool OnEvent( MEvent& e) override;
	virtual void Message( const wchar_t* szMsg) override;
	virtual void Update( float fDelta) override;
	virtual void OnPreRender() override;
	virtual void OnRender() override;
	virtual void OnActive( bool bActive) override;

	virtual void OnPreLoading() override;
	virtual void OnLoading() override;
	virtual void OnPaused() override;
	virtual void OnLoaded() override;


#ifndef _PUBLISH
public:
	void ReloadDashBoard();
	XDebugDashBoard* GetDashBoard()				{ return &m_DebugDashBoard;				}
#endif
};
