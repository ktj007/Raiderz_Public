#pragma once

#include "XGlueFunction.h"
#include "XScreenEffectManager.h"

/// lua에서 사용하는 스크린이펙트 API
class XGlueScreenEffectFunction : public XGlueFunction
{
public:
	virtual void DeclGlueFunc(MWLua* pLua);

	void					Add(const char* strListFileName, XSCREENEFFECT_CHARACTER_ATT charAtt, AnimationLoopType aniLoop);		// 간단한 스크린 이펙트 보여주기
	void					SetPos(float x, float y, float z, XSCREENEFFECT_LOCATION_TYPE scrBase);									// 간단한 스크린 이펙트 위치
	void					Register(const char* strScreenEffectName, 
									const char* strListFileName, 
									XSCREENEFFECT_CHARACTER_ATT charAtt,
									AnimationLoopType aniLoop);																		// 스크린 이펙트 등록
	void					SetRegPos(const char* strScreenEffectName, 
									float x, float y, float z, 
									XSCREENEFFECT_LOCATION_TYPE scrBase);															// 스크린 이펙트 위치
	void					SetRegStr(const char* strScreenEffectName, const char* strContents);									// 스크린 이펙트 문자열
	void					SetChgAni(const char* strScreenEffectName, const char* strAnimation, XSCREENEFFECT_ANIMATION_END_ATT aniDel);	// 스크린 이펙트 애니메이션 변경
	void					Draw(const char* strScreenEffectName);																	// 스크린 이펙트 그리기
	void					AniStop(const char* strScreenEffectName);
};
