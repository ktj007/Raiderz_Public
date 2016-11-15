#include "stdafx.h"
#include "RToolHelper.h"
#include "RMeshUtil.h"

namespace rs3 {

/*
void RToolHelper::DrawLine(RVector* vec,int size,DWORD color)
{
	draw_line(vec, size, color);	
}

void RToolHelper::DrawGrid(const RVector pos,float w,float h,int cnt_x,int cnt_y,DWORD color)
{
	_draw_grid(REngine::GetDevicePtr(), pos, w, h, cnt_x, cnt_y, color);
}
*/

void RToolHelper::DrawCenter(float size_w,float size_h,DWORD color)
{
	RMatrix  t_mat;
	t_mat.MakeIdentity();

	REngine::GetDevice().SetTransform(RST_WORLD,t_mat);
	REngine::GetDevice().SetTexture(0,R_NONE);
	REngine::GetDevice().SetFvF(RLVertexType);

	// 세로 라인
	RLVertex t_vert[4];

	RLVertex* vert = &t_vert[0];

	vert->p.x	= -size_w/2.f;
	vert->p.z	= 0.f;
	vert->p.y	= 0.f;
	vert->color	= color;

	vert++;

	vert->p.x	= size_w/2.f;
	vert->p.z	= 0.f;
	vert->p.y	= 0.f;
	vert->color	= color;

	vert++;

	vert->p.x	= 0.f;
	vert->p.z	= 0.f;
	vert->p.y	= size_h/2.f;
	vert->color	= color;

	vert++;

	vert->p.x	= 0.f;
	vert->p.z	= 0.f;
	vert->p.y	= -size_h/2.f;
	vert->color	= color;

	REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST,2, (LPVOID) t_vert, sizeof(RLVertex));
}

}
