#pragma once

#include <vector>
#include <string>

#include "MXml.h"

namespace rs3 {

class RS_API RUnitTestRunner
{
public:

	//////////////////////////////////////////////////////////////////////////
	// 싱글턴
	//
	static RUnitTestRunner& GetInstance()
	{
		static RUnitTestRunner runner;
		return runner;
	}

	//////////////////////////////////////////////////////////////////////////
	// Init / Halt / Reset
	//
	void		Init(const char* szAppName, const char* szScreenShotFolder, const char* szOutXMLFileName, bool bMakeReferrenceImageMode);
	void		Destroy();
	void		InitLog(const char* szLogFolder, const char* szLogFileName);
	void		FinalizeLog();

	bool		InitEngine(HWND _hWnd, bool bConsole, bool bUseScreenShot);
	void		HaltEngine(bool bNeedToResetForNewCreation = true);
	void		ResetEngine();
	void		AddDirectionalLight();
	void		DelDirectionalLight();

	void		InitMockEngine();
	void		HaltMockEngine(bool bNeedToResetForNewCreation = true);
	void		ResetMockEngine();


	//////////////////////////////////////////////////////////////////////////
	// 타이머 업데이트
	//
	void		ResetTimer();
	void		ResetCamera();
//	void		UpdateTimer(float fDelta);
	void		UpdateTimerForcely(DWORD _dwElapsedTime);


	//////////////////////////////////////////////////////////////////////////
	// 씬매니저 업데이트
	//
	void		UpdateScene(bool bUpdateTimer);
	void		GrabScreenShot(const char* _pSzFileNamePreFix,RCameraSceneNode* pCamera = NULL);
	void		UpdateSceneAndFlipDevice(bool bUpdateTimer = false);

	//////////////////////////////////////////////////////////////////////////
	// 스크릿샷 관련 ,, 위의것도 내리고 싶다
	void		GetImageFileName( char* szImageFilename, const char* szImageName);
	void		GetRefImageFileName( char* szRefImageFilename, const char* szImageName);

	bool		TestScreenShot(const char* szFileName);
	void		CompareImages( const char* szRefImage, const char* szNewImage, int* pDiffPixels, int* pTotalPixels);

	void		SaveImageFile(const char* _pSzFileNamePreFix, RTexture* pTexture);

	int			GetFailedShotCount();

private:

	RUnitTestRunner()
		: m_bMock(false)
		, m_bConsole(false)
		, m_bUseScreenShot(false)
		, m_bMakeReferrenceImageMode(false)
		, m_hWnd(0)
	{
	}

	bool		m_bMock;
	HWND		m_hWnd;
	bool		m_bConsole;
	bool		m_bUseScreenShot;
	bool		m_bMakeReferrenceImageMode;

	string		m_strAppName;
	string		m_strOutXMLFileName;
	string		m_strScreenShotFolder;
	int			m_nFailShotCount;

	MXml		m_xml;
	MXmlElement *m_pRootElement;	///< 스샷 결과 출력용 XML

	bool		IsEnableScreenShot(){ return (m_bUseScreenShot && m_bMock == false); }
};

inline int RUnitTestRunner::GetFailedShotCount() { return m_nFailShotCount; }

}