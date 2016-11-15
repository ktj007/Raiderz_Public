#pragma once

#include "GDef.h"
#include "CSChrInfo.h"
#include "MMemPool.h"
#include "GFieldInfo.h"

class GFieldInfo;
class GSectorAIEventInvokerMgr;
class GPacketRouteLimiterMgr;

TEST_FORWARD_FT(FieldGrid, FGrid, Init);
TEST_FORWARD_FT(FieldGrid, FGrid, LookupForCell);
TEST_FORWARD_FT(FieldGrid, FGrid, HugeCell);

//////////////////////////////////////////////////////////////////////////
//
//	GFieldGrid
//
//////////////////////////////////////////////////////////////////////////

class GFieldGrid : public MMemPool<GFieldGrid>
{
	TEST_FRIEND_FT(FieldGrid, FGrid, Init);
	TEST_FRIEND_FT(FieldGrid, FGrid, LookupForCell);
	TEST_FRIEND_FT(FieldGrid, FGrid, HugeCell);
public:

	enum MovingDirection
	{
		MD_NONE=-1,
		MD_LEFT=0,
		MD_TOPLEFT,
		MD_TOP,
		MD_TOPRIGHT,
		MD_RIGHT,
		MD_BOTTOMRIGHT,
		MD_BOTTOM,
		MD_BOTTOMLEFT,
		MD_FULL,
		MD_SAME,
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//	Sector
	//
	//////////////////////////////////////////////////////////////////////////

	class Sector : public MTestMemPool<GFieldGrid::Sector>
	{
	public:
		class IVisitor
		{
			friend class Sector;
			virtual void OnVisit(const MUID& uid) = 0;
		};

		typedef set<MUID>	EntitySet;
	public:
		Sector();
		~Sector();

		// 엔티티 추가
		void AddEntity(ENTITY_TYPEID nEntityTypeID, const MUID& uid);
		// 엔티티 삭제
		void RemoveEntity(ENTITY_TYPEID nEntityTypeID, const MUID& uid);
		// 엔티티 목록 초기화
		void ClearEntity();
		// 엔티티가 존재하는지 여부
		bool IsExist(const MUID& uid) const;
		// 엔티티 갯수
		size_t GetEntityQty(ENTITY_TYPEID type) const;
		// 엔티티 갯수
		size_t GetEntityQty() const;

		// 엔티티들 방문
		void Visit(IVisitor& visitor,const ENTITY_TYPEID& type) const;

		// 좌표
		void			SetPosition(int x,int y) { m_position.x = x; m_position.y = y; }
		const MPoint&	Position() const { return m_position; }

	private:
		void VisitAllTypes(IVisitor& visitor) const;

	private:
		// 엔티티 타입별 셋
		EntitySet	m_setEntities[ETID_MAX];
		// 엔티티 전체 셋
		EntitySet	m_setAllEntities;
		MPoint		m_position;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//	Selector
	//
	//////////////////////////////////////////////////////////////////////////

	class SectorSelector
	{
	public:
		class IVisitor
		{
			friend class SectorSelector;
			//todo virtual void OnVisit(const Sector& sector) = 0;
			virtual void OnVisit(Sector& sector) = 0;
		};

		class IRefusableVisitor
		{
			friend class SectorSelector;
			virtual void OnVisit(const Sector& sector,bool& continueVisit) = 0;
		};

	public:
		SectorSelector(const GFieldGrid& grid);
		
		// 개별 섹터 반환
		GFieldGrid::Sector* Lookup( int x, int y ) const;
		// 개별 거대 섹터 반환
		GFieldGrid::Sector* LookupHugeSector( int x, int y ) const;

		// 아래 메서드 들의 인자 halfSideCnt(섹터 덩어리 한 면의 반개. 예 - 5 x 5섹터일 경우 가운데를제외 한 왼쪽2개, 오른쪽2개)는?
		// 섹터 검색은 현재 중앙을 기준으로 좌4, 우4개 이상 하지 않는다. 더 많은 섹터 검색은 추후, 다른 요구사항이 있을 경우 다시 생각해 보기로 한다.
		// (huge섹터는 일반 섹터의 HUGE_SECTOR_SCALE배이고, 한면에 huge섹터 3개이다. 그리고 중심을 기준으로 검색하기 때문에 HUGE_SECTOR_SCALE * 3 / 2가 된다.)
		// aabb나 radius또한 마찬가지로 계산 결과가 광범위한 섹터를 요구하더라도 13x13이상 하지 않는다.(사실 거대섹터 크기는 12x12지만 가운데 중심섹터 1개와 좌 우로 6개씩 6 + 1 + 6 = 13이 되어버린다.. )

