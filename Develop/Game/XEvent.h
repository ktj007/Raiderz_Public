#ifndef _XMESSAGE_H
#define _XMESSAGE_H

/// 메세지 결과값
enum XEventResult
{
	MR_FALSE = 0,
	MR_TRUE,
	MR_IGNORED,
	MR_ERROR
};

/// 로컬 메세지
class XEvent
{
public:
	int		m_nID;
	void*	m_pData;
	void*	m_pResult;

	XEvent() : m_nID(0), m_pData(NULL), m_pResult(NULL) {}
	XEvent(const int nID) : m_nID(nID), m_pData(NULL), m_pResult(NULL) {}
	XEvent(const int nID, void* data) : m_nID(nID), m_pData(data), m_pResult(NULL) {}
	XEvent(const int nID, void* data, void* result) : m_nID(nID), m_pData(data), m_pResult(result) {}
};

//////////////////////////////////////////////////////////////////////////


#endif // #ifndef _XMESSAGE_H