#include "stdafx.h"
#include "ROccluderList.h"

namespace rs3
{
	ROccluderList::ROccluderList( void )
	{
	}

	ROccluderList::~ROccluderList( void )
	{

	}

	void ROccluderList::AddOccluderPointsSet( ROccluderPointsSet*	pOccluderPointsSet )
	{
		_ASSERT(NULL != pOccluderPointsSet);
		_ASSERT(false == ExistOccluderPointsSet(pOccluderPointsSet));
		m_OccluderList.insert(pOccluderPointsSet);
	}

	void ROccluderList::RemoveOccluderPointsSet( ROccluderPointsSet*	pOccluderPointsSet )
	{
		_ASSERT(NULL != pOccluderPointsSet);
		_ASSERT(true == ExistOccluderPointsSet(pOccluderPointsSet));
		m_OccluderList.erase(pOccluderPointsSet);
	}

	bool ROccluderList::ExistOccluderPointsSet( ROccluderPointsSet*	pOccluderPointsSet )
	{
		_ASSERT(NULL != pOccluderPointsSet);
		if( m_OccluderList.end() == m_OccluderList.find(pOccluderPointsSet) )	return false;
		return true;
	}

	void ROccluderList::Clear( void )
	{
		m_OccluderList.clear();
	}


}