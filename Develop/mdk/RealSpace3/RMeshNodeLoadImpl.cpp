#include "stdafx.h"
#include "MFileSystem.h"
#include "RMesh.h"
#include "RMeshNode.h"
#include "RMeshNodeLoadImpl.h"

#include "RD3DVertexUtil.h"

namespace rs3 {


#define RFILEREAD1(str)																\
{	int _size = 0;																	\
	if ( pFile->Read( &_size, sizeof( int)) == false)	return false;				\
	str.resize( _size);																\
	if ( pFile->Read( &str[ 0], _size) == false)		return false;				\
	if ( _size > 0  &&  str[ _size - 1] == 0)			str.resize( _size - 1);		\
}

#define RFILEREAD2(buff,read)														\
{	if ( pFile->Read( (buff), (read)) == false)			return false;				\
}


bool RMeshNodeLoaderImpl_v12::Load(RMeshNode *pNode, RFile *pFile)
{
	_ASSERT(pNode);
	_ASSERT(pFile);

	if ( !LoadName(pNode, pFile))
		return false;
	if ( !LoadInfo(pNode, pFile))
		return false;
	if ( !LoadVertex(pNode, pFile))
		return false;
	if ( !LoadFace(pNode, pFile))
		return false;
	if ( !LoadVertexInfo(pNode, pFile))
		return false;
	if ( !LoadEtc(pNode, pFile))
		return false;

	// 만들어져 있는 상태인지 아닌지를 검사하여,
	// 빈 노드에는 노드의 원점을 BoundingBox 에 넣는다. ( 오류 방지 )

	if ( pNode->m_boundingBox.vmin.x == FLT_MAX )	
	{
		pNode->m_boundingBox.vmin = RVector(-0.1f,-0.1f,-0.1f);
		pNode->m_boundingBox.vmax = RVector( 0.1f, 0.1f, 0.1f);
	}

	return true;
}

bool RMeshNodeLoaderImpl_v13::Load(RMeshNode *pNode, RFile *pFile)
{
	_ASSERT(pNode);
	_ASSERT(pFile);

	if ( !LoadName(pNode, pFile))
		return false;
	if ( !LoadInfo(pNode, pFile))
		return false;
	if ( !LoadVertex(pNode, pFile))
		return false;
	if ( !LoadFace(pNode, pFile))
		return false;
	if ( !LoadVertexInfo(pNode, pFile))
		return false;
	if ( !LoadEtc(pNode, pFile))
		return false;

	// v13 에서는 BoundingBox 값은 로드 되므로, 문제 없음.

	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadName(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD1( pNode->m_Name);
	RFILEREAD1( pNode->m_ParentName);
	RFILEREAD2( &pNode->m_nParentNodeID, 4);
	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadInfo(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_dwFlag,4);
	RFILEREAD2(&pNode->m_nAlign,4);

	if (pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER11)
	{
		int nAniPartsType;
		RFILEREAD2(&nAniPartsType,4);
		int nPartsPosInfoType;
		RFILEREAD2(&nPartsPosInfoType,4);
		int nPartsType;
		RFILEREAD2(&nPartsType,4);
	}

	RFILEREAD2(&pNode->m_matLocal, sizeof(RMatrix));
	pNode->m_matResult = pNode->m_matLocal;

	if (pNode->m_pParentMesh->GetFileVersion()>=EXPORTER_MESH_VER11)
	{
		RFILEREAD2(&pNode->m_fBaseVisibility, 4);
	}

	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadVertex(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		RVector* vTangentTanTable = NULL;

		vTangentTanTable = new RVector[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(vTangentTanTable,sizeof(RVector)*pNode->m_nTangentTanTableCnt);

		// copy
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		for( int i = 0; i < pNode->m_nTangentTanTableCnt; ++i )
		{
			pNode->m_pTangentTanTable[i] = RVector4(vTangentTanTable[i], 1);	// w값 default는 1	
		}

		SAFE_DELETE_ARRAY(vTangentTanTable);
	}	

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadFace(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nFaceCnt, 4);

	if (pNode->m_nFaceCnt) 
	{
		if (pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER12)
		{
			pNode->m_pAllFaceSubData = new RFaceSubData[3*pNode->m_nFaceCnt];
			pNode->m_pPolygonTable = new RMeshPolygonData[pNode->m_nFaceCnt];

			for(int i=0;i<pNode->m_nFaceCnt;i++)
			{
				pNode->m_pPolygonTable[i].nVertices = 3;
				pNode->m_pPolygonTable[i].index = pNode->m_pAllFaceSubData+i*3;

				RFaceSubData_v12 arrTemp[3];
				RFILEREAD2(arrTemp, sizeof(RFaceSubData_v12)*3);
				for(int j=0; j<3; ++j)
				{
					pNode->m_pPolygonTable[i].index[j].p = arrTemp[j].p;
					pNode->m_pPolygonTable[i].index[j].n = arrTemp[j].n;
					pNode->m_pPolygonTable[i].index[j].uv = arrTemp[j].uv;
					pNode->m_pPolygonTable[i].index[j].uv2 = -1;
					pNode->m_pPolygonTable[i].index[j].n_bin = arrTemp[j].n_bin;
					pNode->m_pPolygonTable[i].index[j].n_tan = arrTemp[j].n_tan;
				}

				RFILEREAD2(&pNode->m_pPolygonTable[i].mtrl_id, sizeof(short));
			}
			// 호환을 위해
			pNode->m_nTotalDegree = pNode->m_nFaceCnt*3;
			pNode->m_nTotalTriangles = pNode->m_nFaceCnt;
		}
		else
		// 새 버젼은 폴리곤 단위
		{
			RFILEREAD2(&pNode->m_nTotalDegree,4);
			RFILEREAD2(&pNode->m_nTotalTriangles,4);

			pNode->m_pAllFaceSubData = new RFaceSubData[pNode->m_nTotalDegree];
			pNode->m_pPolygonTable = new RMeshPolygonData[pNode->m_nFaceCnt];

			RFaceSubData* pCurrent = pNode->m_pAllFaceSubData;

			for (int i=0;i<pNode->m_nFaceCnt;i++)
			{
				int nDeg = 0;
				RFILEREAD2(&nDeg,4);
				pNode->m_pPolygonTable[i].nVertices = nDeg;
				pNode->m_pPolygonTable[i].index = pCurrent;

				RFaceSubData_v12* pTemp = new RFaceSubData_v12[nDeg];
				RFILEREAD2(pTemp, sizeof(RFaceSubData_v12)*nDeg);
				for(int j=0; j<nDeg; ++j)
				{
					pNode->m_pPolygonTable[i].index[j].p = pTemp[j].p;
					pNode->m_pPolygonTable[i].index[j].n = pTemp[j].n;
					pNode->m_pPolygonTable[i].index[j].uv = pTemp[j].uv;
					pNode->m_pPolygonTable[i].index[j].uv2 = -1;
					pNode->m_pPolygonTable[i].index[j].n_bin = pTemp[j].n_bin;
					pNode->m_pPolygonTable[i].index[j].n_tan = pTemp[j].n_tan;
				}
				delete[] pTemp;
			

				RFILEREAD2(&pNode->m_pPolygonTable[i].mtrl_id, sizeof(short));

				pCurrent+= nDeg;
			}
			_ASSERT(pCurrent - pNode->m_pAllFaceSubData == pNode->m_nTotalDegree);
		}
	}
	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadVertexInfo(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nPointColorCnt,4 );
	if (pNode->m_nPointColorCnt) 
	{
		pNode->m_pPointColorTable = new RVector[pNode->m_nPointColorCnt];
		RFILEREAD2(pNode->m_pPointColorTable, sizeof(RVector)*pNode->m_nPointColorCnt);
	}

	if( (pNode->m_nPointCnt==0) || (pNode->m_nFaceCnt==0) ) {
		pNode->AddFlag(RM_FLAG_DUMMY_MESH);
	}

	RFILEREAD2(&pNode->m_nMtrlID,4 );
	RFILEREAD2(&pNode->m_nPhysiqueCnt,4 );

	if (pNode->m_nPhysiqueCnt) 
	{
		int nSize = 0;

		// 맥스에서 낸 경고, polygon object 가 아닌 mesh 로 스키닝하면 발생함
		_ASSERT(pNode->m_nPointCnt==pNode->m_nPhysiqueCnt);
		//pNode->m_pParentMesh->m_isPhysiqueMesh = true;

		pNode->m_pPhysique = new RPhysiqueInfo[pNode->m_nPhysiqueCnt];
		ZeroMemory(pNode->m_pPhysique, pNode->m_nPhysiqueCnt * sizeof(RPhysiqueInfo));

		for ( int j=0;j<pNode->m_nPhysiqueCnt;j++) 
		{
			RFILEREAD2(&nSize,4);
			pNode->m_pPhysique[j].Create( nSize );

			for( int k=0;k<nSize;k++) 
			{
				RFILEREAD2(&pNode->m_pPhysique[j].m_Data[k].cid,sizeof(WORD));
				RFILEREAD2(&pNode->m_pPhysique[j].m_Data[k].pid,sizeof(WORD));
				RFILEREAD2(&pNode->m_pPhysique[j].m_Data[k].weight,sizeof(float));
			}

			// 피지크가 3개까지만 가능하므로 재처리한다. 이거 어찌해야하노.. -_-
			// 셰이더에 넘기는건 float4니까 하나는 유추하는 걸로 해서 5개까지 가능..  3->5 - 090707, COMMENT OZ
			// 하지~만~ 매트릭스 인덱스가 4개까지만 된다는거~~~ 긍까 블렌드는 4개
			if (nSize > PHYSIQUE_MAX_WEIGHT)
			{
				// TODO : 손질이 필요하다 -_-
				RPhysiqueInfo& physique = pNode->m_pPhysique[j];
				for (int i = 0; i<nSize; i++)
				{
					for ( int j = i+1; j<nSize; j++)
					{
						if(physique.GetWeight(i)<physique.GetWeight(j))
						{
							RPhysiqueSubData temp;
							temp = physique.m_Data[i];
							physique.m_Data[i] = physique.m_Data[j];
							physique.m_Data[j] = temp;
						}
					}
				}

#if PHYSIQUE_MAX_WEIGHT == 4
				float fSum3 = physique.m_Data[0].weight + physique.m_Data[1].weight + physique.m_Data[2].weight + physique.m_Data[3].weight;
				physique.m_Data[0].weight = physique.m_Data[0].weight / fSum3;
				physique.m_Data[1].weight = physique.m_Data[1].weight / fSum3;
				physique.m_Data[2].weight = physique.m_Data[2].weight / fSum3;
				physique.m_Data[3].weight = physique.m_Data[3].weight / fSum3;
				physique.m_num = 4;
#else
				float fSum3 = physique.m_Data[0].weight + physique.m_Data[1].weight + physique.m_Data[2].weight;
				physique.m_Data[0].weight = physique.m_Data[0].weight / fSum3;
				physique.m_Data[1].weight = physique.m_Data[1].weight / fSum3;
				physique.m_Data[2].weight = physique.m_Data[2].weight / fSum3;
				physique.m_num = 3;
#endif
			}
		}
	}
	return true;
}

bool RMeshNodeLoaderImpl_v12::LoadEtc(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nBoneTableCnt,4);
	if(pNode->m_nBoneTableCnt) 
	{
		pNode->m_pBoneTable = new RMatrix[pNode->m_nBoneTableCnt];
		pNode->m_pBoneTableIndex = new WORD[pNode->m_nBoneTableCnt];

		RFILEREAD2(pNode->m_pBoneTable,sizeof(RMatrix)*pNode->m_nBoneTableCnt);
		RFILEREAD2(pNode->m_pBoneTableIndex,sizeof(WORD)*pNode->m_nBoneTableCnt);
	}

	RFILEREAD2(&pNode->m_nVertexIndexTableCnt,4);
	if (pNode->m_nVertexIndexTableCnt) 
	{
		pNode->m_pVertexIndexTable	= new RVertexIndex[pNode->m_nVertexIndexTableCnt];
		RVertexIndex_v12* arrTemp		= new RVertexIndex_v12[pNode->m_nVertexIndexTableCnt];
		RFILEREAD2(arrTemp,sizeof(RVertexIndex_v12)*pNode->m_nVertexIndexTableCnt);

		for(int i=0; i<pNode->m_nVertexIndexTableCnt; ++i)
		{
			pNode->m_pVertexIndexTable[i].p		= arrTemp[i].p;
			pNode->m_pVertexIndexTable[i].n		= arrTemp[i].n;
			pNode->m_pVertexIndexTable[i].uv	= arrTemp[i].uv;
			pNode->m_pVertexIndexTable[i].uv2	= -1;
			pNode->m_pVertexIndexTable[i].n_bin	= arrTemp[i].n_bin;
			pNode->m_pVertexIndexTable[i].n_tan	= arrTemp[i].n_tan;
		}

		delete[] arrTemp;
	}

	//////////////////////////////////////////////
	if (pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER12)
	{
		if(pNode->m_nFaceCnt) 
		{
			pNode->m_pFaceIndexTable = new WORD[pNode->m_nFaceCnt*3];
			RFILEREAD2(pNode->m_pFaceIndexTable,sizeof(WORD)*pNode->m_nFaceCnt*3);
		}
		pNode->m_nFaceIndexTableCnt = pNode->m_nFaceCnt*3;
	}
	else
	{
		int nPrimitiveType = 0;
		RFILEREAD2(&nPrimitiveType,4);	// 현재 쓰이고 있지 않다 strips 를 위해 남겨둔것

		RFILEREAD2(&pNode->m_nFaceIndexTableCnt,4);
		pNode->m_pFaceIndexTable = new WORD[pNode->m_nFaceIndexTableCnt];
		RFILEREAD2(pNode->m_pFaceIndexTable,sizeof(WORD) *pNode->m_nFaceIndexTableCnt);
	}

	RFILEREAD2(&pNode->m_nMtrlTableInfoCnt,4);

	if (pNode->m_nMtrlTableInfoCnt) 
	{
		pNode->m_pMtrlTableInfo = new RMtrlTableInfo[pNode->m_nMtrlTableInfoCnt];
#ifdef MESH_STRIP_TEST
		m_pMtrlTableInfoStrips = new RMtrlTableInfo[m_nMtrlTableInfoCnt];
		m_pMtrlTableInfoDefault = m_pMtrlTableInfo;
#endif

		if(pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER9)
		{
			for( int i=0; i<pNode->m_nMtrlTableInfoCnt; i++)
			{
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].mtrlid,sizeof(int));
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].offset,sizeof(WORD));
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].count,sizeof(WORD));
				pNode->m_pMtrlTableInfo[i].nSubMaterialIDForDrawMasking = 0;
			}
		}
		else
		{
			RFILEREAD2(pNode->m_pMtrlTableInfo, sizeof(RMtrlTableInfo)*pNode->m_nMtrlTableInfoCnt);
		}
	}

	pNode->m_bipID = RSkeleton::GetBipID(pNode->m_Name.c_str());

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Version 13

