#pragma once

#include "RTypes.h"
#include "RPType.h"
#include "RD3DVertexUtil.h"

namespace rs3 {

/**
 	클래스 이름과 다르게, Position의 배치에 따라서 Cardinal 혹은 Linear 을 선택해서 보간한다.
 */
class RCardinal
{
	typedef vector<RVector>	lsPos;
	typedef lsPos::iterator	itPos;

private:
	lsPos		vPt		;						// Points
	float		fT		;						// Tension
	float		fS		;						// Step
	RMatrix		mtCd	;						// cardinal Matrix
	bool		bC		;

	float		m_fTime	;
	float		m_fLifeTime;

	int			iN;
	std::vector<RVector>	vCurves;			// for Curve

	void		CurveUP();

public:
	RCardinal(void);
	virtual ~RCardinal(void);

	void		Destroy();
	void		Generate(bool loop, const std::vector<REmitterVariable> &vectorVars, float fLifeTime);

	void		Reset();
	RVector		Update(float fElapseTime);
	void		RenderPath(RDevice *pdevice);
};

}
