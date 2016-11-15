#include "stdafx.h"
#include "XUIBalloonTextBox.h"
#include "RCameraSceneNode.h"
#include "RSUtil.h"
#include "XMyPlayer.h"
#include "XModuleCollision.h"

#define DEFAULT_WIDTH		160
#define DEFAULT_HEIGHT		60
#define BALLONTEXT_SHOW_DIATANCE	2000.f


// XUIBalloonTextBox
XUIBalloonTextBox::XUIBalloonTextBox( const char* szName, MWidget* pParent, MListener* pListener)
: MTextView( szName, pParent, pListener)
{
	m_bShow = false;
	m_bEnable = false;
	m_bStatic = true;

	SetSize( DEFAULT_WIDTH, DEFAULT_HEIGHT);
	SetLookAndFill( MDEFAULT_BALLOONTEXTBOXLOOK);
	SetWrapping( false);

	m_pScrollbar->SetAlwaysHide( true);

	m_tFade.SetReverse( true);
	m_tFade.SetArrivalTime( 300);
}

// PopupBalloon
void XUIBalloonTextBox::PopupBalloon( const wstring& strMessage, float fLifeTime)
{
	if (CheckDistance() == false) return;
	if (CheckPickLine() == false) return;

	wstring _str;
	XObject* pObject = gg.omgr->FindObject( m_UID);
	if (pObject == NULL)	return;

	_str = L"{COLOR r=255 g=255 b=128}";
	_str += pObject->GetName();
	_str += L"\n{COLOR r=160 g=160 b=160}";
	_str += strMessage;

	SetText( MLocale::ConvUTF16ToAnsi(_str.c_str()).c_str());

	MSIZE _size = GetPageSize();
	SetSize( DEFAULT_WIDTH, max( _size.h, DEFAULT_HEIGHT));

	Show( true);

	m_tLife.SetArrivalTime( (int)( fLifeTime * 1000.0f));
	m_tLife.Start();
}


// GetDrawState
int XUIBalloonTextBox::GetDrawState()
{
	if ( m_bShow == false  &&  m_tFade.IsArrival() == false)		return MDS_DRAW;
	return MWidget::GetDrawState();
}


// OnRun
void XUIBalloonTextBox::OnRun( DWORD dwCurrTime)
{
	MTextView::OnRun( dwCurrTime);


	// Set show/hide
	if ( m_tLife.IsArrival() == true)
	{
		Show( false);
		return;
	}


	// Set position
	if ( gg.omgr == NULL)	return;

	if (CheckDistance() == false) return;

	RePosition();
}


// OnPreDraw
void XUIBalloonTextBox::OnPreDraw( MDrawContext* pDC)
{
	float ratio = (float)m_tFade.GetRatio();
	pDC->SetOpacity( pDC->GetOpacity() * ratio);
}


// OnShow
void XUIBalloonTextBox::OnShow( bool bShow)
{
	MTextView::OnShow( bShow);

	m_tFade.SetReverse( !bShow);
}

void XUIBalloonTextBox::RePosition()
{
	RVector2 vScreen = GetBalloonPosition();
	if(vScreen == RVector2(0, 0))
		return;

	SetPosition( vScreen.x - (m_Rect.w >> 1), vScreen.y - m_Rect.h);
}

//거리와 카메라 체크
bool XUIBalloonTextBox::CheckDistance()
{
	RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
	if ( pCamera == NULL)	return false;

	XObject* pObject = gg.omgr->FindObject( m_UID);
	if ( pObject == NULL)	return false;

	// 카메라 뒤쪽에 있으면 hide 시키고 리턴
	vec3 vPos = pObject->GetPosition();
	vPos.z = 0.f;

	vec3 vCameraPos = pCamera->GetPosition();
	vCameraPos.z = 0.f;

	vec3 vMyPos = gvar.Game.pMyPlayer->GetPosition();
	vMyPos.z = 0.f;

	float fDist = vMyPos.DistanceTo( vPos);

	vec3 dir = (vPos - vCameraPos).Normalize();
	float fFront = pCamera->GetDirection().DotProduct( dir);

	if ( fDist > BALLONTEXT_SHOW_DIATANCE  ||  fFront < 0.f)
	{
		Show( false);
		return false;
	}

	return true;
}

