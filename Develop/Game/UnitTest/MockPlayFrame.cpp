#include "stdafx.h"
#include "MockPlayFrame.h"
#include "XGame.h"

bool MockPlayFrame::Open( const wchar_t* args )
{
	// XPlayFrame에 필요한 객체
	_ASSERT(m_pMovableDebugger == NULL);
	_ASSERT(m_pGame == NULL);
	m_pMovableDebugger = new XMovableDebugger();
	m_pGame = new XGame();

	m_pLoadingFrame->Start();
//	CreateLoadingMgr(m_pGame, &m_LoadingFrame);

	return true;
}

void MockPlayFrame::Close()
{
//	DestroyLoadingMgr();

	if (m_pMovableDebugger)
	{
		SAFE_DELETE(m_pMovableDebugger);
	}

	if (m_pGame)
		m_pGame->Destroy();

	SAFE_DELETE(m_pGame);
}