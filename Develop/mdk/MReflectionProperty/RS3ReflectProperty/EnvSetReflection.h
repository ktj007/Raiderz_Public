#pragma once

#include "BaseObject.h"
#include "SceneNodeCommonProperty.h"
#include "REnvironment.h"

using namespace rs3;

class CEnvSetRef: public CRTTIClass<CEnvSetRef, CBaseObject, SPID_ENVSET>
{
public:
	CEnvSetRef() {}

	void	SetEnvSet( REnvironment::SEnvSet* pEnvSet ) { _ASSERT(pEnvSet); m_pEnvSet = pEnvSet; }

	// Light
	DWORD	GetAmbient() { return ConvertVector4ToRGB(m_pEnvSet->vAmbient); }
	void	SetAmbient( DWORD dwAmbient ) { m_pEnvSet->vAmbient = ConvertRGBToVector4(dwAmbient); }

	DWORD	GetDiffuse() { return ConvertVector4ToRGB(m_pEnvSet->vDiffuse); }
	void	SetDiffuse( DWORD dwDiffuse ) { m_pEnvSet->vDiffuse = ConvertRGBToVector4(dwDiffuse); }

	float	GetPower() { return m_pEnvSet->fPower; }
	void	SetPower( float fPower ) { m_pEnvSet->fPower = fPower; }

	float	GetShadowLuminosity() { return m_pEnvSet->fShadowLuminosity; }
	void	SetShadowLuminosity( float fShadowLuminosity ) { m_pEnvSet->fShadowLuminosity = fShadowLuminosity; }

	float	GetSubLightIntensity() { return m_pEnvSet->fSubLightIntensity; }
	void	SetSubLightIntensity(float fSubLightIntensity) { m_pEnvSet->fSubLightIntensity = fSubLightIntensity; }

	// Fog
	bool	GetFogEnable() { return !m_pEnvSet->bFogDisable; }
	void	SetFogEnable( bool bEnable ) { m_pEnvSet->bFogDisable = !bEnable; }

	DWORD	GetFogColor() { return ConvertVector4ToRGB(m_pEnvSet->vFogColor); }
	void	SetFogColor( DWORD dwFogColor ) { m_pEnvSet->vFogColor = ConvertRGBToVector4(dwFogColor); }

	float	GetFogNear() { return m_pEnvSet->fNear; }
	void	SetFogNear( float fFogNear ) { m_pEnvSet->fNear = fFogNear; }

	float	GetFogFar() { return m_pEnvSet->fFar; }
	void	SetFogFar( float fFogFar ) { m_pEnvSet->fFar = fFogFar; }

	// Wind
	float	GetWindDirectionX() { return m_pEnvSet->vWindDirection.x; }
	float	GetWindDirectionY() { return m_pEnvSet->vWindDirection.y; }
	float	GetWindDirectionZ() { return m_pEnvSet->vWindDirection.z; }
	void	SetWindDirectionX( float fWindDirectionX ) { m_pEnvSet->vWindDirection.x = fWindDirectionX; }
	void	SetWindDirectionY( float fWindDirectionY ) { m_pEnvSet->vWindDirection.y = fWindDirectionY; }
	void	SetWindDirectionZ( float fWindDirectionZ ) { m_pEnvSet->vWindDirection.z = fWindDirectionZ; }

	float	GetWindStrength() { return m_pEnvSet->fWindStrength; }
	void	SetWindStrength( float fWindStrength ) { m_pEnvSet->fWindStrength = fWindStrength; }

	// Cloud Shadow
	bool	GetCloudVisible() { return m_pEnvSet->m_bCloudVisible; }
	void	SetCloudVisible( bool bCloudVisible ) { m_pEnvSet->m_bCloudVisible = bCloudVisible; }

	float	GetCloudShadowTranslateU() { return m_pEnvSet->m_vCloudShadowTranslate.x; }
	float	GetCloudShadowTranslateV() { return m_pEnvSet->m_vCloudShadowTranslate.y; }
	void	SetCloudShadowTranslateU( float fCloudShadowTranslateU ) { m_pEnvSet->m_vCloudShadowTranslate.x = fCloudShadowTranslateU; }
	void	SetCloudShadowTranslateV( float fCloudShadowTranslateV ) { m_pEnvSet->m_vCloudShadowTranslate.y = fCloudShadowTranslateV; }

