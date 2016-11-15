#include "stdafx.h"

#include "MPxObjStatic.h"
#include "MPhysX.h"

#include "NxCooking.h"

#include "mmsystem.h"

namespace physx {

MPxObjStatic::MPxObjStatic()
{
	// Sector 계열 초기화
	m_nUsingSquareGridMeshCount = 0;
	m_nUsingCubeGridMeshCount = 0;

	m_pArraySectorCubeGrid = NULL;
	m_pArraySectorSquareGrid = NULL;

	m_pSectorBuffTemp = NULL;
	m_pSectorBuff_Old = &m_vecSectorBuff_0;
	m_pSectorBuff_Current = &m_vecSectorBuff_1;

	m_nCubeXCount = 0;
	m_nCubeYCount = 0;
	m_nCubeZCount = 0;
	m_nSquareDivision = 0;
}

MPxObjStatic::~MPxObjStatic()
{
	// cube sector
	if (m_pArraySectorCubeGrid)
	{
		int nCubeSectorCount = m_nCubeXCount * m_nCubeYCount * m_nCubeZCount;
		for(int i = 0; i < nCubeSectorCount; ++i)
		{
			SECTOR_INFO* pSector = m_pArraySectorCubeGrid[i];
			if (pSector)
			{
				ClearNxInfoAndNxActor(pSector->m_NXInfo, true);
				delete pSector;
			}
		}

		delete[] m_pArraySectorCubeGrid;
		m_pArraySectorCubeGrid = NULL;
	}

	// square sector
	if (m_pArraySectorSquareGrid)
	{
		int nSquareSectorCount = m_nSquareDivision * m_nSquareDivision;
		for(int i = 0; i < nSquareSectorCount; ++i)
		{
			SECTOR_INFO* pSector = m_pArraySectorSquareGrid[i];
			if (pSector)
			{
				ClearNxInfoAndNxActor(pSector->m_NXInfo, true);
				delete pSector;
			}
		}

		delete[] m_pArraySectorSquareGrid;
		m_pArraySectorSquareGrid = NULL;
	}
}

bool MPxObjStatic::AddStaticSubActorsFromBuilder( MPxObjStaticSubActorBuilder& _rStaticSubActorBuilder )
{
	_ASSERT(m_pPhysX != NULL);

	bool bStaticMeshEnable = false;

	//////////////////////////////////////////////////////////////////////////
	//
	// cube sector build
	//
	//////////////////////////////////////////////////////////////////////////

	MBox totalBound;
	float fPieceCubeSize;
	bool bBuilderableInfo = _rStaticSubActorBuilder.GetCubeSectorInfo(totalBound, fPieceCubeSize);

	//mlog("%u px1\n", timeGetTime());

	if (bBuilderableInfo)
	{
		bStaticMeshEnable = true;

		++m_nUsingCubeGridMeshCount;
		_ASSERT( m_nUsingCubeGridMeshCount == 1 ); //현재는 하나의 섹터메시만 가능합니다.

		m_fCubeXStart = totalBound.vmin.x;
		m_fCubeYStart = totalBound.vmin.y;
		m_fCubeZStart = totalBound.vmin.z;

		// 거리
		float x = totalBound.vmax.x - m_fCubeXStart;
		float y = totalBound.vmax.y - m_fCubeYStart;
		float z = totalBound.vmax.z - m_fCubeZStart;

		m_nCubeXCount = (int)(x / fPieceCubeSize);
		if (m_nCubeXCount == 0)
			m_nCubeXCount = 1;
		m_fCubeXInterval = x / (float)m_nCubeXCount;

		m_nCubeYCount = (int)(y / fPieceCubeSize);
		if (m_nCubeYCount == 0)
			m_nCubeYCount = 1;
		m_fCubeYInterval = y / (float)m_nCubeYCount;

		m_nCubeZCount = 1;//(int)(z / fPieceCubeSize);
		if (m_nCubeZCount == 0)
			m_nCubeZCount = 1;
		m_fCubeZInterval = z / (float)m_nCubeZCount;

		// 섹터 액터 생성
		_ASSERT(m_pArraySectorCubeGrid == NULL);
		m_pArraySectorCubeGrid = new SECTOR_INFO*[m_nCubeXCount * m_nCubeYCount * m_nCubeZCount];

		// 섹터 정보를 만들 박스
		MBox pieceBox;

		// 섹터 만들기
		for (int i = 0; i < m_nCubeXCount; ++i)
		{
			for (int j = 0; j < m_nCubeYCount; ++j)
			{
				for (int k = 0; k < m_nCubeZCount; ++k)
				{
					pieceBox.vmin.Set(m_fCubeXStart + i * m_fCubeXInterval, m_fCubeYStart + j * m_fCubeYInterval, m_fCubeZStart + k * m_fCubeZInterval);
					pieceBox.vmax.Set(m_fCubeXStart + (i+1) * m_fCubeXInterval , m_fCubeYStart + (j+1) * m_fCubeYInterval, m_fCubeZStart + (k+1) * m_fCubeZInterval);

					SECTOR_INFO* pSector = NULL;
					std::vector<NxVec3>* pVertices = new std::vector<NxVec3>;
					std::vector<NxU32>* pFaces = new std::vector<NxU32>;

					// 메시 정보 생성
					_rStaticSubActorBuilder.GetCubeSectorMesh(*pVertices, *pFaces, pieceBox);
					if ( false == pVertices->empty() )
					{
						pSector = new SECTOR_INFO;
						pSector->m_pVertices = pVertices;
						pSector->m_pFaces = pFaces;
						pSector->m_bHeightField = false;
						pSector->m_PieceBox = pieceBox;
					}
					else
					{
						delete pVertices;
						delete pFaces;
					}

					m_pArraySectorCubeGrid[(k*m_nCubeXCount*m_nCubeYCount) + (j*m_nCubeXCount) + i] = pSector;
				}
			}
		}
	}

	//mlog("%u px2\n", timeGetTime());

	//////////////////////////////////////////////////////////////////////////
	//
	// grid sector
	//
	//////////////////////////////////////////////////////////////////////////

	int nDivisionCount, nRowCount, nColCount;
	float fRowScale, fColScale;
	bBuilderableInfo = _rStaticSubActorBuilder.GetSquareSectorInfo(nDivisionCount, nRowCount, nColCount, fRowScale, fColScale);

	if (bBuilderableInfo)
	{
		bStaticMeshEnable = true;

		++m_nUsingSquareGridMeshCount;
		_ASSERT( m_nUsingSquareGridMeshCount == 1 ); //현재는 하나의 섹터메시만 가능합니다.

		// 간격 구하기
		m_nSquareDivision = nDivisionCount;
		int nGapSizeWidth	=  nRowCount / nDivisionCount;
		int nGapSizeHeight	= nColCount / nDivisionCount;

		// 2n 승이 아닌경우에 대한 에러 처리
		if(nRowCount % nDivisionCount)
		{
			nGapSizeWidth++;
		}	
		if(nColCount % nDivisionCount)
		{
			nGapSizeHeight++;
		}

		m_fSquareXInterval = nGapSizeWidth * fRowScale;
		m_fSquareYInterval = nGapSizeHeight * fColScale;

		int y1	= 0;
		int y2	= 0;
		int nIndex = 0;

		// 섹터 액터 생성
		m_pArraySectorSquareGrid = new SECTOR_INFO*[nDivisionCount * nDivisionCount];

		// 섹터 만들기
		for(int nH = 0; nH < nDivisionCount; nH++)
		{
			int nCurrentGapHeight = nGapSizeHeight;
			int nCalGapHeight = (nH + 1) * nGapSizeHeight;
			if(nCalGapHeight > nColCount)
			{
				nCurrentGapHeight = nColCount - (nH * nGapSizeHeight);
			}

			y2 = y1 + nCurrentGapHeight;

			int x1 = 0;
			int x2 = 0;
			for(int nW = 0; nW < nDivisionCount; nW++)
			{

				// 위치 체크
				int nCurrentGapWidth = nGapSizeWidth;
				int nCalGapWidth = (nW + 1) * nGapSizeWidth;
				if(nCalGapWidth > nRowCount)
				{
					nCurrentGapWidth = nRowCount - (nW * nGapSizeWidth);
				}

				x2 = x1 + nCurrentGapWidth;

				SECTOR_INFO* pSector = NULL;
				std::vector<NxVec3>* pVertices = new std::vector<NxVec3>;
				_rStaticSubActorBuilder.GetSquareSectorMesh(*pVertices, x1, y1, x2, y2);

				if ( false == pVertices->empty() )
				{
					pSector = new SECTOR_INFO;
					std::vector<NxU32>* pFaces = new std::vector<NxU32>;

					int nMapTriangleCount = nCurrentGapWidth * nCurrentGapHeight * 2;

					int nFacesSize = nCurrentGapHeight * nCurrentGapWidth * 6;		// first triangle + second triangle = 6

					if (nFacesSize > 0)	pFaces->resize(nFacesSize);

					int k = 0;
					int nFacesVecIndex = 0;

					for(int i = 0; i < nCurrentGapHeight; i++)
					{
						for(int j = 0; j < nCurrentGapWidth; j++)
						{
							// Create first triangle
							(*pFaces)[nFacesVecIndex++] = (j + 1 + i * (nCurrentGapWidth + 1));
							(*pFaces)[nFacesVecIndex++] = (j   + (i + 1) * (nCurrentGapWidth + 1));
							(*pFaces)[nFacesVecIndex++] = (j + 1 + (i + 1) * (nCurrentGapWidth + 1));

							// Create second triangle
							(*pFaces)[nFacesVecIndex++] = (j + 1 + i * (nCurrentGapWidth + 1));
							(*pFaces)[nFacesVecIndex++] = (j   + i * (nCurrentGapWidth + 1));
							(*pFaces)[nFacesVecIndex++] = (j   + (i + 1) * (nCurrentGapWidth + 1));
						}
					}

					pSector->m_pVertices = pVertices;
					pSector->m_pFaces = pFaces;
					pSector->m_bHeightField = true;
				}
				else
				{
					delete pVertices;
				}

				m_pArraySectorSquareGrid[nH * nDivisionCount + nW] = pSector;

				x1 = x2;
				nIndex++;
			}
			y1 = y2;
		}
	}

	//mlog("%u px3\n", timeGetTime());

	return bStaticMeshEnable;
}

void MPxObjStatic::ActivateStaticSubActorsByAreaList( const std::vector< const MBox* >& _rAreaList )
{
	m_pSectorBuff_Old->clear();
	MBox pieceAABB;
	for (std::vector< const MBox* >::const_iterator itr = _rAreaList.begin(); itr != _rAreaList.end(); ++itr)
	{
		const MBox* pPieceAABB = *itr;

		if (m_pArraySectorCubeGrid)
		{
			//////////////////////////////////////////////////////////////////////////
			// CubeGrid
			//
			int nXStart = (int)((pPieceAABB->vmin.x - m_fCubeXStart)/m_fCubeXInterval);
			if (nXStart < 0 ) nXStart = 0;
			int nXEnd = (int)((pPieceAABB->vmax.x - m_fCubeXStart)/m_fCubeXInterval);
			if (nXEnd >= m_nCubeXCount) nXEnd = m_nCubeXCount - 1;

			int nYStart = (int)((pPieceAABB->vmin.y - m_fCubeYStart)/m_fCubeYInterval);
			if (nYStart < 0 ) nYStart = 0;
			int nYEnd = (int)((pPieceAABB->vmax.y - m_fCubeYStart)/m_fCubeYInterval);
			if (nYEnd >= m_nCubeYCount) nYEnd = m_nCubeYCount - 1;

			int nZStart = (int)((pPieceAABB->vmin.z - m_fCubeZStart)/m_fCubeZInterval);
			if (nZStart < 0 ) nZStart = 0;
			int nZEnd = (int)((pPieceAABB->vmax.z - m_fCubeZStart)/m_fCubeZInterval);
			if (nZEnd >= m_nCubeZCount) nZEnd = m_nCubeZCount - 1;

			for (int i = nXStart; i <= nXEnd; ++i) for (int j = nYStart; j <= nYEnd; ++j) for (int k = nZStart; k <= nZEnd; ++k)
			{
				SECTOR_INFO* pSector = m_pArraySectorCubeGrid [(k*m_nCubeXCount*m_nCubeYCount) + (j*m_nCubeXCount) + i];
				if (pSector && pSector->m_PieceBox.ContainsAABB(*pPieceAABB))
					CreateSectorActor(pSector, m_pSectorBuff_Old);
			}
		}

		if (m_pArraySectorSquareGrid)
		{
			//////////////////////////////////////////////////////////////////////////
			// SquareGrid
			//

			// 6 7 8
			// 3 4 5
			// 0 1 2
			const MVector3& pos = pPieceAABB->GetCenter();
			int nCenterIndex, nLT,nL,nLB,nT,nB,nRT,nR,nRB;
			int nTotalCount = m_nSquareDivision * m_nSquareDivision;

			#define _CREATE_SQUARE_SECTOR( index ) \
			if(index >=0 && index < nTotalCount) { CreateSectorActor(m_pArraySectorSquareGrid[index], m_pSectorBuff_Old); }

			nCenterIndex = (int)(pos.y / m_fSquareYInterval) + m_nSquareDivision * (int)(pos.x / m_fSquareXInterval); //그리드는 양수로만 설정되어있다.
			_CREATE_SQUARE_SECTOR(nCenterIndex);

			nT	= nCenterIndex + (m_nSquareDivision);
			_CREATE_SQUARE_SECTOR( nT );

			nB	= nCenterIndex - (m_nSquareDivision);
			_CREATE_SQUARE_SECTOR( nB );

			nL = nCenterIndex + 1;
			if((nL % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nL );

			nR = nCenterIndex - 1;
			if((nCenterIndex % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nR );

			nLT = nT + 1;
			if((nLT % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nLT );

			nLB	= nB + 1;
			if((nLB % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nLB );

			nRT = nT - 1;
			if((nT % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nRT );

			nRB	= nB - 1;
			if((nB % m_nSquareDivision) > 0)
				_CREATE_SQUARE_SECTOR( nRB );
		}
	}

	// 현재섹터 지우고 추가한 섹터를 현재섹터로 변경
	for (std::vector<SECTOR_INFO*>::iterator itr = m_pSectorBuff_Current->begin(); itr != m_pSectorBuff_Current->end(); ++itr)
	{
		SECTOR_INFO* pSector = *itr;
		pSector->m_nRefCount -= 1;
		if (pSector->m_nRefCount == 0 && pSector->m_NXInfo.m_pActor)
		{
			ReserveToReleaseNxActor(pSector->m_NXInfo.m_pActor);
			pSector->m_NXInfo.m_pActor = NULL;
		}
	}

	// Buff Swap
	m_pSectorBuffTemp = m_pSectorBuff_Old;
	m_pSectorBuff_Old = m_pSectorBuff_Current;
	m_pSectorBuff_Current = m_pSectorBuffTemp;
}

void MPxObjStatic::ActivateStaticSubActorsAll()
{
	if (m_pArraySectorCubeGrid)
	{
		// cube sector
		int nCubeSectorCount = m_nCubeXCount * m_nCubeYCount * m_nCubeZCount;
		for(int i = 0; i < nCubeSectorCount; ++i)
		{
			SECTOR_INFO* pSector = m_pArraySectorCubeGrid[i];
			if (pSector)
			{
				CreateSectorActor(pSector, NULL);
			}
		}
	}

	if (m_pArraySectorSquareGrid)
	{
		// square sector
		int nSquareSectorCount = m_nSquareDivision * m_nSquareDivision;
		for(int i = 0; i < nSquareSectorCount; ++i)
		{
			SECTOR_INFO* pSector = m_pArraySectorSquareGrid[i];
			if (pSector)
			{
				CreateSectorActor(pSector, NULL);
			}
		}
	}
}

void MPxObjStatic::CreateSectorActor( SECTOR_INFO* pSector, std::vector<SECTOR_INFO*>* pVecSectorBufferToAdd )
{
	_ASSERT(pSector != NULL);
	pSector->m_nRefCount += 1;

	bool bWaitForCreation = true;
	if (pVecSectorBufferToAdd)
	{
		pVecSectorBufferToAdd->push_back(pSector);
		bWaitForCreation = false;
	}

	// 있으면 참조카운트만 올리고 리턴
	if (pSector->m_NXInfo.m_pActor != NULL )
	{
		_ASSERT(pSector->m_pVertices == NULL);
		_ASSERT(pSector->m_pFaces == NULL);

		return;
	}

	// Shape 만들기
	bool bStaticMeshAvailable = true;
	if (pSector->m_pVertices)
	{
		_ASSERT(pSector->m_pFaces);

		// shape 만들기
		std::string strShapeName;
		MPhysXShape::MakeUniqueName(strShapeName);

		MPhysXShape* pShape = m_pPhysX->CreatePxShape(MPhysXShape::PSHT_STATIC_MESH, strShapeName);
		bStaticMeshAvailable = pShape->CreateStaticMeshShape(*(pSector->m_pVertices), *(pSector->m_pFaces), pSector->m_bHeightField);
		if (bStaticMeshAvailable)
		{
			pSector->m_NXInfo.m_pPhysXShape = pShape;
			delete pSector->m_pVertices;
			pSector->m_pVertices = NULL;

			delete pSector->m_pFaces;
			pSector->m_pFaces = NULL;
		}
		else
		{
			m_pPhysX->RemovePxShape(strShapeName);
		}
	}

	// NxActor 만들어주기
	if (bStaticMeshAvailable)
		CreateNxActorByNxInfo( pSector->m_NXInfo, bWaitForCreation );
}

} // namespace physx