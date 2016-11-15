#include "stdafx.h"
#include "RVType.h"

const DWORD RVt_pos::FVF = (D3DFVF_XYZ);

const DWORD RVt_pos_clr::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

const DWORD RVt_pos_tex1::FVF = (D3DFVF_XYZ | D3DFVF_TEX1);

const DWORD RVt_pos_point_clr::FVF = (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE);

const DWORD RVt_pos_clr_tex1::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

const DWORD RVt_pos_clr_tex2::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEX2 );

const DWORD RVt_particle::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) );

const DWORD RVt_pos_tex_rhw::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

const DWORD RVt_pos_normal_tex1::FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

const DWORD RVt_pos4_normal_clr_tex1::FVF = (D3DFVF_XYZW | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);


#ifdef _DEBUG
const DWORD RVt_cloth_pos::FVF = (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1));

// RVt_cloth_normal rendering
const DWORD RVt_cloth_normal::FVF = (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE1(1));

// Anchor points
const DWORD RVt_cloth_anchor::FVF = (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1));
#else
const DWORD RVt_cloth_pos::FVF = (D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1));

const DWORD RVt_cloth_normal::FVF = (D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE1(1));

// Anchor points
const DWORD RVt_cloth_anchor::FVF = (D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1));
#endif

//void RAABBBound::Render(LPDIRECT3DDEVICE9 pDevice)
//{
//	static float baseVertexArray[] = 
//	{
//		-1.0f, 1.0f, 1.0f,
//		 1.0f, 1.0f, 1.0f,
//		-1.0f,-1.0f, 1.0f,
//		 1.0f,-1.0f, 1.0f,
//		-1.0f, 1.0f,-1.0f,
//		 1.0f, 1.0f,-1.0f,
//		-1.0f,-1.0f,-1.0f,
//		 1.0f,-1.0f,-1.0f,
//	}; // 8°³ vertex
//
//	static WORD IndexArray[] =
//	{
//		 0, 1, 1, 3, 3, 2, 2, 0
//		,0, 4, 1, 5, 2, 6, 3, 7
//		,4, 5, 5, 7, 7, 6, 6, 4
//	}; // 12°³ line
//
//	float VertexArray[8*3];
//
//	for ( int i = 0; i<8; ++i )
//	{
//		int nIndex = i*3;
//		VertexArray[nIndex] = baseVertexArray[nIndex]*vHalfRange.x + vCenter.x; ++nIndex;
//		VertexArray[nIndex] = baseVertexArray[nIndex]*vHalfRange.y + vCenter.y; ++nIndex;
//		VertexArray[nIndex] = baseVertexArray[nIndex]*vHalfRange.z + vCenter.z;
//	}
//
//	pDevice->SetFVF( D3DFVF_XYZ );
//	pDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12,(void*)IndexArray, D3DFMT_INDEX16,(void*)VertexArray, sizeof(float)*3);
//}

void _getpath(const char *szFullName, char *path)
{
	const char *p1 = strrchr(szFullName,'/');
	const char *p2 = strrchr(szFullName,'\\');
	const char *p = max(p1, p2);

	int len = 0;
	if ( p )
		len = p - szFullName;

	strncpy(path, szFullName,len+1);
	return;
}

void _getname(const char *szFullName, char *name)
{
	const char *p1 = strrchr(szFullName,'/');
	const char *p2 = strrchr(szFullName,'\\');
	const char *p = max(p1, p2);
	if ( p )
		strncpy(name, p+1, strlen(p)-1);
	else
		strcpy(name, szFullName);

	return;
}