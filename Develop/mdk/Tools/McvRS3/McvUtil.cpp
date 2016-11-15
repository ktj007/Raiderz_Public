#include "stdafx.h"

#include "McvUtil.h"

RLVertex	g_grid_vert[1000];

//<---------------------------------------------------------------------

void GetModelTry(RLVertex* pVert,int size,DWORD color,int* face_num)
{
	RLVertex _vert[4];

	RLVertex* t_vert = &_vert[0];

	t_vert->p.x		= 0.f*size;
	t_vert->p.z		= 0.f*size;
	t_vert->p.y		= 0.5f*size;
	t_vert->color	= color;
	t_vert++;

	t_vert->p.x		=-0.5f*size;
	t_vert->p.z		= 0.f*size;
	t_vert->p.y		=-0.5f*size;
	t_vert->color	= color;
	t_vert++;

	t_vert->p.x		= 0.5f*size;
	t_vert->p.z		= 0.f*size;
	t_vert->p.y		=-0.5f*size;
	t_vert->color	= color;
	t_vert++;

	t_vert->p.x		= 0.f*size;
	t_vert->p.z		= 1.5f*size;
	t_vert->p.y		= 0.f*size;
	t_vert->color	= color;

	pVert[0] = _vert[0];
	pVert[1] = _vert[1];
	pVert[2] = _vert[2];

	pVert[3] = _vert[0];
	pVert[4] = _vert[1];
	pVert[5] = _vert[3];

	pVert[6] = _vert[1];
	pVert[7] = _vert[2];
	pVert[8] = _vert[3];

	pVert[9]  = _vert[3];
	pVert[10] = _vert[2];
	pVert[11] = _vert[0];

	// face index num

	*face_num = 4;
}

void DrawTry(RMatrix mat,float size,DWORD color)
{
	int face_num = 0;

	GetModelTry(g_grid_vert,size,color,&face_num);

	GetDevice()->SetTransform(RST_WORLD,mat);
	GetDevice()->SetTexture(0,R_NONE);
	GetDevice()->SetFvF(RLVertexType);

	GetDevice()->DrawPrimitiveUP(RPT_TRIANGLELIST,face_num, (LPVOID) g_grid_vert, sizeof(RLVertex));
}

void DrawMatrix(RMatrix mat,float size)
{
	RVector pos   = RVector(0,0,0);
	RVector right = RVector(1,0,0)*size;
	RVector up	  = RVector(0,0,1)*size;
	RVector dir   = RVector(0,1,0)*size;

	GetDevice()->SetTransform(RST_WORLD,mat);
	GetDevice()->SetTexture(0,R_NONE);
	GetDevice()->SetFvF(RLVertexType);
	
	RLVertex* t_vert = &g_grid_vert[0];

	////////////////////////////////////

	t_vert->p		= pos;
	t_vert->color	= 0xff00ff00;
	t_vert++;

	t_vert->p		= right;
	t_vert->color	= 0xff00ff00;
	t_vert++;

	////////////////////////////////////

	t_vert->p		= pos;
	t_vert->color	= 0xffff0000;
	t_vert++;

	t_vert->p		= up;
	t_vert->color	= 0xffff0000;
	t_vert++;

	////////////////////////////////////

	t_vert->p		= pos;
	t_vert->color	= 0xff0000ff;
	t_vert++;

	t_vert->p		= dir;
	t_vert->color	= 0xff0000ff;
	t_vert++;

	////////////////////////////////////

	GetDevice()->DrawPrimitiveUP(RPT_LINELIST,3, (LPVOID) g_grid_vert, sizeof(RLVertex));

	RMatrix _mat = mat;
	RMatrix _tposmat;
	RMatrix _trotmat;

	// right

	_tposmat.SetTranslationMatrix(right);
	_trotmat.SetRotationMatrix(RVector(0.f,0.f,-3.14f/2.f));
//	D3DXMatrixTranslation(&_tposmat,right.x,right.y,right.z);
//	D3DXMatrixRotationZ(&_trotmat,-3.14/2.f);
	_mat = _trotmat*_tposmat*_mat;
	DrawTry(_mat,size/10,0xff00ff00);

	// up

	_mat = mat;
//	D3DXMatrixTranslation(&_tposmat,up.x,up.y,up.z);
	_tposmat.SetTranslationMatrix(up);
	_mat = _tposmat*_mat;
	DrawTry(_mat,size/10,0xffff0000);

	// dir

	_mat = mat;
//	D3DXMatrixTranslation(&_tposmat,dir.x,dir.y,dir.z);
//	D3DXMatrixRotationX(&_trotmat,3.14/2.f);
	_tposmat.SetTranslationMatrix(dir);
	_trotmat.SetRotationMatrix(RVector(-3.14f/2.f,0.f,0.f));
	_mat = _trotmat*_tposmat*_mat;
	DrawTry(_mat,size/10,0xff0000ff);

}

