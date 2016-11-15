#ifndef _McvMain_h
#define _McvMain_h

class CMcvMain 
{
public:
	CMcvMain();
	virtual ~CMcvMain();

public:

	////////////////////////////////////////////////

	bool Create(HWND hWnd);
	RSceneManager* CreateSceneManager();
	void Restore();
	void Destroy();
	void Update();
	void Render();

	////////////////////////////////////////////////

	void UpdateKey(float time);
	void UpdateLightKey(float time);
	void UpdateCameraKey(float time);
	void UpdateDirLightMatchCamera();

	void CameraReset();

	void SetSpecularEnable( bool bEnable );
	void SetLightingOnlyEnable( bool bEnable );
	void SetDirLightMatchCamera( bool bMatch ) { m_bDirectionalLightMatchCamera = bMatch; }
	void SetDirLightDiffuseColor( DWORD dwR, DWORD dwG, DWORD dwB );
	void SetDirLightAmbientColor( DWORD dwR, DWORD dwG, DWORD dwB );
	void SetPointLightEnable( bool bEnable, unsigned int index );

	bool				IsFullScreen()			{ return m_bFullScreen; }
	bool				IsActive()				{ return m_bActive; }

	RDevice				*GetDevice()			{ return m_pDevice; }

	int					GetWidth()				{ return m_nWidth; }
	int					GetHeight()				{ return m_nHeight; }

	bool				RenderAllActorBoundBox( const RSceneNodeList& nodes);

	////////////////////////////////////////////////

private:
	void InitLight();
	RVector4 ConvertColor( DWORD dwR, DWORD dwG, DWORD dwB );

private:

	bool			m_bFullScreen;
	bool			m_bActive;
	int				m_nWidth;
	int				m_nHeight;

	RDevice*		m_pDevice;
	RSceneManager*	m_pManager;

public:

	RActor*			m_pActor;
	RActorNode*		m_pSelectedActorNode;

	static const int NUM_POINTLIGHTS = 3;
	RPointLightSceneNode*	m_pPointLights;

	RDirectionalLightSceneNode*	m_pDirectionalLight;
	bool			m_bDirectionalLightMatchCamera;

	CTCamera		m_Camera;

	float			m_fRotateZ;

	RVector			m_vLightPos;
	RVector			m_vMapObjPos;

	RVector			m_vModelScale;

	float			m_fWheel;
	int				m_nWheel;

	HWND			m_hWnd;

	bool			m_bShowBones;
	bool			m_bShowCollision;
};

#endif//_McvMain_h