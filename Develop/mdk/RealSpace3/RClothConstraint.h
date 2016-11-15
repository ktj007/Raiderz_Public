#pragma once

#include "RTypes.h"
#include "RClothType.h"
#include "RClothParticle.h"

namespace rs3
{
	enum ConstraintType
	{
		INVALID_CONSTRAINT = 0,
		DISTANCE_COSTRAINT = 1,
		STRETCH_HORIZONTAL_CONSTRAINT = 2,
		STRETCH_VERTICAL_CONSTRAINT = 3,
	};

	class RClothConstraintStiffness
	{
	public:
		RClothConstraintStiffness()
			:distanceStiffness(1.0f),stretchHorizontalStiffness(1.0f),stretchVerticalStiffness(1.0)
		{
		}
		
		RClothConstraintStiffness& operator=(const RClothConstraintStiffness& rhs);
	public:
		float	distanceStiffness;
		float	stretchHorizontalStiffness;
		float	stretchVerticalStiffness;
	};


	class RClothConstraint
	{
	public:
		RClothConstraint(void);
		~RClothConstraint(void);

	public:
		void solve_distance_constraint_that_is_to_move_paticles(RClothParticle* pParticles,float distanceStiffness,float distanceRange);
		void solve_stretch_constraint_that_is_to_move_paticles(RClothParticle* pParticles,float stretchHorizontalStiffness,float stretchHorizontalRange);

	public://Set,Get
		inline	ConstraintType	getConstraintType(void)	const			{	return m_type;	}
		inline	void			setConstraintType(ConstraintType type)	{	m_type =  type;	}

		inline	float			getRestDistance(void)		const			{	return m_restDistance;			}
		inline	void			setRestDistance(float distance)				{	m_restDistance = distance;	}

				//void			setParticles(RClothParticle* pParticle0,RClothParticle* pParticle1);
				void			setParticleIndices(RC_VERTEX_INDEX  index0,RC_VERTEX_INDEX  index1);
		//inline	RClothParticle* getParticle0(void)		const			{	return m_pParticles[0]; }
		//inline	RClothParticle* getParticle1(void)		const			{	return m_pParticles[1]; }

		inline	RClothParticle& getParticle(RClothParticle* pParticles,RC_VERTEX_INDEX index)
		{
			return pParticles[index];
		}

		void DebugDraw(RClothParticle* pParticles);

	private:
		//RClothParticle*	m_pParticles[2];
		//RC_VERTEX_INDEX	m_particleIndices[2];

		RC_VERTEX_INDEX	m_particleIndex0;
		RC_VERTEX_INDEX	m_particleIndex1;
		ConstraintType	m_type;
		float			m_restDistance;
	};
}
