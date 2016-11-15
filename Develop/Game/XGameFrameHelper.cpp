#include "stdafx.h"
#include "XGameFrameHelper.h"
#include "XProfiler.h"
#include "XMovableDebugger.h"

#include "XPost_GM.h"
#include "XBaseApplication.h"
#include "XMyPlayer.h"

#include "XSoundResMgr.h"

#ifdef _MADDUCK
	#include "XMagicArea.h"
	//#include "XSystem.h"
	#include "XGame.h"
	#include "XModuleMyControl.h"
#endif

bool XGameFrameHelper::OnDebugEvent(MEvent& e)
{
	if(global.ui /*&& global.ui->IsFocusMainInput()*/)
	{
		switch(e.nMessage)
		{
		case MWM_CHAR:
			{
#ifndef _PUBLISH
				switch (e.nKey) 
				{
				case 'b':
					{
						XScreenDebugger::GetInstance()->Show(!XScreenDebugger::GetInstance()->IsVisible());
						return true;
					}
					break;
				}
#endif // _PUBLISH
			}
			break;
			// 테스트용으로 쓰는 KeyDown입니다.
		case MWM_KEYDOWN:
			{
				switch (e.nKey) 
				{
				case VK_F5:
					{
						static int nTarget = 0;

						if(nTarget == 0)
							nTarget = 1;
						else
							nTarget = 0;

					}
					break;
				}
				break;
			}
		}
	}

	switch(e.nMessage)
	{
	case MWM_KEYDOWN:
		{
			int nKeyPadNum = 0;
			switch (e.nKey) 
			{
#ifndef _PUBLISH
			case VK_F4:
				{
					if (e.bCtrl == false)	global.app->ToggleSolidWire();
				}
				break;
			case VK_F3:
				{
					if(e.bAlt)
					{
						PFC_RESET;
					}
					else
					{
					}
				}
				break;
#endif // _PUBLISH
#ifdef _MADDUCK
			case VK_F7:
				{
					gvar.Game.pMyPlayer->GetModuleMyControl()->SetTargetIndicatorToggle(false);
				}
				break;
			case VK_F9:
				{
					global.ui->OnPresentationUpper(L"나레이션을 테스트합니다.");
					global.ui->OnPresentationLower(L"나레이션을 테스트합니다.");
					global.script->GetGlueGameEvent().OnGameEvent( "MAPSIGN", "테스트영역");
				}
				break;
#endif // _madduck
			// bparts
			case VK_NUMPAD1:
				{
					nKeyPadNum = 1;
					break;
				}
			case VK_NUMPAD2:
				{
					nKeyPadNum = 2;
					break;
				}
			case VK_NUMPAD3:
				{
					nKeyPadNum = 3;
					break;
				}
			case VK_NUMPAD4:
				{
					nKeyPadNum = 4;
					break;
				}
			}
			if (nKeyPadNum > 0 && e.bCtrl == true && gg.omgr)
			{
				for (XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
				{
					XObject* pTarget = (*itor).second;
					if (pTarget == gvar.Game.pMyPlayer)
						continue;

					if (pTarget->GetPosition().DistanceTo(gvar.Game.pMyPlayer->GetPosition()) < 1000.f)
					{
						XPostGM_BreakPart(pTarget->GetUID(), nKeyPadNum);
					}
				}
			}
		}
		break;
	}
	return false;
}

void XGameFrameHelper::RenderMint()
{
//	TIMEPROFILE_THIS_FUNCTION;

	//////////////////////////////////////////////////////////////////////////
	// 렌더링 순서
	// 1. 게임 내용 (engine)
	// 2. 3D 글자
	// 3. screen effect ( screen 카메라를 써서 )
	// 4. Mint

	// mint를 사용하지 않은 ui 렌더링 ( screeen effect manager / pane manager )
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(104);

	global.mint->GetDC()->BeginDraw();

	// 디버그 정보 그리기
	XScreenDebugger::GetInstance()->Draw(global.ui->GetDC());

	//if (XMovableDebugger::GetInstancePtr())
	//{
	//	XMovableDebugger::GetInstancePtr()->Draw(global.ui->GetDC());
	//}

	global.mint->GetDC()->EndDraw();

	{
		TIMEPROFILE("MINT");
		// UI 그리기
		PFC_B("global.mint->Draw()");
		global.mint->Draw();
		PFC_E;
	}
}

