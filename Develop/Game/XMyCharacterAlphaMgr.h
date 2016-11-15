#pragma once

class XModuleEntity;

/// 내 캐릭터의 알파 관리자
class XMyCharacterAlphaMgr
{
private:
	bool		m_bAlphaState;
public:
	XMyCharacterAlphaMgr() : m_bAlphaState(false) {}
	void Check(float fCameraDist, XModuleEntity* pModuleEntity);

	const float CONST_ALPHA_VALUE()	{ return 0.1f; }			///< 투명화되었을 때 알파값
	const float CONST_ALPHA_DIST()	{ return 40.0f; }			///< 카메라와 내캐릭터간의 거리가 이 거리보다 짧으면 알파 처리함
	const float CONST_FADE_TIME()	{ return 0.2f; }			///< 알파 처리 시간
};
