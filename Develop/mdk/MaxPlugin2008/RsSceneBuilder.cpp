// RsSceneBuilder.cpp: implementation of the RsSceneBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "MXml.h"

#include "RsSceneBuilder.h"
#include "RVersions.h"

#include "FileInfo.h"
#include "RToken.h"
#include "Max.h"

#include "MDebug.h"

#include "RsBuilder.h"

#define XML_FORMAT_FLOAT	"%.7f"

char *Format(char *buffer,RVector3 &v)
{
	sprintf(buffer,XML_FORMAT_FLOAT" "XML_FORMAT_FLOAT" "XML_FORMAT_FLOAT,v.x,v.y,v.z);
	return buffer;
}

char *Format(char *buffer,float f)
{
	sprintf(buffer,XML_FORMAT_FLOAT,f);
	return buffer;
}

char *Format(char *buffer,DWORD dw)
{
	sprintf(buffer,"%x",dw);
	return buffer;
}

RGeometry::RGeometry()
{

}

RGeometry::~RGeometry()
{
	for(IDBUILDERMAP::iterator itr = m_trees.begin() ; itr!=m_trees.end(); itr++)
		delete itr->second;
	m_trees.clear();
}

RsBuilder* RGeometry::GetTree( MIDLIDPAIR idPair)
{
	IDBUILDERMAP::iterator itr = m_trees.find( idPair);
	if(itr == m_trees.end() )
	{
		RsBuilder* pNewBuilder = new RsBuilder;
		m_trees.insert( IDBUILDERMAP::value_type( idPair, pNewBuilder));
		pNewBuilder->m_nMaterial = idPair.first;
		pNewBuilder->m_nLightMapID = idPair.second;
		pNewBuilder->BeginBuild();
		return pNewBuilder;
	}
	return itr->second;
}

RCell::~RCell()
{
	for(IDGEOMETRYMAP::iterator itr = m_mapGeometryList.begin() ; itr!=m_mapGeometryList.end(); itr++)
		delete itr->second;
	m_mapGeometryList.clear();

	for(size_t i = 0; i < m_cellFaces.size(); ++i)
		delete m_cellFaces[i];
	m_cellFaces.clear();
}

RGeometry* RCell::GetGeometry(DWORD idFVF)
{
	IDGEOMETRYMAP::iterator itr = m_mapGeometryList.find( idFVF );
	if( itr == m_mapGeometryList.end() )
	{
		RGeometry* pNewGeometry = new RGeometry;
		m_mapGeometryList.insert( IDGEOMETRYMAP::value_type( idFVF, pNewGeometry ) );
		pNewGeometry->m_dwFVF = idFVF;
		return pNewGeometry;
	}
	return itr->second;
}

RsSceneBuilder::RsSceneBuilder()
{
	MapName[0] = NULL;
}
 
RsSceneBuilder::~RsSceneBuilder()
{
	Destroy();
}

void RsSceneBuilder::Destroy()
{
	for(RCellList::iterator is = m_cells.begin(); is!=m_cells.end(); is++)
		delete *is;
	m_cells.clear();

	for(RPortalList::iterator i=m_portals.begin();i!=m_portals.end();i++)
		delete *i;
	m_portals.clear();

}

// Dummy function for progress bar
DWORD WINAPI fn2(LPVOID arg)
{
	return(0);
}

