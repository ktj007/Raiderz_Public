#pragma once

#include <d3d9.h>
#include <d3dx9.h>

void draw_matrix(LPDIRECT3DDEVICE9 dev,const D3DXMATRIX &worldmat,float size);

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

void draw_grid(LPDIRECT3DDEVICE9 dev,const D3DXVECTOR3 &pos,float w,float h,int cnt_x,int cnt_y,DWORD color);