		// 특정 섹터 좌표 인근의 셀들을 방문
		void VisitNeighbors(IVisitor& visitor, const MPoint& sectorNo, long halfSideCnt) const;
		// 특정 섹터 좌표 인근의 셀들을 방문
		void VisitNeighbors(IRefusableVisitor& visitor, const MPoint& sectorNo, long halfSideCnt) const;
		// 특정 좌표 인근의 섹터들을 방문
		void VisitNeighborsByPos(IVisitor& visitor, const vec3& position, long halfSideCnt) const;
		// 특정 좌표 범위 내 인근의 섹터들을 방문
		void VisitNeighborsByRadius(IVisitor& visitor, const vec3& position, const float& radius) const;
		// 특정 좌표위의 사각형과 겹치는 섹터들을 방문
		void VisitNeighborsIntersectWithAABB(IVisitor& visitor, const vec3& position, const MBox& aabb) const;
		// 특정 사각형 안의 섹터들을 방문
		void VisitNeighborsIntersectWithRect(IVisitor& visitor, const MRect& rect) const;
		// 특정 셀 좌표 인근의 거대섹터들 방문
		void VisitHugeNeighbors(IVisitor& visitor, const MPoint& sectorNo) const;

		const GFieldGrid& Grid() const { return m_Grid; }

	protected:
		const GFieldGrid& m_Grid;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//	EntitySelector
	//
	//////////////////////////////////////////////////////////////////////////

	class EntitySelector : public MTestMemPool<EntitySelector>
	{
	public:
		EntitySelector(const SectorSelector& sectorSelector);

		// 특정 섹터좌표에 해당하는 섹터와 인근 섹터들의 엔티티들 방문
		void Visit(Sector::IVisitor& visitor,ENTITY_TYPEID type, const MPoint& sectorNo, const long& halfSideCnt ) const;
		// 특정 좌표에 해당하는 섹터와 인근 섹터들의 엔티티들 방문
		void VisitByPos(Sector::IVisitor& visitor, ENTITY_TYPEID type, const vec3& position, const long& halfSideCnt) const;
		// 특정 좌표 범위 내 인근의 섹터들의 엔티티들 방문
		void VisitByRadius(Sector::IVisitor& visitor, ENTITY_TYPEID type, const vec3& position, const float& radius) const;
		// 특정 좌표위의 사각형과 겹치는 섹터들을 방문
		void VisitIntersectWithAABB(Sector::IVisitor& visitor, ENTITY_TYPEID type, const vec3& position, const MBox& aabb) const;
		// 특정 사각형 안의 섹터들을 방문
		void VisitIntersectWithRect(Sector::IVisitor& visitor, ENTITY_TYPEID type, const MRect& rect) const;
		// 특정 셀 좌표 인근의 거대섹터들 방문
		void VisitInHugeSectors(Sector::IVisitor& visitor, ENTITY_TYPEID type, const MPoint& sectorNo) const;
		// 특정 셀 좌표 인근의 거대섹터들 방문
		void VisitInHugeSectorsByPos(Sector::IVisitor& visitor, ENTITY_TYPEID type, const vec3& position) const;

		// 특정 셀 주변에 플레이어가 없는지 검사
		bool IsEmptyPlayers(MPoint ptSector, int nSize=1) const;
		bool IsEmptyPlayers(vec3 vPoint, int nSize=1) const;

	private:
		const SectorSelector& m_sectorSelector;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//	DeltaSelector
	//
	//////////////////////////////////////////////////////////////////////////

	class DeltaSelector : public MTestMemPool<DeltaSelector>
	{
	public:
		DeltaSelector(const SectorSelector& sectorSelector);

		// 변경된 셀 좌표에 따른 이동방향 반환
		MovingDirection CalcMovingDirection(MPoint ptOld, MPoint ptNew) const;

