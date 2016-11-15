#pragma once

#include <string>
#include "RTypes.h"

using namespace std;

namespace rs3
{

enum RSHADOWMETHOD;

class RBenchmarkConfig
{
public:
	RBenchmarkConfig();
	~RBenchmarkConfig();

	bool	ToggleNavigationMode() { m_bNavigationMode = !m_bNavigationMode; return m_bNavigationMode; }
	bool	GetNavigationMode() const { return m_bNavigationMode; }

	string	GetSceneFileName() const { return m_SceneFileName; }
	string	GetSceneEnvFileName() const { return m_SceneEnvFileName; }

	string	GetFilePathString() const { return m_FilePathString; }
	int		GetScreenWidth() const { return m_ScreenWidth; }
	int		GetScreenHeight() const { return m_ScreenHeight; }
	bool	GetFullScreen() const { return m_FullScreen; }

	void	SaveCameraTransform( RVector vPos, RVector vDir );
	RVector	GetCameraPosition() const { return m_CameraPosition; }
	RVector GetCameraDirection() const { return m_CameraDirection; }
	float	GetCameraFov() const { return m_CameraFov; }
	float	GetCameraAspectRatio() const { return m_CameraAspectRatio; }
	float	GetCameraNear() const { return m_CameraNear; }
	float	GetCameraFar() const { return m_CameraFar; }

	bool	GetShadowEnable() const { return m_ShadowEnable; }
	RSHADOWMETHOD GetShadowType() const;
	int		GetShadowTextureSize() const { return m_ShadowTextureSize; }
	int		GetShadowSplitCount() const { return m_ShadowSplitCount; }
	float	GetShadowSplitDistance( int index ) const { return m_ShadowSplitDistance[index]; }

	bool	GetHDREnable() const { return m_HDREnable; }

	bool	GetDOFEnable() const { return m_DOFEnable; }
	float	GetDOFFocusDist() const { return m_DOFFocusDist; }
	float	GetDOFFocusRange() const { return m_DOFFocusRange; }
	float	GetDOFCoCSize() const { return m_DOFCoCSize; }

	bool	GetDistortionEnable() const { return m_DistortionEnable; }
	bool	GetSoftParticleEnable() const { return m_SoftParticleEnable; }

	bool	GetSSAOEnable() const { return m_SSAOEnable; }
	int		GetSSAOQuality() const { return m_SSAOQuality; }

	bool	LoadConfig();
	bool	ReloadConfig();
	bool	SaveConfig();

private:
	static const char* KEYWORD_CONFIG_FILENAME;

	bool	ReadConfigFile( const string& fileName );
	float	GetCameraFov( int width, int height, float angle );

	string	m_ConfigFileName;

	// 네비게이션 모드
	bool	m_bNavigationMode;

	// 씬 설정
	string	m_SceneFileName;
	string	m_SceneEnvFileName;

	// 리소스 경로
	string	m_FilePathString;

	// 화면 설정
	int		m_ScreenWidth;
	int		m_ScreenHeight;
	bool	m_FullScreen;

	// 카메라 설정
	RVector	m_CameraPosition;
	RVector	m_CameraDirection;
	float	m_CameraFovAngle;
	float	m_CameraNear;
	float	m_CameraFar;
	float	m_CameraFov;
	float	m_CameraAspectRatio;

	// 그림자 설정
	bool	m_ShadowEnable;
	string	m_ShadowType;
	int		m_ShadowTextureSize;
	int		m_ShadowSplitCount;
	float	m_ShadowSplitDistance[4];

	// 포스트 이펙트 설정
	bool	m_HDREnable;

	bool	m_DOFEnable;
	float	m_DOFFocusDist;
	float	m_DOFFocusRange;
	float	m_DOFCoCSize;

	bool	m_DistortionEnable;
	bool	m_SoftParticleEnable;

	bool	m_SSAOEnable;
	int		m_SSAOQuality;
};

}