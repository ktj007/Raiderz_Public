#pragma once

#include "RActor.h"
#include "RClothActorNode.h"
#include "MMemPool.h"

namespace rs3 
{
	class RClothWorldActorNode : public RClothActorNode, public MMemPool<RClothWorldActorNode>
	{
	public:
		RClothWorldActorNode(void);
		virtual ~RClothWorldActorNode(void);

	//--[ RActorNode에서의 함수 ]
	public:
		virtual bool	OnCreateDeviceDependentPart(void);	//오버라이딩
		virtual void	RenderPrimitive(int index);			//오버라이딩
		virtual void	SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj);

	//--[ RClothActorNode에서의 함수 ]
	protected:
		virtual void	RenderCollision(int index);

	//--[ RClothSimulator의 함수 ]
	protected:
		virtual void		resolveCollision(void);
		virtual	void		simulate(float timeStep);

	//--[ RClothWorldActorNode의 함수 ]
	protected:
		bool  		intersectWithPartitionedOBB( ROBB &obbForWorldShape );
		void		skinningAttachingPoints(void);
		void		warpCloth(void);

	private:
		RBox		m_clothAABBForDebugRender;
		bool		m_bIntersectWithPartitionedOBBForDebugRender;


	};
}//rs3