		// 셀 이동방향에 따른 신규 셀 방문
		void VisitNewSectors(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, const MPoint& oldSectorPt, const MPoint& newSectorPt, int nSize ) const;
		// 셀 이동방향에 따른 제거될 셀 방문
		void VisitOldSectors(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, const MPoint& oldSectorPt, const MPoint& newSectorPt, int nSize ) const;
		// 셀 이동방향에 따른 신규 거대셀 방문
		void VisitNewHugeSectors(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, const MPoint& oldSectorPt, const MPoint& newSectorPt ) const;
		// 셀 이동방향에 따른 제거될 거대셀 방문
		void VisitOldHugeSectors(Sector::IVisitor& visitor, ENTITY_TYPEID entityType, const MPoint& oldSectorPt, const MPoint& newSectorPt ) const;

		// 셀 이동방향에 따른 신규 셀 반환
		void GetDeltaNewSectors( vector<Sector*>& vecSectors, MPoint ptOld, MPoint ptNew, int nSize=1 ) const;
		// 셀 이동방향에 따른 신규 거대 셀 반환
		void GetDeltaNewHugeSectors( vector<Sector*>& vecSectors, MPoint ptOld, MPoint ptNew) const;
	
	private:
		// 셀 이동방향에 따른 셀 반환 재귀 구현함수
		void GetDeltaSectorsImpl( bool bOld, vector<Sector*>& vecSectors, MovingDirection nDirection, MPoint ptSector, int nSize, MovingDirection nInnerCallType=MD_NONE, bool bIsHuge=false) const;
		
		typedef GFieldGrid::Sector* (GFieldGrid::SectorSelector::*SectorLookupFunc)(int,int) const;
		void VisitImpl( Sector::IVisitor& visitor, ENTITY_TYPEID entityType, SectorLookupFunc sectorLookUp, bool bOld, MovingDirection nDirection, const MPoint& ptSector, int nSize, MovingDirection nInnerCallType=MD_NONE) const;

	private:
		const SectorSelector& m_sectorSelector;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//	BoundChecker
	//
	//////////////////////////////////////////////////////////////////////////

	class BoundChecker : public MTestMemPool<BoundChecker>
	{
	public:
		class Lisener
		{
		public:
			virtual void OnChangeOutters(int nPlayerQtyInBoundary) = 0;
		};

		BoundChecker(GFieldGrid::SectorSelector& selector);

		void SetLisener(Lisener* pLisener);
		void SetBoundary(MRect rtBoundary);
		void OnEnterSector(const MPoint& sectorPt, MUID uidPlayer);
		void OnLeaveSector(const MPoint& sectorPt, MUID uidPlayer);

	private:
		bool IsBoundary(const MPoint& sectorPt) const;
		int GetPlayerQtyInBoundary();
		void GetPlayerQtyInBoundaryImpl(int sx, int sy, int& outnPlayerQty);
	private:
		MRect m_rtBoundary;
		Lisener* m_pLisener;
		set<MUID> m_setOutters;
		GFieldGrid::SectorSelector& m_Selector;
	};

public:
	GFieldGrid();
	~GFieldGrid(void);

	// 초기 설정
	void Init(const MUID uidField, const GFieldInfo* pFieldInfo);
	// 초기화
	void Clear();
	// 엔티티 추가
	void AddEntity(Sector& ptSector, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge=false);
	// 엔티티 삭제
	bool RemoveEntity(const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge=false);
	// 엔티티 이동
	void MoveEntity( const MPoint& ptOld, const MPoint& ptNew, ENTITY_TYPEID nEntityTypeID, const MUID& uid, bool bIsHuge=false );
	// 셀렉터 반환
	SectorSelector* GetSectorSelector();
	// 엔티티 셀렉터 반환
	EntitySelector* GetEntitySelector();
	// 델타 셀렉터 반환
	DeltaSelector* GetDeltaSelector();
	// 바운드체커 생성
	void CreateBoundChecker(BoundChecker::Lisener* const pLisener);

	const GFieldInfo* const GetFieldInfo() const;

