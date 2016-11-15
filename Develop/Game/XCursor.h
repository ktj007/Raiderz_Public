#pragma once

#include "MTypes.h"

class XCursor
{
private:
	MPoint			m_ptLast;
	uint32			m_nLastTime;
	bool			m_bVisible;
	void			ShowCursor(bool bVisible);
public:
	XCursor();
	bool IsVisible()		{ return m_bVisible; }
	void Show(bool bVisible);
};
