#include "StdAfx.h"
#include "GPickingZTable.h"
#include "GPathFinder.h"
#include "GCollisionTree.h"
#include "MUtil.h"
#include "MTime.h"
#include <limits>
#include "MXml.h"
#include "MLocale.h"

//////////////////////////////////////////////////////////////////////////
//
//	Cell
//
//////////////////////////////////////////////////////////////////////////

long GPickingZTable::Cell::s_nTotalQty = 0;
const int nCellMaxSize = PICKING_CHUNK_SIZE/PICKING_CELL_SIZE+1;

GPickingZTable::Cell::Cell() 
: m_nHeightQty(0)
, m_HeightData(NULL)
{
	++s_nTotalQty;
}

GPickingZTable::HeightData* GPickingZTable::Cell::NewHeightData( uint8 nHeightQty )
{
	switch (nHeightQty)
	{
	case 1: return (HeightData*)(new HeightData());
	case 2: return (HeightData*)(new HeightData2());
	case 3: return (HeightData*)(new HeightData3());
	case 4: return (HeightData*)(new HeightData4());
	case 5: return (HeightData*)(new HeightData5());
	case 6: return (HeightData*)(new HeightData6());
	case 7: return (HeightData*)(new HeightData7());
	case 8: return (HeightData*)(new HeightData8());
	case 9: return (HeightData*)(new HeightData9());
	case 10: return (HeightData*)(new HeightData10());
	default:
		{
			_ASSERT(nHeightQty > HeightDataMax::DataSize);
		}
	}

	//mlog("Qty__ = %d\n", nHeightQty);
	return new HeightData[nHeightQty];
}

void GPickingZTable::Cell::DeleteHeightData()
{
	if (m_HeightData == NULL) return;

	switch (m_nHeightQty)
	{
	case 1: delete (HeightData*)(m_HeightData);		break;
	case 2: delete (HeightData2*)(m_HeightData);	break;
	case 3: delete (HeightData3*)(m_HeightData);	break;
	case 4: delete (HeightData4*)(m_HeightData);	break;
	case 5: delete (HeightData5*)(m_HeightData);	break;
	case 6: delete (HeightData6*)(m_HeightData);	break;
	case 7: delete (HeightData7*)(m_HeightData);	break;
	case 8: delete (HeightData8*)(m_HeightData);	break;
	case 9: delete (HeightData9*)(m_HeightData);	break;
	case 10: delete (HeightData10*)(m_HeightData);	break;
	default:
		{
			_ASSERT(m_nHeightQty > HeightDataMax::DataSize);
			SAFE_DELETE_ARRAY(m_HeightData);
		}
	}

	m_HeightData = NULL;
}

HEIGHT_TYPE GPickingZTable::Cell::GetHeight(uint8 nIndex) const
{
	if (m_nHeightQty <= 0)
		return INVALID_PICKING_HEIGHT;

	switch (m_nHeightQty)
	{
	case 1: return ((HeightData*)(m_HeightData))->m_nHeight[nIndex];
	case 2: return ((HeightData2*)(m_HeightData))->m_nHeight[nIndex];
	case 3: return ((HeightData3*)(m_HeightData))->m_nHeight[nIndex];
	case 4: return ((HeightData4*)(m_HeightData))->m_nHeight[nIndex];
	case 5: return ((HeightData5*)(m_HeightData))->m_nHeight[nIndex];
	case 6: return ((HeightData6*)(m_HeightData))->m_nHeight[nIndex];
	case 7: return ((HeightData7*)(m_HeightData))->m_nHeight[nIndex];
	case 8: return ((HeightData8*)(m_HeightData))->m_nHeight[nIndex];
	case 9: return ((HeightData9*)(m_HeightData))->m_nHeight[nIndex];
	case 10: return ((HeightData10*)(m_HeightData))->m_nHeight[nIndex];

	}
	return m_HeightData[nIndex].m_nHeight[0];
}

