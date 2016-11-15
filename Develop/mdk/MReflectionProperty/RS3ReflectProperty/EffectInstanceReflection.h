#pragma once

#include "BaseObject.h"
#include "SceneNodeCommonProperty.h"
#include "REffectInstance.h"

using namespace rs3;

class CEffectInstancePropertyRef : public CRTTIClass<CEffectInstancePropertyRef, CSceneNodeCommonProperty,SPID_EFFECT>
{
public:
	CEffectInstancePropertyRef() {}

	virtual void SetSceneNode(rs3::RSceneNode* pSceneNode)
	{ 
		CSceneNodeCommonProperty::SetSceneNode(pSceneNode);

		m_pEffectInstance =(REffectInstanceSceneNode*)pSceneNode; 
	}


	//------------------------------------------------------------------------------------------------------
	//Setter
	void SetAutoStart(bool bEnable)					{ m_pEffectInstance->SetAutoStart( bEnable ); }
	//------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------
	//Getter
	bool GetAutoStart()								{ return m_pEffectInstance->GetAutoStart(); }
	//------------------------------------------------------------------------------------------------------



	static void RegisterReflection()
	{
		RegisterProperty<bool>("자동 시작",&GetAutoStart,&SetAutoStart, 0, 0, "이펙트 자동시작 여부", "Effect 속성");
	}

private:
	REffectInstanceSceneNode* m_pEffectInstance;
};