#include "stdafx.h"
#include "d3exutil.h"
#include <assert.h>

struct RVt_pos_clr
{
	D3DXVECTOR3 vPos;
	DWORD dwColor;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

RVt_pos_clr g_pVertexArray[2000];

L3DICONObject::L3DICONObject()
{
	m_pBox = NULL;
	m_pSphere = NULL;
}

L3DICONObject::~L3DICONObject()
{

}

void L3DICONObject::RenderBox()
{
    assert(m_pBox);
	m_pBox->DrawSubset(0);
}

void L3DICONObject::RenderSphere()
{
	assert(m_pSphere);
	m_pSphere->DrawSubset(0);
}

void L3DICONObject::ReleaseObject()
{
	if ( m_pBox )
	{
		m_pBox->Release();
		m_pBox = NULL;
	}
	if ( m_pSphere )
	{
		m_pSphere->Release();
		m_pSphere = NULL;
	}
}

void L3DICONObject::CreateObject(LPDIRECT3DDEVICE9 lpd3dDevice)
{
	if ( m_pBox == NULL )
		D3DXCreateBox(lpd3dDevice, 1.0f, 1.0f, 1.0f,&m_pBox, NULL);
	if ( m_pSphere == NULL )
		D3DXCreateSphere(lpd3dDevice, 1.0f, 16, 16, &m_pSphere, NULL);
}

L3DICONObject* L3DICONObject::GetInstance()
{
	static L3DICONObject kObject;
	return &kObject;
}
