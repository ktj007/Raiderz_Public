#include "stdafx.h"
#include "XEventID.h"
#include "XMagicTalent.h"
#include "XTarget.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XProjectile.h"

void XMagicTalent::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
}

void XMagicTalent::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str(), L"", m_pTalentInfo->m_nMovableType);
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);

	// 만약 원거리 공격 타입이면 발사체 발사
	// 또한 사용 애니메이션이 있어야 한다.
	if(((m_pTalentInfo->m_nSkillType == ST_ARCHERY) || (m_pTalentInfo->m_nSkillType == ST_MAGIC)) &&
		(!m_pTalentInfo->m_szUseAnimation.empty()))
	{
		// XModuleEntity에게 이벤트 후 발사 명령

		//mlog0("TALENT ACT %d\n", m_pTalentInfo->m_nID);

		//mlog0("발사준비 %d(%s)\n", m_pTalentInfo->m_nID, m_pTalentInfo->m_szUseAnimation.c_str());
	}

	// 기타 처리
	// 발사 메세지 자신에게 보내기
	SendMsg(XEVTL_TALENT_ACT_SPELL);
}

void XMagicTalent::OnStart()
{
	// 탤런트 시작 알림
	SendMsg(XEVTL_TALENT_START);
}

void XMagicTalent::SendMsg(int nEventID)
{
	bool bSetMsg = false;
	XEvent evt;

	switch(nEventID)
	{
	case XEVTL_TALENT_START:
	case XEVTL_TALENT_ACT_SPELL:
	case XEVTL_TALENT_RECASTING:
		{
			evt.m_nID			= nEventID; 
			evt.m_pData			= m_pTalentInfo;
			bSetMsg				= true;
		}
		break;
	}

	if(bSetMsg == true)
	{
		m_pOwner->Event(evt);
	}
}

void XMagicTalent::OnReCastingTalent( int nTalentID, int nStep )
{
	// 탤런트 ID가 같은지 비교.
	// 현재 쓰고 있는 탤런트가 같아야 현재 단계가 맞는지 알 수 있다.
	if(m_pTalentInfo->m_nID == nTalentID)
	{
		SendMsg(XEVTL_TALENT_RECASTING);
	}
}

bool XMagicTalent::IsNeedTarget()
{
	if(m_pTalentInfo->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_TARGET)
		return true;

	return false;
}

XEventResult XMagicTalent::OnEvent( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_UPPER_MOTION_PLAYONCE:
		{
			if(m_pTalentInfo->m_nMovableType == MUT_USE)
			{
				wchar_t* szMotionName = MOTION_NAME_IDLE;
				if (m_pOwner->GetEntityType() == ETID_NPC) szMotionName = MOTION_NAME_NPC_IDLE;

				XModuleMotion * pModuleMotion = m_pOwner->GetModuleMotion();
				if(pModuleMotion)
					pModuleMotion->ChangeMotion(szMotionName);
			}
		}
		break;
	}

	return MR_FALSE;
}