	float	GetCloudShadowScaleU() { return m_pEnvSet->m_vCloudShadowScale.x; }
	float	GetCloudShadowScaleV() { return m_pEnvSet->m_vCloudShadowScale.y; }
	void	SetCloudShadowScaleU( float fCloudShadowScaleU ) { m_pEnvSet->m_vCloudShadowScale.x = fCloudShadowScaleU; }
	void	SetCloudShadowScaleV( float fCloudShadowScaleV ) { m_pEnvSet->m_vCloudShadowScale.y = fCloudShadowScaleV; }

	// Grass
	float	GetGrassJigglingPeriod() { return m_pEnvSet->m_fGrassJigglingPeriod; }
	void	SetGrassJigglingPeriod( float fGrassJigglingPeriod ) { m_pEnvSet->m_fGrassJigglingPeriod = fGrassJigglingPeriod; }

	float	GetGrassJigglingStrength() { return m_pEnvSet->m_fGrassJigglingStrength; }
	void	SetGrassJigglingStrength( float fGrassJigglingStrength ) { m_pEnvSet->m_fGrassJigglingStrength = fGrassJigglingStrength; }

	float	GetGrassIncliningStrength() { return m_pEnvSet->m_fGrassIncliningStrength; }
	void	SetGrassIncliningStrength( float fGrassIncliningStrength ) { m_pEnvSet->m_fGrassIncliningStrength = fGrassIncliningStrength; }

	// HDR
	float	GetHDRBrightPassThresholdR() { return m_pEnvSet->vHDRBrightPassThreshold.x; }
	float	GetHDRBrightPassThresholdG() { return m_pEnvSet->vHDRBrightPassThreshold.y; }
	float	GetHDRBrightPassThresholdB() { return m_pEnvSet->vHDRBrightPassThreshold.z; }
	void	SetHDRBrightPassThresholdR( float fHDRBrightPassThresholdR ) { m_pEnvSet->vHDRBrightPassThreshold.x = fHDRBrightPassThresholdR; }
	void	SetHDRBrightPassThresholdG( float fHDRBrightPassThresholdG ) { m_pEnvSet->vHDRBrightPassThreshold.y = fHDRBrightPassThresholdG; }
	void	SetHDRBrightPassThresholdB( float fHDRBrightPassThresholdB ) { m_pEnvSet->vHDRBrightPassThreshold.z = fHDRBrightPassThresholdB; }

	float	GetHDRBloomBlurDeviation() { return m_pEnvSet->fHDRBloomBlurDeviation; }
	void	SetHDRBloomBlurDeviation( float fHDRBloomBlurDeviation ) { m_pEnvSet->fHDRBloomBlurDeviation = fHDRBloomBlurDeviation; }

	float	GetHDRBloomBlurMultiplier() { return m_pEnvSet->fHDRBloomBlurMultiplier; }
	void	SetHDRBloomBlurMultiplier( float fHDRBloomBlurMultiplier ) { m_pEnvSet->fHDRBloomBlurMultiplier = fHDRBloomBlurMultiplier; }

