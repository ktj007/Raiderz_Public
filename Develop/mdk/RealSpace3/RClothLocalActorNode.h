#pragma once

#include "RActor.h"
#include "RClothActorNode.h"

namespace rs3 
{
	class RClothLocalActorNode : public RClothActorNode, public MMemPool<RClothLocalActorNode>
	{
	public:
		RClothLocalActorNode(void);
		virtual ~RClothLocalActorNode(void);

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

	//--[ RClothLocalActorNode 의 함수 ]
	protected:
		void		skinningAttachingPoints(void);
		
		void		virtuallyTranslate(void);
		void		getVirtualTranslation(RVector *pVirtualTranslation);
		void		translateMovableClothParticles(RVector *pVirtualTranslation);

	private:
		RBox		m_clothAABBForDebugRender;

	};
}//rs3