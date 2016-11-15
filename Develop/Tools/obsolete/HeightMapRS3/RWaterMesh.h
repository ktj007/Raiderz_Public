#pragma once

#include <d3d9.h>
#include "RVType.h"

class RWaterMesh
{
protected:
	RVt_pos_tex1 *m_pVertexBuffer;
	int m_nPrimitiveNum;
	float m_fWaterLevel;
	
	LPDIRECT3DTEXTURE9 m_pReflectionMap;
	int m_nMapSize;
public:
	RWaterMesh();
	~RWaterMesh();

	void Draw(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX *pViewMat);
};
