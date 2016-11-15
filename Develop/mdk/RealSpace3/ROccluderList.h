#pragma once

#include "ROccluderContext.h"
#include <set>

namespace rs3
{
	class ROccluderPointsSet;

	class ROccluderList
	{
	public:
		ROccluderList(void);
		~ROccluderList(void);

	public:
		void		AddOccluderPointsSet(ROccluderPointsSet*	pOccluderPointsSet);
		void		RemoveOccluderPointsSet(ROccluderPointsSet* pOccluderPointsSet);
		bool		ExistOccluderPointsSet(ROccluderPointsSet* pOccluderPointsSet);
		void		Clear(void);	

		unsigned int		Size(void)	{ return m_OccluderList.size(); }
		

	public:
		typedef std::set<ROccluderPointsSet*> ROCCLUDERPOINTSSET_LIST;
		ROCCLUDERPOINTSSET_LIST		m_OccluderList;
	};
}