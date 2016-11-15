#include "stdafx.h"
#include "toolCamera.h"
#include <assert.h>

using namespace rs3;

MImplementRTTI(L3DCamera, RCameraSceneNode);
MImplementRTTI(L3DFPSCamera, RCameraSceneNode);

L3DCamera::L3DCamera()
: m_defEye(30,-30,30)
, m_defAt(0, 0, 0)
{
	m_viewMatrix.MakeIdentity();

	RVector vD = m_defAt - m_defEye;
	m_fDist = vD.Length();
}

L3DCamera::~L3DCamera()
{
}

void L3DCamera::Reset()
{
	m_position = m_defEye;
	m_at = m_defAt;

	m_up = RVector(0, 0, 1);
	m_direction = m_at - m_position;

	m_fDist = m_direction.Length();
	m_direction /= m_fDist;

	UpdateViewFrustrum();
}

void L3DCamera::UpdateTarget(RVector &vAt)
{
	m_at = vAt;
	m_position = m_at - m_direction*m_fDist;
}

void L3DCamera::Zoom(float n)
{
	m_fDist += n;

	m_position = m_at + ( -m_direction*m_fDist); 
}

void L3DCamera::RotationH(float n)
{
	D3DXMATRIX mat;
	D3DXVECTOR3 vInvDir(-m_direction.x, -m_direction.y, -m_direction.z);
	D3DXVECTOR3 vUp(m_up.x, m_up.y, m_up.z);

	D3DXMatrixRotationAxis(&mat,&vUp, D3DXToRadian(n));
	D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

	m_direction = RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z);
	m_position = m_at + ( -m_direction*m_fDist );
}

void L3DCamera::RotationV(float n)
{
	D3DXMATRIX mat;
	D3DXVECTOR3 vInvDir(-m_direction.x, -m_direction.y, -m_direction.z);
	D3DXVECTOR3 vAxis(m_viewMatrix._11, m_viewMatrix._21, m_viewMatrix._31);

	D3DXMatrixRotationAxis(&mat,&vAxis, D3DXToRadian(n));
	D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

	m_direction = RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z);
	m_position = m_at + ( -m_direction*m_fDist );
}

void L3DCamera::MoveAtPoint(const RVector &vec)
{
	m_at.x += vec.x;
	m_at.y += vec.y;
	m_at.z += vec.z;

	m_position.x += vec.x;
	m_position.y += vec.y;
	m_position.z += vec.z;

	m_direction = m_at - m_position;

	m_fDist = m_direction.Length();
	m_direction /= m_fDist;
}

void L3DFPSCamera::Reset()
{
	m_position = RVector(0,0,0);
	m_direction = RVector(0,1,0);
	m_up = RVector(0,0,1);
}

void L3DFPSCamera::RotationH(float n)
{
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetRotationZ(D3DXToRadian(n));
	RVector vDir;
	mat.TransformNormal(m_direction, vDir);

	m_direction = vDir;
}

void L3DFPSCamera::RotationV(float n)
{
	RMatrix mat;
	RVector vAxis;
	vAxis = m_direction.CrossProduct(m_up);
	vAxis.Normalize();

	mat.MakeIdentity();
	mat.SetRotationMatrixAxis(vAxis, D3DXToRadian(n));

	RVector vDir;
	mat.TransformNormal(m_direction, vDir);

	m_direction = vDir;
}