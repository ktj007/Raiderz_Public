#pragma once

#include "BaseObject.h"
#include "SceneNodeCommonProperty.h"
#include "RSpotLightSceneNode.h"

using namespace rs3;

class CSpotLightPropertyRef : public CRTTIClass<CSpotLightPropertyRef, CSceneNodeCommonProperty,SPID_SPOT_LIGHT>
{
public:
	CSpotLightPropertyRef() {}

	virtual void SetSceneNode(rs3::RSceneNode* pSceneNode)
	{ 
		CSceneNodeCommonProperty::SetSceneNode(pSceneNode);

		m_pSpotLight =(RSpotLightSceneNode*)pSceneNode; 
	}


	//------------------------------------------------------------------------------------------------------
	//Setter
	void SetMapFileName(const char* strFilePath)	{ m_pSpotLight->SetSpotLightTexture(strFilePath); }
	void SetFar(float fFar)							{ m_pSpotLight->SetFar(fFar); }
	void SetFov(float fFov)							{ m_pSpotLight->SetFov(fFov); }
	void SetAttenuationStart(float fAttenuStart)	{ m_pSpotLight->SetAttenuationStart(fAttenuStart); }
	void SetColor(DWORD dwColor)					{ m_pSpotLight->SetColorVector(ConvertRGBToVector3(dwColor)); }
	void SetIntensity(float fIntensity)				{ m_pSpotLight->m_fApplyIntensity =fIntensity; m_pSpotLight->m_fIntensity = fIntensity; }
	void SetShadowCast(bool bShadowCast)			{ m_pSpotLight->SetShadowCast(bShadowCast); }
	void SetUserRenderMinArea(bool bEnable)			{ m_pSpotLight->SetUseRenderMinArea(bEnable); }
	void SetRenderMinArea(float fMinArea)			{ m_pSpotLight->SetRenderMinArea(fMinArea); }
	//------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------
	//Getter
	const char* GetMapFileName()					{ return m_pSpotLight->GetSpotLightTextureFileName().c_str(); }
	float GetFar()									{ return m_pSpotLight->GetFar(); }
	float GetFov()									{ return m_pSpotLight->GetFov(); }
	float GetAttenuationStart()						{ return m_pSpotLight->GetAttenuationStart(); }
	DWORD GetColor()								{ return ConvertVector3ToRGB(m_pSpotLight->GetColorVector()); }
	float GetIntensity()							{ return m_pSpotLight->m_fIntensity; }
	bool GetShadowCast()							{ return m_pSpotLight->GetShadowCast(); }
	bool GetUserRenderMinArea()						{ return m_pSpotLight->GetUseRenderMinArea(); }
	float GetRenderMinArea()						{ return m_pSpotLight->GetRenderMinArea(); }
	//------------------------------------------------------------------------------------------------------



	static void RegisterReflection()
	{
		const char* strCategory = "Spot Light 속성";
		{
			RegisterProperty<const char*>("투영맵 파일",&GetMapFileName,&SetMapFileName, 0, 0, "투영맵 파일", strCategory, PE_FILENAME);
			RegisterProperty<float>("범위",&GetFar,&SetFar, 0, 0, "범위", strCategory);
			RegisterProperty<float>("조명각",&GetFov,&SetFov, 0, 0, "조명각", strCategory);
			RegisterProperty<float>("감쇠시작",&GetAttenuationStart,&SetAttenuationStart, 0, 0, "감쇠 시작 거리", strCategory);
			RegisterProperty<DWORD>("색",&GetColor,&SetColor, 0, 0, "색", strCategory, PE_COLOR);
			RegisterProperty<float>("세기",&GetIntensity,&SetIntensity, 0, 0, "세기", strCategory);
		}


		const char* strCategoryShadow = "그림자 속성";
		{
			RegisterProperty<bool>("그림자 생성",&GetShadowCast,&SetShadowCast, 0, 0, "그림자 생성 여부", strCategoryShadow);
			RegisterProperty<bool>("그림자 생성 최소넓이 사용",&GetUserRenderMinArea,&SetUserRenderMinArea, 0, 0, "그림자 생성 최소넓이 사용 여부", strCategoryShadow);
			RegisterProperty<float>("그림자 생성 최소넓이(%)",&GetRenderMinArea,&SetRenderMinArea, 0, 0, "Light 공간상에서 값보다 작은 오브젝트는 그림자가 생성되지 않음", strCategoryShadow);
		}
	}


private:
	RSpotLightSceneNode* m_pSpotLight;
};