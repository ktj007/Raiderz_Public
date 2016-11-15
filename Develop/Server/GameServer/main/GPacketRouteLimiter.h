#pragma once

class GPacketRouteLimiter
{
public:
	GPacketRouteLimiter(void);
	~GPacketRouteLimiter(void);

	// 복잡레벨 반환
	int CalcCrowdedLevel(int nPlayerQty) const;
	// 패킷 제한 횟수
	int LimitPacketQty(int nPlayerQty) const;
	// 라우팅 시도
	bool TryRoute(int nPlayerQty, int nCmdID, MUID uidSender);
	// 등록된 플레이어 수
	int GetRegisteredPlayerQty() const;
	// 섹터값이 생성시킨 섹터값 추가
	void AddOwner(uint32 nFlatIdx);
	// 섹터값이 생성시킨 섹터값 제거
	void RemoveOwner(uint32 nFlatIdx);
	// 섹터 소유값이 없는지 여부
	bool IsEmptyOwner() const;
	// 해당 섹터값이 생성시킨 섹터인지 여부
	bool IsOwnerSector(uint32 nFlatIdx);
private:
	void InsertCircularQueue(MUID uid);
private:
	typedef map<int,int>				PacketCountMap;
	typedef map<MUID,PacketCountMap>	PlayerPacketCountMap;

	PlayerPacketCountMap m_mapCounts;
	deque<MUID>			 m_vecCircularQueue;
	set<uint32>			 m_setOwnerSector;
};
