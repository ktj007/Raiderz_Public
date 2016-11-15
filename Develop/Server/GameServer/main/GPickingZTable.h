#pragma once

#include "GDef.h"
#include <hash_map>
#include <fstream>
#include <ostream>
#include "MStatisticsProfiler.h"

#define PICKING_CELL_SIZE	(40)
#define PICKING_CHUNK_SIZE	(10*100)
#define INVALID_PICKING_HEIGHT	0


class GCollisionTree;
class GPathFinder;

typedef uint16 HEIGHT_TYPE;

TEST_FORWARD_N(PickingZTable, Init);
TEST_FORWARD_N(PickingZTable, ConvertToIndex);
class GFieldInfo;

class GPickingZTable : public MTestMemPool<GPickingZTable>
{
	TEST_FRIEND_N(PickingZTable, Init);
	TEST_FRIEND_N(PickingZTable, ConvertToIndex);
public:

	struct Desc
	{
		int nZoneID;
		float fMinX, fMinY, fMaxX, fMaxY;
		GCollisionTree* pCollisionTree;
		GPathFinder*	pPathFinder;

		Desc(): nZoneID(-1), fMinX(0.0f), fMinY(0.0f), fMaxX(0.0f), fMaxY(0.0f), pCollisionTree(NULL), pPathFinder(NULL) {}
	};

#pragma pack(push, old)			//////////////////////////////////////////////////////////
#pragma pack(1)

	template <int n>
	class THeightData : public MMemPool<THeightData<n>, 1000>
	{
	public:
		enum { DataSize = n };

		HEIGHT_TYPE		m_nHeight[n];
	};

	typedef THeightData<1>		HeightData;
	typedef THeightData<2>		HeightData2;
	typedef THeightData<3>		HeightData3;
	typedef THeightData<4>		HeightData4;
	typedef THeightData<5>		HeightData5;
	typedef THeightData<6>		HeightData6;
	typedef THeightData<7>		HeightData7;
	typedef THeightData<8>		HeightData8;
	typedef THeightData<9>		HeightData9;
	typedef THeightData<10>		HeightData10;
	typedef HeightData10		HeightDataMax;

	class Cell : public MMemPool<Cell, 10000>
	{
	public:
		Cell();
		Cell(uint8 nHeightQty);
		~Cell();

		void DeleteHeightData();

		void MakeHeightArray(uint8 nSize);
		int GetHeight(float fPosZ, int nBoundMax) const;
		HEIGHT_TYPE GetHeight(uint8 nIndex) const;
		void SetHeight(uint8 nIndex, HEIGHT_TYPE nHeight );
		void LoadFromFile(ifstream& s);
		void SaveToFile(ofstream& s);

		HeightData*	NewHeightData(uint8 nHeightQty);

		static long s_nTotalQty;
	private:
		// 결과 높이값
		HeightData*		m_HeightData;
		// 높이값 갯수
		uint8 m_nHeightQty;
	};

	class Chunk : public MTestMemPool<Chunk>
	{
	public:
		HEIGHT_TYPE*	PickingTable;
		HEIGHT_TYPE	nDefaultHeight;

		Chunk() : PickingTable(NULL), nDefaultHeight(INVALID_PICKING_HEIGHT) {}
	};
#pragma pack(pop, old)		//////////////////////////////////////////////////////////

public:
	GPickingZTable(const Desc& desc);
	~GPickingZTable(void);

	// 초기화
	void Clear();
	// Zpicking 영역 저장 파일 불러오기
	void LoadPickingZSize(wstring& strFilename);
	// 파일로부터 불러오기
	void LoadFromFile(wstring& strFilename);
	// 파일로 저장하기
	void SaveToFile(wstring& strFilename);

	// 샘플링 작업 수행
	void	RunSamplingWork();
	// Z좌표 피킹
	float Pick( float fPosX, float fPosY, float fPosZ );
private:
	// 좌표를 그리드 인덱스로 변환
	MPoint	ConvertToChunkIdx(float fPosX, float fPosY);
	// X 그리드 인덱스를 좌표로 변경
	float ConvertToPosX(int nChunkX, int nCellX);
	// Y 그리드 인덱스를 좌표로 변경
	float ConvertToPosY(int nChunkY, int nCellY);
	// 유효한 인덱스인지 여부
	bool	IsValidIndex(int rx, int ry);
	// 해당 좌표에 맞는 샘플링 계산 (실제 피킹)
	bool GetSampling(int nIdxX, int nIdxY);
	// 피킹 정보를 저장
	void RecordPickingData( vector<HEIGHT_TYPE> &vecPickData, Chunk& chunk, int nChuckX, int nChuckY, int nCellX, int nCellY, HEIGHT_TYPE nCurHeight );
	// 해당 청크가 유효(네비게이션 메쉬 위에 위치)한지 여부 
	bool IsValidChunk( int nChuckX, int nChuckY );
	// 샘플링 구현함수
	bool GetSamplingImpl(vector<HEIGHT_TYPE>& vecPickData, float fPosX, float fPosY, float fPosZ);
	bool GetSamplingImpl(Cell** pCell, float fPosX, float fPosY, float fPosZ);

	const MBox& GetBoundingBox() const;
	inline uint32 CalcChunkPoint(int nChunkX, int nChunkY);
	inline uint32 CalcCellPoint(int nChunkX, int nChunkY, int nCellX, int nCellY);
	inline uint32 CalcPickingTablePoint(int nCellX, int nCellY);
	MPoint ConvertToPickingTableIdx(int nChunkX, int nChunkY, float fPosX, float fPosY);

	bool CheckBoundingBoxZ() const;
	float GetBoundingBoxHeight() const;

	float GetMinX();
	float GetMinY();
	float GetMaxX();
	float GetMaxY();
private:
	typedef stdext::hash_map<uint32,Cell*> CellMap;
	Chunk*		m_ChunkTable;
	CellMap		m_mapCells;
	int			m_nChunkSizeX;
	int			m_nChunkSizeY;
	Desc		m_Desc;
	// 지정 영역 사용 여부
	bool		m_bPartialArea;
	// 지정 영역
	MRect		m_rtPartialArea;
};



uint32 GPickingZTable::CalcChunkPoint(int nChunkX, int nChunkY)
{
	return	(nChunkY*m_nChunkSizeX)+nChunkX;
}

uint32 GPickingZTable::CalcCellPoint(int nChunkX, int nChunkY, int nCellX, int nCellY)
{
	return	((nChunkY*PICKING_CHUNK_SIZE+nCellY)*m_nChunkSizeX*PICKING_CHUNK_SIZE)+ // y좌표
		(PICKING_CHUNK_SIZE*nChunkX)+nCellX;	// x좌표
}

uint32 GPickingZTable::CalcPickingTablePoint(int nCellX, int nCellY)
{
	const int nCellMaxSize = PICKING_CHUNK_SIZE/PICKING_CELL_SIZE+1;
	return	(nCellY*nCellMaxSize)+nCellX;
}


