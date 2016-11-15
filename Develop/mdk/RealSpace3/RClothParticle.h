#pragma once

#include "RTypes.h"

namespace rs3
{
	class RClothParticle
	{
	public:
		RClothParticle(void);
		~RClothParticle(void);

	public:
		rs3::RVector	oriP;			//처음위치
		rs3::RVector	p;				//위치
		rs3::RVector	newP;			//다음속도
		rs3::RVector	v;				//속도
		rs3::RVector	n;				//노말
		rs3::RVector    color;			//색깔
		float			inverseMass;	// 1/질량

	};
}
