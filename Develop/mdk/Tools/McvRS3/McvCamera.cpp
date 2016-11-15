#include "stdafx.h"

#include "McvCamera.h"
#include "McvConfig.h"
#include "RRenderHelper.h"

#define INIT_DIST 200.f
#define INIT_ROTX -15

CTCamera::CTCamera() 
{
	m_fRotX		= INIT_ROTX;
	m_fRotY		= 0.f;
	m_fDist		= INIT_DIST;

//	m_fFov		= D3DX_PI/4.f;
//	m_fAspect	= 800/600.f;
	m_fNearZ		= 10.f;
//	m_fFarZ		= INIT_FARZ;

	m_fFarZ		=	McvConfig::GetInstance().m_fFarZ;

//	m_position		= RVector(0.f,-m_fDist,80.f);
//	m_direction		= RVector(0.f,1.f,0.f);
//	m_vUp		= RVector(0.f,0.f,1.f);
	SetTransform(RVector(0.f,-m_fDist,80.f),RVector(0.f,1.f,0.f));

	m_vTargetPos	= RVector(0.f,0.f,40.f);
}

CTCamera::~CTCamera() 
{ 

}

void CTCamera::ResetSize() 
{
//	m_fAspect = w/(float)h;
	SetFov(D3DX_PI/4.f);

	UpdateData();
}



void CTCamera::UpdateData() 
{
	m_fRotX = min(max(m_fRotX,-85),85);

	RVector vRot;

	RMatrix rot;
	rot.SetRotationMatrix( D3DX_PI/180.f*m_fRotX, 0.f, D3DX_PI/180.f*m_fRotY );
	vRot = RVector(0.f,-1.f,0.f) * rot;

	vRot.Normalize();

	RVector position = m_vTargetPos + (vRot * m_fDist);
	RVector direction = m_vTargetPos - position;
	direction.Normalize();

	Set(position,direction);

//	Set(m_position,-m_direction);

//	m_matView.SetLookAtMatrixRH( m_position, m_vTargetPos, m_vUp);
//	m_matProj.SetProjectionMatrixFovRH( m_fFovY, m_fAspectRatio, m_fNearZ, m_fFarZ);

	SetNearFarZ(m_fNearZ,m_fFarZ);

//	GetDevice()->SetTransform(RST_VIEW,m_matView);
//	GetDevice()->SetTransform(RST_PROJECTION,m_matProj);
}

void CTCamera::Reset() 
{
	m_fDist = INIT_DIST;

	m_vTargetPos = RVector(0.f,0.f,40.f);

	m_fRotX = INIT_ROTX;
	m_fRotY = 0.f;

	UpdateData();
}

void CTCamera::Reset(RBox boxModel) 
{
	if(boxModel.vmin == MVector3(FLT_MAX,FLT_MAX,FLT_MAX) &&
		boxModel.vmax == MVector3(-FLT_MAX,-FLT_MAX,-FLT_MAX))
	{
		boxModel.Add(RVector(0,0,0));
	}



	float fDiagonal = (boxModel.vmax - boxModel.vmin).Length();
	m_fDist = INIT_DIST + fDiagonal*.7f;

	m_vTargetPos = boxModel.GetCenter();

	m_fRotX = INIT_ROTX;
	m_fRotY = 0.f;

// 	if(fDiagonal * 1.5f>INIT_FARZ)
// 		m_fFarZ = fDiagonal * 1.5f;
// 	else
// 		m_fFarZ = INIT_FARZ;

	UpdateData();
}

RVector CTCamera::GetPos() 
{
	return GetPosition();
}

RVector CTCamera::GetDir() 
{
//	return m_direction.Normalize();
	return GetDirection().Normalize();
}

RVector CTCamera::GetRight() 
{
	RVector right = CrossProduct(GetUp(),GetDirection());
	return right.Normalize();
}

void CTCamera::Up(float f)		{ m_vTargetPos.z += f; }
void CTCamera::Down(float f)	{ m_vTargetPos.z -= f; }
void CTCamera::Front(float f)	{ m_vTargetPos += GetDir() * f; }
void CTCamera::Back(float f)	{ m_vTargetPos -= GetDir() * f; }
void CTCamera::Left(float f)	{ m_vTargetPos += GetRight() * f; }
void CTCamera::Right(float f)	{ m_vTargetPos -= GetRight() * f; }

void CTCamera::CenterZoomIn(float f)		{ m_fDist += f*2; }
void CTCamera::CenterZoomOut(float f)		{ m_fDist -= f*2; }
void CTCamera::CenterRotationLeft(float f)	{ m_fRotY -= f; }
void CTCamera::CenterRotationRight(float f) { m_fRotY += f; }
void CTCamera::CenterRotationUp(float f)	{ m_fRotX += f; }
void CTCamera::CenterRotationDown(float f)	{ m_fRotX -= f; }


////////////////////////////////
void CTCamera::OnPreRender()
{
	UpdateViewFrustrum();

	GetDevice()->SetTransform(RST_VIEW,m_viewMatrix);
	GetDevice()->SetTransform(RST_PROJECTION,m_projMatrix);
}