	static void RegisterReflection()
	{
		{
			const char* strCategory = "Light 속성";
			RegisterProperty<DWORD>( "Ambient 색", &GetAmbient, &SetAmbient, 0, 0, "Ambient 색", strCategory, PE_COLOR );
			RegisterProperty<DWORD>( "Diffuse 색", &GetDiffuse, &SetDiffuse, 0, 0, "Diffuse 색", strCategory, PE_COLOR );
			RegisterProperty<float>( "세기", &GetPower, &SetPower, 0, 0, "세기", strCategory );
			RegisterProperty<float>( "그림자 최소밝기", &GetShadowLuminosity, &SetShadowLuminosity, 0, 0, "그림자 최소밝기", strCategory );
			RegisterProperty<float>( "보조 라이트 세기", &GetSubLightIntensity, &SetSubLightIntensity, 0, 0, "보조 라이트 세기", strCategory );
		}
		{
			const char* strCategory = "Fog 속성";
			RegisterProperty<bool>( "Fog 켜기", &GetFogEnable, &SetFogEnable, 0, 0, "Fog 켜기", strCategory );
			RegisterProperty<DWORD>( "Fog 색", &GetFogColor, &SetFogColor, 0, 0, "Fog 색", strCategory, PE_COLOR );
			RegisterProperty<float>( "Fog Near", &GetFogNear, &SetFogNear, 0, 0, "Fog Near", strCategory );
			RegisterProperty<float>( "Fog Far", &GetFogFar, &SetFogFar, 0, 0, "Fog Far", strCategory );
		}
		{
			const char* strCategory = "HDR 속성";
			RegisterProperty<float>( "블룸 경계값 R", &GetHDRBrightPassThresholdR, &SetHDRBrightPassThresholdR, 0, 0, "블룸 경계값 R", strCategory );
			RegisterProperty<float>( "블룸 경계값 G", &GetHDRBrightPassThresholdG, &SetHDRBrightPassThresholdG, 0, 0, "블룸 경계값 G", strCategory );
			RegisterProperty<float>( "블룸 경계값 B", &GetHDRBrightPassThresholdB, &SetHDRBrightPassThresholdB, 0, 0, "블룸 경계값 B", strCategory );
			RegisterProperty<float>( "블룸 블러 정도", &GetHDRBloomBlurDeviation, &SetHDRBloomBlurDeviation, 0, 0, "값이 클수록 흐려진다.", strCategory );
			RegisterProperty<float>( "블룸 블러 세기", &GetHDRBloomBlurMultiplier, &SetHDRBloomBlurMultiplier, 0, 0, "블러 결과값에 곱하는 수치. 값이 클수록 진해진다.", strCategory );
		}
		{
			const char* strCategory = "Wind 속성";
			RegisterProperty<float>( "Wind 방향 X", &GetWindDirectionX, &SetWindDirectionX, 0, 0, "Wind 방향 X", strCategory );
			RegisterProperty<float>( "Wind 방향 Y", &GetWindDirectionY, &SetWindDirectionY, 0, 0, "Wind 방향 Y", strCategory );
			RegisterProperty<float>( "Wind 방향 Z", &GetWindDirectionZ, &SetWindDirectionZ, 0, 0, "Wind 방향 Z", strCategory );
			RegisterProperty<float>( "Wind 세기", &GetWindStrength, &SetWindStrength, 0, 0, "Wind 세기", strCategory );
		}
		{
			const char* strCategory = "풀 속성";
			RegisterProperty<float>( "떨림 주기", &GetGrassJigglingPeriod, &SetGrassJigglingPeriod, 0, 0, "떨림 주기", strCategory );
			RegisterProperty<float>( "떨림 세기", &GetGrassJigglingStrength, &SetGrassJigglingStrength, 0, 0, "떨림 세기", strCategory );
			RegisterProperty<float>( "기울여짐 세기", &GetGrassIncliningStrength, &SetGrassIncliningStrength, 0, 0, "기울여짐 세기", strCategory );
		}
		{
			const char* strCategory = "구름 그림자 속성";
			RegisterProperty<bool>( "구름 그림자 켜기", &GetCloudVisible, &SetCloudVisible, 0, 0, "구름 그림자 켜기", strCategory );
			RegisterProperty<float>( "U Ani", &GetCloudShadowTranslateU, &SetCloudShadowTranslateU, 0, 0, "U Ani", strCategory );
			RegisterProperty<float>( "V Ani", &GetCloudShadowTranslateV, &SetCloudShadowTranslateV, 0, 0, "V Ani", strCategory );
			RegisterProperty<float>( "U Scale", &GetCloudShadowScaleU, &SetCloudShadowScaleU, 0, 0, "U Scale", strCategory );
			RegisterProperty<float>( "V Scale", &GetCloudShadowScaleV, &SetCloudShadowScaleV, 0, 0, "V Scale", strCategory );
		}
	}

protected:
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

	REnvironment::SEnvSet* m_pEnvSet;
};