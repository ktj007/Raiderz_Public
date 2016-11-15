#include "stdafx.h"
#include "XCombo.h"
#include "XMyActionNormalAttack.h"

void XCombo::Reserve( TALENT_NORMAL_ATTACK_TYPE nNextAttackType)
{
	if ( m_bEnable == false)
		return;

	m_nNextAttackType = nNextAttackType;
	m_bComboReserved = true;
	m_nReservedTime = XGetNowTime();
}

void XCombo::ReleaseReserved()
{
	m_nNextAttackType = NA_NA;
	m_bComboReserved = false;
	m_nReservedTime = 0;
}

void XCombo::Init()
{
	m_nNextAttackType = NA_NA;
	m_bComboReserved = false;
	m_nCurrentCombo = 0;
	m_nReservedTime = 0;
}

bool XCombo::CheckReservedTime()
{
	if (IsReserved() == false) return false;

	// 예약된 액션은 최초 입력 시간으로부터 일정 시간까지만 유효하게 함
	if ( (XGetNowTime() - m_nReservedTime) > 500)
	{
		ReleaseReserved();
		return false;
	}

	return true;
}

void XCombo::OnAttack( TALENT_NORMAL_ATTACK_TYPE nAttackType )
{
	ReleaseReserved();

	if (nAttackType >= NA_NORMAL1 && nAttackType <= NA_NORMAL6)
	{
		m_nCurrentCombo++;
	}
	else if ((nAttackType == NA_SWORD_DRAWING) ||
		(nAttackType == NA_ADVANCE) )
	{
		m_nCurrentCombo = 1;
	}

}

void XCombo::SetEnable( bool bEnable)
{
	if ( bEnable == false  &&  m_bEnable != bEnable)
		Init();

	m_bEnable = bEnable;
}

TALENT_NORMAL_ATTACK_TYPE XCombo::GetNextNormalAttack()
{
	int nCombo = m_nCurrentCombo;

	if (nCombo <= 0) return NA_NORMAL1;
	else if (nCombo == 1) return NA_NORMAL2;
	else if (nCombo == 2) return NA_NORMAL3;
	else if (nCombo == 3) return NA_NORMAL4;
	else if (nCombo == 4) return NA_NORMAL5;
	else if (nCombo == 5) return NA_NORMAL6;
	return NA_NORMAL1;
}

bool XCombo::CheckCanReserveCombo()
{
	if ((m_nCurrentCombo < 0) || (m_nCurrentCombo >= MAX_ATTACK_COMBO)) return false;

	if(m_bComboReserved && CheckReservedTime() == false)
		return false;

	return true;
}

