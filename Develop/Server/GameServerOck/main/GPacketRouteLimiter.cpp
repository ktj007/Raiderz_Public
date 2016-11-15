#include "StdAfx.h"
#include "GPacketRouteLimiter.h"
#include "GConst.h"

GPacketRouteLimiter::GPacketRouteLimiter(void)
{
}

GPacketRouteLimiter::~GPacketRouteLimiter(void)
{
}

int GPacketRouteLimiter::CalcCrowdedLevel( int nPlayerQty ) const
{
	return nPlayerQty/GConst::SECTOR_CROWDED_LEVEL_CAPACITY;
}

int GPacketRouteLimiter::LimitPacketQty(int nPlayerQty) const
{
	int nLevel = CalcCrowdedLevel(nPlayerQty);
	return nLevel * GConst::SECTOR_PACKET_IGNORE_EACH_COUNT;
}

bool GPacketRouteLimiter::TryRoute(int nPlayerQty, int nCmdID, MUID uidSender )
{
	PlayerPacketCountMap::iterator itCount = m_mapCounts.find(uidSender);
	if (itCount == m_mapCounts.end())
	{
		InsertCircularQueue(uidSender);
		m_mapCounts.insert(make_pair(uidSender, PacketCountMap()));
		itCount = m_mapCounts.find(uidSender);
	}

	PacketCountMap& count_map = itCount->second;
	PacketCountMap::iterator itPacketID = count_map.find(nCmdID);
	if (itPacketID == count_map.end())
	{
		count_map.insert(make_pair(nCmdID, 0));
		itPacketID = count_map.find(nCmdID);
	}
	
	int& nCount = itPacketID->second;
	++nCount;

	if (nCount > LimitPacketQty(nPlayerQty))
	{
		nCount = 0;
		return true;
	}

	return false;
}

void GPacketRouteLimiter::InsertCircularQueue( MUID uid )
{
	m_vecCircularQueue.push_back(uid);

	if ((int)m_vecCircularQueue.size() <= GConst::SECTOR_PACKET_LIMIT_QUEUE_SIZE)
		return; // 큐 최대크기 초과되지 않음

	MUID uidErased = m_vecCircularQueue.front();
	m_vecCircularQueue.pop_front();
	m_mapCounts.erase(uidErased);
}

int GPacketRouteLimiter::GetRegisteredPlayerQty() const
{
	DCHECK(m_vecCircularQueue.size() == m_mapCounts.size());
	return m_vecCircularQueue.size();
}

void GPacketRouteLimiter::AddOwner( uint32 nFlatIdx )
{
	m_setOwnerSector.insert(nFlatIdx);
}

void GPacketRouteLimiter::RemoveOwner( uint32 nFlatIdx )
{
	m_setOwnerSector.erase(nFlatIdx);
}

bool GPacketRouteLimiter::IsOwnerSector( uint32 nFlatIdx )
{
	return (m_setOwnerSector.find(nFlatIdx) != m_setOwnerSector.end());
}

bool GPacketRouteLimiter::IsEmptyOwner() const
{
	return m_setOwnerSector.empty();
}
