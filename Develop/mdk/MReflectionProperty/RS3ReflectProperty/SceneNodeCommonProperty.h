#pragma once

#include "BaseObject.h"

using namespace rs3;

#define COMMON_CATEGORY "기본 속성"

enum SceneNodePropertyID
{
	SPID_COMMON =1,
	SPID_DIRECTIONAL_LIGHT,
	SPID_POINT_LIGHT,
	SPID_SPOT_LIGHT,
	SPID_DISTORTION,
	SPID_EFFECT,
	SPID_EFFECT_BASE,
	SPID_EMITTER,
	SPID_SUB_LIGHT,
	SPID_ENVSET,
	SPID_OCCLUER
};

class CSceneNodeCommonProperty: public CRTTIClass<CSceneNodeCommonProperty, CBaseObject,SPID_COMMON>
{
public:
	CSceneNodeCommonProperty() {}

	void SetName(char* name)	{ m_pSceneNode->SetNodeName(std::string(name)); }
	void SetPositionX(float fX) { m_vPosition.x = fX; m_pSceneNode->SetPosition(m_vPosition); }
	void SetPositionY(float fY) { m_vPosition.y = fY; m_pSceneNode->SetPosition(m_vPosition); }
	void SetPositionZ(float fZ) { m_vPosition.z = fZ; m_pSceneNode->SetPosition(m_vPosition); }

	void SetDirectionX(float fX) { m_vDirection.x = fX; SetDirection(); }
	void SetDirectionY(float fY) { m_vDirection.y = fY; SetDirection(); }
	void SetDirectionZ(float fZ) { m_vDirection.z = fZ; SetDirection(); }

	void SetUpX(float fX) { m_vUp.x = fX; SetDirection(); }
	void SetUpY(float fY) { m_vUp.y = fY; SetDirection(); }
	void SetUpZ(float fZ) { m_vUp.z = fZ; SetDirection(); }

	void SetScale(float fScale)	{ m_pSceneNode->SetScale(RVector(fScale,fScale,fScale)); }


	char* GetName()		 { return const_cast<char*>(m_pSceneNode->GetNodeName().c_str()); }
	float GetPositionX() { return m_vPosition.x; }
	float GetPositionY() { return m_vPosition.y; }
	float GetPositionZ() { return m_vPosition.z; }

	float GetDirectionX() { return m_vDirection.x; }
	float GetDirectionY() { return m_vDirection.y; }
	float GetDirectionZ() { return m_vDirection.z; }

	float GetUpX() { return m_vUp.x; }
	float GetUpY() { return m_vUp.y; }
	float GetUpZ() { return m_vUp.z; }

	float GetScale() { return max(max(m_pSceneNode->GetScale().x,m_pSceneNode->GetScale().y),m_pSceneNode->GetScale().z); }

	static void RegisterReflection()
	{
		RegisterProperty<char*>("이름",&GetName,&SetName, 0, 0, "이름", COMMON_CATEGORY);
		RegisterProperty<float>("위치(X)",&GetPositionX,&SetPositionX, 0, 0, "위치(X)", COMMON_CATEGORY);
		RegisterProperty<float>("위치(Y)",&GetPositionY,&SetPositionY, 0, 0, "위치(Y)", COMMON_CATEGORY);
		RegisterProperty<float>("위치(Z)",&GetPositionZ,&SetPositionZ, 0, 0, "위치(Z)", COMMON_CATEGORY);

		RegisterProperty<float>("방향(X)",&GetDirectionX,&SetDirectionX, 0, 0, "방향(X)", COMMON_CATEGORY);
		RegisterProperty<float>("방향(Y)",&GetDirectionY,&SetDirectionY, 0, 0, "방향(Y)", COMMON_CATEGORY);
		RegisterProperty<float>("방향(Z)",&GetDirectionZ,&SetDirectionZ, 0, 0, "방향(Z)", COMMON_CATEGORY);

		RegisterProperty<float>("Up(X)",&GetUpX,&SetUpX, 0, 0, "Up 벡터(X)", COMMON_CATEGORY);
		RegisterProperty<float>("Up(Y)",&GetUpY,&SetUpY, 0, 0, "Up 벡터(Y)", COMMON_CATEGORY);
		RegisterProperty<float>("Up(Z)",&GetUpZ,&SetUpZ, 0, 0, "Up 벡터(Z)", COMMON_CATEGORY);

		RegisterProperty<float>("스케일",&GetScale,&SetScale, 0, 0, "스케일", COMMON_CATEGORY);

	}

	virtual void SetSceneNode(RSceneNode* pSceneNode)
	{
		m_pSceneNode = pSceneNode;

		m_vPosition = m_pSceneNode->GetPosition();
		m_vDirection = m_pSceneNode->GetDirection();
		m_vUp = m_pSceneNode->GetUp();
	}

protected:
	void SetDirection() { m_pSceneNode->SetDirection(m_vDirection, m_vUp); }


	RVector4 ConvertRGBToVector4(DWORD dwColor) { return RVector4(ConvertRGBToVector3(dwColor), 1); }
	RVector3 ConvertRGBToVector3(DWORD dwColor)
	{
		float fR = ((float)( GetRValue( dwColor )))/255.f;
		float fG = ((float)( GetGValue( dwColor )))/255.f;
		float fB = ((float)( GetBValue( dwColor )))/255.f;

		return RVector3(fR, fG, fB);
	}

	DWORD ConvertVector4ToRGB(RVector4 vColor) { return ConvertVector3ToRGB(RVector3(vColor.x, vColor.y, vColor.z)); }
	DWORD ConvertVector3ToRGB(RVector3 vColor)
	{
		WORD wR = (WORD)(vColor.x*255);
		WORD wG = (WORD)(vColor.y*255);
		WORD wB = (WORD)(vColor.z*255);

		return RGB(wR, wG, wB);
	}

	


	RSceneNode* m_pSceneNode;

	RVector m_vPosition;
	RVector m_vDirection;
	RVector m_vUp;
};