#include "stdafx.h"
#include "RClothSimulator.h"
#include "RClothInfo.h"
#include "RClothCollision.h"


namespace rs3
{
	RClothSimulator::RClothSimulator(void)
		:m_pClothInfo(0)
	{	
	}

	RClothSimulator::~RClothSimulator(void)
	{
		releaseClothInfo();
	}

	void RClothSimulator::integrateByExplicitEulerMethod( float timeStep )
	{
		_ASSERT( NULL != getClothInfo());

		RClothParticle *pParticle = getClothInfo()->getParticles();	_ASSERT(NULL != pParticle);
		RVector	vGravity = RVector(	getClothInfo()->getParameter()->getGravityX(),
									getClothInfo()->getParameter()->getGravityY(),
									getClothInfo()->getParameter()->getGravityZ()	);

		for(int i=0;i< getClothInfo()->getTheNumberOfParticles();i++)
		{
			if( pParticle[i].inverseMass == 0 )	continue;
			pParticle[i].v += timeStep*vGravity;
			pParticle[i].newP = pParticle[i].p + timeStep*pParticle[i].v;
			pParticle[i].v = RVector::ZERO;
		}
	}

	void RClothSimulator::resolveDistanceConstraint( void )
	{
		RClothParticle *pParticles = getClothInfo()->getParticles();

		float distanceStiffness				= getParameter()->getDistanceStiffness();
		float distanceRange					= getParameter()->getDistanceRange();

		_ASSERT( NULL != getClothInfo());
		for(int i=0;i<getClothInfo()->getTheNumberOfDistanceConstraints();i++)
		{
			RClothConstraint& clothConstraint = getClothInfo()->getDistanceConstraint(i);
			clothConstraint.solve_distance_constraint_that_is_to_move_paticles(pParticles,distanceStiffness,distanceRange);
		}

	}

	void RClothSimulator::resolveStrechingConstraint( void )
	{
		RClothParticle *pParticles = getClothInfo()->getParticles();

		float distanceStiffness				= getParameter()->getDistanceStiffness();
		float distanceRange					= getParameter()->getDistanceRange();
		float stretchHorizontalStiffness	= getParameter()->getStretchHorizontalStiffness();
		float stretchHorizontalRange		= getParameter()->getStretchHorizontalRange();
		float stretchVerticalStiffness		= getParameter()->getStretchVerticalStiffness();
		float getStretchVerticalRange		= getParameter()->getStretchVerticalRange();

		_ASSERT( NULL != getClothInfo());
		for(int i=0;i<getClothInfo()->getTheNumberOfStretchConstraints();i++)
		{
			RClothConstraint& clothConstraint = getClothInfo()->getStretchConstraint(i);
			if( clothConstraint.getConstraintType() == STRETCH_HORIZONTAL_CONSTRAINT )
			{
				clothConstraint.solve_stretch_constraint_that_is_to_move_paticles(pParticles,stretchHorizontalStiffness,stretchHorizontalRange);
			}
			else
			{
				clothConstraint.solve_stretch_constraint_that_is_to_move_paticles(pParticles,stretchVerticalStiffness,getStretchVerticalRange);
			}
			
		}
	}

	void RClothSimulator::resolveACollisionWithAllParticles( const RCapsule& capsule )
	{
		const RVector&	endPoint	= capsule.bottom;
		RVector			n			= capsule.top - capsule.bottom;
		float			length		= n.Length();	n.Normalize();
		float			r			= capsule.radius;

		RC_TRIANGLE*		pTriangles;
		int					theNumberOfTriangles;
		RClothParticle*		pParticles		= getClothInfo()->getParticles();

		pTriangles				= getClothInfo()->getType2_Triangles();
		theNumberOfTriangles	= getClothInfo()->getTheNumberOfType2_Triangles();
		for(int i=0;i<theNumberOfTriangles;i++)
		{
			const RC_VERTEX_INDEX& index0 = pTriangles[i].index0;
			const RC_VERTEX_INDEX& index1 = pTriangles[i].index1;
			const RC_VERTEX_INDEX& index2 = pTriangles[i].index2;

			//_ASSERT(  (pParticles[index0].color.x > ATTACHING_RED_VALUE));
			//_ASSERT(  (pParticles[index1].color.x > ATTACHING_RED_VALUE));
			//_ASSERT( !(pParticles[index2].color.x > ATTACHING_RED_VALUE));

			//RC_TRIANGLE_TYPE_2_RED
			RClothCollision::CapsuleConstraint(pParticles[index0].newP,pParticles[index2].newP,endPoint,n,length,r);
		}

		pTriangles				= getClothInfo()->getType1_Triangles();
		theNumberOfTriangles	= getClothInfo()->getTheNumberOfType1_Triangles();
		for(int i=0;i<theNumberOfTriangles;i++)
		{
			const RC_VERTEX_INDEX& index0 = pTriangles[i].index0;
			const RC_VERTEX_INDEX& index1 = pTriangles[i].index1;
			const RC_VERTEX_INDEX& index2 = pTriangles[i].index2;

		//	_ASSERT(  (pParticles[index0].color.x > ATTACHING_RED_VALUE));
		//	_ASSERT( !(pParticles[index1].color.x > ATTACHING_RED_VALUE));
		//	_ASSERT( !(pParticles[index2].color.x > ATTACHING_RED_VALUE));


			//RC_TRIANGLE_TYPE_1_RED
			RClothCollision::CapsuleConstraint(pParticles[index0].newP,pParticles[index1].newP,pParticles[index2].newP,endPoint,n,length,r);
		}

		pTriangles				= getClothInfo()->getType0_Triangles();
		theNumberOfTriangles	= getClothInfo()->getTheNumberOfType0_Triangles();
		for(int i=0;i<theNumberOfTriangles;i++)
		{
			const RC_VERTEX_INDEX& index0 = pTriangles[i].index0;
			const RC_VERTEX_INDEX& index1 = pTriangles[i].index1;
			const RC_VERTEX_INDEX& index2 = pTriangles[i].index2;

//			_ASSERT( !(pParticles[index0].color.x > ATTACHING_RED_VALUE));
//			_ASSERT( !(pParticles[index1].color.x > ATTACHING_RED_VALUE));
//			_ASSERT( !(pParticles[index2].color.x > ATTACHING_RED_VALUE));

			//RC_TRIANGLE_TYPE_0_RED
			RClothCollision::CapsuleConstraint(pParticles[index0].newP,pParticles[index1].newP,pParticles[index2].newP,endPoint,n,length,r);
		}
	}

