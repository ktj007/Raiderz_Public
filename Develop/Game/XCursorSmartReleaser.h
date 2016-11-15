#pragma once

class XCursor;
class XUISystem;


#define CURSOR_SMART_RELEASE_TIME  (0.01f)		// 키 누르면 거의 즉시 해제

// 커서가 나왔을 때 이동키를 누르면 자동적으로 커서를 해제하는 클래스
class XCursorSmartReleaser
{
private:
	float		m_fElapsedTime;
	bool		IsUpdateEnabled(XCursor* pCursor, XUISystem* pUISystem, XController* pController);
	void		DoIt(XCursor* pCursor, XUISystem* pUISystem);
public:
	XCursorSmartReleaser() : m_fElapsedTime(0.0f) {}
	void Update(float fDelta, XCursor* pCursor, XUISystem* pUISystem, XController* pController);
};