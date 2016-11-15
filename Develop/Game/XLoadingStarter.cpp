#include "stdafx.h"
#include "XLoadingStarter.h"
#include "XGame.h"
#include "XLoadingFrame.h"

XLoadingStarter_Game::XLoadingStarter_Game( XGame* pGame, XLoadingFrame* pLoadingFrame ) : m_pGame(pGame), m_pLoadingFrame(pLoadingFrame)
{

}

XLoadingStarter_Game::~XLoadingStarter_Game()
{

}

void XLoadingStarter_Game::Start()
{
// 	_ASSERT(m_pLoadingFrame->IsOpened() == false);
// 
// 	m_pLoadingFrame->Open(NULL);
// 	m_pLoadingFrame->RenderOnUpdate();
}