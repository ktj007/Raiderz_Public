#include "StdAfx.h"
#include "XDebugLootFrm.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleUI.h"
#include "XGameTransformControllerManager.h"
#include "XInteractionInfo.h"

XDebugLootFrm::XDebugLootFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	ShowTitleBar( false);
	SetRect(10, 120, 480, 140);
//	SetOpacity(0.6f);
	SetText("Loot info");
}

void XDebugLootFrm::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	if (gg.omgr == NULL) return;

	ResetContent();

	wchar_t szBuf[512];

	vector<XObject*> vecNPC;
	gg.omgr->GetSortDistanceEntityTypeList(vecNPC, ETID_NPC);

	int i =0;
	for each (XObject* obj in vecNPC)
	{
		XNonPlayer* pTargetNPC = AsNPC(obj);
		if (pTargetNPC && pTargetNPC->GetInfo())
		{
			if (pTargetNPC->IsLootable())
			{
				swprintf_s(szBuf, 512, L"[%d] Lootable NPC : %s - %s - m_bshowLootbleTargetEffect == %s",
					i++, pTargetNPC->GetName(), obj->GetModuleMotion()->GetCurrMotion().c_str(),
					obj->GetModuleUI()->IsShowLootbleTargetEffect() ? L"true" : L"false");
				AddString(MLocale::ConvUTF16ToAnsi(szBuf).c_str());
			}
		}
	}

	swprintf_s(szBuf, 512, L"------------------------------------------------");
	AddString(MLocale::ConvUTF16ToAnsi(szBuf).c_str());

	XObject* pObj = gg.omgr->Find(XGetInteractionInfo().GetInteractionSearchUID());
	if( pObj && pObj->GetEntityType() == ETID_NPC )
	{
		XNonPlayer* pTargetNPC = AsNPC(pObj);
		if (pTargetNPC && pTargetNPC->GetInfo())
		{
			if (pTargetNPC->IsLootable())
			{
				swprintf_s(szBuf, 512, L"Lootable Target : %s", pTargetNPC->GetName());
				AddString(MLocale::ConvUTF16ToAnsi(szBuf).c_str());

				XModuleEntity* pModuleEntity = pObj->GetModuleEntity();
				if (pModuleEntity)
				{
					swprintf_s(szBuf, 512, L"ani = %s , (%3.2f / %3.2f)", pModuleEntity->GetAnimationController()->GetAnimationName().c_str(),
						pModuleEntity->GetAnimationController()->GetCurFrameToSecond(),
						pModuleEntity->GetAnimationController()->GetMaxFrameToSecond());
					AddString(MLocale::ConvUTF16ToAnsi(szBuf).c_str());
				}
			}
		}
	}
}