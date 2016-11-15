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
		D3DXCreateSphere(lpd3dDevice, 1.0f, 32, 32, &m_pSphere, NULL);
}

L3DICONObject* L3DICONObject::GetInstance()
{
	static L3DICONObject kObject;
	return &kObject;
}



void draw_matrix(LPDIRECT3DDEVICE9 dev,const D3DXMATRIX &worldmat,float size)
{
	D3DXVECTOR3 pos   = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 right = D3DXVECTOR3(1,0,0)*size;
	D3DXVECTOR3 up	  = D3DXVECTOR3(0,1,0)*size;
	D3DXVECTOR3 dir   = D3DXVECTOR3(0,0,1)*size;

	dev->SetTransform( D3DTS_WORLD, &worldmat );
	dev->SetTexture(0, NULL);
	dev->SetFVF( RVt_pos_clr::FVF );

	RVt_pos_clr *t_vert = &g_pVertexArray[0];
	////////////////////////////////////
	t_vert->vPos.x		= pos.x;
	t_vert->vPos.y		= pos.y;
	t_vert->vPos.z		= pos.z;
	t_vert->dwColor		= D3DCOLOR_XRGB(255, 0, 0);

	t_vert++;

	t_vert->vPos.x		= right.x;
	t_vert->vPos.y		= right.y;
	t_vert->vPos.z		= right.z;
	t_vert->dwColor		= D3DCOLOR_XRGB(255, 0, 0);

	t_vert++;

	////////////////////////////////////

	t_vert->vPos.x		= pos.x;
	t_vert->vPos.y		= pos.y;
	t_vert->vPos.z		= pos.z;
	t_vert->dwColor		= D3DCOLOR_XRGB(0, 255, 0);

	t_vert++;

	t_vert->vPos.x		= up.x;
	t_vert->vPos.y		= up.y;
	t_vert->vPos.z		= up.z;
	t_vert->dwColor 	= D3DCOLOR_XRGB(0, 255, 0);

	t_vert++;

	////////////////////////////////////

	t_vert->vPos.x		= pos.x;
	t_vert->vPos.y		= pos.y;
	t_vert->vPos.z		= pos.z;
	t_vert->dwColor		= D3DCOLOR_XRGB(0, 0, 255);

	t_vert++;

	t_vert->vPos.x		= dir.x;
	t_vert->vPos.y		= dir.y;
	t_vert->vPos.z		= dir.z;
	t_vert->dwColor		= D3DCOLOR_XRGB(0, 0, 255);

	t_vert++;

	////////////////////////////////////

	dev->DrawPrimitiveUP(D3DPT_LINELIST, 3, (LPVOID)g_pVertexArray, sizeof(RVt_pos_clr));
	return;
}

void draw_grid(LPDIRECT3DDEVICE9 dev, const D3DXVECTOR3 &pos,float w,float h,int cnt_x,int cnt_y,DWORD color)
{
	if(cnt_x > 500) cnt_x = 500;
	if(cnt_y > 500) cnt_y = 500;

	int size	= 0;

	float x_w	= 0;
	float x_h	= 0;

	float z_add = h * (float)cnt_y;
	float x_add = w * (float)cnt_x;

	D3DXMATRIX  t_mat;
	D3DXMatrixIdentity(&t_mat);
	dev->SetTransform( D3DTS_WORLD, &t_mat );

	dev->SetTexture(0, NULL);
	dev->SetFVF( RVt_pos_clr::FVF );

	// 세로 라인

	RVt_pos_clr *t_vert = &g_pVertexArray[0];

	for(int i=0;i<cnt_x+1;i++) {

		t_vert->vPos.x		= pos.x + x_w;
		t_vert->vPos.y		= pos.y;
		t_vert->vPos.z		= pos.z;
		t_vert->dwColor	= color;

		t_vert++;

		t_vert->vPos.x		= pos.x + x_w;
		t_vert->vPos.y		= pos.y - z_add;
		t_vert->vPos.z		= pos.z;
		t_vert->dwColor	= color;

		t_vert++;

		x_w += w;
		size++;
	}

	dev->DrawPrimitiveUP(D3DPT_LINELIST, size, (LPVOID)g_pVertexArray, sizeof(RVt_pos_clr));

	// 가로라인

	t_vert = &g_pVertexArray[0];
	size	= 0;

	for(i=0;i<cnt_y+1;i++) {

		t_vert->vPos.x		= pos.x;
		t_vert->vPos.y		= pos.y - x_h;
		t_vert->vPos.z		= pos.z;
		t_vert->dwColor	= color;

		t_vert++;

		t_vert->vPos.x		= pos.x + x_add;
		t_vert->vPos.y		= pos.y - x_h;
		t_vert->vPos.z		= pos.z;
		t_vert->dwColor	= color;

		t_vert++;

		x_h += h;
		size++;
	}

	dev->DrawPrimitiveUP(D3DPT_LINELIST, size, (LPVOID)g_pVertexArray, sizeof(RVt_pos_clr));
}


