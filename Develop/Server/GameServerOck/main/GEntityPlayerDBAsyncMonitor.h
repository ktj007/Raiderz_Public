#ifndef _GENTITYPLAYER_DBASYNCMONITOR_H
#define _GENTITYPLAYER_DBASYNCMONITOR_H


class GEntityPlayerDBAsyncMonitor : public MTestMemPool<GEntityPlayerDBAsyncMonitor>
{
public :
	GEntityPlayerDBAsyncMonitor() : m_nDBAsyncRefCount(0) {}
	~GEntityPlayerDBAsyncMonitor() {}

	DWORD GetRefCount() { return m_nDBAsyncRefCount; }
	void IncreaseRefCount() { ++m_nDBAsyncRefCount; }
	void DecreaseRefCount() { _ASSERT(0 < m_nDBAsyncRefCount); --m_nDBAsyncRefCount; }

private :
	DWORD m_nDBAsyncRefCount;
};


#endif