void DrawGrid(RVector pos,float w,float h,int cnt_x,int cnt_y,DWORD color)
{
	if(cnt_x > 500) cnt_x = 500;
	if(cnt_y > 500) cnt_y = 500;

	int size	= 0;

	float x_w	= 0;
	float x_h	= 0;

	float y_add = h * (float)cnt_y;
	float x_add = w * (float)cnt_x;

	RMatrix  t_mat;
	t_mat.MakeIdentity();
	
	GetDevice()->SetTransform(RST_WORLD,t_mat);
	GetDevice()->SetTexture(0,R_NONE);
	GetDevice()->SetFvF(RLVertexType);

	// 세로 라인

	RLVertex* t_vert = &g_grid_vert[0];

	for(int i=0;i<cnt_x+1;i++) {

		t_vert->p.x		= pos.x + x_w;
		t_vert->p.z		= pos.z;
		t_vert->p.y		= pos.y;
		t_vert->color	= color;

		t_vert++;

		t_vert->p.x		= pos.x + x_w;
		t_vert->p.z		= pos.z;
		t_vert->p.y		= pos.y - y_add;
		t_vert->color	= color;

		t_vert++;

		x_w += w;
		size++;
	}

	GetDevice()->DrawPrimitiveUP(RPT_LINELIST,size, (LPVOID) g_grid_vert, sizeof(RLVertex));


	// 가로라인

	t_vert = &g_grid_vert[0];
	size	= 0;

	for(int i=0;i<cnt_y+1;i++) {

		t_vert->p.x		= pos.x;
		t_vert->p.z		= pos.z;
		t_vert->p.y		= pos.y - x_h;
		t_vert->color	= color;

		t_vert++;

		t_vert->p.x		= pos.x + x_add;
		t_vert->p.z		= pos.z;
		t_vert->p.y		= pos.y - x_h;
		t_vert->color	= color;

		t_vert++;

		x_h += h;
		size++;
	}

	GetDevice()->DrawPrimitiveUP(RPT_LINELIST,size, (LPVOID) g_grid_vert, sizeof(RLVertex));

}


void DrawCenter(float size_w,float size_h,DWORD color)
{
	RMatrix  t_mat;
	t_mat.MakeIdentity();

	GetDevice()->SetTransform(RST_WORLD,t_mat);
	GetDevice()->SetTexture(0,R_NONE);
	GetDevice()->SetFvF(RLVertexType);

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

	GetDevice()->DrawPrimitiveUP(RPT_LINELIST,2, (LPVOID) t_vert, sizeof(RLVertex));
}

void SetLight(int mode,RVector& pos)
{
	D3DLIGHT9 light;

	ZeroMemory( &light, sizeof(D3DLIGHT9) );

	light.Type        = D3DLIGHT_POINT;

	light.Diffuse.r   = 1.0f;
	light.Diffuse.g   = 1.0f;
	light.Diffuse.b   = 1.0f;

	light.Specular.r	= 1.f;
	light.Specular.g	= 1.f;
	light.Specular.b	= 1.f;

	light.Position.x   = pos.x;
	light.Position.y   = pos.y;
	light.Position.z   = pos.z;

	light.Range        = 1000.0f;

	light.Attenuation0 = 0.9f;
	light.Attenuation1 = 0.0f;

	GetDevice()->SetLight(mode, (void*)&light);
	GetDevice()->SetLightEnable(mode,true);
	GetDevice()->SetLighting(true);
}
