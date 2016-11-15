#ifndef _GLOG_CACHE_H
#define _GLOG_CACHE_H


#include "atlstr.h"


template<typename T>
class GLogCache
{
public :
	GLogCache(size_t nMaxLogCount = MAX_LOG_COUNT, DWORD nMaxElapseTime = MAX_CACHING_ELAPSED_TIME)  
		: m_nIdx(0), m_nMaxLogCount(nMaxLogCount), m_nMaxElapseTime(nMaxElapseTime)
	{ 
		m_nLastWriteLogingTime = GetTime(); 
		m_Log = new T[nMaxLogCount];
	}

	virtual ~GLogCache() 
	{
		SAFE_DELETE_ARRAY(m_Log);
	}

	enum
	{
		MAX_LOG_COUNT				= 5,
		MAX_CACHING_ELAPSED_TIME	= 10 * 1000, // 10√ .
	};

	void PushBack(const T& data, bool bDirectPost = false)
	{
		_ASSERT(NULL != m_Log);

		if (NULL == m_Log)
			return;

		m_Log[m_nIdx++] = data;

		if (IsFirstLog())
			SetLastWriteLogTime();

		if (bDirectPost || IsOverflowCount() || IsOverflowTime())
			PostLog();					
	}

	void PostLog() 
	{ 
		_ASSERT(NULL != m_Log);

		if((0 < m_nIdx) && (NULL != m_Log)) 
		{
			m_strSQL.ReleaseBuffer();
			OnPostLog(); 
			SetLastWriteLogTime();
			Reset();
		}
	}

	uint32 GetLastWriteLogTime() { return m_nLastWriteLogingTime; }

	bool IsOverflowTime()	{ return m_nMaxElapseTime <= (GetTime() - m_nLastWriteLogingTime); }
	bool IsOverflowCount()	{ return m_nMaxLogCount <= m_nIdx;}
	bool IsHaveLog()			{ return 0 < m_nIdx; }

protected :
	virtual void OnPostLog() = 0;

	virtual bool IsFirstLog()	{ return 1 == m_nIdx; }
	
	void SetLastWriteLogTime() { m_nLastWriteLogingTime = GetTime(); }

	virtual void Reset()
	{
		for (size_t i = 0; i < m_nIdx; ++i)
			m_Log[i].Reset();

		m_nIdx = 0;
	}

	virtual DWORD GetTime() { 	return timeGetTime(); }

protected :
	size_t			m_nIdx;
	const size_t	m_nMaxLogCount;
	const DWORD		m_nMaxElapseTime;
	T*				m_Log;
	uint32			m_nLastWriteLogingTime;
	CString			m_strSQL;
};


#endif