#pragma once

#include "RDevice.h"
#include "RVType.h"

#include <list>
#include <vector>

#include "MOBox.h"

namespace rs3 {

class RTreeSystem;
class RTreeInstance;
class RTerrain;
class RTerrainResource;
class RTerrainPatch
{
public:
	typedef RVt_pos_normal_tex1 RVt_Terrain;

public:
	MOBox m_kObb;

protected:
	RDevice *m_pDevice;

	HVERTEXBUFFER m_hVertexBuffer;
	RVt_Terrain *m_pVertexArray;
	RTexture* m_pLayerMask[2];
	bool m_bNeedUpdateLayerMaskTexture;

	int m_iWidthCount;
	int m_iHeightCount;

	RBoundingBox m_kAABB;
	RBoundingSphere m_kSphere;
	
	int m_iLevel;
	int m_iNearFlag;

	int m_nX, m_nY;

	RTerrainResource *m_pTerrain;
public:
	RTerrainPatch();
	virtual ~RTerrainPatch();

	void Init(RTerrainResource *pTerrain);

	void SetDevice(RDevice *pDevice) { m_pDevice = pDevice; }

	void UpdateVertexBuffer();
	bool CreateVertexBuffer(int nLODLevel, int nWidth);
	bool CreateLayerMaskTexture( int nMaskTextureCount, DWORD dwMaxLayerWidth, DWORD dwMaxLayerHeight);
	bool DeleteLayerMaskTexture();
	bool IsNeedUpdateLayerMaskTexture()	{ return m_bNeedUpdateLayerMaskTexture;}
	void NeedUpdateLayerMaskTexture( bool b)	{ m_bNeedUpdateLayerMaskTexture=b;}

	bool CreateDisplacementMapFromMemory(float* const pDisplacementMap, RVector3* const pNormalMap, int nPitch, int nLODLevel, RECT *pRect
		, int nMaskWidthNum = 1, int nMaskHeightNum = 1);

	const HVERTEXBUFFER GetVertexBuffer()	{	return m_hVertexBuffer;	}
	int GetVertexNum(int nLevel)	{	return m_pVertexNum[nLevel];	}

	RTexture*	GetLayerMaskTexture( int nMaskTextureIdx);

	void SetLevel(int n){	m_iLevel = n;	}
	int GetLevel()		{	return m_iLevel; }

	void SetNearFlag(int n)		{	m_iNearFlag = n; }
	int GetNearFlag()			{	return m_iNearFlag; }

	int *m_pVertexNum;
	int *m_pVertexStart;

	int m_iVertexBufferNum;

	RBoundingBox* const GetAABBBoundBox()		{	return &m_kAABB; }
	RBoundingSphere* const GetBoundSphere()		{	return &m_kSphere; }

	RVt_Terrain* GetVertesArray() { return m_pVertexArray;}

	int GetX()	{	return m_nX; }
	int GetY()	{	return m_nY; }

	void SetXY(int x, int y)
	{
		m_nX = x;
		m_nY = y;
	}
};

}
