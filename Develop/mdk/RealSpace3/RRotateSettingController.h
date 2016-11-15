#pragma once

#include "RSceneNodeController.h"

namespace rs3 {

// Enable() 될 때마다 지정된 축과 회전 범위 내에서 랜덤하게 로컬 회전값을 지정해준다.
class RRotateSettingController : public RSceneNodeController
{
public:
	DECLARE_NODE_ID_STRING( "RotateSettingController" );

	RRotateSettingController( RVector vRotateAxis, float fRotateMin, float fRotateMax );

	float	GetRotateMin() { return m_fRotateMin; }
	float	GetRotateMax() { return m_fRotateMax; }

protected:
	virtual void	OnAdd() override;
	virtual void	OnRemove() override;

	virtual void	OnEnable() override;
	virtual void	OnDisable() override;

private:
	void	ResetLocalTransform();

	RVector	m_vDirectionOrg;
	RVector	m_vRotateAxis;
	float	m_fRotateMin;
	float	m_fRotateMax;
};

}