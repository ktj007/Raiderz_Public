#pragma once

#include "RClothInfo.h"

namespace rs3
{
	class RS_API RClothSimulator
	{
	public:
		RClothSimulator(void);
		~RClothSimulator(void);

	protected:
		virtual		void		simulate(float timeStep) = 0;
		void		simulateResolvingLogic(float timeStep);
		void		integrateByExplicitEulerMethod(float timeStep);
			
		void		resolveDistanceConstraint(void);
		void		resolveStrechingConstraint(void);
		virtual		void	resolveCollision(void) = 0;

		void		resolveACollisionWithAllParticles(const RCapsule& capsule);

		void		integrateByVerletMethod(float timeStep);
			void		dampingJigglingVelocity(RVector *pLastV,RVector *pCurrentV);
		void		caculateVelocity(float timeStep);

	protected:
		RClothInfo*	allocateClothInfo(void);
		void		releaseClothInfo(void);

		void	transformParticles( RMatrix &matClothWorld );
		void	transformAttachingParticles(const RMatrix &matClothWorld );
		void	caculateClothAABB(RBox *pAABB);

	public://set,get
		inline		void				setClothInfo(RClothInfo* pClothInfo)	{ m_pClothInfo = pClothInfo; }
		inline		RClothInfo*			getClothInfo(void)	const				{ return m_pClothInfo;	}
		inline		RClothParameter*	getParameter(void) const				{ return m_pClothInfo->getParameter();	}

	private:
		RClothInfo*	m_pClothInfo;
	};
}