bool RMeshNodeLoaderImpl_v13::LoadEtc(RMeshNode *pNode, RFile *pFile)
{
	if ( !RMeshNodeLoaderImpl_v12::LoadEtc(pNode, pFile))
		return false;

	pFile->Read((void*)pNode->m_boundingBox.vmin.v, sizeof(float)*3);
	pFile->Read((void*)pNode->m_boundingBox.vmax.v, sizeof(float)*3);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Version 14

bool RMeshNodeLoaderImpl_v14::LoadVertex(RMeshNode *pNode, RFile *pFile)
{
	DWORD dwFVF;
	RFILEREAD2(&dwFVF, 4);
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		RVector* vTangentTanTable = NULL;

		vTangentTanTable = new RVector[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(vTangentTanTable,sizeof(RVector)*pNode->m_nTangentTanTableCnt);

		// copy
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		for( int i = 0; i < pNode->m_nTangentTanTableCnt; ++i )
		{
			pNode->m_pTangentTanTable[i] = RVector4(vTangentTanTable[i], 1);	// w값 default는 1	
		}

		SAFE_DELETE_ARRAY(vTangentTanTable);
	}	

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Version 15

bool RMeshNodeLoaderImpl_v15::LoadFace(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nFaceCnt, 4);

	if (pNode->m_nFaceCnt) 
	{
		RFILEREAD2(&pNode->m_nTotalDegree,4);
		RFILEREAD2(&pNode->m_nTotalTriangles,4);

		pNode->m_pAllFaceSubData = new RFaceSubData[pNode->m_nTotalDegree];
		pNode->m_pPolygonTable = new RMeshPolygonData[pNode->m_nFaceCnt];

		RFaceSubData* pCurrent = pNode->m_pAllFaceSubData;

		for (int i=0;i<pNode->m_nFaceCnt;i++)
		{
			int nDeg = 0;
			RFILEREAD2(&nDeg,4);
			pNode->m_pPolygonTable[i].nVertices = nDeg;
			pNode->m_pPolygonTable[i].index = pCurrent;

			RFILEREAD2(pNode->m_pPolygonTable[i].index, sizeof(RFaceSubData)*nDeg);
			RFILEREAD2(&pNode->m_pPolygonTable[i].mtrl_id, sizeof(short));

			pCurrent+= nDeg;
		}
		_ASSERT(pCurrent - pNode->m_pAllFaceSubData == pNode->m_nTotalDegree);
	}
	return true;
}

bool RMeshNodeLoaderImpl_v15::LoadVertex(RMeshNode *pNode, RFile *pFile)
{
	DWORD dwFVF;
	RFILEREAD2(&dwFVF, 4);
	int nLightMapID;
	RFILEREAD2(&nLightMapID, 4);
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		RVector* vTangentTanTable = NULL;

		vTangentTanTable = new RVector[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(vTangentTanTable,sizeof(RVector)*pNode->m_nTangentTanTableCnt);

		// copy
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		for( int i = 0; i < pNode->m_nTangentTanTableCnt; ++i )
		{
			pNode->m_pTangentTanTable[i] = RVector4(vTangentTanTable[i], 1);	// w값 default는 1	
		}

		SAFE_DELETE_ARRAY(vTangentTanTable);
	}	

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	int nLightMapTexCoordTableCnt;
	RFILEREAD2(&nLightMapTexCoordTableCnt,4 );
	pFile->SeekFromCurrent(sizeof(RVector)*nLightMapTexCoordTableCnt);
	return true;
}

bool RMeshNodeLoaderImpl_v15::LoadEtc(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_nBoneTableCnt,4);
	if(pNode->m_nBoneTableCnt) 
	{
		pNode->m_pBoneTable = new RMatrix[pNode->m_nBoneTableCnt];
		pNode->m_pBoneTableIndex = new WORD[pNode->m_nBoneTableCnt];

		RFILEREAD2(pNode->m_pBoneTable,sizeof(RMatrix)*pNode->m_nBoneTableCnt);
		RFILEREAD2(pNode->m_pBoneTableIndex,sizeof(WORD)*pNode->m_nBoneTableCnt);
	}

	RFILEREAD2(&pNode->m_nVertexIndexTableCnt,4);
	if (pNode->m_nVertexIndexTableCnt) 
	{
		pNode->m_pVertexIndexTable	= new RVertexIndex[pNode->m_nVertexIndexTableCnt];
		RFILEREAD2(pNode->m_pVertexIndexTable,sizeof(RVertexIndex)*pNode->m_nVertexIndexTableCnt);
	}

	//////////////////////////////////////////////
	if (pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER12)
	{
		if(pNode->m_nFaceCnt) 
		{
			pNode->m_pFaceIndexTable = new WORD[pNode->m_nFaceCnt*3];
			RFILEREAD2(pNode->m_pFaceIndexTable,sizeof(WORD)*pNode->m_nFaceCnt*3);
		}
		pNode->m_nFaceIndexTableCnt = pNode->m_nFaceCnt*3;
	}
	else
	{
		int nPrimitiveType = 0;
		RFILEREAD2(&nPrimitiveType,4);	// 현재 쓰이고 있지 않다 strips 를 위해 남겨둔것

		RFILEREAD2(&pNode->m_nFaceIndexTableCnt,4);
		pNode->m_pFaceIndexTable = new WORD[pNode->m_nFaceIndexTableCnt];
		RFILEREAD2(pNode->m_pFaceIndexTable,sizeof(WORD) *pNode->m_nFaceIndexTableCnt);
	}

	RFILEREAD2(&pNode->m_nMtrlTableInfoCnt,4);

	if (pNode->m_nMtrlTableInfoCnt) 
	{
		pNode->m_pMtrlTableInfo = new RMtrlTableInfo[pNode->m_nMtrlTableInfoCnt];
#ifdef MESH_STRIP_TEST
		m_pMtrlTableInfoStrips = new RMtrlTableInfo[m_nMtrlTableInfoCnt];
		m_pMtrlTableInfoDefault = m_pMtrlTableInfo;
#endif

		if(pNode->m_pParentMesh->GetFileVersion()<EXPORTER_MESH_VER9)
		{
			for( int i=0; i<pNode->m_nMtrlTableInfoCnt; i++)
			{
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].mtrlid,sizeof(int));
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].offset,sizeof(WORD));
				RFILEREAD2(&pNode->m_pMtrlTableInfo[i].count,sizeof(WORD));
				pNode->m_pMtrlTableInfo[i].nSubMaterialIDForDrawMasking = 0;
			}
		}
		else
		{
			RFILEREAD2(pNode->m_pMtrlTableInfo, sizeof(RMtrlTableInfo)*pNode->m_nMtrlTableInfoCnt);
		}
	}

	pNode->m_bipID = RSkeleton::GetBipID(pNode->m_Name.c_str());

	pFile->Read((void*)pNode->m_boundingBox.vmin.v, sizeof(float)*3);
	pFile->Read((void*)pNode->m_boundingBox.vmax.v, sizeof(float)*3);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// ver 16
