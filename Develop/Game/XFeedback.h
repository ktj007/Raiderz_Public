#pragma once

#include "MTypes.h"
#include "MThread2.h"
#include <map>
#include <list>
using namespace std;

/// 피드백 글 정보
class XFeedbackInfo
{
public:
	int		m_nID;
	int		m_nFieldID;
	vec3	m_vPos;
	vec3	m_vDir;
	wstring	m_strComment;
	wstring	m_strUserID;
	wstring	m_strName;
	wstring	m_strDateTime;
	wstring	m_strTag;

	MUID	m_uidObject;

	XFeedbackInfo() : m_nID(0), m_nFieldID(0), m_vPos(vec3::ZERO), m_vDir(vec3(0.0f, 1.0f, 0.0f)), m_uidObject(MUID::ZERO)
	{

	}
};

/// 피드백 정보 관리자
class XFeedbackInfoMgr
{
public:
	typedef list<XFeedbackInfo*>		FEEDBACK_LIST;
	typedef map<int, FEEDBACK_LIST*>	FEEDBACKS_MAP;
private:
	cml2::MCriticalSection				m_Lock;
protected:
	bool								m_bShowFeedback;
	FEEDBACKS_MAP						m_FeedbackMap;

	void ParseFeedback(MXmlElement* pFeedbackElement, MXml* pXml);

	void Clear();

	void ClearObjects();
public:
	XFeedbackInfoMgr();
	virtual ~XFeedbackInfoMgr();
	bool LoadFromXmlStream(const wchar_t* szStream);

	void MakeObjects();
	void SetShowFeedback(bool bShow);
	bool GetShowFeedback() { return m_bShowFeedback; }
	bool IsEmpty() { return m_FeedbackMap.empty(); }

	FEEDBACK_LIST* FindFeedbackList(int nFieldID);
};