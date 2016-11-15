#include "stdafx.h"
#include "RClothParticle.h"

namespace rs3
{
	RClothParticle::RClothParticle(void)
		:oriP(RVector::ZERO),p(RVector::ZERO),newP(RVector::ZERO),v(RVector::ZERO),n(RVector::ZERO),
		color(RVector::ZERO),inverseMass(1.0f)
	{
	}

	RClothParticle::~RClothParticle(void)
	{
	}

}