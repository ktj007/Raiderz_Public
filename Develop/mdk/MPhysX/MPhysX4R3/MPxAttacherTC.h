#pragma once

#include "MPxTransformController.h"

namespace rs3 { class RActor; }

namespace physx
{

class MPxAttacherTC : public MPxTcImpl<MPxAttacherTC>
{
private:

	rs3::RActor*	m_pActorDest;

	// Src 본에서 Dest 본으로 붙이는 겁니다.
	string			m_strDestBoneName;					// 붙일곳 본 이름
	string			m_strSrcBoneName;					// 붙이기 위한 본 이름
	float			m_fInterpolation;
	float			m_fElapsed;

	// bone transform에 대한 offset
	MMatrix			m_matOffsetTransform;

public:
	MPxAttacherTC(MPxTcEntity* pTcEntity)
		: MPxTcImpl(pTcEntity), m_pActorDest(NULL)
		, m_fInterpolation(-1.f), m_fElapsed(0.f)
	{}

	virtual ~MPxAttacherTC(){}

	virtual void	Update(float fDelta);
	virtual void	Stop(){ EndAttacher(); }

	bool			StartAttacher(rs3::RActor* pActorDest, const char* chDestBoneName, const char* chSrcBoneName, float fInterpolation, const MMatrix& matOffsetTransform);
	void			EndAttacher();
};

}
