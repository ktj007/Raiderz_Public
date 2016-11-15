#pragma once


class XDuelGestureChecker
{
private:
	wstring					m_strMotionName;

public:
	XDuelGestureChecker() {}
	virtual ~XDuelGestureChecker() {}

	void					Init();
	void					SetGesture(const wchar_t* szMotionName);
	bool					CheckUsableGesture(XObject* pObject);
	const wchar_t*			GetGesture() { return m_strMotionName.c_str(); }

};
