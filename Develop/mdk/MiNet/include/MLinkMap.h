#pragma once

#include "MLink.h"

namespace minet {


class MLinkMap
{
public:
	typedef map<MUID, MLink*>		LINK_MAP;
	typedef std::map<MUID, MUID>	REALLOC_UID_MAP;
protected:
	LINK_MAP					m_LinkMap;
	REALLOC_UID_MAP				m_ReAllocUIDMap;
public:
	cml2::MCriticalSection		m_csLock;
public:
	MLinkMap();
	~MLinkMap();
	void Add(MLink* pLink);
	bool Delete(MUID uidLink, MUID* poutReAllocedUID=NULL);
	void Clear();
	bool ReAllocUID(MUID uidOrigin, MUID uidReAlloc);
	MLink* GetLink(const MUID uidLink);		// GetLink함수 안에서 AddRef 하므로 주의.
	MLink* GetLinkFromReAllocTable( const MUID uidLink );

	// 쓰레드에 안전하지 않음 ---------------------

	LINK_MAP& GetLinkMap()	{ return m_LinkMap; }	
	// 쓰레드에 안전하지 않음 --------------------^
};


} // namespace minet {