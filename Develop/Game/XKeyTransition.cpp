#include "stdafx.h"
#include "XKeyTransition.h"

static unsigned int WeaponTypeToKeyCondWC(WEAPON_TYPE nType)
{
	switch (nType)
	{
	case WEAPON_NONE:			return WC_NONE;
	case WEAPON_1H_SLASH:		return WC_1HS;
	case WEAPON_1H_BLUNT:		return WC_1HB;
	case WEAPON_1H_PIERCE:		return WC_1HP;
	case WEAPON_TWO_HANDED:		return WC_2HD;
	case WEAPON_STAFF:			return WC_STF;
	case WEAPON_ARCHERY:		return WC_ARC;
	case WEAPON_2H_BLUNT:		return WC_2HB;
	case WEAPON_DUAL_WIELD:		return WC_DWD;
	case WEAPON_DUAL_PIERCE:	return WC_DWP;
	default: _ASSERT(0);
	}
	return WC_NONE;
}

ACTION_STATE XKeyConditionChecker::CheckKeyCondition( XVirtualKey nVirtualKey, unsigned int nKeyState, CHAR_STANCE nCurrStance)
{
	// 몇개되지 않아 순환 탐색
	for (size_t i = 0; i < m_vecMyActionTransitions.size(); i++)
	{
		// 키 체크
		if (nVirtualKey == m_vecMyActionTransitions[i].Condition.nKey)
		{
			if ( nKeyState == m_vecMyActionTransitions[i].Condition.nKeyState)
			{
				WEAPON_TYPE nWeaponType = gvar.MyInfo.EquipmentSlot.GetCurrWeaponType();

				// 무기 체크
				if (m_vecMyActionTransitions[i].Condition.nWeaponState & WeaponTypeToKeyCondWC(nWeaponType))
				{
					// 자세 체크
					if ( ((nCurrStance == CS_NORMAL) && (m_vecMyActionTransitions[i].Condition.nStances & _STANCE_NORMAL)) ||
						((nCurrStance == CS_BATTLE) && (m_vecMyActionTransitions[i].Condition.nStances & _STANCE_BATTLE)) )
					{
						return m_vecMyActionTransitions[i].nNextState;
					}
				}
			}
		}
	}

	return ACTION_STATE_NONE;
}

void XKeyConditionChecker::AddCondition( XMyActionTransition& transition )
{
	m_vecMyActionTransitions.push_back(transition);
}