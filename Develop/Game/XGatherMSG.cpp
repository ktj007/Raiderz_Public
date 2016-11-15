#include "StdAfx.h"
#include "XGatherMSG.h"
#include "XNonPlayer.h"
#include "XMyPlayer.h"
#include "XInteractionInfo.h"
#include "CCommandResultTable.h"
#include "XSoundResMgr.h"
#include "XStrings.h"

bool XGatherMSG::Check( const UIID nUIID )
{
	if (XGetInteractionInfo().GetSearchUIID() != UIID_INVALID)
	{
		XNonPlayer* pNPC = gg.omgr->FindNPC_UIID(XGetInteractionInfo().GetSearchUIID());
		if (pNPC == NULL) return false;
		if (pNPC->GetInfo()->nGatherType == GATHER_NONE) return false;

		vec3 pos = pNPC->GetPosition();
		float dis = pos.XYDistanceTo(gvar.Game.pMyPlayer->GetPosition());
		if (dis > CHECK_INTERACTION_DISTANCE) return false;	// 인터랙션 가능 거리보다 멀면 메세지 출력안되도록

		int nGatherTier = pNPC->GetInfo()->eIT % (IT_GATHER_1-1);
		if (nGatherTier > 10) return false;

		wstring strKey;
		GATHER_TYPE gatherType = pNPC->GetInfo()->nGatherType;

		if(gatherType != GATHER_LUMBER && false == gvar.MyInfo.Inventory.IsHaveGatherTool(gatherType))
		{
			strKey = XGetStr( L"LOOTING_NEEDGATHERINGTOOL");
			XCmdResultSound(CR_FAIL_LOOT_GATHER_NEED_GATHERTOOL);
		}
		else if (false == gvar.MyInfo.Talent.IsLearnedGatherTalent(1))
		{
			strKey = XGetStr( L"LOOTING_NEEDTOREADMANUAL");
			XCmdResultSound(CR_FAIL_LOOT_GATHER_NEED_TRAINING);			
		}
		else if (false == gvar.MyInfo.Talent.IsLearnedGatherTalent(nGatherTier))
		{
			strKey = XGetStr( L"LOOTING_NEEDTOREADADVANCEDMANUAL");
			XCmdResultSound(CR_FAIL_LOOT_GATHER_NEED_HIGHER_TRAINING);			
		}
#ifdef _DEBUG
		else
		{
			strKey = L"채집 인터랙션이 불가능합니다.";
		}
#endif	

		if (!strKey.empty())
		{
			global.ui->OnPresentationLower(strKey, PRESENTATION_ICON_EXCLAMATION);
			return true;
		}
	}
	return false;
}