#include "stdafx.h"
#include "MLinkMap.h"

namespace minet {


	MLinkMap::MLinkMap()
	{

	}

	MLinkMap::~MLinkMap()
	{
		Clear();
	}

	void MLinkMap::Add( MLink* pLink )
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		m_LinkMap.insert(LINK_MAP::value_type(pLink->GetUID(), pLink));
	}

	bool MLinkMap::Delete( MUID uidLink, MUID* poutReAllocedUID/*=NULL*/ )
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		LINK_MAP::iterator itor = m_LinkMap.find(uidLink);
		if (itor == m_LinkMap.end())
		{
			REALLOC_UID_MAP::iterator itorReallocUIDMap = m_ReAllocUIDMap.find(uidLink);
			if (itorReallocUIDMap == m_ReAllocUIDMap.end())
				return false;

			MUID uidReAlloced = (*itorReallocUIDMap).second;

			itor = m_LinkMap.find(uidReAlloced);
			if (itor == m_LinkMap.end())
				return false;

			if (NULL != poutReAllocedUID)
				*poutReAllocedUID = uidReAlloced;
		}
		
		MLink* pLink = (*itor).second;

		pLink->Drop();

		m_LinkMap.erase(itor);

		m_ReAllocUIDMap.erase(uidLink);

		return true;
	}

	void MLinkMap::Clear()
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		for(LINK_MAP::iterator itor = m_LinkMap.begin(); itor != m_LinkMap.end(); itor++)
		{
			MLink* pLink = (*itor).second;

			pLink->Drop();
		}

		m_LinkMap.clear();
		m_ReAllocUIDMap.clear();
	}

	MLink* MLinkMap::GetLink( const MUID uidLink )
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		LINK_MAP::iterator itor = m_LinkMap.find(uidLink);
		if (itor != m_LinkMap.end())
		{
			MLink* pLink = (*itor).second;
			pLink->AddRef();

			return pLink;
		}
		return NULL;
	}

	MLink* MLinkMap::GetLinkFromReAllocTable( const MUID uidLink )
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		REALLOC_UID_MAP::iterator itor = m_ReAllocUIDMap.find(uidLink);
		if (itor == m_ReAllocUIDMap.end())
			return NULL;

		MUID uidReAlloced = (*itor).second;
		m_ReAllocUIDMap.erase(uidLink);

		LINK_MAP::iterator itorLinkMap = m_LinkMap.find(uidReAlloced);
		if (itorLinkMap == m_LinkMap.end())
			return NULL;
		
		MLink* pLink = (*itorLinkMap).second;
		pLink->AddRef();

		return pLink;
	}

	bool MLinkMap::ReAllocUID( MUID uidOrigin, MUID uidReAlloc )
	{
		cml2::MCriticalSection::Guard guard(m_csLock);

		LINK_MAP::iterator itor = m_LinkMap.find(uidOrigin);
		if (itor == m_LinkMap.end())
			return false;
		
		MLink* pLink = (*itor).second;
		m_LinkMap.erase(itor);

		pLink->ReAllocUID(uidReAlloc);
		m_LinkMap.insert(LINK_MAP::value_type(uidReAlloc, pLink));

		m_ReAllocUIDMap.insert(REALLOC_UID_MAP::value_type(uidOrigin, uidReAlloc));

		return true;
	}







} // namespace minet