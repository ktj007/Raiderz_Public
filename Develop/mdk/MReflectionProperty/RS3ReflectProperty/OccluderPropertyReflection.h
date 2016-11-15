#pragma once

#include "BaseObject.h"
#include "SceneNodeCommonProperty.h"
#include "ROccluderSceneNode.h"

using namespace rs3;

class COccluderPropertyRef : public CRTTIClass<COccluderPropertyRef, CSceneNodeCommonProperty,SPID_OCCLUER>
{
public:
	COccluderPropertyRef(void):m_pOccluderSceneNode(NULL) {}

	virtual void SetSceneNode(rs3::RSceneNode* pSceneNode)
	{ 
		CSceneNodeCommonProperty::SetSceneNode(pSceneNode);

		m_pOccluderSceneNode =(ROccluderSceneNode*)pSceneNode; 
	}

	//------------------------------------------------------------------------------------------------------
	//Setter
	void	SetLocalScaleX(float x)	{	RVector3 L = m_pOccluderSceneNode->GetLocalScale(); L.x = x; m_pOccluderSceneNode->SetLocalScale(L);	}
	void	SetLocalScaleY(float y)	{	RVector3 L = m_pOccluderSceneNode->GetLocalScale(); L.y = y; m_pOccluderSceneNode->SetLocalScale(L);	}
	void	SetLocalScaleZ(float z)	{	RVector3 L = m_pOccluderSceneNode->GetLocalScale(); L.z = z; m_pOccluderSceneNode->SetLocalScale(L);	}
	//------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------
	//Getter
	float	GetLocalScaleX(void)	{	return m_pOccluderSceneNode->GetLocalScale().x;	}
	float	GetLocalScaleY(void)	{	return m_pOccluderSceneNode->GetLocalScale().y;	}
	float	GetLocalScaleZ(void)	{	return m_pOccluderSceneNode->GetLocalScale().z;	}
	//------------------------------------------------------------------------------------------------------



	static void RegisterReflection()
	{
		{
			const char* strCategory= "오클루더 Local Scale";
			RegisterProperty<float>("Local Scale X",&GetLocalScaleX,&SetLocalScaleX, 0, 0, "Local Sacle X값", strCategory);
			RegisterProperty<float>("Local Scale Y",&GetLocalScaleY,&SetLocalScaleY, 0, 0, "Local Sacle Y값", strCategory);
			RegisterProperty<float>("Local Scale Z",&GetLocalScaleZ,&SetLocalScaleZ, 0, 0, "Local Sacle Z값", strCategory);
		}
	}

private:
	ROccluderSceneNode* m_pOccluderSceneNode;
};