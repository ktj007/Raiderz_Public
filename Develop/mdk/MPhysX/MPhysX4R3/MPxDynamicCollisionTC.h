#pragma once

#include "MPxTransformController.h"
#include "MPxDynamicObjectEntity.h"

namespace physx
{

class MPxDynamicCollisionTC : public MPxTcImpl<MPxDynamicCollisionTC>
{
public:
	MPxDynamicCollisionTC(MPxTcEntity* pTcEntity);
	virtual ~MPxDynamicCollisionTC();


	//////////////////////////////////////////////////////////////////////////
	// MPxTransformController Interface
	//
	virtual void						Update(float fDelta);
	virtual void						Stop();


	//////////////////////////////////////////////////////////////////////////
	// 충돌 활성 / 비활성
	//
	void								ActivateCollision();
	void								DeActivateCollision();

	void								SetCollisionPrefix(const std::string& _rStrPrefix){ m_strCollisionPrefix = _rStrPrefix; }

private:

	MPxObjDynamic*						m_pPxObjectDynamicBody;
	MPxDynamicObjectEntity*				m_pDynamicBodyEntity;
	std::string							m_strCollisionPrefix;
};

}
