#pragma once

#include "RTypes.h"
#include "RPType.h"

namespace rs3 {

class RBspline
{
	typedef vector<RVector>	lsPos;
	typedef lsPos::iterator	itPos;

public:
	lsPos		vPt		;						// Points
	RVector*	pCrv	;						// for Curve
	float		fS		;						// Step
	RMatrix		mtCd	;						// cardinal Matrix

	int			iC		;						// current vertex
	bool		bC		;

	float		m_fTime	;
	float		m_fLifeTime;

	int				iN;
	RTestVertex*	pVtx;

public:
	RBspline(void);
	virtual ~RBspline(void);

	void		Destroy();

	void		Generate(bool loop, std::vector<REmitterVariable> &vectorVars);
	void		Generate(int nCount, vec3* pVec);
	void		CurveUP();

	INT			VtxUp();
	RVector		Update(float fElapseTime);
	void		Render(RDevice* pDevice);
};

}
