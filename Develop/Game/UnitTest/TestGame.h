#pragma once

#include "MockNetClient.h"
#include "MWidget.h"
#include "MockAnimationController.h"
#include "RMesh.h"

using namespace rs3;

class XGame;
class MockController;
class XMyPlayer;
class XNetPlayer;
class MWidget;

class TestGame
{
private:
	XGame*				m_pGame;
	MockController*		m_pController;
	mint3::MWidget*		m_pFocusWidget;

	TestAniDataSet		m_PlayerAniDataSet;
	void				InitGlobalVar();
	void				AddPlayerColCapsule();

	RMesh*				m_pDefaultMesh;
public:
	TestGame();
	~TestGame();

	void			Update(float fDelta);

	XGame*			GetGame() { return m_pGame; }
	MockController*	GetController() { return m_pController; }
	XMyPlayer*		GetMyPlayer();
	TestAniDataSet& GetPlayerAniDataSet() { return m_PlayerAniDataSet; }
	RMesh*			GetDefaultMesh() { return m_pDefaultMesh; }
};
