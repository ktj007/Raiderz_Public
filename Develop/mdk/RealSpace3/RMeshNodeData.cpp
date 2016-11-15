#include "stdafx.h"

#include "RMeshNodeData.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////////////
// RMeshNodeData Class

RMeshNodeData::RMeshNodeData() : m_fBaseVisibility(1), m_fBaseNoiseRef(1)
{
	m_nID = -1;
	m_nUserID = -1;
	m_nMtrlID = -1;

	m_matLocal.MakeIdentity();
	m_matResult.MakeIdentity();

	m_pPointTable		= NULL;
	m_pPointTableOrg	= NULL;
	m_pPointColorTable	= NULL;
	m_pPhysique			= NULL;
	m_pPolygonTable		= NULL;
	m_pPointNormalTable	= NULL;

	m_pNormalTable		= NULL;
	m_pTangentTanTable	= NULL;
	m_pTangentBinTable	= NULL;
	m_pTexCoordTable	= NULL;
	m_pTexCoordExtraTable = NULL;
	m_pBoneTable		= NULL;
	m_pBoneTableIndex	= NULL;

	m_pVertexIndexTable	= NULL;
	m_pMtrlTableInfo	= NULL;
	m_pFaceIndexTable	= NULL;

	m_pAllFaceSubData	= NULL;

	m_nVertexIndexTableCnt	= 0;
	m_nMtrlTableInfoCnt		= 0;

	m_nPointCnt			= 0;
	m_nFaceCnt			= 0;
	m_nPhysiqueCnt		= 0;
	m_nPointNormalCnt	= 0;
	m_nPointColorCnt	= 0;
	m_nBoneTableCnt		= 0;

	m_nNormalTableCnt		= 0;
	m_nTangentTanTableCnt	= 0;
	m_nTangentBinTableCnt	= 0;
	m_nTexCoordTableCnt		= 0;
	m_nTexCoordExtraTableCnt = 0;

	m_nTotalDegree = 0;
	m_nTotalTriangles = 0;

	m_boundingBox.Initialize();

#ifdef MESH_STRIP_TEST
	m_pMtrlTableInfoStrips = NULL;
	m_pMtrlTableInfoDefault = NULL;
#endif
}

RMeshNodeData::~RMeshNodeData() 
{
	SAFE_DELETE_ARRAY(m_pPointTable);
	SAFE_DELETE_ARRAY(m_pPointTableOrg);
	SAFE_DELETE_ARRAY(m_pPointColorTable);
	SAFE_DELETE_ARRAY(m_pPointNormalTable);

	SAFE_DELETE_ARRAY(m_pNormalTable);
	SAFE_DELETE_ARRAY(m_pTangentTanTable);
	SAFE_DELETE_ARRAY(m_pTangentBinTable);

	SAFE_DELETE_ARRAY(m_pTexCoordTable);
	SAFE_DELETE_ARRAY(m_pTexCoordExtraTable);

	SAFE_DELETE_ARRAY(m_pPhysique);
	SAFE_DELETE_ARRAY(m_pPolygonTable);
	SAFE_DELETE_ARRAY(m_pBoneTable);
	SAFE_DELETE_ARRAY(m_pBoneTableIndex);

	SAFE_DELETE_ARRAY(m_pVertexIndexTable);
	SAFE_DELETE_ARRAY(m_pFaceIndexTable);

	SAFE_DELETE_ARRAY(m_pAllFaceSubData);

#ifdef MESH_STRIP_TEST
	SAFE_DELETE_ARRAY(m_pMtrlTableInfoDefault);
	SAFE_DELETE_ARRAY(m_pMtrlTableInfoStrips);
#else
	SAFE_DELETE_ARRAY(m_pMtrlTableInfo);
#endif
}

int RMeshNodeData::GetFacePosIndex(int findex,int pindex)
{
	return m_pPolygonTable[findex].index[pindex].p;
}

int RMeshNodeData::GetFaceNormalIndex(int findex,int pindex)
{
	return m_pPolygonTable[findex].index[pindex].n;
}

int RMeshNodeData::GetFaceUVIndex(int findex,int pindex)
{
	return m_pPolygonTable[findex].index[pindex].uv;
}

RVector RMeshNodeData::GetFacePos(int findex,int pindex)
{
	return m_pPointTable[m_pPolygonTable[findex].index[pindex].p];
}

RVector RMeshNodeData::GetFaceNormal(int findex,int pindex)
{
	return m_pNormalTable[m_pPolygonTable[findex].index[pindex].n];
}

RVector RMeshNodeData::GetFaceUV(int findex,int pindex)
{
	return m_pTexCoordTable[m_pPolygonTable[findex].index[pindex].uv];
}

int RMeshNodeData::GetFaceMtrl(int findex)
{
	return m_pPolygonTable[findex].mtrl_id;
}

void RMeshNodeData::BBoxClear()
{
	m_boundingBox.Initialize();
}

void RMeshNodeData::CalcLocalBBox()
{
	BBoxClear();
	for(int i=0;i<m_nPointCnt;i++)
	{
		m_boundingBox.Add(m_pPointTable[i]);
	}
}

////////////////////////////////////////////////////////////////////////
// mtrl

RMeshNodeMtrl::RMeshNodeMtrl()
{
	m_pMtrlTable = NULL;
	m_nMtrlCnt = 0;
}

RMeshNodeMtrl::~RMeshNodeMtrl()
{
	SAFE_DELETE_ARRAY(m_pMtrlTable);
	m_nMtrlCnt = 0;
}

int RMeshNodeMtrl::GetMtrlCount()
{
	return m_nMtrlCnt;
}

RMaterial* RMeshNodeMtrl::GetMtrl(int i)
{
	if(i<m_nMtrlCnt) {
		if(m_pMtrlTable) {
			return m_pMtrlTable[i];
		}
	}

	return NULL;
}

}
