#pragma once

#include "XBaseApplication.h"
#include "MockPlayFrame.h"

class MockGameApp : public XBaseApplication
{
protected:
	virtual void			CreateGlobal() {}
public:
	MockGameApp()
	{
		m_nMode = EM_GAME;
	}
	virtual ~MockGameApp() {}
	virtual bool			OnCreate() 
	{ 
		return true; 
	}
	virtual void			OnDestroy() {}

	void Update()
	{
		OnUpdate();
	}
};