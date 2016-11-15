#include "stdafx.h"
#include "RSUtil.h"

#include "d3dx9.h"

namespace rs3 {

void GetScreenFromWorld(MVector2 *pScreen, const MVector3 *position, const RViewport *pViewport, const MMatrix *pWorldMatrix, const MMatrix *pViewMatrix, const MMatrix *pProjectionMatrix)
{
	D3DVIEWPORT9 d3dViewport;
	d3dViewport.X = pViewport->x;
	d3dViewport.Y = pViewport->y;
	d3dViewport.Width = pViewport->nWidth;
	d3dViewport.Height = pViewport->nHeight;
	d3dViewport.MinZ = pViewport->fMinZ;
	d3dViewport.MaxZ = pViewport->fMaxZ;

	D3DXMATRIX world(pWorldMatrix->m);
	D3DXMATRIX view(pViewMatrix->m);
	D3DXMATRIX projection(pProjectionMatrix->m);

	D3DXVECTOR3 pos(position->x, position->y, position->z);
	D3DXVECTOR3 V;

	D3DXVec3Project(&V,&pos,&d3dViewport,&projection,&view, &world);

	pScreen->x = V.x;
	pScreen->y = V.y;
}

void GetScreenFromWorld(MVector3 *pScreen, const MVector3 *position, const RViewport *pViewport, const MMatrix *pWorldMatrix, const MMatrix *pViewMatrix, const MMatrix *pProjectionMatrix)
{
	D3DVIEWPORT9 d3dViewport;
	d3dViewport.X = pViewport->x;
	d3dViewport.Y = pViewport->y;
	d3dViewport.Width = pViewport->nWidth;
	d3dViewport.Height = pViewport->nHeight;
	d3dViewport.MinZ = pViewport->fMinZ;
	d3dViewport.MaxZ = pViewport->fMaxZ;

	D3DXMATRIX world(pWorldMatrix->m);
	D3DXMATRIX view(pViewMatrix->m);
	D3DXMATRIX projection(pProjectionMatrix->m);

	D3DXVECTOR3 pos(position->x, position->y, position->z);
	D3DXVECTOR3 V;

	D3DXVec3Project(&V,&pos,&d3dViewport,&projection,&view, &world);

	pScreen->x = V.x;
	pScreen->y = V.y;
	pScreen->z = V.z;
}

void GetScreenFromWorld(MVector2 *pScreen, const MVector3 *position, RDevice *pDevice)
{
	const RViewport &viewport = pDevice->GetViewport();
	const MMatrix &worldMatrix = pDevice->GetTransform(RST_WORLD);
	const MMatrix &viewMatrix = pDevice->GetTransform(RST_VIEW);
	const MMatrix &projectionMatrix = pDevice->GetTransform(RST_PROJECTION);

	GetScreenFromWorld(pScreen, position,&viewport,&worldMatrix,&viewMatrix,&projectionMatrix);
	return;
}

}