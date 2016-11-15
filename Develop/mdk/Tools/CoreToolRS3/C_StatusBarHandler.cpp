#include "StdAfx.h"
#include "C_StatusBarHandler.h"
#include "VL_StatusBar.h"

CStatusBarHandler::CStatusBarHandler()
: m_mousePosX(0), m_mousePosY(0), m_mousePosZ(0), m_bUsingMousePos(true),
  m_cameraPosX(0.f), m_cameraPosY(0.f), m_cameraPosZ(0.f),
  m_cameraDirX(0.f), m_cameraDirY(0.f), m_cameraDirZ(0.f)
{
}

CStatusBarHandler::~CStatusBarHandler(void)
{
}

void CStatusBarHandler::ShowCurrentPickingAndCamera(const int x, const int y, const int z, bool bShowPicking)
{
	m_mousePosX = x;
	m_mousePosY = y;
	m_mousePosZ = z;

	m_bUsingMousePos = bShowPicking;
	UpdatePickingAndCameraInfo();
}

void CStatusBarHandler::ShowCurrentCameraPositionAndDirection(const float posX, const float posY, const float posZ, const float dirX, const float dirY, const float dirZ )
{
	m_cameraPosX = posX;
	m_cameraPosY = posY;
	m_cameraPosZ = posZ;

	m_cameraDirX = dirX;
	m_cameraDirY = dirY;
	m_cameraDirZ = dirZ;

	UpdatePickingAndCameraInfo();
}

void CStatusBarHandler::ShowCurrentCameraDOF(const float fDist, const float fRange, const float fCoC)
{
	m_dist = fDist;
	m_range = fRange;
	m_CoC = fCoC;

	UpdateDOFInfo();
}

void CStatusBarHandler::ShowStatusBarMessage(const char* _pMessage)
{
	if (!m_pStatusBar)
		return;

	m_pStatusBar->SetPaneText(/*0,*/ _pMessage);
}

void CStatusBarHandler::UpdatePickingAndCameraInfo()
{
	if (!m_pStatusBar)
		return;

	if (m_bUsingMousePos)
	{
		m_cstrContactPos.Format( "마우스 %d, %d, %d | 카메라 위치 %f, %f, %f | 카메라 방향 %f, %f, %f",
			m_mousePosX, m_mousePosY, m_mousePosZ, m_cameraPosX, m_cameraPosY, m_cameraPosZ, m_cameraDirX, m_cameraDirY, m_cameraDirZ);
	}
	else
	{
		m_cstrContactPos.Format( "카메라 위치 %f, %f, %f | 카메라 방향 %f, %f, %f",
			m_cameraPosX, m_cameraPosY, m_cameraPosZ, m_cameraDirX, m_cameraDirY, m_cameraDirZ);
	}

	static CString strLastMessage;
	if( strLastMessage != m_cstrContactPos )
	{
		m_pStatusBar->SetPaneText(/*0,*/ m_cstrContactPos);
		strLastMessage = m_cstrContactPos;
	}
}


void CStatusBarHandler::UpdateDOFInfo()
{
	if (!m_pStatusBar)
		return;

	m_cstrContactPos.Format( "DOF | 포커스 거리 %f, 포커스 범위 %f, 착란원 크기 %f", m_dist, m_range, m_CoC);
	m_pStatusBar->SetPaneText(/*0,*/ m_cstrContactPos);
}
