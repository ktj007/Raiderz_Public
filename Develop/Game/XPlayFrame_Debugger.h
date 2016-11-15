#pragma once

#include "XCameraControllerGroup.h"

class XRecorder;

class XPlayFrame_Debugger
{
private:
	FrameCounter			m_FrameCounter;
	XRecorder*				m_pRecorder;
	XCameraType				m_nLastCameraType;

	void RenderSensor();
	void RenderSoundRange();
	void RenderCollision();
	void RenderSectorGrid();
	void RenderArea();

public:
	XPlayFrame_Debugger();
	~XPlayFrame_Debugger();
	void Create();
	void Destroy();

	void RenderDebug();

	// 디버그용 이벤트는 이 함수에 넣으세요
	bool OnDebugEvent(MEvent& e);

	void ReloadConst();
	void ToggleRecordReplay();
	void StartRecordReplay();
	void EndRecordReplay();
	void RenderRecordMessage();

	bool ToggleFreeLookCamera();
};