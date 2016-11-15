/// @file WS_Shared.h
#pragma once

#include <map>
#include <vector>
#include <deque>
#include <string>

class CWorkSpaceManager;
class CPluginInterface;

struct SPluginInfo
{
	enum PLUGIN_STATE {
		LOADED,
		FAILED,
		UNLOADED
	};

	SPluginInfo() 
		: m_pPluginInterface( NULL )
		, m_state( UNLOADED )
		, m_hModule( 0 ) 
	{}

	CPluginInterface*	m_pPluginInterface;
	PLUGIN_STATE		m_state;
	HMODULE				m_hModule;
	string				m_strModuleName;
};

class CPluginInterface
{
	friend class CWorkSpaceManager;

public:
	virtual bool			Initialize(CWorkSpaceManager* pWorkSpaceManager) = 0;
	virtual void			OnCreateMainWindow() {}
	virtual void			Finalize(CWorkSpaceManager* pWorkSpaceManager) = 0;
	virtual HINSTANCE		GetInstanceHandle() = 0;

protected:
	CWorkSpaceManager*		m_pWorkSpaceManager;
};
