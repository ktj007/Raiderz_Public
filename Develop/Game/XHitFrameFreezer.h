#pragma once

class XObject;

// 타격감을 위해 공격자가 공격시 판정되면 모션을 경직시킨다.
class XHitFrameFreezer
{
private:
	bool m_bReserve;
	float m_fReserveElapsedTime;
	wstring	m_strCurrAni;

	void FreezeFrame(XObject* pAttacker);
public:
	XHitFrameFreezer() : m_bReserve(false) {}
	void OnHit(XObject* pAttacker, const wchar_t* szCurrAni);
	void Update(float fDelta, XObject* pAttacker, const wchar_t* szCurrAni);
};
