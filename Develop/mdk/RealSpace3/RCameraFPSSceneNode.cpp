#include "StdAfx.h"

#include "mmsystem.h"

#include "RCameraFPSSceneNode.h"
#include "RDevice.h"
#include "RD3DApplication.h"

namespace rs3 {

RCameraFPSSceneNode::RCameraFPSSceneNode(void) : RCameraSceneNode(), m_nLastTime(0), m_fMoveSpeed(200.0f), m_fRotateSpeed(0.01f)
{
	m_fCameraX = -.5f*MMath::PI;
	m_fCameraZ = .5f*MMath::PI;
}

RCameraFPSSceneNode::~RCameraFPSSceneNode(void)
{

}

void RCameraFPSSceneNode::OnPreRender( RCameraSceneNode* pCurrentCamera)
{
	if (REngine::GetSceneManager().GetPrimaryCamera() != this) return;

	unsigned long int nNowTime = timeGetTime();
	if (m_nLastTime == 0) m_nLastTime = timeGetTime();
	float fDelta = (nNowTime - m_nLastTime) / 1000.0f;
	m_nLastTime = nNowTime;

	float fRotateStep= fDelta;
	float fMoveStep= fDelta * m_fMoveSpeed;

	if(IsKeyDown(VK_LEFT))	m_fCameraZ+=fRotateStep;
	if(IsKeyDown(VK_RIGHT)) m_fCameraZ-=fRotateStep;
	if(IsKeyDown(VK_UP))	m_fCameraX-=fRotateStep;
	if(IsKeyDown(VK_DOWN))	m_fCameraX+=fRotateStep;

	int iMouseDeltaX = 0, iMouseDeltaY = 0;
#ifdef WIN32
	// D3DApplication이 아니면 대략 낭패 - bird
	HWND hWnd = RApplication::GetInstance().GetWindowHandle();
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	iMouseDeltaX = pt.x - REngine::GetDevice().GetScreenWidth()/2;
	iMouseDeltaY = pt.y - REngine::GetDevice().GetScreenHeight()/2;

	{
		POINT pt={REngine::GetDevice().GetScreenWidth()/2, REngine::GetDevice().GetScreenHeight()/2};
		ClientToScreen(hWnd, &pt);
		SetCursorPos(pt.x,pt.y);
	}
#endif
	m_fCameraZ += (-iMouseDeltaX * m_fRotateSpeed);
	m_fCameraX += (iMouseDeltaY * m_fRotateSpeed);
	m_fCameraX = max(0.01f, min(MMath::PI-0.01f, m_fCameraX));


	RVector newDirection = RVector(cosf(m_fCameraZ)*sinf(m_fCameraX),
						sinf(m_fCameraZ)*sinf(m_fCameraX),
						cosf(m_fCameraX));


	RVector dir = newDirection;
	RVector right = RVector(0,0,1).CrossProduct(dir);
	right.Normalize();

	RVector posDiff(0,0,0);

	if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
	if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
	if(IsKeyDown('A')) posDiff+=fMoveStep*right;
	if(IsKeyDown('D')) posDiff-=fMoveStep*right;
	if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);

	RVector newPosition = GetPosition() + posDiff;
//	SetPosition(newPosition);
//	m_position += posDiff;

	SetTransform(newPosition,newDirection);
}

bool RCameraFPSSceneNode::IsKeyDown(int vKey)
{
#ifdef WIN32
	return ((GetAsyncKeyState(vKey) & 0x8000)!=0);
#endif

	return false;
}

}