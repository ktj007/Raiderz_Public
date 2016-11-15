#pragma once

#include "M_LogicObject.h"

class CControlMediator;
class CControlSceneProperty;
class CLogicObjectRenderer;
class CSceneNodeCommonProperty;

/** @brief CLogicObject_AABBPicking - aabb 로 피킹하는 LogicObject */

class CORETOOL_API CLogicObject_AABBPicking : public CLogicObject
{
public:
	CLogicObject_AABBPicking();

	// 상속받아 구현
	virtual void	OnRender() override {}

	virtual void	OnSelected_(bool bSelected) override {}
	
	virtual CLogicObject*	OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to) override;
	virtual void			OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum) override;

	void			SetAABB( rs3::RBoundingBox& aabb );
};
