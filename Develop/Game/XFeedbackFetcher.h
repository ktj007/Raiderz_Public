#pragma once

#include "MHttp.h"

class XFeedbackInfoMgr;

class XFeedbackFetcher
{
private:
protected:
	MHttp	m_HttpSpooler;
	MHttp	m_HttpSpooler2;
	XFeedbackInfoMgr*	m_pFeedbackInfoMgr;

	static bool RecvCallback(void* pCallbackContext, char* pRecvData);
	void OnRecv(wchar_t* pRecvData);
public:
	XFeedbackFetcher(XFeedbackInfoMgr* pMgr);
	virtual ~XFeedbackFetcher();
	void Fetch();
	void FetchWeb();
};