void GPickingZTable::Cell::SetHeight(uint8 nIndex, HEIGHT_TYPE nHeight )
{
	VALID(nIndex < m_nHeightQty);

	switch (m_nHeightQty)
	{
	case 1: ((HeightData*)(m_HeightData))->m_nHeight[nIndex] = nHeight;		break;
	case 2: ((HeightData2*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 3: ((HeightData3*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 4: ((HeightData4*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 5: ((HeightData5*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 6: ((HeightData6*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 7: ((HeightData7*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 8: ((HeightData8*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 9: ((HeightData9*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	case 10: ((HeightData10*)(m_HeightData))->m_nHeight[nIndex] = nHeight;	break;
	default:
		{
			_ASSERT(m_nHeightQty > HeightDataMax::DataSize);
			m_HeightData[nIndex].m_nHeight[0] = nHeight;
		}
	}
}

GPickingZTable::Cell::Cell( uint8 nHeightQty )
: m_nHeightQty(nHeightQty)
{
	++s_nTotalQty;

	m_HeightData = NewHeightData(nHeightQty);
}

GPickingZTable::Cell::~Cell()
{
	DeleteHeightData();
}

int GPickingZTable::Cell::GetHeight(float fPosZ, int nBoundMax) const
{
	if (m_nHeightQty <= 0)
		return INVALID_PICKING_HEIGHT;

	for (uint8 i=0; i<m_nHeightQty; ++i)
	{
		int nCurHeight = nBoundMax - GetHeight(i);
		// 유효한 블럭인지 확인
		if (fPosZ < nBoundMax &&
			fPosZ >= nCurHeight)
		{
			return nCurHeight;
		}

		nBoundMax = nCurHeight;
	}

	return INVALID_PICKING_HEIGHT;
}


void GPickingZTable::Cell::LoadFromFile(ifstream& s)
{
	s.read((char*)&m_nHeightQty, sizeof m_nHeightQty);
	if (m_nHeightQty <= 0)
		return;

	MakeHeightArray(m_nHeightQty);
	s.read((char*)m_HeightData, sizeof m_HeightData[0]*m_nHeightQty);
}

void GPickingZTable::Cell::SaveToFile(ofstream& s)
{
	s.write((char*)&m_nHeightQty, sizeof m_nHeightQty);
	if (m_nHeightQty <= 0)
		return;

	s.write((char*)m_HeightData, sizeof m_HeightData[0]*m_nHeightQty);
}

void GPickingZTable::Cell::MakeHeightArray(uint8 nSize)
{
	DCHECK(nSize > 0);

	DeleteHeightData();

	m_nHeightQty = nSize;
	m_HeightData = NewHeightData(m_nHeightQty);

}



//////////////////////////////////////////////////////////////////////////
//
//	GPickingZTable
//
//////////////////////////////////////////////////////////////////////////

GPickingZTable::GPickingZTable( const Desc& desc )
: m_Desc(desc)
, m_bPartialArea(false)
{
	DCHECK(desc.pCollisionTree);
	DCHECK(desc.pPathFinder);
}

GPickingZTable::~GPickingZTable(void)
{
	Clear();
}

void GPickingZTable::Clear()
{
	for (int y = 0; y < m_nChunkSizeY; y++)
	{
		for (int x = 0; x < m_nChunkSizeX; x++)
		{
			Chunk& chunk = m_ChunkTable[CalcChunkPoint(x, y)];
			SAFE_DELETE_ARRAY(chunk.PickingTable);
		}
	}

	SAFE_DELETE_ARRAY(m_ChunkTable);

	for (CellMap::iterator itor = m_mapCells.begin(); itor != m_mapCells.end(); ++itor)
	{
		Cell* pCell = (*itor).second;
		delete pCell;
	}
}

void GPickingZTable::RunSamplingWork()
{
	VALID(m_Desc.pCollisionTree);

	if (!CheckBoundingBoxZ())
	{
		mlog3("PickingZ File Create Failed: bounding-box height is too large. (limit to under 60000)\n");
		return;
	}

	float fWidth = GetMaxX() - GetMinX();
	float fHeight = GetMaxY() - GetMinY();

	m_nChunkSizeX = (int)(fWidth / PICKING_CHUNK_SIZE) + 1;
	m_nChunkSizeY = (int)(fHeight / PICKING_CHUNK_SIZE) + 1;

	m_ChunkTable = new Chunk[m_nChunkSizeX*m_nChunkSizeY];

	for (int y = 0; y < m_nChunkSizeY; y++)
	{
		for (int x = 0; x < m_nChunkSizeX; x++)
		{
			GetSampling(x, y);
		}
	}

	mlog("PickingZ File Created. (cell qty: %d)\n", GPickingZTable::Cell::s_nTotalQty);
}

bool GPickingZTable::CheckBoundingBoxZ() const
{
	float fDiffZ = GetBoundingBox().maxz - GetBoundingBox().minz;
	if ((HEIGHT_TYPE)fDiffZ >= (std::numeric_limits<HEIGHT_TYPE>::max)())
		return false; // 허용 범위 초과

	return true;
}

bool GPickingZTable::IsValidIndex(int rx, int ry)
{
	if ( rx<0 || ry<0 || rx>=m_nChunkSizeX || ry>=m_nChunkSizeY ) 
	{
		return false;
	}
	return true;
}

float GPickingZTable::Pick( float fPosX, float fPosY, float fPosZ )
{
	PFI_BLOCK_THISFUNC(9096);	

	MPoint ptChunk = ConvertToChunkIdx(fPosX, fPosY);
	if (!IsValidIndex(ptChunk.x, ptChunk.y))
		return INVALID_PICKING_HEIGHT;

	MPoint ptPickingTable = ConvertToPickingTableIdx(ptChunk.x, ptChunk.y, fPosX, fPosY);
	Chunk& chunk = m_ChunkTable[CalcChunkPoint(ptChunk.x, ptChunk.y)];
	if (chunk.PickingTable)
	{
		float fRet = chunk.PickingTable[CalcPickingTablePoint(ptPickingTable.x, ptPickingTable.y)];
		if (fRet != INVALID_PICKING_HEIGHT)
			return GetBoundingBox().maxz - fRet;
	}	

	uint32 nFlatPoint = CalcCellPoint(ptChunk.x, ptChunk.y, ptPickingTable.x, ptPickingTable.y);
	CellMap::iterator it = m_mapCells.find(nFlatPoint);
	if (it != m_mapCells.end())
	{
		Cell* pCell = (*it).second;
		return (float)pCell->GetHeight(fPosZ, (int)GetBoundingBox().maxz);
	}

	if (chunk.nDefaultHeight == INVALID_PICKING_HEIGHT)
		return INVALID_PICKING_HEIGHT; // 피킹 전처리가 안되는 곳

	return GetBoundingBox().maxz - chunk.nDefaultHeight;
}

MPoint GPickingZTable::ConvertToChunkIdx(float fPosX, float fPosY)
{
	int x = (int)((fPosX - GetMinX()) / PICKING_CHUNK_SIZE);
	int y = (int)((fPosY - GetMinY()) / PICKING_CHUNK_SIZE);

	return MPoint(x, y);
}

MPoint GPickingZTable::ConvertToPickingTableIdx(int nChunkX, int nChunkY, float fPosX, float fPosY)
{
	int x = (int)(fPosX - GetMinX() - (nChunkX * PICKING_CHUNK_SIZE)) / PICKING_CELL_SIZE;
	int y = (int)(fPosY - GetMinY() - (nChunkY * PICKING_CHUNK_SIZE)) / PICKING_CELL_SIZE;

	return MPoint(x, y);
}

float GPickingZTable::ConvertToPosX(int nChunkX, int nCellX)
{
	return GetMinX() +
		float((nChunkX * PICKING_CHUNK_SIZE)) +
		float((nCellX * PICKING_CELL_SIZE) + (PICKING_CELL_SIZE/2)
		);
}

float GPickingZTable::ConvertToPosY(int nChunkY, int nCellY)
{
	return GetMinY() +
		float((nChunkY * PICKING_CHUNK_SIZE)) +
		float((nCellY * PICKING_CELL_SIZE) + (PICKING_CELL_SIZE/2)
		);
}

bool GPickingZTable::GetSampling(int nChuckX, int nChuckY)
{
	bool bIsValidChunk = 
		IsValidChunk(nChuckX, nChuckY);

	Chunk& chunk = m_ChunkTable[CalcChunkPoint(nChuckX, nChuckY)];
	if (bIsValidChunk)
	{
		chunk.PickingTable = new HEIGHT_TYPE[nCellMaxSize*nCellMaxSize];
		memset(chunk.PickingTable, 0, sizeof(chunk.PickingTable[0])*nCellMaxSize*nCellMaxSize);
	}

	int64 nSumOfPickVars=0;
	uint32 nPickingCount=0;
	int32 nMinPickedVar = INVALID_PICKING_HEIGHT;
	int32 nMaxPickedVar = INVALID_PICKING_HEIGHT;


	for (int nCellX=0; nCellX<nCellMaxSize; ++nCellX)
	{
		for (int nCellY=0; nCellY<nCellMaxSize; ++nCellY)
		{
			float fPosX = ConvertToPosX(nChuckX, nCellX);
			float fPosY = ConvertToPosY(nChuckY, nCellY);
			vector<HEIGHT_TYPE> vecPickData;
			GetSamplingImpl(vecPickData, fPosX, fPosY, GetBoundingBox().maxz);
			if (vecPickData.empty())
				continue; // 피킹 되지 않음

			HEIGHT_TYPE nCurHeight = vecPickData.front();
			if (bIsValidChunk)
			{
				RecordPickingData(vecPickData, chunk, nChuckX, nChuckY, nCellX, nCellY, nCurHeight);

				nMinPickedVar = min(nCurHeight, nMinPickedVar);
				nMaxPickedVar = max(nCurHeight, nMaxPickedVar);
			}
			else
			{
				nSumOfPickVars += nCurHeight;
				++nPickingCount;
			}
		}
	}

	static const int nSameHeightGap = 10; // 0.1m
	bool bSameHeight=false;
	if (nMinPickedVar != INVALID_PICKING_HEIGHT &&
		nMaxPickedVar != INVALID_PICKING_HEIGHT)
	{
		if (nMaxPickedVar-nMaxPickedVar <= nSameHeightGap)
			bSameHeight = true;
	}

	if (bSameHeight)
	{
		SAFE_DELETE_ARRAY(chunk.PickingTable);
	}

	chunk.nDefaultHeight = nSumOfPickVars==0?0:HEIGHT_TYPE(nSumOfPickVars/nPickingCount);

	return true;
}

float GPickingZTable::GetBoundingBoxHeight() const
{
	return GetBoundingBox().maxz - GetBoundingBox().minz;
}

bool GPickingZTable::GetSamplingImpl(vector<HEIGHT_TYPE>& vecPickData, float fPosX, float fPosY, float fPosZ)
{
	GCollisionTree* pCollisionTree = m_Desc.pCollisionTree; 
	VALID_RET(pCollisionTree, false);

	float maxz = GetBoundingBox().maxz;
	float minz = GetBoundingBox().minz;

	// 지형과 충돌처리
	rs3::RCollisionPickInfo CollInfo;
	CollInfo.m_inPickOrigin		= vec3(fPosX, fPosY, fPosZ);
	CollInfo.m_inPickTo			= vec3(fPosX, fPosY, minz);

	if (!pCollisionTree->Pick(CollInfo, false, ~rs3::RCollisionTriangle::WATER))
		return false;

	// INVALID 값으로 뽑히면 약간 낮은 값으로 바꿔줌 (INVALID값을 따로 쓰고 있으므로 겹쳐지지 않도록)
	float& fPickZ = CollInfo.m_outPosition.z;
	if ((int)fPickZ == INVALID_PICKING_HEIGHT)
		fPickZ = -1;

	//float fAdj = fPosZ+(fPickZ>0?-fPickZ:fPickZ);
	float fAdj = fPosZ-fPickZ;

	// 바운딩 박스 최대 범위를 벗어남
	VALID_RET(fPickZ <= fPosZ, true); // 피킹 초기 위치가 더 커야함
	VALID_RET(fAdj >= 0.0f, true); // 상대값이 음수일 수 없음
	if(fAdj >= GetBoundingBoxHeight())
		return true;

	vecPickData.push_back(HEIGHT_TYPE(fAdj));

	if (fPickZ <= minz)
		return true; // 바닥까지 피킹 완료

	return GetSamplingImpl(vecPickData, fPosX, fPosY, fPickZ-1);
}


void GPickingZTable::LoadPickingZSize(wstring& strFilename)
{
	MXml kXml;
	if ( !kXml.LoadFile(MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str()))
		return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("AREA");
	if (pRootElement == NULL)
		return;

	string strText;
	if ( _Contents(strText, pRootElement))
	{
		sscanf(strText.c_str(),"%d %d %d %d %d",
			&m_bPartialArea, &m_rtPartialArea.left, &m_rtPartialArea.top, &m_rtPartialArea.right, &m_rtPartialArea.bottom);
	}
}

void GPickingZTable::LoadFromFile(wstring& strFilename)
{
	GCollisionTree* pCollisionTree = m_Desc.pCollisionTree; 
	VALID(pCollisionTree);

	ifstream s(strFilename.c_str(), ios::in|ios::binary);
	s.read((char*)&m_nChunkSizeX, sizeof m_nChunkSizeX);
	s.read((char*)&m_nChunkSizeY, sizeof m_nChunkSizeY);

	m_ChunkTable = new Chunk[m_nChunkSizeX*m_nChunkSizeY];
	DCHECK(m_nChunkSizeX > 0);
	DCHECK(m_nChunkSizeY > 0);

	for (int y = 0; y < m_nChunkSizeY; y++)
	{
		for (int x = 0; x < m_nChunkSizeX; x++)
		{
			Chunk& chunk = m_ChunkTable[CalcChunkPoint(x, y)];
			bool bIsValidChunk = false;
			s.read((char*)&bIsValidChunk, sizeof bIsValidChunk);
			if (bIsValidChunk)
			{
				chunk.PickingTable = new HEIGHT_TYPE[nCellMaxSize*nCellMaxSize];
				memset(chunk.PickingTable, 0, sizeof(chunk.PickingTable[0])*nCellMaxSize*nCellMaxSize);
				s.read((char*)chunk.PickingTable, sizeof(chunk.PickingTable[0])*nCellMaxSize*nCellMaxSize);
			}
			else
			{
				s.read((char*)&chunk.nDefaultHeight, sizeof chunk.nDefaultHeight);
			}
		}
	}

	int nMultiCellSize = 0;
	s.read((char*)&nMultiCellSize, sizeof nMultiCellSize);

	for (int i=0; i<nMultiCellSize; ++i)
	{
		uint32 nFlatPoint=-1;
		s.read((char*)&nFlatPoint, sizeof nFlatPoint);


		Cell* pNewCell = new Cell();
		m_mapCells.insert(std::make_pair(nFlatPoint, pNewCell));
		m_mapCells[nFlatPoint]->LoadFromFile(s);
	}


	dlog("PickingZ File Loaded. (cell qty: %d)\n", GPickingZTable::Cell::s_nTotalQty);
}

void GPickingZTable::SaveToFile(wstring& strFilename)
{
	ofstream s(strFilename.c_str(), ios::out|ios::binary);
	DCHECK(m_nChunkSizeX > 0);
	DCHECK(m_nChunkSizeY > 0);
	s.write((char*)&m_nChunkSizeX, sizeof m_nChunkSizeX);
	s.write((char*)&m_nChunkSizeY, sizeof m_nChunkSizeY);

	for (int y = 0; y < m_nChunkSizeY; y++)
	{
		for (int x = 0; x < m_nChunkSizeX; x++)
		{
			Chunk& chunk = m_ChunkTable[CalcChunkPoint(x, y)];
			bool bIsValidChunk = chunk.PickingTable!=NULL;
			s.write((char*)&bIsValidChunk, sizeof bIsValidChunk);
			if (bIsValidChunk)
			{
				s.write((char*)chunk.PickingTable, sizeof(chunk.PickingTable[0])*nCellMaxSize*nCellMaxSize);
			}
			else
			{
				s.write((char*)&chunk.nDefaultHeight, sizeof chunk.nDefaultHeight);
			}
		}
	}

	int nMultiCellSize = m_mapCells.size();
	s.write((char*)&nMultiCellSize, sizeof nMultiCellSize);
	CellMap::iterator itEnd = m_mapCells.end();
	for (CellMap::iterator it = m_mapCells.begin();
		it != itEnd;
		++it)
	{
		uint32 nFlatPoint=it->first;
		s.write((char*)&nFlatPoint, sizeof nFlatPoint);

		Cell* pCell = it->second;
		pCell->SaveToFile(s);
	}
}

bool GPickingZTable::IsValidChunk( int nChuckX, int nChuckY )
{
	VALID_RET(m_Desc.pPathFinder, false);

	float maxz = GetBoundingBox().maxz;// + 200.0f;

	for (int nCellX=0; nCellX<nCellMaxSize; ++nCellX)
	{
		for (int nCellY=0; nCellY<nCellMaxSize; ++nCellY)
		{
			float fPosX = ConvertToPosX(nChuckX, nCellX);
			float fPosY = ConvertToPosY(nChuckY, nCellY);
			SPathMesh* pPathMesh = m_Desc.pPathFinder->GetMesh(m_Desc.nZoneID);
			if (m_Desc.pPathFinder->IsValidPosition(pPathMesh, vec3(fPosX, fPosY, maxz)))
			{
				return true;
			}
		}
	}	
	return false;
}

void GPickingZTable::RecordPickingData( vector<HEIGHT_TYPE> &vecPickData, 
									   Chunk& chunk, 
									   int nChuckX, int nChuckY,
									   int nCellX, int nCellY, 
									   HEIGHT_TYPE nCurHeight )
{
	if (vecPickData.size() == 1)
	{
		// 피킹 테이블에 저장
		chunk.PickingTable[CalcPickingTablePoint(nCellX, nCellY)] = nCurHeight;
		return;
	}

	uint32 nFlatPoint = CalcCellPoint(nChuckX, nChuckY, nCellX, nCellY);

	Cell* pNewCell = new Cell();
	m_mapCells.insert(std::make_pair(nFlatPoint, pNewCell));

	pNewCell->MakeHeightArray(vecPickData.size());
	for (size_t i=0; i<vecPickData.size(); ++i)
	{
		pNewCell->SetHeight(i, vecPickData[i]);
	}
}

const MBox& GPickingZTable::GetBoundingBox() const
{
	GCollisionTree* pCollisionTree = m_Desc.pCollisionTree; 
	DCHECK(pCollisionTree);
	return pCollisionTree->GetBoundingBox();
}

float GPickingZTable::GetMinX()
{
	if (m_bPartialArea)
	{
		return (float)m_rtPartialArea.left;
	}

	return m_Desc.fMinX;
}

float GPickingZTable::GetMinY()
{
	if (m_bPartialArea)
	{
		return (float)m_rtPartialArea.top;
	}

	return m_Desc.fMinY;
}

float GPickingZTable::GetMaxX()
{
	if (m_bPartialArea)
	{
		return (float)m_rtPartialArea.right;
	}

	return m_Desc.fMaxX;
}

float GPickingZTable::GetMaxY()
{
	if (m_bPartialArea)
	{
		return (float)m_rtPartialArea.bottom;
	}

	return m_Desc.fMaxY;
}