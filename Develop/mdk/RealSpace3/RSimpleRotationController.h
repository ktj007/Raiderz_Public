#pragma once

// 특정 actornode 에 단순히 회전값을 주기 위해 만들어진 controller.
// model 좌표계로 축과 회전을 설정한다. 이후 필요하면 local, world 좌표계등으로 확장하자

#include <list>
#include "RSceneNodeController.h"

namespace rs3 {

class RS_API RRotationController : public RSceneNodeController
{
	RVector		m_axis;
	float		m_fAngle;
	RMatrix		m_matAffectedTransform;		// 실제로 돌린 변환을 담아둔다.

public:
	DECLARE_NODE_ID_STRING("RotationController");

	RRotationController(void);
	virtual ~RRotationController(void);

	virtual void Update(RCameraSceneNode* pCamera) override;

	void SetRotation(const RVector& axis, float fAngle);

	const RMatrix& GetAffectedTransform() { return m_matAffectedTransform; }

};

}