	// 그리드 가로 갯수
	inline int GetSizeX() const;
	// 그리드 세로 갯수
	inline int GetSizeY() const;
	// 셀 찾기
	inline Sector* Lookup(int x, int y) const;
	inline Sector* Lookup(MPoint ptSector) const;
	// 유효한 셀인지 검사
	inline bool IsValidIndex(int x, int y) const;
	inline bool IsValidIndex(MPoint ptSector) const;
	// 1차원 좌표계로 변경
	inline uint32 CalcFlatPoint(int x, int y) const;
	inline uint32 CalcFlatPoint(MPoint ptSector) const;
	// 3차원 좌표를 셀 인덱스로 변경
	inline MPoint ConvertToIdx(vec3 vPoint) const;
	// 필드 바운드박스 최소좌표
	inline float GetFieldMinX() const;
	// 필드 바운드박스 최소좌표
	inline float GetFieldMinY() const;
	// 필드 바운드박스 최대좌표
	inline float GetFieldMaxX() const;
	// 필드 바운드박스 최대좌표
	inline float GetFieldMaxY() const;

	// 패킷 라우트 제한 관리자 반환
	GPacketRouteLimiterMgr* GetPacketRouteLimiterMgr();

private:
	void AddEntitiyToHugeSector( const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, MUID uidEntity );
	void RemoveEntityFromHugeSector( const MPoint& ptSector, ENTITY_TYPEID nEntityTypeID, MUID uidEntity );
	GFieldGrid::Sector* const LookupHugeSector( MPoint ptSector ) const;
private:
	typedef map<uint32,Sector> HugeSectorMap;	// pair<flat_point, huge_mob_qty>

	const GFieldInfo*	m_pFieldInfo;
	Sector*		m_Sectors;
	int			m_nSizeX;
	int			m_nSizeY;
	GSectorAIEventInvokerMgr*	m_pAIInvokerMgr;
	HugeSectorMap	m_mapHugeSectors;
	SectorSelector* m_pSelector;
	EntitySelector* m_pEntitySelector;
	DeltaSelector* m_pDeltaSelector;
	GPacketRouteLimiterMgr* m_pRouteLimiterMgr;
	BoundChecker*			m_pBoundChecker;
	
};


//
// Inline Implements
//

int GFieldGrid::GetSizeX() const
{
	return m_nSizeX;
}

int GFieldGrid::GetSizeY() const
{
	return m_nSizeY;
}

float GFieldGrid::GetFieldMinX() const
{
	return m_pFieldInfo->MinX();
}

float GFieldGrid::GetFieldMinY() const
{
	return m_pFieldInfo->MinY();
}

float GFieldGrid::GetFieldMaxX() const
{
	return m_pFieldInfo->MaxX();
}

float GFieldGrid::GetFieldMaxY() const
{
	return m_pFieldInfo->MaxY();
}

uint32 GFieldGrid::CalcFlatPoint(int x, int y) const
{
	_ASSERT(0 <= x && x < m_nSizeX);
	_ASSERT(0 <= y && y < m_nSizeY);
	return	(y*m_nSizeX)+x;
}

uint32 GFieldGrid::CalcFlatPoint(MPoint ptSector) const
{
	return	CalcFlatPoint(ptSector.x, ptSector.y);
}

MPoint GFieldGrid::ConvertToIdx( vec3 vPoint ) const
{
	return MPoint(
		(int)((vPoint.x - GetFieldMinX()) / SECTOR_SIZE), 
		(int)((vPoint.y - GetFieldMinY()) / SECTOR_SIZE)
		);
}

GFieldGrid::Sector* GFieldGrid::Lookup(int x, int y) const
{
	PFC_THISFUNC;
	if (!IsValidIndex(x, y)) 
		return NULL;

	return &m_Sectors[CalcFlatPoint(x,y)];
}

GFieldGrid::Sector* GFieldGrid::Lookup(MPoint ptSector) const
{
	return Lookup(ptSector.x, ptSector.y);
}

bool GFieldGrid::IsValidIndex(int x, int y) const
{
	if ( x<0 || y<0 || x>=m_nSizeX || y>=m_nSizeY ) 
	{
		return false;
	}
	return true;
}

bool GFieldGrid::IsValidIndex(MPoint ptSector) const
{
	return IsValidIndex(ptSector.x, ptSector.y);
}