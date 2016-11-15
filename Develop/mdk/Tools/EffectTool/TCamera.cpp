#include "TCamera.h"
#include "RRenderHelper.h"
#include "MProfiler.h"

using namespace rs3;

namespace EffectTool
{

#define INIT_DIST 200.f
#define INIT_ROTX -15

const float FOV = 70.f;

TCamera::TCamera()
{
	m_fRotX		= INIT_ROTX;
	m_fRotY		= 0.f;
	m_fDist		= INIT_DIST;

	m_fNearZ	= 10.f;
	m_fFarZ		= 10000.f;

	SetPosition(0.f,-m_fDist,150.f);
	SetDirection(RVector(0.f,1.f,0.f));
	m_vUp = RVector(0.f,0.f,1.f);
	m_vTargetPos= RVector(0.f,0.f,0.f);
}

TCamera::~TCamera() 
{ 
}

void TCamera::ResetSize() 
{
	SetFov(FOV);
	UpdateData();
}

void TCamera::UpdateData() 
{
	RVector vRot;

	RMatrix rot;
	rot.SetRotationMatrix( MMath::PI/180.f*m_fRotX, 0.f, MMath::PI/180.f*m_fRotY );
	vRot = RVector(0.f,-1.f,0.f) * rot;

	vRot.Normalize();

	SetPosition(m_vTargetPos + (vRot * m_fDist));

	RVector vDir = m_vTargetPos - GetPosition();
	vDir.Normalize();
	SetDirection(vDir);

	SetNearFarZ(m_fNearZ,m_fFarZ);
}

void TCamera::Reset(RVector vPos /* = RVector(0.f,0.f,90.f) */)
{
	m_fDist = INIT_DIST;

	m_vTargetPos = vPos;

	m_fRotX = INIT_ROTX;
	m_fRotY = 0.f;

	UpdateData();
}

RVector TCamera::GetRight() 
{
	RVector right = GetDirection().CrossProduct(m_vUp);
	return right.Normalize();
}

void TCamera::Up(float f)	{ m_vTargetPos.z += f; }
void TCamera::Down(float f)	{ m_vTargetPos.z -= f; }
void TCamera::Front(float f){ m_vTargetPos += GetDirection() * f; }
void TCamera::Back(float f)	{ m_vTargetPos -= GetDirection() * f; }
void TCamera::Left(float f)	{ m_vTargetPos += GetRight() * f; }
void TCamera::Right(float f){ m_vTargetPos -= GetRight() * f; }

void TCamera::CenterZoomIn(float f)			{ m_fDist += f*2; }
void TCamera::CenterZoomOut(float f)		{ m_fDist -= f*2; }
void TCamera::CenterRotationLeft(float f)	{ m_fRotY -= f; }
void TCamera::CenterRotationRight(float f)	{ m_fRotY += f; }
void TCamera::CenterRotationUp(float f)
{ 
	m_fRotX += f; 
	if (m_fRotX > 85) m_fRotX = 85; 
	if (m_fRotX < -85) m_fRotX = -85; 
}
void TCamera::CenterRotationDown(float f)	
{ 
	m_fRotX -= f; 
	if (m_fRotX > 85) m_fRotX = 85; 
	if (m_fRotX < -85) m_fRotX = -85; 
}

////////////////////////////////
void TCamera::OnPreRender()
{
	UpdateViewFrustrum();

	REngine::GetDevice().SetTransform(RST_VIEW,m_viewMatrix);
	REngine::GetDevice().SetTransform(RST_PROJECTION,m_projMatrix);
}

bool TCamera::OnMouseMove(int drag, RVector2 point, int delta)
{
	float t = INIT_DIST * 0.003f;

	if(delta != 0)
	{
		m_fDist = (-delta * t) + m_fDist;
	}

	RVector2 pos = point - m_oldCursorPos;
	if(pos.x == 0.0f && pos.y == 0.0f)
	{
		return false;
	}

	//ÈÙ´©¸£°í
	if(drag == 3)
	{
			m_fDist = (pos.y * t) + m_fDist;
	}
	else
	if(drag == 2)
	{
		// pan
		if(pos.x > 0)
			Left((float)-pos.x * t);
		else 
			Right((float)pos.x * t);

		if(pos.y > 0)
			Up((float)pos.y * t);
		else 
			Down((float)-pos.y * t);
	}
	else
	if( drag == 1)
	{
		//rot
		if(pos.x > 0)
			CenterRotationLeft((float)pos.x * t );
		else 
			CenterRotationRight((float)-pos.x * t);

		if(pos.y > 0)
			CenterRotationUp((float)-pos.y * t);
		else 
			CenterRotationDown((float)pos.y * t);

	}

	if( m_fDist < 0)
		m_fDist = 0.003f;

	m_oldCursorPos = point;

	return true;
}

}