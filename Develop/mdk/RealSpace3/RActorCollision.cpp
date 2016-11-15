#include "stdafx.h"
#include "RRenderHelper.h"
#include "RActorCollision.h"
#include "RActor.h"


namespace rs3 {

RActorCollisionObject::RActorCollisionObject() : 
	m_matTransform(RMatrix::IDENTITY),
	m_pParent(NULL)
{
}


RActorCollision::RActorCollision()
{

}

RActorCollision::~RActorCollision()
{
	m_vCollisions.clear();
}

void RActorCollision::RenderCollisionObject(MCollision::MCollisionMeshType eType)
{
 	RRenderHelper::SetRenderState(false);

	RMatrix mTrans;
	RMatrix tt;

	for (size_t i=0; i<m_vCollisions.size(); i++)
	{
		RActorCollisionObject& obj = m_vCollisions[i];

		if(obj.m_MeshType != eType) continue;

//		mTrans = mTransform;
		mTrans = obj.m_matTransform * obj.m_pParent->GetWorldTransform();

		switch(obj.m_Type)
		{
		case MCollision::SPHERE :
			{
				RRenderHelper::RenderSphere(mTrans, obj.m_Sphere.radius, 0x66ff00ff);
			}
			break;

		case MCollision::CAPSULE :
			{
				RRenderHelper::RenderCapsule(mTrans, obj.m_Capsule, 0x66ff00ff);
			}
			break;

		case MCollision::BOX :
			{
				RRenderHelper::RenderBox(mTrans, obj.m_Box, 0x66ff00ff);
			}
			break;
		}
		
	}

	RRenderHelper::EndRenderState();

}

void RActorCollision::RenderCollisionObject()
{
	RenderCollisionObject(MCollision::MESHTYPE_NORMAL);
	RenderCollisionObject(MCollision::MESHTYPE_CLOTH);
}

void RActorCollision::Add(MCollision* pCollision, RSceneNode* pParent)
{
	RActorCollisionObject t;

	t.m_Type = pCollision->m_type;
	switch(pCollision->m_type)
	{
	case MCollision::SPHERE :
		{
			t.m_Sphere.radius = pCollision->m_fRadius;
			t.m_Sphere.center = RVector(0,0,0);
		}
		break;

	case MCollision::CAPSULE :
		{
			t.m_Capsule.bottom.Set(0, 0, pCollision->m_fRadius);
			t.m_Capsule.top.Set(0, 0, pCollision->m_fHeight - pCollision->m_fRadius);
			t.m_Capsule.radius = pCollision->m_fRadius;
			t.m_Capsule.height = pCollision->m_fHeight - pCollision->m_fRadius * 2;
		}
		break;

	case MCollision::BOX :
		{
			t.m_Box.vmin = RVector(-pCollision->m_fRadius / 2, -pCollision->m_fLength / 2, 0.0f);
			t.m_Box.vmax = RVector( pCollision->m_fRadius / 2,  pCollision->m_fLength / 2, pCollision->m_fHeight);
		}
		break;

	}

	t.m_matTransform		= pCollision->m_matLocal;
	t.m_pParent				= pParent;
	t.m_strRelatedNodeNames	= pCollision->m_strRelatedNodeNames;

	m_vCollisions.push_back(t);
	RActorCollisionObject* itrObject = &m_vCollisions.back();
	itrObject->m_MeshType = pCollision->m_meshType;
}

void RActorCollision::Add(MCollision::MCollisionType eCollisionType, MCollision::MCollisionMeshType eMeshType, float fRad, const RVector& b, const RVector& t, const RMatrix& tm,RSceneNode* pParent)
{
	// 항상 캐슐만 들어온다.
	RActorCollisionObject tmp;

	tmp.m_Type = eCollisionType;
	switch(eCollisionType)
	{
	case MCollision::SPHERE :
		{
			tmp.m_Sphere.radius = fRad;
			tmp.m_Sphere.center = RVector(0,0,0);
		}
		break;

	case MCollision::CAPSULE :
		{
			tmp.m_Capsule = RCapsule(b, t, fRad);
		}
		break;

	}
	
	tmp.m_matTransform	= tm;
	tmp.m_pParent		= pParent;

	m_vCollisions.push_back(tmp);
	RActorCollisionObject* itrObject = &m_vCollisions.back();
	itrObject->m_MeshType = eMeshType;
}

void RActorCollision::Update(RActor* pActor)
{

}

void RActorCollision::Destroy()
{

}

void RActorCollision::Remove( MCollision* pCollision, RSceneNode* pParent )
{
	struct SFuncRemovePredicator
	{
		SFuncRemovePredicator(RSceneNode* pParent) : m_pParent(pParent) {}

		bool operator()(const RActorCollisionObject& _rCollsionObject)
		{
			return (_rCollsionObject.m_pParent == m_pParent);
		}

		RSceneNode* m_pParent;
	};
	// remove - erase 합성, by Effective STL
	m_vCollisions.erase( std::remove_if(m_vCollisions.begin(), m_vCollisions.end(), SFuncRemovePredicator(pParent)) );
}

}
