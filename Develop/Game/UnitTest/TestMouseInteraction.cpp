#include "stdafx.h"
#include "XMyPlayer.h"
#include "MockPlayer.h"
#include "FTestGame.h"
#include "XModuleMovable.h"
#include "MockController.h"

SUITE(MouseInteraction)
{
	class FMouseInteraction : public FTestGame
	{
	public:
		FMouseInteraction() : FTestGame()
		{
			m_pMyPlayer		= m_TestGame.GetMyPlayer();

			info.motion->AddMotionForTest(
				L"<MOTION name='run' stance='all' loop='true' cancelEnabled='true' cameraLook='true'>	\
				<Default>	<Sequence ani='run_f'/> </Default>	\
				<Forward>	<Sequence ani='run_f'/> </Forward>	\
				<Backward>	<Sequence ani='run_b'/> </Backward>	\
				<Left>		<Sequence ani='run_l'/> </Left>		\
				<Right>		<Sequence ani='run_r'/> </Right>	\
				</MOTION>");
		}

		virtual ~FMouseInteraction()
		{

		}

		XMyPlayer* m_pMyPlayer;
	};

	TEST_FIXTURE(FMouseInteraction, TestMouseMove)
	{
		vec3 vCurPos = m_pMyPlayer->GetPosition();
		vec3 vNextPos = vCurPos + vec3(0, 0, 1000);
		m_TestGame.GetController()->TestMouseMove(vNextPos);

		XModuleMovable* pModuleMovable = m_pMyPlayer->GetModuleMovable();

		XMovableFactor movableFactor = pModuleMovable->GetMovableFactor();
		CHECK(movableFactor.MOUSE_MOVE.m_bWorking);
		CHECK(movableFactor.INTERPOLATION.m_bWorking);
	}
}