	void RClothSimulator::integrateByVerletMethod( float timeStep )
	{
		_ASSERT( NULL != getClothInfo());

		RClothParticle *pParticle = getClothInfo()->getParticles();		_ASSERT(NULL != pParticle);

		for(int i=0;i< getClothInfo()->getTheNumberOfParticles();i++)
		{
			pParticle[i].p = pParticle[i].newP;
		}
	}

	void RClothSimulator::caculateVelocity(float timeStep)
	{
		_ASSERT( NULL != getClothInfo());

		RClothParticle *pParticle = getClothInfo()->getParticles();		_ASSERT(NULL != pParticle);

		for(int i=0;i< getClothInfo()->getTheNumberOfParticles();i++)
		{
			RVector lastV = pParticle[i].v;
			
			pParticle[i].v.x = getParameter()->getDampingCoefficientX() * (pParticle[i].newP.x - pParticle[i].p.x)/timeStep;
			pParticle[i].v.y = getParameter()->getDampingCoefficientY() * (pParticle[i].newP.y - pParticle[i].p.y)/timeStep;
			pParticle[i].v.z = getParameter()->getDampingCoefficientZ() * (pParticle[i].newP.z - pParticle[i].p.z)/timeStep;

			dampingJigglingVelocity(&lastV,&pParticle[i].v);
		}
	}

	void RClothSimulator::dampingJigglingVelocity( RVector *pLastV,RVector *pCurrentV )
	{
		if( pLastV->DotProduct(*pCurrentV) >= 0 )	return;

		//서로 반대방향이면
		(*pCurrentV) = getParameter()->getDampingJigglingVelocity() * (*pCurrentV);	//많이 damping
	}

	void RClothSimulator::caculateClothAABB(RBox *pAABB)
	{
		_ASSERT(NULL != pAABB);

		pAABB->Initialize();
		RClothParticle *pParticle = getClothInfo()->getParticles();	_ASSERT(NULL != pParticle);
		for(int i=0;i< getClothInfo()->getTheNumberOfParticles();i++)
		{
			if( pParticle[i].inverseMass == 0 )	continue;
			pAABB->Add(pParticle[i].newP);
		}
	}

	void RClothSimulator::transformParticles( RMatrix &matClothWorld )
	{
		RPFC_THISFUNC;

		RClothParticle*		pParticles		= getClothInfo()->getParticles();
		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			pParticles[i].p		= pParticles[i].oriP * matClothWorld;
			pParticles[i].newP	= pParticles[i].p;
		}
	}

	void RClothSimulator::transformAttachingParticles(const RMatrix &matClothWorld )
	{
		RPFC_THISFUNC;

		RClothParticle*		pParticles		= getClothInfo()->getParticles();
		for(int i=0;i<getClothInfo()->getTheNumberOfParticles();i++)
		{
			if( pParticles[i].inverseMass != 0.0f )	continue;
			pParticles[i].p		= pParticles[i].oriP * matClothWorld;
			pParticles[i].newP	= pParticles[i].p;
		}
	}

	RClothInfo* RClothSimulator::allocateClothInfo( void )
	{
		releaseClothInfo();
		RClothInfo *pClothInfo = new RClothInfo;
		return pClothInfo;
	}

	void RClothSimulator::releaseClothInfo( void )
	{
		if( NULL != m_pClothInfo )
		{
			delete m_pClothInfo;
			m_pClothInfo = NULL;
		}
	}

	void RClothSimulator::simulateResolvingLogic(float timeStep)
	{
		RPFC_THISFUNC;

		char*	szResolvingLogic	= getClothInfo()->getParameter()->getResolvingLogic();	_ASSERT( NULL != szResolvingLogic );
		int		sizeOfResolvingLogic = strlen(szResolvingLogic);

		for(int i=0;i<sizeOfResolvingLogic;i++)
		{
			switch(szResolvingLogic[i])
			{
				case 'd':
				case 'D':
				{
					resolveDistanceConstraint();
				}break;

				case 's':
				case 'S':
				{
					resolveStrechingConstraint();
				}break;

				case 'v':
				case 'V':
				{
					caculateVelocity(timeStep);			
				}break;

				case 'c':
				case 'C':
				{
					resolveCollision();			
				}break;
				
				default:
				{
					_ASSERT(false);
				}
			}//switch
		}//for
	}//end of function


}




