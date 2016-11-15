#include "stdafx.h"
#include "XLoadingCompleter.h"
#include "XGame.h"
#include "XLoadingFrame.h"
#include "XCameraManager.h"
#include "XController.h"

XLoadingCompleter_Game::XLoadingCompleter_Game( XGame* pGame, XLoadingFrame* pLoadingFrame ) : m_pGame(pGame), m_pLoadingFrame(pLoadingFrame)
{

}

XLoadingCompleter_Game::~XLoadingCompleter_Game()
{

}

void XLoadingCompleter_Game::WorldLoadingFinish()
{
	XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
	if (pCameraController)
	{
		pCameraController->Init(XGetMyUID());
	}

	m_pGame->OnWorldLoadingComplete();

}

void XLoadingCompleter_Game::Complete()
{
//	m_pLoadingFrame->Close();
}

