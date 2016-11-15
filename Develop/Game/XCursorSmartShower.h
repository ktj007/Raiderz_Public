#pragma once

class XCursorSmartShower
{
protected:
	bool		m_bNowKeyDown;
	virtual void ShowCursor(bool bVisible);
	virtual bool IsMouseCursorVisible();
public:
	XCursorSmartShower() : m_bNowKeyDown(false) {}
	virtual ~XCursorSmartShower() {}

	bool OnKeyDown();
	void OnKeyUp();
};