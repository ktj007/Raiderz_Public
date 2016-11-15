#pragma once

#include "XMouseBase.h"
#include "XEffectInvoker.h"

class XMouseMove : public XMouseBase
{
private:
	vector<EFFECT_RESULT_DATA>	m_vecEffectData;

private:
	bool CheckMoveTerrain(vec3 vPos, vec3 vDir, vec3& vOut, vec3& vNor);

	void DrawMovePoint(vec3 vPos);
	void DelMovePoint();

public:
	XMouseMove(XObject* pOwner):XMouseBase(pOwner) {}
	virtual ~XMouseMove() {}

	bool OnClick(int x, int y, vec3& vMove);
};