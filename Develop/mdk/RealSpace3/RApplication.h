#ifndef _RAPPLICATION_H
#define _RAPPLICATION_H

#include "MSingleton.h"
#include "RListener.h"
#include "RRenderingCallbackInterface.h"


#include <map>
using namespace std;

namespace rs3 {

class RS_API RApplication : public MSingleton <RApplication> , public RListener, public RRenderingCallbackInterface
{
private:
	void						CreateApplicationInfo();
protected:
	bool			m_bActive;

#ifdef WIN32
	HWND			m_hWnd;
	DWORD			m_dwMainThreadId;
#endif

	RDevice				*m_pDevice;
	RSceneManager		*m_pSceneManager;
	

	RDEVICESTATUS	m_lastDeviceStatus;		///< 디바이스 상태
	DWORD			m_dwClearColor;			///< 백버퍼 클리어 컬러


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 어플리케이션 업데이트 - 씬매니저 업데이트 - 씬 매니저 랜더 - 어플리케이션 OnPreRender/OnRender
	//
	void						Update();
	void						Render();


	// virtual 함수들. 어플리케이션이 각자 정의해서 쓴다
	virtual bool				OnInit()				{ return true; }
	virtual	bool				OnCreate()				{ return true; }
	virtual void				OnIdle()				{ }
	virtual void				OnActive(bool bActive)	{ }
	virtual void				OnPreUpdate()			{ }
	virtual void				OnUpdate()				{ }
	virtual void				OnPostUpdate()			{ }
	virtual void				OnPreRender()			{ }
	virtual void				OnRender()				{ }
	virtual void				OnPostRender()			{ }
	virtual void				OnPreFlip()				{ }
	virtual void				OnFlip()				{ }
	virtual void				OnDestroy()				{ }

	virtual void				OnLostDevice()			{ }
	virtual void				OnResetDevice()			{ }
	virtual bool				ResetDevice();

	// Create / Destroy
	virtual bool				CreateDevice() = 0;
	virtual void				DestroyDevice() = 0;

	// sceneManager
	virtual RSceneManager*		CreateSceneManager();
	virtual bool				OnCreateSceneManager();
	virtual void				OnDestroySceneManager();

	// update functions
	virtual void				UpdateTimer();
	virtual void				UpdateBackgroundWorker();

private:
	void PreFlip();
	void PostFlip();

public:
	RApplication();
	virtual ~RApplication();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Run : 부모가 기본적인 뼈대를 제공하고 RApplication을 상속받는 자식이 세부를 결정(Template Method)
	// - OnInit(자식)
	// - Create < CreateDevice(자식) - CreateSceneManager(자식) - OnCreateSceneManager(자식)  >
	// - OnCreate(자식)
	// - MainLoop(자식, 순수가상함수)
	// - Destroy() < OnDestroy(자식) - OnDestroySceneManager(자식) - DestroyDevice(자식)
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////

	int							Run();

	virtual bool				Create();

	virtual int					MainLoop() = 0;
	virtual void				Destroy();

	virtual void				Activate(bool bActive) = 0;
	bool						IsActive();
	bool						IsFullScreen();

	void						SetClearColor(DWORD dwColor);
	virtual char*				GetName() = 0;

#ifdef WIN32
	HWND						GetWindowHandle()		{ return m_hWnd; }
	void						GetSystemMemoryInfo(DWORD& dwTotalPhy, DWORD& dwAvailablePhy)
	{
		MEMORYSTATUS status; status.dwLength = sizeof(MEMORYSTATUS);
		::GlobalMemoryStatus(&status);
		dwTotalPhy = (DWORD)status.dwTotalPhys;
		dwAvailablePhy = (DWORD)status.dwAvailPhys;
	}
	bool						IsMainThread()			{ 	return m_dwMainThreadId == GetCurrentThreadId(); }
#endif

};

//----------------------------------------------------------------------------------------------------
inline bool RApplication::IsActive()					{ return m_bActive; }
//----------------------------------------------------------------------------------------------------
inline void	RApplication::SetClearColor(DWORD dwColor)	{ m_dwClearColor = dwColor; }
//----------------------------------------------------------------------------------------------------

}

#endif
