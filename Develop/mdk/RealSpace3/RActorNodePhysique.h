#pragma once

#include "RActorNode.h"
#include "MMemPool.h"

namespace rs3 {

///	RActorNodePhysique 는 CPU physique
class RS_API RActorNodePhysique : public RActorNode
{
	MDeclareRTTI;

protected:
	unsigned int					m_nLastUpdatedFrame;	///< 한 프레임 내의 중복업데이트를 막기 위한 변수

	//bool							m_bResetVertexAgainstMatrix;	// 베이스 매트릭스 Inv에 곱해졌는지 

public:
	explicit RActorNodePhysique(ACTOR_NODE_TYPE eActorNodeType);
	virtual ~RActorNodePhysique(void);

	virtual void					UpdateTransformAndVertex();
	virtual void					ResetTransform();

	/// cpu 로 physique 를 처리하는 펑션
	int								UpdatePoint_SoftPhysiqueAnimation();

	virtual void					RenderNormal(DWORD dwColor);	// 디버깅 인포 : 노멀
	virtual void					RenderWire(DWORD dwColor);		// 디버깅 인포 : 와이어

	virtual void					RenderPrimitive(int index);		// 마테이얼에 열견된 버텍스의 DP
	virtual void					RenderAllNodePrimitive();

	virtual bool					OnPick(RPICKINFO& pickInfo);

	virtual bool					Validate();

	//virtual bool					ResetVertexAgainstMatrix();		// 피직은 버텍스를 베이스 매트릭스 Inv에 곱해진 결과로 가지고 있는다. 

};

class RS_API RActorNodePhysiqueGPU : public RActorNodePhysique, public MMemPool<RActorNodePhysiqueGPU, 50000>
{
	friend RActor;

	MDeclareRTTI;

public:
	RActorNodePhysiqueGPU(void);
	virtual ~RActorNodePhysiqueGPU(void);

	virtual void					SetShaderTransformConstant(const RMatrix& matView, const RMatrix& matViewProj);

	virtual void					RenderPrimitive(int index);		// 마테이얼에 열견된 버텍스의 DP
	virtual void					RenderAllNodePrimitive();
};

}