bool RsSceneBuilder::Build(Interface *i, RMaxMaterialKeeper* pMaterials, int nPolygonPerNodes)
{
	int nTotalTree = 0;
	int nCurrentTree = 0;
	// material id 와 lightmap id 가 같은것들끼리 aabbtree 로 묶는다
	for(RCellList::iterator is = m_cells.begin(); is != m_cells.end(); is++)
	{
		RCell *pCell = *is;
		// Cell->Geometry->Polygons 별로 나눈다
		for( RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin() ; itr != pCell->m_mapGeometryList.end(); itr++ )
		{
			RGeometry* pGeometry = itr->second;
			for( int i = 0; i < (int)pGeometry->m_sourcePolygons.size(); i++ )
			{
				rpolygon* pPoly = pGeometry->m_sourcePolygons[i];
				RGeometry::MIDLIDPAIR idpair( pPoly->pMaterial->GetIndex(), pPoly->nLightMapID );
				RsBuilder* pBuilder = pGeometry->GetTree(idpair);
				pBuilder->AddPolygon(pPoly);
			}
			nTotalTree += pGeometry->m_trees.size();
		}
	}

	i->ProgressStart( GetString(IDS_PROGRESS_MSG2), TRUE, fn2, NULL );

	// aabbtree 를 만들고, 크기가 큰것을 쪼갠다
	for( RCellList::iterator is = m_cells.begin(); is != m_cells.end(); is++ )
	{
		RCell *pCell = *is;
		for( RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin() ; itr != pCell->m_mapGeometryList.end(); itr++ )
		{
			RGeometry* pGeometry = itr->second;
			for( RGeometry::IDBUILDERMAP::iterator itr = pGeometry->m_trees.begin() ; itr != pGeometry->m_trees.end(); )
			{
				RsBuilder* pBuilder = itr->second;
				pBuilder->EndBuild(nPolygonPerNodes);

				// 버텍스 수를 체크해서 64k 를 넘어가면 쪼갠다
				int nVertexCount = pBuilder->m_pRootNode->GetVertexCount();
				if( pBuilder->m_pRootNode->GetVertexCount() > 65535 )
				{
					mlog(" >64k vertex. split\n");

					int nVertexCountChild0 = pBuilder->m_pRootNode->m_pChildren[0]->GetVertexCount();
					int nVertexCountChild1 = pBuilder->m_pRootNode->m_pChildren[1]->GetVertexCount();

					// Child1
					RGeometry::IDBUILDERMAP::key_type idPair = itr->first;
					RsBuilder* pNewBuilder = new RsBuilder;
					pNewBuilder->m_bBuilt = true;
					pGeometry->m_trees.insert( RGeometry::IDBUILDERMAP::value_type( idPair , pNewBuilder));
					
					pNewBuilder->m_nMaterial = idPair.first;
					pNewBuilder->m_nLightMapID = idPair.second;
					pNewBuilder->m_pRootNode = pBuilder->m_pRootNode->m_pChildren[1];

					
					
					RAABBTreeNode* pNodeToDelete = pBuilder->m_pRootNode;
					pBuilder->m_pRootNode = pBuilder->m_pRootNode->m_pChildren[0];

					pNodeToDelete->m_pChildren[0] = NULL;
					pNodeToDelete->m_pChildren[1] = NULL;
					delete pNodeToDelete;
					
					nTotalTree++;
				}else{
					itr++;
					nCurrentTree++;
				}
				i->ProgressUpdate((int)((float)nCurrentTree/nTotalTree * 100.0f)); 
			}
		}
	}

	// 버텍스 버퍼 / 인덱스 버퍼 생성
	for( RCellList::iterator is = m_cells.begin(); is != m_cells.end(); is++ )
	{
		RCell *pCell = *is;
		for( RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin() ; itr != pCell->m_mapGeometryList.end(); itr++ )
		{
			RGeometry* pGeometry = itr->second;
			for( RGeometry::IDBUILDERMAP::iterator itr = pGeometry->m_trees.begin() ; itr != pGeometry->m_trees.end(); itr++ )
			{
				RsBuilder* pBuilder = itr->second;
				int nCount = pBuilder->m_pRootNode->GetVertexCount();
				_ASSERT(nCount<=65535);
				pBuilder->BuildVertex();
				i->ProgressUpdate((int)((float)nCurrentTree/nTotalTree * 100.0f)); 
			}
		}
	}
	i->ProgressEnd();

	return true;
}

bool RsSceneBuilder::SaveRS(const char *fileName)
{
	ConnectPortals();
	ValidatePortals();

	FILE *file=fopen(fileName,"wb+");
	if(!file) return false;

	RHEADER header(RS3_ID,RS3_VERSION);
	fwrite(&header,sizeof(RHEADER),1,file);

	int nPortal = (int)m_portals.size();
	fwrite(&nPortal,sizeof(nPortal),1,file);
	for(RPortalList::iterator i=m_portals.begin();i!=m_portals.end();i++)
	{
		RPortal *pPortal = *i;
		int nLength = pPortal->m_strName.length();
		fwrite(&nLength, sizeof(nLength),1,file);
		fwrite(pPortal->m_strName.c_str(), nLength, 1, file);
		fwrite(&pPortal->nVertices,sizeof(int),1,file);
		fwrite(pPortal->m_pVertices,sizeof(Point3),pPortal->nVertices,file);
		fwrite(&pPortal->nCellID1,sizeof(int),1,file);
		fwrite(&pPortal->nCellID2,sizeof(int),1,file);
	}

	int nSector = (int)m_cells.size();
	fwrite(&nSector,sizeof(nSector),1,file);

	for(RCellList::iterator is = m_cells.begin(); is!=m_cells.end(); is++)
	{
		RCell *pCell = *is;
		mlog("writing %s ( cell mesh %d planes ), ", pCell->m_strName.c_str(), pCell->m_planes.size() );

		int nLength = pCell->m_strName.length();
		fwrite(&nLength, sizeof(nLength),1,file);
		fwrite(pCell->m_strName.c_str(), nLength, 1, file);
		int nPlanes = (int)pCell->m_planes.size();
		fwrite(&nPlanes,sizeof(nPlanes),1,file);
		for( list<RPlane>::iterator ip = pCell->m_planes.begin(); ip!=pCell->m_planes.end(); ip++) 
		{
			RPlane plane = *ip;
			fwrite(&plane,sizeof(RPlane),1,file);
		}

		int nCellFaces = (int)pCell->m_cellFaces.size();
		fwrite(&nCellFaces,sizeof(int),1,file);
		for( size_t i = 0; i < pCell->m_cellFaces.size(); i++ )
		{
			RCellFace* pCellFace = pCell->m_cellFaces[i];
			fwrite(&pCellFace->nVerts,sizeof(int),1,file);
			fwrite(pCellFace->pVerts,sizeof(RVector)*pCellFace->nVerts,1,file);
		}

		// RGeometry 정보
		int nGeometry = (int)pCell->m_mapGeometryList.size();
		fwrite(&nGeometry, sizeof(nGeometry), 1, file);

		for( RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin() ; itr != pCell->m_mapGeometryList.end(); itr++ )
		{
			RGeometry* pGeometry = itr->second;
			mlog("%d polygon(s), ", pGeometry->m_sourcePolygons.size() );

			// Geometry FVF
			fwrite(&pGeometry->m_dwFVF, sizeof(DWORD), 1, file);

			SaveRS_Tree(pGeometry, file);
		}
	}

	fclose(file);
	return true;
}

