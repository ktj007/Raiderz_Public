#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <RCameraSceneNode.h>

class L3DICONObject
{
protected:
	L3DICONObject();
	~L3DICONObject();

	LPD3DXMESH m_pSphere;
	LPD3DXMESH m_pBox;
public:
	void CreateObject(LPDIRECT3DDEVICE9 lpd3dDevice);
	void ReleaseObject();

	void RenderBox();
	void RenderSphere();
	static L3DICONObject* GetInstance();
};

// 결국 RCameraSceneNode를 상속받음 .
class L3DCamera : public rs3::RCameraSceneNode
{
public:
	//D3DXVECTOR3 m_vEye;
	//D3DXVECTOR3 m_vAt;
	//D3DXVECTOR3 m_vUp;
	//D3DXVECTOR3 m_vDir;
	
	rs3::RVector m_at;
	float m_fDist;
public:
	L3DCamera() : m_fDist(0.0f)
	{
		m_viewMatrix.MakeIdentity();
	}
	virtual ~L3DCamera() {}

	void Reset()
	{
		//m_vEye = D3DXVECTOR3(30,-30, 30);
		//m_vAt = D3DXVECTOR3(0, 0, 0);
		//m_vUp = D3DXVECTOR3(0, 0, 1);
		//m_vDir = m_vAt - m_vEye;

		m_position = rs3::RVector(0,-30,30);
		m_at = rs3::RVector(0, 0, 0);
		m_up = rs3::RVector(0, 0, 1);
		m_direction = m_at - m_position;
		
		m_fDist = m_direction.Length();
		m_direction /= m_fDist;

		UpdateViewFrustrum();
	}



	void Zoom(float n)
	{
		m_fDist += n;
		if ( m_fDist < 2.0f )
			m_fDist = 2.0f;

		m_position = m_at + ( -m_direction*m_fDist); 
	}
	
	void RotationH(float n)
	{
		D3DXMATRIX mat;
		D3DXVECTOR3 vInvDir(-m_direction.x, -m_direction.y, -m_direction.z);
		D3DXVECTOR3 vUp(m_up.x, m_up.y, m_up.z);

		D3DXMatrixRotationAxis(&mat,&vUp, D3DXToRadian(n));
		D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

		m_direction = rs3::RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z);
		m_position = m_at + ( -m_direction*m_fDist );
	}
	
	void RotationV(float n)
	{
		D3DXMATRIX mat;
		D3DXVECTOR3 vInvDir(-m_direction.x, -m_direction.y, -m_direction.z);
		D3DXVECTOR3 vAxis(m_viewMatrix._11, m_viewMatrix._21, m_viewMatrix._31);

		D3DXMatrixRotationAxis(&mat,&vAxis, D3DXToRadian(n));
		D3DXVec3TransformCoord(&vInvDir,&vInvDir,&mat);

		m_direction = rs3::RVector(-vInvDir.x,-vInvDir.y,-vInvDir.z);
		m_position = m_at + ( -m_direction*m_fDist );
	}

	void MoveAtPoint(const D3DXVECTOR3 &vec)
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

	const rs3::RMatrix& GetProjectionMatrix() const
	{
		return m_projMatrix;
	}
};