bool RMeshNodeLoaderImpl_v16::LoadVertex(RMeshNode *pNode, RFile *pFile)
{
	DWORD dwFVF;
	RFILEREAD2(&dwFVF, 4);
	int nLightMapID;
	RFILEREAD2(&nLightMapID, 4);
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(pNode->m_pTangentTanTable,sizeof(RVector4)*pNode->m_nTangentTanTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	int nLightMapTexCoordTableCnt;
	RFILEREAD2(&nLightMapTexCoordTableCnt,4 );
	pFile->SeekFromCurrent(sizeof(RVector)*nLightMapTexCoordTableCnt);
	return true;
}


bool RMeshNodeLoaderImpl_v17::LoadVertex( RMeshNode *pNode, RFile *pFile )
{
	DWORD dwFVF;
	RFILEREAD2(&dwFVF, 4);
	int nLightMapID;
	RFILEREAD2(&nLightMapID, 4);
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(pNode->m_pTangentTanTable,sizeof(RVector4)*pNode->m_nTangentTanTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	return true;
}

bool RMeshNodeLoaderImpl_v18::LoadVertex( RMeshNode *pNode, RFile *pFile )
{
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(pNode->m_pTangentTanTable,sizeof(RVector4)*pNode->m_nTangentTanTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordExtraTableCnt,4 );

	if (pNode->m_nTexCoordExtraTableCnt)
	{
		pNode->m_pTexCoordExtraTable = new RVector[pNode->m_nTexCoordExtraTableCnt];
		RFILEREAD2(pNode->m_pTexCoordExtraTable,sizeof(RVector)*pNode->m_nTexCoordExtraTableCnt);
	}

	return true;
}

///////////////////////////////////////////////
///// v19
/*bool RMeshNodeLoaderImpl_v19::Load( RMeshNode * pNode, RFile *pFile)
{
	_ASSERT(pNode);
	_ASSERT(pFile);

	if ( !LoadName(pNode, pFile))
		return false;
	if ( !LoadInfo(pNode, pFile))
		return false;
	if ( !LoadVertex(pNode, pFile))
		return false;
	if ( !LoadFace(pNode, pFile))
		return false;
	if ( !LoadVertexInfo(pNode, pFile))
		return false;
	if ( !LoadEtc(pNode, pFile))
		return false;

	return true;
}*/

bool RMeshNodeLoaderImpl_v20::LoadName( RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD1( pNode->m_Name);
	RFILEREAD2( &pNode->m_nParentNodeID, 4);
	RFILEREAD1( pNode->m_ParentName);

	return true;
}

bool RMeshNodeLoaderImpl_v20::LoadInfo(RMeshNode *pNode, RFile *pFile)
{
	RFILEREAD2(&pNode->m_matLocal, sizeof(RMatrix));
	pNode->m_matResult = pNode->m_matLocal;
	
	RFILEREAD2(&pNode->m_fBaseVisibility, 4);
	RFILEREAD2(&pNode->m_dwFlag,4);
	RFILEREAD2(&pNode->m_nAlign,4);
	RFILEREAD2(&pNode->m_LODProjectionIndex, 4); //is this LOD Projection Index?

	return true;
}

bool RMeshNodeLoaderImpl_v20::LoadVertex( RMeshNode *pNode, RFile *pFile )
{
	RFILEREAD2(&pNode->m_nPointCnt, 4);
	if ( pNode->m_nPointCnt )
	{
		pNode->m_pPointTable = new RVector[pNode->m_nPointCnt];
		RFILEREAD2(pNode->m_pPointTable, sizeof(RVector)*pNode->m_nPointCnt);
		pNode->CalcLocalBBox();
	}

	RFILEREAD2(&pNode->m_nTexCoordTableCnt,4 );

	if (pNode->m_nTexCoordTableCnt)
	{
		pNode->m_pTexCoordTable = new RVector[pNode->m_nTexCoordTableCnt];
		RFILEREAD2(pNode->m_pTexCoordTable,sizeof(RVector)*pNode->m_nTexCoordTableCnt);
	}

	RFILEREAD2(&pNode->m_nTexCoordExtraTableCnt, 4 );
	if (pNode->m_nTexCoordExtraTableCnt)
	{
		pNode->m_pTexCoordExtraTable = new RVector[pNode->m_nTexCoordExtraTableCnt];
		RFILEREAD2(pNode->m_pTexCoordExtraTable,sizeof(RVector)*pNode->m_nTexCoordExtraTableCnt);
	}

	RFILEREAD2(&pNode->m_nNormalTableCnt,4 );
	if ( pNode->m_nNormalTableCnt ) 
	{
		pNode->m_pNormalTable = new RVector[pNode->m_nNormalTableCnt];
		RFILEREAD2(pNode->m_pNormalTable, sizeof(RVector)*pNode->m_nNormalTableCnt);
	}
	
	RFILEREAD2(&pNode->m_nTangentTanTableCnt,4 );

	if ( pNode->m_nTangentTanTableCnt ) 
	{
		pNode->m_pTangentTanTable = new RVector4[pNode->m_nTangentTanTableCnt];
		RFILEREAD2(pNode->m_pTangentTanTable,sizeof(RVector4)*pNode->m_nTangentTanTableCnt);
	}

	RFILEREAD2(&pNode->m_nTangentBinTableCnt,4 );

	if ( pNode->m_nTangentBinTableCnt) 
	{
		pNode->m_pTangentBinTable = new RVector[pNode->m_nTangentBinTableCnt];
		RFILEREAD2(pNode->m_pTangentBinTable,sizeof(RVector)*pNode->m_nTangentBinTableCnt);
	}
	return true;
}

bool RMeshNodeLoaderImpl_v20::LoadEtc(RMeshNode *pNode, RFile *pFile)
{

	int nPrimitiveType = 0;
	RFILEREAD2(&nPrimitiveType, 4);

	RFILEREAD2(&pNode->m_nVertexIndexTableCnt,4);
	if (pNode->m_nVertexIndexTableCnt) 
	{
		pNode->m_pVertexIndexTable	= new RVertexIndex[pNode->m_nVertexIndexTableCnt];
		RFILEREAD2(pNode->m_pVertexIndexTable,sizeof(RVertexIndex)*pNode->m_nVertexIndexTableCnt);
	}

	RFILEREAD2(&pNode->m_nBoneTableCnt,4);
	if(pNode->m_nBoneTableCnt) 
	{
		pNode->m_pBoneTable = new RMatrix[pNode->m_nBoneTableCnt];
		pNode->m_pBoneTableIndex = new WORD[pNode->m_nBoneTableCnt];

		RFILEREAD2(pNode->m_pBoneTable,sizeof(RMatrix)*pNode->m_nBoneTableCnt);
		RFILEREAD2(pNode->m_pBoneTableIndex,sizeof(WORD)*pNode->m_nBoneTableCnt);
	}

	RFILEREAD2(&pNode->m_nMtrlTableInfoCnt,4);

	if (pNode->m_nMtrlTableInfoCnt) 
	{
		pNode->m_pMtrlTableInfo = new RMtrlTableInfo[pNode->m_nMtrlTableInfoCnt];
#ifdef MESH_STRIP_TEST
		m_pMtrlTableInfoStrips = new RMtrlTableInfo[m_nMtrlTableInfoCnt];
		m_pMtrlTableInfoDefault = m_pMtrlTableInfo;
#endif
		RFILEREAD2(pNode->m_pMtrlTableInfo, sizeof(RMtrlTableInfo)*pNode->m_nMtrlTableInfoCnt);
	}

	RFILEREAD2(&pNode->m_nFaceIndexTableCnt,4);
	if(pNode->m_nFaceIndexTableCnt)
	{
		pNode->m_pFaceIndexTable = new WORD[pNode->m_nFaceIndexTableCnt];
		RFILEREAD2(pNode->m_pFaceIndexTable,sizeof(WORD) *pNode->m_nFaceIndexTableCnt);
	}

	pNode->m_bipID = RSkeleton::GetBipID(pNode->m_Name.c_str());

	pFile->Read((void*)pNode->m_boundingBox.vmin.v, sizeof(float)*3);
	pFile->Read((void*)pNode->m_boundingBox.vmax.v, sizeof(float)*3);

	if (pNode->m_boundingBox.vmin.x == FLT_MAX)
	{
		pNode->m_boundingBox.vmin = RVector(-0.1f, -0.1f, -0.1f);
		pNode->m_boundingBox.vmax = RVector(0.1f, 0.1f, 0.1f);
	}

	return true;
}


};