void RsSceneBuilder::SaveRS_Tree(RGeometry* pGeometry, FILE *file)
{
	int nNodeCount = 0;
	int nPolygonCount = 0;
	int nVerticesCount = 0;
	int nIndicesCount = 0;

	for(RGeometry::IDBUILDERMAP::iterator itr = pGeometry->m_trees.begin() ; itr!=pGeometry->m_trees.end(); itr++)
	{
		RsBuilder* pBuilder = itr->second;
		nNodeCount += pBuilder->m_pRootNode->GetNodeCount();
		nPolygonCount += pBuilder->m_pRootNode->GetPolygonCount();
		nIndicesCount += ((RRsNode*)pBuilder->m_pRootNode)->GetIndicesCount();
		nVerticesCount+= pBuilder->m_vertices.size();
	}

	fwrite(&nNodeCount,sizeof(int),1,file);
	fwrite(&nPolygonCount,sizeof(int),1,file);
	fwrite(&nVerticesCount,sizeof(int),1,file);
	fwrite(&nIndicesCount,sizeof(int),1,file);

	mlog("%d nodes, %d polygon, %d vertices,  %d indices\n", nNodeCount, nPolygonCount, nVerticesCount, nIndicesCount );

	// 버텍스리스트 저장
	for(RGeometry::IDBUILDERMAP::iterator itr = pGeometry->m_trees.begin() ; itr!=pGeometry->m_trees.end(); itr++)
	{
		RsBuilder* pBuilder = itr->second;
		for(size_t iv = 0 ; iv < pBuilder->m_vertices.size(); iv++) 
		{
			rvertex *pv = pBuilder->m_vertices[iv];
			pv->Save(file);
		}
	}

	// 트리들 저장
	int nTree = pGeometry->m_trees.size();
	fwrite(&nTree,sizeof(nTree),1,file);
	for(RGeometry::IDBUILDERMAP::iterator itr = pGeometry->m_trees.begin() ; itr!=pGeometry->m_trees.end(); itr++)
	{
		RsBuilder* pBuilder = itr->second;
		pBuilder->Save(file);
	}
}

void RsSceneBuilder::ConnectPortals()		// 포탈을 섹터와 연결한다
{
	for(RPortalList::iterator i=m_portals.begin();i!=m_portals.end();i++)
	{
		RPortal *pPortal = *i;
		pair<int, RCell*> cellInfo = m_cells.Find( pPortal->m_strParentCellName.c_str() );
		pPortal->nCellID1 = cellInfo.first;
		pPortal->m_pParentCell = cellInfo.second;

		if(pPortal->nCellID1==-1) 
			mlog3("cannot find parent cell in portal %s\n", pPortal->m_strName.c_str());

		if(pPortal->m_strTargetCellName.empty())
			pPortal->nCellID2 = 0;	// global cell
		else
		{
			pair<int, RCell*> cellInfo2 = m_cells.Find( pPortal->m_strTargetCellName.c_str() );

			pPortal->nCellID2 = cellInfo2.first;
			if(pPortal->nCellID2==-1) 
				mlog3("cannot find target cell in portal %s\n", pPortal->m_strName.c_str());
		}
	}
}

void RsSceneBuilder::ValidatePortals()		// 포탈 데이터 검증
{
	for(RPortalList::iterator i=m_portals.begin();i!=m_portals.end();i++)
	{
		RPortal *pPortal = *i;
		RCell* pCell = pPortal->m_pParentCell;
		if(pCell)
		{
			const Point3& pV = pPortal->m_pVertices[0];
			bool isAlign = pCell->IsAlign( pPortal->nVertices, pPortal->m_pVertices );
			if( !isAlign )
				mlog3("portal %s isn't align parent cell.\n", pPortal->m_strName.c_str());
		}
	}
}

void RsSceneBuilder::AddCell(RCell* pCell)
{ 
	pair<int, RCell*> cellInfo = m_cells.Find(pCell->m_strName.c_str());
	if(cellInfo.first != -1 )
		mlog3("cell %s duplicated", pCell->m_strName.c_str() );

	m_cells.push_back(pCell); 
}

bool RsSceneBuilder::IsEmpty()
{
	if(m_cells.size()>1) return false;

	// 기본 global cell 은 무조건 있으므로 이게 비어있으면 아무것도 없는것이다
	RCell* pGlobalCell = m_cells.front();
	return pGlobalCell->m_mapGeometryList.empty();
}
