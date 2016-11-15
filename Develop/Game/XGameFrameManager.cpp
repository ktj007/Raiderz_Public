#include "stdafx.h"
#include "XGameFrameManager.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// XGameFrameManager ////////////////////////////////////////////////////////////////////////
XGameFrameManager::XGameFrameManager() : m_pCurFrame(NULL), m_bReserved(false), m_nPrevFrame(-1), m_nNextFrame(-1)
{
}

XGameFrameManager::~XGameFrameManager()
{
	DeleteAll();
}

bool XGameFrameManager::OnEvent(MEvent& e)
{
	if (m_pCurFrame) return m_pCurFrame->OnEvent(e);
	return false;
}

void XGameFrameManager::Add(XBaseFrame* pGameFrame)
{
	_ASSERT( pGameFrame->m_pFrameManager == NULL && "잘못된 프레임매니저");
	pGameFrame->m_pFrameManager = this;
	m_FrameMap.insert(map<int, XBaseFrame*>::value_type(pGameFrame->GetID(), pGameFrame));
}

void XGameFrameManager::DeleteAll()
{
	if (m_pCurFrame)
	{
		m_pCurFrame->Close( -1);
		m_pCurFrame = NULL;
	}

	while (!m_FrameMap.empty())
	{
		map<int, XBaseFrame*>::iterator itor = m_FrameMap.begin();
		delete (*itor).second;
		m_FrameMap.erase(itor);
	}

	m_bReserved = false;
}

void XGameFrameManager::Change(int nFrameID)
{
	m_bReserved = true;
	m_nNextFrame = nFrameID;
}

void XGameFrameManager::Update(float fDelta)
{
	PFC_THISFUNC;

	// SceneManager와 타이머 싱크
//	REngine::GetSceneManager().SyncTimerToApp(fDelta);

	// Update가 Render보다 먼저 불린다고 가정하고 만듦.
	if (m_bReserved &&
		(m_pCurFrame && m_pCurFrame->CheckCloseState()))
	{
		PerformChange(m_nNextFrame);

		m_nNextFrame = -1;
		m_bReserved = false;
	}

	if (m_pCurFrame)
		m_pCurFrame->Update(fDelta);
}

void XGameFrameManager::OnPreRender()
{
	if (m_pCurFrame) m_pCurFrame->OnPreRender();
}

void XGameFrameManager::OnRender()
{
	if (m_pCurFrame) m_pCurFrame->OnRender();
}

void XGameFrameManager::OnActive(bool bActive)
{
	if (m_pCurFrame) m_pCurFrame->OnActive(bActive);
}

bool XGameFrameManager::Start(int nFrameID, const wchar_t* szArgs)
{
	return PerformChange( nFrameID, szArgs);
}

bool XGameFrameManager::PerformChange( int nFrameID, const wchar_t* szArgs)
{
	PFC_THISFUNC;

	if (m_pCurFrame != NULL)
	{
		m_nPrevFrame = m_pCurFrame->GetID();
		m_pCurFrame->Close( nFrameID);
	}

	map<int, XBaseFrame*>::iterator itor = m_FrameMap.find(nFrameID);
	if (itor != m_FrameMap.end())
	{
		m_pCurFrame = (*itor).second;
		return m_pCurFrame->Open( m_nPrevFrame, szArgs);
	}
	else
	{
		m_pCurFrame = NULL;
	}
	return false;
}

int XGameFrameManager::GetCurrFrameID()
{
	if (m_pCurFrame) return m_pCurFrame->GetID();
	return -1;
}

int XGameFrameManager::GetPrevFrameID()
{
	return m_nPrevFrame;
}

XBaseFrame* XGameFrameManager::GetFrame( int nFrameID)
{
	map<int, XBaseFrame*>::iterator itor = m_FrameMap.find( nFrameID);
	if ( itor == m_FrameMap.end())		return NULL;
	return (*itor).second;
}

XBaseFrame* XGameFrameManager::GetFrameFromName(const wchar_t* szFrameName)
{
	for (map<int, XBaseFrame*>::iterator itor = m_FrameMap.begin(); itor != m_FrameMap.end(); ++itor)
	{
		XBaseFrame* pFrame = (*itor).second;
		if (!_wcsicmp(szFrameName, pFrame->GetName()))
		{
			return pFrame;
		}
	}
	return NULL;
}

void XGameFrameManager::Message( const wchar_t* szMsg )
{
	if (m_pCurFrame) m_pCurFrame->Message(szMsg);
}

void XGameFrameManager::OnPreFlip()
{
	if (m_pCurFrame) m_pCurFrame->OnPreFlip();
}
