#include "stdafx.h"
#include "RClothConstraint.h"
#include "RRenderHelper.h"

#pragma warning( disable : 4482 )  // 비표준 확장이 사용되었습니다. 정규화된 이름에 '~~:~~~~' 열거형을 사용했습니다.

namespace rs3
{
	RClothConstraintStiffness& RClothConstraintStiffness::operator=( const RClothConstraintStiffness& rhs )
	{
		distanceStiffness = rhs.distanceStiffness;
		stretchHorizontalStiffness = rhs.stretchHorizontalStiffness;
		stretchVerticalStiffness = rhs.stretchVerticalStiffness;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------

	RClothConstraint::RClothConstraint(void)
		:m_type(INVALID_CONSTRAINT),m_restDistance(0.0f)
	{
		m_particleIndex0 = m_particleIndex1 = 0;
	}

	RClothConstraint::~RClothConstraint(void)
	{
	}


	void RClothConstraint::solve_distance_constraint_that_is_to_move_paticles(RClothParticle* pParticles,float distanceStiffness,float distanceRange)
	{
		_ASSERT( NULL != pParticles	);
		RClothParticle	&particle1 = getParticle(pParticles,m_particleIndex1);
		RClothParticle	&particle0 = getParticle(pParticles,m_particleIndex0);
		RVector			&p1	= particle1.newP;
		RVector			&p0	= particle0.newP;

		if( (particle1.inverseMass + particle0.inverseMass) == 0.0 )	return;

		RVector delta = p1 - p0;
		float deltaLength = delta.Length();
		if( (deltaLength - getRestDistance()) != 0.0f )
		{
			float diff = (deltaLength - getRestDistance()) / (deltaLength*( particle1.inverseMass + particle0.inverseMass ) );
			p0 += (distanceStiffness * particle0.inverseMass*diff) * delta;
			p1 -= (distanceStiffness * particle1.inverseMass*diff) * delta;
		}
	}

	void RClothConstraint::solve_stretch_constraint_that_is_to_move_paticles(RClothParticle* pParticles,float stretchStiffness,float stretchRange)
	{
		_ASSERT( NULL != pParticles	);
		//RClothParticle	&particle1 = *m_pParticles[1];
		//RClothParticle	&particle0 = *m_pParticles[0];
		RClothParticle	&particle1 = getParticle(pParticles,m_particleIndex1);
		RClothParticle	&particle0 = getParticle(pParticles,m_particleIndex0);

		RVector			&p1	= particle1.newP;
		RVector			&p0	= particle0.newP;

		if( (particle1.inverseMass + particle0.inverseMass) == 0.0 )	return;

		RVector delta = p1 - p0;
		float deltaLength = delta.Length();
		if( deltaLength > 0.0f && deltaLength < getRestDistance()*stretchRange ) //미는힘
		//if( (deltaLength - getRestDistance()) != 0.0f )
		{
			float diff = (deltaLength - getRestDistance()) / (deltaLength*( particle1.inverseMass + particle0.inverseMass ) );
			p0 += (stretchStiffness * particle0.inverseMass*diff) * delta ;
			p1 -= (stretchStiffness * particle1.inverseMass*diff) * delta;
		}
	}

/*
	void RClothConstraint::setParticles( RClothParticle* pParticle0,RClothParticle* pParticle1 )
	{
		_ASSERT(NULL != pParticle0);
		_ASSERT(NULL != pParticle1);
		m_pParticles[0] = pParticle0;
		m_pParticles[1] = pParticle1;
	}*/


	void RClothConstraint::setParticleIndices( RC_VERTEX_INDEX index0,RC_VERTEX_INDEX index1 )
	{
		m_particleIndex0 = index0;
		m_particleIndex1 = index1;
	}


	void RClothConstraint::DebugDraw( RClothParticle* pParticles)
	{
		RMatrix mat(RMatrix::IDENTITY);

		RClothParticle	&particle1 = getParticle(pParticles,m_particleIndex1);
		RClothParticle	&particle0 = getParticle(pParticles,m_particleIndex0);

		RVector			&p1	= particle1.p;
		RVector			&p0	= particle0.p;

		RRenderHelper::RenderLine(mat,p0,p1,0xff00ff00);
	}	
}

