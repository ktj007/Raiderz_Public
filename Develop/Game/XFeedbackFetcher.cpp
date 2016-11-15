#include "stdafx.h"
#include "XFeedbackFetcher.h"
#include "XFeedback.h"
#include "XScript.h"

XFeedbackFetcher::XFeedbackFetcher(XFeedbackInfoMgr* pMgr) : m_pFeedbackInfoMgr(pMgr)
{
	m_HttpSpooler.SetRecvCallback(this, XFeedbackFetcher::RecvCallback);
	m_HttpSpooler.Create();

	m_HttpSpooler2.Create();
}

XFeedbackFetcher::~XFeedbackFetcher()
{
	m_HttpSpooler.Destroy();
	m_HttpSpooler2.Destroy();
}

bool XFeedbackFetcher::RecvCallback( void* pCallbackContext, char* pRecvData )
{
	XFeedbackFetcher* pContext = static_cast<XFeedbackFetcher*>(pCallbackContext);
	pContext->OnRecv((wchar_t *)MLocale::ConvAnsiToUTF16(pRecvData).c_str());

	return true;
}

void XFeedbackFetcher::OnRecv( wchar_t* pRecvData )
{
	if (m_pFeedbackInfoMgr)
	{
		m_pFeedbackInfoMgr->LoadFromXmlStream(pRecvData);

		if (global.script)
		{
			vector<string> strFeedbackList;

			XFeedbackInfoMgr::FEEDBACK_LIST* pFeedbackList = m_pFeedbackInfoMgr->FindFeedbackList(gvar.World.GetFieldID());
			
			if (pFeedbackList)
			{
				for (XFeedbackInfoMgr::FEEDBACK_LIST::iterator itor = pFeedbackList->begin();
					itor != pFeedbackList->end(); ++itor)
				{
					XFeedbackInfo* pInfo = (*itor);

					strFeedbackList.push_back(MLocale::ConvUTF16ToAnsi(pInfo->m_strComment.c_str()));
				}
			}
			global.script->GetGlueGameEvent().OnFetchFeedback(strFeedbackList);
		}

	}
}

void XFeedbackFetcher::Fetch()
{
	if (!m_pFeedbackInfoMgr->GetShowFeedback()) return;

	m_HttpSpooler.Query("http://icecream/soap/feedback_xml.php");
}


void XFeedbackFetcher::FetchWeb()
{
	m_HttpSpooler2.Query("http://icecream/soap/feedback.php?mode=fetch");
}
