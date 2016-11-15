#pragma once

#include "MDebug.h"
#include "RD3DApplication.h"
#include "NVPerfUtility.h"

namespace rs3
{

//class RFont;
//class RTextSceneNode;
class RCameraSceneNode;
class RWorldSceneNode;
class RWeatherEffectSet;
class REnvironmentSet;
class RBenchmarkConfig;

class RBenchmarkApp : public RD3DApplication, NVPerfRenderCaller
{
public:
	RBenchmarkApp();
	virtual ~RBenchmarkApp();

	virtual char* GetName() override;

	virtual bool OnInit() override;
	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate() override;
	virtual void OnPreRender() override;
	virtual void OnRender() override;
	virtual void OnPostRender() override;

	virtual int	 MainLoop() override;
	virtual bool OnWndMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pReturn ) override;

	virtual void CallRender() override { Render(); }

private:
	void SaveCameraTransform();
	void LoadCameraTransform();

	void Config();
	void SaveConfig();
	void ReloadConfig();
	void SetScreenConfig( bool bResetDevice );
	void SetCameraConfig();
	void SetShadowConfig();
	void SetPostEffectConfig();

	void ShowHelpMessage();

	void CreateCameraSceneNode( RWorldSceneNode* pWorldSceneNode );
	void DestroyCameraSceneNode();

	void CreateFontAndTextSceneNode();
	void DestroyFontAndTextSceneNode();
	void SetText( const char* pText );

	void CreateScene( const string& sceneFileName );
	void DestroyScene();

	void CreateEnvironment( const string& envFileName );
	void DestroyEnvironment();

	bool CreatePerformanceMonitor();
	void DestroyPerformanceMonitor();

	void ToggleNavigationMode();
	void UpdateCameraParam();
	void CameraMove();

	//RFont*				m_pFont;
	//RTextSceneNode*		m_pTextSceneNodeNode;
	RCameraSceneNode*	m_pCameraSceneNode;
	RWorldSceneNode*	m_pWorldSceneNode;
	RWeatherEffectSet*	m_pWeatherEffectSet;
	REnvironmentSet*	m_pEnvSet;

	RBenchmarkConfig*	m_pBenchmarkConfig;

	NVPerf*				m_pNVPerf;
	bool				m_bBeginNVPerf;

	float				m_fCameraX;
	float				m_fCameraZ;
};

}