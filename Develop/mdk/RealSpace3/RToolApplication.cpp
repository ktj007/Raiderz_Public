#include "StdAfx.h"
#include "RToolApplication.h"
#include "RDeviceD3D.h"
#include "MDebug.h"

namespace rs3 {

RToolApplication::RToolApplication(void) : m_bActive(false), m_pDevice(NULL), m_pSceneManager(NULL)
{
	MInitLog(MLOG_LEVEL_INFORMATION, MLOG_FILE|MLOG_DEBUG);
}

RToolApplication::~RToolApplication(void)
{

}

void RToolApplication ::Destroy()
{
	SAFE_DELETE(m_pSceneManager);
	SAFE_DELETE(m_pDevice);
	//RSetDevice(NULL);
}

bool RToolApplication::Create()
{
	m_bActive = true;

	m_pDevice = new RDeviceD3D();

	// by pok. 씬매니저 변경중
	//RSetDevice(m_pDevice);

	m_pSceneManager = new RSceneManager();
	if(!m_pSceneManager->Create(m_pDevice))
		return false;

	return true;
}

void RToolApplication ::Activate(bool bActive)
{
	m_bActive = bActive;
}

//void RToolApplication::Update()
//{
//	m_pBackgroundWorkManager->Update();
//	m_pSceneManager->Update();
//}
//
//void RToolApplication::Render()
//{
//
//}

}
