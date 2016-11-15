#include "StdAfx.h"
#include "RStaticMeshResourceFileLoadImpl.h"
#include "RStaticMeshResource.h"
#include "RMaterialResource.h"
#include "RMaterial.h"

namespace rs3 {

#define MREAD(x) { if(!file.Read(&x,sizeof(x))) return false; }
//////////////////////////////////////////////////////////////////////////
// version 1
bool RStaticMeshResourceFileLoadImpl_v1::LoadCell(MFile &file, RStaticMeshResource *pStaticMesh)
{
	// RS3_VER1
	RRsNodeLoadImpl *pImpl = NULL;
	pImpl = new RRsNodeLoadImpl_v1;
	_ASSERT(pImpl);

	int nCells;
	MREAD(nCells);
	for( int i = 0; i < nCells; i++ )
	{
		RCell *pCell = new RCell;

		int nLength;
		MREAD(nLength);
		pCell->m_strName.resize(nLength);
		file.Read( &pCell->m_strName[0], nLength );

		int nPlanes;
		MREAD(nPlanes);
		for(int j=0;j<nPlanes;j++) {
			RPlane plane;
			MREAD(plane);
			pCell->m_planes.push_back(plane);
		}

		// VER1 포멧은 하나의 Cell 당 하나의 RGeometry 를 가진다.
		RGeometry* pGeometry = new RGeometry;
		DWORD idFVF = 0;
		idFVF |= RFVF[RFVF_XYZ].fvf;
		idFVF |= RFVF[RFVF_NORMAL].fvf;
		idFVF |= RFVF[RFVF_TEX2].fvf;

		pGeometry->m_dwFVF = idFVF;
		pCell->m_mapGeometryList.insert( RCell::IDGEOMETRYMAP::value_type( idFVF, pGeometry ) );

		LoadGeometry(file, pCell, pGeometry, pStaticMesh, pImpl);

		pStaticMesh->GetCell().push_back(pCell);
	}

	delete pImpl;

	return true;
}

bool RStaticMeshResourceFileLoadImpl_v1::LoadGeometry(MFile &file, RCell* pCell, RGeometry* pGeometry, RStaticMeshResource *pStaticMesh, RRsNodeLoadImpl* pImpl)
{
	MREAD(pGeometry->m_nNode);
	MREAD(pGeometry->m_nPolygon);
	MREAD(pGeometry->m_nVertices);
	MREAD(pGeometry->m_nIndices);

	pGeometry->MakeVertexInfo( pGeometry->m_VertexInfo );

	pGeometry->m_pNodes			= new RRsNode[pGeometry->m_nNode];
	pGeometry->m_pPolygonInfo	= new RPOLYGONINFO[pGeometry->m_nPolygon];
	pGeometry->m_pIndices		= new WORD[pGeometry->m_nIndices];
	pGeometry->m_pVertices		= new char[pGeometry->m_nVertices * pGeometry->m_VertexInfo.size];

	LoadVertex(file, pGeometry);

	MREAD(pGeometry->m_nTrees);

	RRsLoadHelper loading;
	loading.pLoadingNode	= pGeometry->m_pNodes;
	loading.pLoadingInfo	= pGeometry->m_pPolygonInfo;
	loading.pLoadingIndex	= pGeometry->m_pIndices;

	int nBaseVertex = 0;

	list<RRsAABBTree*> additiveTrees;
	list<RRsAABBTree*> alphaTrees;

	for(int k = 0; k < pGeometry->m_nTrees; k++)
	{
		RRsAABBTree* pTree = new RRsAABBTree;

		pCell->m_bb.Initialize();
		pTree->m_nBaseIndex = nBaseVertex;
		loading.nBaseIndex = 0;
		loading.nIndicies = 0;
		loading.pLoadingVertex = pGeometry->m_pVertices + ( nBaseVertex * pGeometry->m_VertexInfo.size );
		pTree->m_pRootNode = loading.pLoadingNode;
		pTree->m_pIndicies = loading.pLoadingIndex;
		MREAD(pTree->m_nMaterial);
		MREAD(pTree->m_nLightmapID);
		MREAD(pTree->m_nVertices);

		if(!pImpl->LoadRsNode(pTree->m_pRootNode, file, loading, pGeometry->m_VertexInfo)) {
			return false;
		}

		loading.pLoadingNode++;
		nBaseVertex+=pTree->m_nVertices;

		pTree->m_nIndexCount	= loading.nIndicies;

		// TEST
		RMaterial* pMaterial = pStaticMesh->GetMaterialList().at(pTree->m_nMaterial)->m_pMaterial;
		if(pMaterial->CheckFlag(RM_FLAG_ADDITIVE))
			additiveTrees.push_back(pTree);
		else if(pMaterial->CheckFlag(RM_FLAG_USEALPHATEST | RM_FLAG_USEOPACITY))
			alphaTrees.push_back(pTree);
		else
			pGeometry->m_pTrees.push_back(pTree);

		pCell->m_bb.Add(pTree->m_pRootNode->m_bb);
	}

	while(!alphaTrees.empty())
	{
		pGeometry->m_pTrees.push_back(alphaTrees.front());
		alphaTrees.pop_front();
	}
	while(!additiveTrees.empty())
	{
		pGeometry->m_pTrees.push_back(additiveTrees.front());
		additiveTrees.pop_front();
	}

	_ASSERT(loading.pLoadingNode - pGeometry->m_pNodes			== pGeometry->m_nNode);
	_ASSERT(loading.pLoadingInfo - pGeometry->m_pPolygonInfo	== pGeometry->m_nPolygon);
	_ASSERT(loading.pLoadingIndex - pGeometry->m_pIndices		== pGeometry->m_nIndices);

	return true;
}

bool RStaticMeshResourceFileLoadImpl_v1::LoadVertex(MFile &file, RGeometry* pGeometry)
{
	for( int k = 0; k < pGeometry->m_nVertices; k++ ) 
	{
		int cursor = pGeometry->m_VertexInfo.pos;
		char *pVertex = pGeometry->m_pVertices + (k * pGeometry->m_VertexInfo.size);
		if( !file.Read( &pVertex[cursor], sizeof(float) * 3 ) ) 
			return false;

		cursor = pGeometry->m_VertexInfo.normal;
		if( !file.Read( &pVertex[cursor], sizeof(float) * 3 ) ) 
			return false;
		((RVector3)pVertex[cursor]).Normalize();

		cursor = pGeometry->m_VertexInfo.texCoord;
		if( !file.Read( &pVertex[cursor], sizeof(float) * 4 ) )
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// version 2 : Static Mesh Aplha (VCOLOR)
//////////////////////////////////////////////////////////////////////////
bool RStaticMeshResourceFileLoadImpl_v2::LoadCell(MFile &file, RStaticMeshResource *pStaticMesh)
{
	// RS3_VER1
	RRsNodeLoadImpl *pImpl = NULL;
	pImpl = new RRsNodeLoadImpl_v1;
	_ASSERT(pImpl);

	int nCells;
	MREAD(nCells);
	for(int i=0;i<nCells;i++)
	{
		RCell *pCell = new RCell;

		int nLength;
		MREAD(nLength);
		pCell->m_strName.resize(nLength);
		file.Read( &pCell->m_strName[0], nLength );

		int nPlanes;
		MREAD(nPlanes);
		for(int j=0;j<nPlanes;j++) {
			RPlane plane;
			MREAD(plane);
			pCell->m_planes.push_back(plane);
		}

		// RGeometry 정보
		int nGeometry;
		MREAD(nGeometry);
		for( int j = 0; j < nGeometry; ++j )
		{
			RGeometry* pGeometry = new RGeometry;

			DWORD dwFVF;
			MREAD(dwFVF);
			pGeometry->m_dwFVF = dwFVF;
			pCell->m_mapGeometryList.insert( RCell::IDGEOMETRYMAP::value_type( dwFVF, pGeometry ) );

			LoadGeometry(file, pCell, pGeometry, pStaticMesh, pImpl);
		}

		pStaticMesh->GetCell().push_back(pCell);
	}

	delete pImpl;

	return true;
}

bool RStaticMeshResourceFileLoadImpl_v2::LoadVertex(MFile &file, RGeometry* pGeometry)
{
	for( int k = 0; k < pGeometry->m_nVertices; k++ ) 
	{
		// Position
		int cursor = pGeometry->m_VertexInfo.pos;
		char *pVertex = pGeometry->m_pVertices + (k * pGeometry->m_VertexInfo.size);
		if( !file.Read( &pVertex[cursor], sizeof(float) * 3 ) ) 
			return false;

		//mlog("%f, %f, %f\n", ((RVector*)(pVertex + cursor))->x, ((RVector*)(pVertex + cursor))->y, ((RVector*)(pVertex + cursor))->z );

		// Normal
		cursor = pGeometry->m_VertexInfo.normal;
		if( !file.Read( &pVertex[cursor], sizeof(float) * 3 ) ) 
			return false;
		((RVector3)pVertex[cursor]).Normalize();

		// Vertex Color
		RVector vColor(1.0f, 1.0f, 1.0f);
		RVector vTmp;
		cursor = pGeometry->m_VertexInfo.color;

		if(cursor >= 0)
		{
			if( !file.Read( &vColor, sizeof(float) * 3 ) )
				return false;

			float f = (vColor.x + vColor.y + vColor.z) / 3.f;
			int gray = f * 255;
			if( gray > 255 )
				gray = 255;

			DWORD dwColor = D3DCOLOR_ARGB( gray, 255, 255, 255 );
			memcpy( &pVertex[cursor], &dwColor, sizeof(DWORD) );
		}
		else
		{
			file.Read( vTmp, sizeof(float) * 3 );
		}

		// Texture Coord
		cursor = pGeometry->m_VertexInfo.texCoord;
		if( !file.Read( &pVertex[cursor], sizeof(float) * 4 ) )
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// version 3 : Polygon에 충돌 정보 기록
//////////////////////////////////////////////////////////////////////////
bool RStaticMeshResourceFileLoadImpl_v3::LoadCell(MFile &file, RStaticMeshResource *pStaticMesh)
{
	// RS3_VER2
	RRsNodeLoadImpl *pImpl = NULL;
	pImpl = new RRsNodeLoadImpl_v2;
	_ASSERT(pImpl);

	int nCells;
	MREAD(nCells);
	for( int i = 0; i < nCells; i++ )
	{
		RCell *pCell = new RCell;

		int nLength;
		MREAD(nLength);
		pCell->m_strName.resize(nLength);
		file.Read( &pCell->m_strName[0], nLength );

		int nPlanes;
		MREAD(nPlanes);
		for(int j=0;j<nPlanes;j++) {
			RPlane plane;
			MREAD(plane);
			pCell->m_planes.push_back(plane);
		}

		// RGeometry 정보
		int nGeometry;
		MREAD(nGeometry);
		for( int j = 0; j < nGeometry; ++j )
		{
			RGeometry* pGeometry = new RGeometry;

			DWORD dwFVF;
			MREAD(dwFVF);
			pGeometry->m_dwFVF = dwFVF;
			pCell->m_mapGeometryList.insert( RCell::IDGEOMETRYMAP::value_type( dwFVF, pGeometry ) );

			LoadGeometry(file, pCell, pGeometry, pStaticMesh, pImpl);
		}

		pStaticMesh->GetCell().push_back(pCell);
	}

	delete pImpl;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// version 4 : Cell Face 정보 기록
//////////////////////////////////////////////////////////////////////////
bool RStaticMeshResourceFileLoadImpl_v4::LoadCell(MFile &file, RStaticMeshResource *pStaticMesh)
{
	// RS3_VER2
	RRsNodeLoadImpl *pImpl = NULL;
	pImpl = new RRsNodeLoadImpl_v2;
	_ASSERT(pImpl);

	int nCells;
	MREAD(nCells);
	for( int i = 0; i < nCells; i++ )
	{
		RCell *pCell = new RCell;

		int nLength;
		MREAD(nLength);
		pCell->m_strName.resize(nLength);
		file.Read( &pCell->m_strName[0], nLength );

		int nPlanes;
		MREAD(nPlanes);
		for(int j=0;j<nPlanes;j++) {
			RPlane plane;
			MREAD(plane);
			pCell->m_planes.push_back(plane);
		}

		int nCellFaces;
		MREAD(nCellFaces);
		for( int j = 0; j < nCellFaces; j++ )
		{
			RCellFace* pCellFace = new RCellFace;
			MREAD(pCellFace->nVerts);
			pCellFace->pVerts = new RVector[pCellFace->nVerts];
			
			for( int k = 0; k < pCellFace->nVerts; k++ )
				MREAD(pCellFace->pVerts[k]);

			pCell->m_cellFaces.push_back(pCellFace);
		}

		// RGeometry 정보
		int nGeometry;
		MREAD(nGeometry);
		for( int j = 0; j < nGeometry; ++j )
		{
			RGeometry* pGeometry = new RGeometry;

			DWORD dwFVF;
			MREAD(dwFVF);
			pGeometry->m_dwFVF = dwFVF;
			pCell->m_mapGeometryList.insert( RCell::IDGEOMETRYMAP::value_type( dwFVF, pGeometry ) );

			LoadGeometry(file, pCell, pGeometry, pStaticMesh, pImpl);
		}

		pStaticMesh->GetCell().push_back(pCell);
	}

	delete pImpl;

	return true;
}

bool RRsNodeLoadImpl_v1::LoadRsNode(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo)
{
	MREAD(pRsNode->m_bb);

	bool bLeaf;
	MREAD(bLeaf);

	if(!bLeaf) 
	{
		pRsNode->m_pChildren[0] = ++loading.pLoadingNode;
		pRsNode->m_pChildren[1] = ++loading.pLoadingNode;

		if(!LoadRsNode(pRsNode->m_pChildren[0], file,loading,VertexInfo)) return false;
		if(!LoadRsNode(pRsNode->m_pChildren[1], file,loading,VertexInfo)) return false;
	}

	MREAD(pRsNode->m_nPolygon);
	pRsNode->m_pPolygonInfo	= loading.pLoadingInfo;
	pRsNode->m_pIndicies	= loading.pLoadingIndex;
	pRsNode->m_nIndicies	= 0;
	pRsNode->m_nMinIndex	= loading.nBaseIndex;

	LoadPolygon(pRsNode, file, loading, VertexInfo);

	pRsNode->m_nMaxIndex = loading.nBaseIndex -1;

	return true;
}

bool RRsNodeLoadImpl_v1::LoadPolygon(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo)
{
	for( int i = 0; i < pRsNode->m_nPolygon; i++ ) 
	{
		RPOLYGONINFO *pInfo = loading.pLoadingInfo++;

		MREAD(pInfo->nMaterial);
		MREAD(pInfo->dwFlags);
		//////////////////////////////////////////////////////////////////////////
		// 폴리곤의 충돌 유무에 대한 정보 VER2 이하에서는 항상 충돌
		pInfo->dwFlags = RM_FLAG_COLLISION_MESH;

		MREAD(pInfo->nVertices);

		int nBaseIndex__;
		MREAD(nBaseIndex__);

		pInfo->pVertices = loading.pLoadingVertex;
		// tangent 를 계산
		//		int nBaseIndex = pInfo->nBaseIndex;

		for( int j = 0; j < pInfo->nVertices-2; j++ ) 
		{
			//	RSTATICMESHVERTEX *x0 = &pInfo->pVertices[0],
			//		*x1 = &pInfo->pVertices[j+1],
			//		*x2 = &pInfo->pVertices[j+2];

			//	RVector tangent;
			//	CalcTangentBinormal(x0->pos, x0->tu1, x0->tv1, x1->pos, x1->tu1, x1->tv1, x2->pos, x2->tu1, x2->tv1, &tangent, NULL);
			//	x0->tangent = x1->tangent = x2->tangent = Normalize(tangent);

			WORD *pIndex = loading.pLoadingIndex;
			*pIndex++ = loading.nBaseIndex;
			*pIndex++ = loading.nBaseIndex + j + 1;
			*pIndex++ = loading.nBaseIndex + j + 2;
			loading.pLoadingIndex+=3;
			loading.nIndicies += 3;
			pRsNode->m_nIndicies += 3;
		}

		loading.pLoadingVertex	+= pInfo->nVertices * VertexInfo.size;
		loading.nBaseIndex		+= pInfo->nVertices;
	}

	return true;
}

bool RRsNodeLoadImpl_v2::LoadPolygon(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo)
{
	for( int i = 0; i < pRsNode->m_nPolygon; i++ ) 
	{
		RPOLYGONINFO *pInfo = loading.pLoadingInfo++;

		MREAD(pInfo->nMaterial);
		MREAD(pInfo->dwFlags);
		MREAD(pInfo->nVertices);

		int nBaseIndex__;
		MREAD(nBaseIndex__);

		pInfo->pVertices = loading.pLoadingVertex;

		for( int j = 0; j < pInfo->nVertices-2; j++ ) 
		{
			WORD *pIndex = loading.pLoadingIndex;
			_ASSERT(loading.nBaseIndex <= 65533);	// 이 경우 생기면 폴리곤이 깨집니다, 새 익스포터로 익스포트 해야함.
			*pIndex++ = loading.nBaseIndex;
			*pIndex++ = loading.nBaseIndex + j + 1;
			*pIndex++ = loading.nBaseIndex + j + 2;
			loading.pLoadingIndex+=3;
			loading.nIndicies += 3;
			pRsNode->m_nIndicies += 3;
		}

		loading.pLoadingVertex	+= pInfo->nVertices * VertexInfo.size;
		loading.nBaseIndex		+= pInfo->nVertices;
	}

	return true;
}

}
