#pragma once

#include "RD3DApplication.h"
#include "RFont.h"
#include "MWLua.h"
#include "RSceneManager.h"

using namespace rs3;

class FrameCounter
{
private:
	unsigned long m_nTime;
	unsigned long m_nFrameCounter;
	unsigned long m_nCurrFrame;
	unsigned long m_nLastTime;
public:
	FrameCounter() : m_nTime(0), m_nFrameCounter(0), m_nCurrFrame(0) { m_nLastTime = timeGetTime(); }
	int Update()
	{
		m_nFrameCounter++;
		unsigned long nNowTime = timeGetTime();

		m_nTime += (nNowTime - m_nLastTime);
		m_nLastTime = nNowTime;

		if (m_nTime >= 1000)
		{
			m_nCurrFrame = m_nFrameCounter;
			m_nFrameCounter = 0;
			m_nTime = 0;
		}

		return m_nCurrFrame;
	}
};

class MyApp : public RD3DApplication
{
protected:
	RFont				*m_pFont;
	MWLua*				m_pLua;
	FrameCounter		m_FrameCounter;

public:
	MyApp(void);
	virtual ~MyApp(void);

	RSceneManager*	GetSceneManager()	{	return RSceneNode::GetSceneManager(); }
	RDevice*		GetDevice()			{	return RSceneNode::GetDevice(); }

	virtual char*	GetName() { return "MintViewer"; }
	virtual bool	Create();

	virtual	void	OnInit();
	virtual bool	OnCreate();
	virtual void	OnUpdate();
	virtual void	OnRender();
	virtual void	OnDestroy();

	void			ChangeDirectoryToModule();
	bool			OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	MWLua*			GetLua() { return m_pLua; }
};
