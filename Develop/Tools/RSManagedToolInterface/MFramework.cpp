#include "stdafx.h"
#include "MFramework.h"
#include "MDevice.h"
#include "MSceneManager.h"
#include "MHelper.h"
#include "MMeshMgr.h"

using namespace RSManaged;
using namespace System;

MFramework::MFramework() 
{
	m_pApplication = createToolApplication();
	m_pDevice = new MDevice();
	m_pSceneManager = new MSceneManager();
	m_pHelper = new MHelper();

	m_pDevice->Set((rs3::RDeviceD3D*)m_pApplication->GetDevice());
	m_pSceneManager->Set(m_pApplication->GetSceneManager());

	m_pMeshMgr = new MMeshMgr();
}
//---------------------------------------------------------------------------
void MFramework::Init()
{
    if (!ms_pkThis)
    {
		ms_pkThis = new MFramework();
	}
}
//---------------------------------------------------------------------------
void MFramework::Final()
{
    if (ms_pkThis)
    {
//		destroyToolApplication();
		ms_pkThis->m_pApplication = NULL;
		ms_pkThis = NULL;
	}
}

bool MFramework::InstanceIsValid()
{
	if (ms_pkThis != NULL) return true;
	return false;
}

MDevice* MFramework::get_Device()
{
	return m_pDevice;
}

MFramework* MFramework::get_Instance()
{
    assert(ms_pkThis != 0);
    return ms_pkThis;
}

MSceneManager* MFramework::get_SceneManager()
{
	return m_pSceneManager;
}

MHelper* MFramework::get_Helper()
{
	return m_pHelper;
}

MMeshMgr* MFramework::get_MeshMgr()
{
	return m_pMeshMgr;
}
