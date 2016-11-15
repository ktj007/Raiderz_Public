#pragma once

class CCoreToolStatusBar;

// statusbar handler
class CORETOOL_API CStatusBarHandler
{
public:

	static CStatusBarHandler* GetInstance()
	{
		static CStatusBarHandler kStatusBarHandler;
		return &kStatusBarHandler;
	}

	void InitStatusBar(CCoreToolStatusBar*	pStatusBar){ m_pStatusBar = pStatusBar; }
	void InvalidateStatusBar(){ m_pStatusBar = NULL; }

	void ShowCurrentPickingAndCamera(const int x, const int y, const int z, bool bShowPicking);
	void ShowCurrentCameraPositionAndDirection(const float posX, const float posY, const float posZ, const float dirX, const float dirY, const float dirZ );
	void ShowCurrentCameraDOF( float f1, float f2, float f3);
	void ShowStatusBarMessage(const char* _pMessage);


private:

	// CMFCHelper에서 관리
	CStatusBarHandler(void);
	~CStatusBarHandler(void);

	void UpdatePickingAndCameraInfo();
	void UpdateDOFInfo();

	CCoreToolStatusBar*	m_pStatusBar;
	CString m_cstrContactPos;

	int m_mousePosX;
	int m_mousePosY;
	int m_mousePosZ;
	bool m_bUsingMousePos;

	float m_cameraPosX;
	float m_cameraPosY;
	float m_cameraPosZ;

	float m_cameraDirX;
	float m_cameraDirY;
	float m_cameraDirZ;

	float m_dist;
	float m_range;
	float m_CoC;
};
