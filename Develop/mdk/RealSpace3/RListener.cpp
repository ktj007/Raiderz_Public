#include "stdafx.h"
#include "RListener.h"

namespace rs3 {

REventBroadcaster::~REventBroadcaster()
{
	_ASSERT(m_listeners.empty());
}

void REventBroadcaster::AddListener(RListener* pListener)
{
	m_listeners.push_back( pListener );
}

void REventBroadcaster::RemoveListener(RListener* pListener)
{
	m_listeners.remove( pListener );
}

void REventBroadcaster::Notify()
{
	list<RListener*>::iterator itr = m_listeners.begin();
	for( ;itr!=m_listeners.end(); ++itr )
	{
		RListener* pListener = *itr;
		pListener->Update();
	}
}


}