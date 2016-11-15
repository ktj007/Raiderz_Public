#pragma once

#include "RActor.h"
#include "RActorNodePhysique.h"

#pragma push_macro("min")
#pragma push_macro("max")
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <vector>
#include <map>
#include <list>
#include <string>

#include "NxPhysics.h"

#pragma pop_macro("min")
#pragma pop_macro("max")


#define INFERRED_WORK

namespace rs3 {

class RPhysXClothController;
class RPhysXClothActorNode : public rs3::RActorNode
{
public:

	RPhysXClothActorNode(NxPhysicsSDK* pSDK, NxScene* pScene, NxCookingInterface* pCooking);
	~RPhysXClothActorNode();

	enum E_CLOTH_STATE
	{
		E_PHYSX = 0,
		E_CPU,

		E_NONE
	};

	void							ConnectClothMeshData(NxMeshData& rMeshData);

	void							ChangeClothState(E_CLOTH_STATE _eClothState);
	E_CLOTH_STATE					GetClothState(){ return m_eClothState; }


protected:

	virtual void					OnAdd();				// 액터에 더해질때 Controller 생성
	virtual void					OnDestroy();			// 액터로부터 Destory 당할때 Controller 소멸관리

	virtual void					UpdateTransformAndVertex();
	virtual void					SetVertexStream();
#ifdef INFERRED_WORK
	virtual void					SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial, bool bFogEnable, bool bBlurAlphaForce = false, bool bSetAboutLight = true);
#else
	virtual void					SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial, bool bFogEnable, bool bBlurAlphaForce = false);
#endif
	virtual void					SetNodeRenderStateForDepth(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial ) override;
			void					SetNodeRenderStateForCloth(  RCameraSceneNode* pCurrentCamera, RMaterial* pMaterial );

	virtual void					RenderPrimitive(int index);


private:

	E_CLOTH_STATE					m_eClothState;
	RPhysXClothController*			m_pController;

	NxPhysicsSDK*					m_pSDK;
	NxScene*						m_pScene;
	NxCookingInterface*				m_pCooking;

	NxMeshData						m_NxClothMeshData;		// interaction data buffer between PhysX and RealSpace

	void ClearNxClothMeshData()
	{
		m_NxClothMeshData.verticesPosBegin = NULL;
		m_NxClothMeshData.verticesNormalBegin = NULL;
	}

	RVector* GetNxClothVertexPosBegin()
	{
		_ASSERT(m_NxClothMeshData.verticesPosBegin != NULL);
		return (RVector*)m_NxClothMeshData.verticesPosBegin;
	}

	RVector* GetNxClothVertexNormalBegin()
	{
		_ASSERT(m_NxClothMeshData.verticesNormalBegin != NULL);
		return (RVector*)m_NxClothMeshData.verticesNormalBegin;
	}

	class RCPUClothMeshData
	{
	public:
		RCPUClothMeshData()
			: m_pOwnerClothActorNode(NULL)
			, m_pVertPos(NULL)
			, m_pVertNorm(NULL)
		{
		}

		~RCPUClothMeshData()
		{
			SAFE_DELETE_ARRAY(m_pVertPos);
			SAFE_DELETE_ARRAY(m_pVertNorm);
		}

		void Init(RPhysXClothActorNode* pOwnerCloth){ m_pOwnerClothActorNode = pOwnerCloth; }
		void UpdateVertex();

		RVector* GetCPUClothVertexPosBegin()
		{
			_ASSERT(m_pVertPos != NULL);
			return m_pVertPos;
		}

		RVector* GetCPUClothVertexNormalBegin()
		{
			_ASSERT(m_pVertNorm != NULL);
			return m_pVertNorm;
		}

	private:

		RPhysXClothActorNode*	m_pOwnerClothActorNode;

		RVector*				m_pVertPos;
		RVector*				m_pVertNorm;

	} m_ClothCPUMeshData;
};

}