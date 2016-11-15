#include "StdAfx.h"
#include "RCameraArcBallSceneNode.h"
#include "RDevice.h"
#include "RD3DApplication.h"

#include "d3dx9.h"

namespace rs3 {

RCameraArcBallSceneNode::RCameraArcBallSceneNode(void) : RCameraSceneNode(), m_fDist(0.0f)
{
	m_viewMatrix.MakeIdentity();
}

RCameraArcBallSceneNode::~RCameraArcBallSceneNode(void)
{

}


void RCameraArcBallSceneNode::OnPreRender( RCameraSceneNode* pCurrentCamera)
{
	if (REngine::GetSceneManager().GetPrimaryCamera() != this) return;
}

void RCameraArcBallSceneNode::Reset()
{
//	m_position = rs3::RVector(30,-30,30);
	m_at = rs3::RVector(0, 0, 0);
//	m_up = rs3::RVector(0, 0, 1);
//	m_direction = m_at - m_position;

	m_fDist = 30.f;
	RVector dir = RVector(1,-1,1);

	SetTransform( m_fDist * dir, -dir );
	
	//m_fDist = m_direction.Length();
	//m_direction /= m_fDist;

	UpdateViewFrustrum();
}

void RCameraArcBallSceneNode::Zoom(float n)
{
	m_fDist += n;
	if ( m_fDist < 2.0f )
		m_fDist = 2.0f;

//	m_position = m_at + ( -m_direction*m_fDist); 
	SetPosition( m_at + ( -GetDirection() *m_fDist) ); 
}

void RCameraArcBallSceneNode::RotationH(float n)
{
	RVector dir = GetDirection();
	RVector up = GetUp();

	D3DXMATRIX mat;
	D3DXVECTOR3 vInvDir(-dir.x, -dir.y, -dir.z);
	D3DXVECTOR3 vUp(up.x, up.y, up.z);

	D3DXMatrixRotationAxis(&mat,&vUp, D3DXToRadian(n));
	D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

	SetDirection(rs3::RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z));

//	m_position = m_at + ( -m_direction*m_fDist );
	SetPosition( m_at + ( -GetDirection() *m_fDist) ); 


	//mlog("dir = (%.1f,%.1f,%.1f) , pos=(%.1f,%.1f,%.1f)\n", 
	//	m_direction.x, m_direction.y, m_direction.z,
	//	m_position.x, m_position.y, m_position.z);
}

void RCameraArcBallSceneNode::RotationV(float n)
{
	RVector dir = GetDirection();

	D3DXMATRIX mat;
	D3DXVECTOR3 vInvDir(-dir.x, -dir.y, -dir.z);
	D3DXVECTOR3 vAxis(m_viewMatrix._11, m_viewMatrix._21, m_viewMatrix._31);

	D3DXMatrixRotationAxis(&mat,&vAxis, D3DXToRadian(n));
	D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

	//m_direction = rs3::RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z);
	//m_position = m_at + ( -m_direction*m_fDist );
	SetDirection(rs3::RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z));
	SetPosition( m_at + ( -GetDirection() *m_fDist) ); 


	
}

void RCameraArcBallSceneNode::MoveAtPoint(const RVector &vec)
{
	m_at.x += vec.x;
	m_at.y += vec.y;
	m_at.z += vec.z;

	//m_position.x += vec.x;
	//m_position.y += vec.y;
	//m_position.z += vec.z;

	SetPosition( GetPosition() + vec );

// 	m_direction = m_at - m_position;
// 
// 	m_fDist = m_direction.Length();
// 	m_direction /= m_fDist;

	m_fDist = (m_at - GetPosition()).Length();
}

void RCameraArcBallSceneNode::OnMessage(char* szMsg, void* param)
{
	mlog("msg : %s , param = %s\n", szMsg, (char*)param);

	// 테스트로 넣어봄
	if (!strcmp(szMsg, "MoveAtPoint"))
	{
		RVector vec = RVector(0,0,0);
		sscanf((char*)param, "%f %f %f", &vec.x, &vec.y, &vec.z);
		MoveAtPoint(vec);
	}
	else if (!strcmp(szMsg, "RotationV"))
	{
		float f;
		sscanf((char*)param, "%f", &f);
		RotationV(f);
	}
	else if (!strcmp(szMsg, "RotationH"))
	{
		float f;
		sscanf((char*)param, "%f", &f);
		RotationH(f);
	}
	else if (!strcmp(szMsg, "Zoom"))
	{
		float f;
		sscanf((char*)param, "%f", &f);
		Zoom(f);
	}
}

}
