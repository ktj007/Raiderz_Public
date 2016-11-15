#pragma once

#include "XPlayFrame.h"
#include "XReplayAutoTest.h"

class XReplayAutoTest;

class XScreenShotDumper
{
private:
	unsigned int m_nElapsedTime;
	unsigned int m_nLastTime;
	unsigned int m_nTick;
	wstring m_strFolder;
	wstring m_strReplayScreenShotPath;

	void SaveScreenShot();
public:
	XScreenShotDumper() {}
	~XScreenShotDumper() {}
	void Start(wstring strReplayScreenShotPath);
	void Update();
};

class XReplayFrame : public XPlayFrame
{
private:
	XReplay*	m_pReplay;
	bool		m_bFinalizeReplay;
	wstring		m_strReplayFileName;
	bool		m_bAutoTest;
	bool		m_bReplayAnalyze;
	bool		m_bReplayScreenShot;
	wstring		m_strReplayScreenShotPath;
	XReplayTestBaseline		m_ReplayTestBaseline;

	XReplayAutoTest*	m_pReplayAutoTest;
	XScreenShotDumper	m_ScreenShotDumper;

	MUID m_uidCameraTarget;
	void SetCameraTarget(bool bMyPlayer);
	void OnStopReplay();
	void RenderReplayMessage();
	void RenderBenchmarkMessage();
	void SpeedDownReplay();
	void SpeedUpReplay();
	void TogglePauseReplay();
	void StartReplay();
	void WriteReplayLink();
	void InitializeUI();
	void FinalizeUI();

	void CameraControlOnMouseWheel( MEvent &e );
public:
	XReplayFrame();
	virtual ~XReplayFrame();
	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs);

	void SetupConfigFromAppParams();
	virtual void Close( int nNextFrameID);
	virtual void Update(float fDelta);
	virtual bool OnEvent(MEvent& e);

	virtual void OnRender();
	virtual int GetID()	{ return REPLAY_FRAME_ID; }
};