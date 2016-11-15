#include "stdafx.h"
#include "XFeedbackSystem.h"
#include "XFeedbackFetcher.h"
#include "XFeedback.h"
#include "XSystem.h"
#include "XFeedbackSender.h"
#include "XWidgetNameDef.h"
#include "XMyPlayer.h"

XFeedbackSystem::XFeedbackSystem()
: m_pFeedbackFetcher(NULL)
, m_pFeedbackInfoMgr(NULL)
, m_fUpdateObjectsElapsedTime(0.0f)
, m_fUpdateFetchElapsedTime(0.0f)
{
	m_pFeedbackInfoMgr = new XFeedbackInfoMgr();
	m_pFeedbackFetcher = new XFeedbackFetcher(m_pFeedbackInfoMgr);
}

XFeedbackSystem::~XFeedbackSystem()
{
	SAFE_DELETE(m_pFeedbackFetcher);
	SAFE_DELETE(m_pFeedbackInfoMgr);
}

void XFeedbackSystem::MakeFeedbackObjects()
{
	if (m_pFeedbackInfoMgr)
	{
		m_pFeedbackInfoMgr->MakeObjects();
	}
}

void XFeedbackSystem::UpdateFeedbackObjects( float fDelta )
{
	m_fUpdateObjectsElapsedTime += fDelta;

	if (m_fUpdateObjectsElapsedTime > 5.0f)
	{
		m_fUpdateObjectsElapsedTime = 0.0f;

		MakeFeedbackObjects();
	}
}

void XFeedbackSystem::Update( float fDelta )
{
	UpdateFeedbackObjects(fDelta);
}

void XFeedbackSystem::ReloadFeedbackFromWeb()
{
	if (m_pFeedbackFetcher)
	{
		m_pFeedbackFetcher->FetchWeb();
		m_pFeedbackFetcher->Fetch();
	}
}

void XFeedbackSystem::FetchFeedbackWeb()
{
	if (m_pFeedbackFetcher)
		m_pFeedbackFetcher->FetchWeb();
}

void XFeedbackSystem::SetShowFeedback( bool bShow )
{
	if (!m_pFeedbackInfoMgr) return;

	m_pFeedbackInfoMgr->SetShowFeedback(bShow);

	if (bShow)
	{
		if (m_pFeedbackInfoMgr->IsEmpty())
		{
			m_pFeedbackFetcher->Fetch();
		}
	}
}

void XFeedbackSystem::Fetch()
{
	if (!m_pFeedbackFetcher) return;

	m_pFeedbackFetcher->Fetch();
}

void XFeedbackSystem::SendFeedback( const wchar_t* szText )
{
	if (gvar.Game.pMyPlayer == NULL) return;

	wstring strName = gvar.MyInfo.ChrInfo.szName;
	wstring strUserID = gvar.MyInfo.strLoginID;
	wstring strTag;
	int nFieldID = gvar.World.GetFieldID();

	vec3 pos = gvar.Game.pMyPlayer->GetPosition();
	vec3 dir = gvar.Game.pMyPlayer->GetDirection();


	XFeedbackSender feedbackSender;
	feedbackSender.Send(strUserID.c_str(), 
		strName.c_str(),
		nFieldID,
		pos,
		dir,
		strTag.c_str(),
		szText
		);

	FetchFeedbackWeb();
}

void XFeedbackSystem::UpdateFetch( float fDelta )
{
	m_fUpdateFetchElapsedTime += fDelta;

	if (global.mint == NULL) return;

	MWidget* pFeedbackWindow = global.mint->FindWidget(WIDGET_NAME_FEEDBACK_WINDOW);

	if (!pFeedbackWindow || !pFeedbackWindow->GetShow())
	{
		return;
	}

	if (m_fUpdateFetchElapsedTime > 5.0f)
	{
		m_fUpdateFetchElapsedTime = 0.0f;
		Fetch();
	}
}