bool XUIBalloonTextBox::CheckPickLine()
{
	XModuleCollision * pModuleCollision = gvar.Game.pMyPlayer->GetModuleCollision();
	if (pModuleCollision)
	{
		RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
		if ( pCamera == NULL)	return false;

		vec3 vPos = pCamera->GetPosition();
		vec3 vStart	= vPos;

		XObject* pObject = gg.omgr->FindObject( m_UID);
		if ( pObject == NULL) return false;

		vec3 vEnd	= pObject->GetPosition() + RVector(0,0,100);

		vec3 vOutPos;
		vec3 vRenderDir;
		vec3 vRenderUp;

		if(pModuleCollision->PickRayToTerrain(vStart, vEnd, vOutPos, vRenderUp))
		{
			return false;
		}
	}

	return true;
}

RVector2 XUIBalloonTextBox::GetBalloonPosition()
{
	RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
	if ( pCamera == NULL)	return RVector2(0, 0);

	XObject* pObject = gg.omgr->FindObject( m_UID);
	if ( pObject == NULL)	return RVector2(0, 0);

	RDevice *pDevice = REngine::GetDevicePtr();
	if ( pDevice == NULL)	return RVector2(0, 0);

	RVector2 vScreenNamePos;
	RVector vNamePos = pObject->GetNamePos();
	rs3::GetScreenFromWorld( &vScreenNamePos, &vNamePos, &pDevice->GetViewport(), &RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());

	// 충돌 캡슐이 1개인 경우에만 하자
	XModuleCollision* pModuleCollision = pObject->GetModuleCollision();
	if(pModuleCollision == NULL)
		return vScreenNamePos;

	CSMeshInfo * pMeshInfo = pModuleCollision->GetMeshInfo();
	if(pMeshInfo == NULL ||
		pMeshInfo->m_vecColCapsules.empty() ||
		pMeshInfo->m_vecColCapsules.size() > 1)
		return vScreenNamePos;

	CSColCapsule colCapsule = pMeshInfo->m_vecColCapsules[0];
	MCapsule calCapsule = pModuleCollision->CalCollisionCapsuleAnimation(&colCapsule, pObject);

	//////////////////////////////////////////////////////////////////////////
	// 계산
	RVector2 vScreenPos;
	RVector2 vScreenPos2;

	vec3 vCameraDir = pCamera->GetDirection();
	XMath::NormalizeZ0(vCameraDir);
	vec3 vRightDir = vCameraDir.CrossProduct(vec3::AXISZ);
	vec3 vFinalPos = pObject->GetPosition() + (vCameraDir * calCapsule.radius)/* + (vRightDir * colCapsule.m_Capsule.radius)*/;
	vFinalPos.z += calCapsule.height + (calCapsule.radius * 2.0f);

	rs3::GetScreenFromWorld( &vScreenPos, &vFinalPos, &pDevice->GetViewport(), &RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());

	// 높이값 조절
	//float fDot = pCamera->GetDirection().DotProduct(vec3::AXISZ);
	//if(fDot > 0.2f)
	{
		vec3 vTPos = pObject->GetPosition();
		vTPos.z += calCapsule.height + (calCapsule.radius * 3.0f);
		//vTPos.z += vNamePos.z;

		//float fFulValue = 1.0f - 0.2f;
		//float fCulValue = fDot - 0.2f;

		//float t = fCulValue / fFulValue;
		//if(t >= 1.0f)
		//	t = 1.0f;

		//vec3 vResult;
		//vFinalPos.InterpolateTo(vTPos, t, vResult);

		//vFinalPos = vResult;

		rs3::GetScreenFromWorld( &vScreenPos2, &vTPos, &pDevice->GetViewport(), &RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());
	}

	if(vScreenPos.y > vScreenPos2.y)
	{
		vScreenPos = vScreenPos2;
	}
		
	vScreenPos.y -= 5;

	return vScreenPos;
}
