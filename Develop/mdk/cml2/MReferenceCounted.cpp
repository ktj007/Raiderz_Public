#include "stdafx.h"
#include "MReferenceCounted.h"
#include <cassert>

MReferenceCounted::MReferenceCounted() : m_nReferenceCounter(1)
{

}

MReferenceCounted::~MReferenceCounted()
{

}

void MReferenceCounted::AddRef() const
{
	::InterlockedIncrement(&m_nReferenceCounter);
}

bool MReferenceCounted::Drop() const
{
	assert(m_nReferenceCounter > 0);

	if (0 == ::InterlockedDecrement(&m_nReferenceCounter))
	{
		delete this;
		return true;
	}

	return false;
}

long MReferenceCounted::GetReferenceCount() const
{
	return m_nReferenceCounter;
}








MRefMgr::~MRefMgr()
{
	for each (std::pair<const MReferenceCounted*,int> RefData in m_RefObjs)
	{
		for (int i=0; i<RefData.second; i++)
			RefData.first->Drop();
	}

	m_RefObjs.clear();
}

void MRefMgr::AddRef( const MReferenceCounted* pObj )
{
	MAP_REF::iterator iter = m_RefObjs.find(pObj);

	if (iter == m_RefObjs.end())
	{
		m_RefObjs.insert(MAP_REF::value_type(pObj, 1));
	}
	else
	{
		iter->second++;
	}

	pObj->AddRef();
}

void MRefMgr::Drop( const MReferenceCounted* pObj )
{
	MAP_REF::iterator iter = m_RefObjs.find(pObj);
	assert(iter != m_RefObjs.end());
	if (iter == m_RefObjs.end())	return;

	int& nCounter = iter->second;
	--nCounter;
	if (nCounter <= 0)
	{
		m_RefObjs.erase(pObj);
	}

	pObj->Drop();
}

