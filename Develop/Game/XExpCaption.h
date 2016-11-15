#pragma once

#include "XCharacterCaption.h"

class XExpCaption : public XCombatCaption
{
	friend class XCaptionMgr;	// 생성 / 소멸은 매니저를 통해
private:
protected:
	virtual bool OnUpdate(float fDelta, RMatrix& viewInverse);
	void UpdatePosition( const float fElapsedTime, const float fDelta );
	DWORD UpdateColor( const float fElapsedTime );
public:
	XExpCaption(MID nID, rs3::RSceneNode* pSceneNode);
	virtual ~XExpCaption() {}
	void Setup(int nExp, vec3& pos);
};
