#include "stdafx.h"
#include "XBuffAttribute.h"
#include "XGlobal.h"
#include "XTalentInfo.h"
#include "XTalentInfoMgr.h"

void XBuffAttribute::ParseBuffAttr( const XBuffInfo* pBuffInfo, int nStackCount /*= 1*/ )
{
	switch ( pBuffInfo->m_nPassiveExtraAttrib)
	{
	case BUFPEA_DISABLE_TALENTS :		bDisabledTalent = true;		break;
	case BUFPEA_DISABLE_TALENT_MELEE :	bDisabledMelee = true;		break;
	case BUFPEA_DISABLE_TALENT_RANGE :	bDisabledRange = true;		break;
	case BUFPEA_DISABLE_TALENT_MAGIC :	bDisabledMagic = true;		break;
	case BUFPEA_INVINCIBILITY :			bInvincibled = true;		break;
	case BUFPEA_ROOT :					bRooted = true;				break;
	case BUFPEA_SLEEP :					bSleeped = true;			break;
	}

	switch ( pBuffInfo->m_nPassiveExtraAttrib2)
	{
	case BUFPEA_DISABLE_TALENTS :		bDisabledTalent = true;		break;
	case BUFPEA_DISABLE_TALENT_MELEE :	bDisabledMelee = true;		break;
	case BUFPEA_DISABLE_TALENT_RANGE :	bDisabledRange = true;		break;
	case BUFPEA_DISABLE_TALENT_MAGIC :	bDisabledMagic = true;		break;
	case BUFPEA_INVINCIBILITY :			bInvincibled = true;		break;
	case BUFPEA_ROOT :					bRooted = true;				break;
	case BUFPEA_SLEEP :					bSleeped = true;			break;
	}

	if (pBuffInfo->m_bUseChangeEquipmentSlot == true)		bTotem = true;

	if (pBuffInfo->IsRideBuff())
	{
		m_bMounted = true;
	}

	m_fMoveSpeedRatio += pBuffInfo->m_ActorModifier.fMoveSpeed * nStackCount;

	// 이동 스피드는 0.1 이하가 될수 없다. (0.1 이하가 되면 버그가 생길 수 있다.)
	if(m_fMoveSpeedRatio < MIN_MOVE_SPEED)
		m_fMoveSpeedRatio = MIN_MOVE_SPEED;
}

bool XBuffAttribute::IsUsableTalent( XTalentInfo* pTalentInfo)
{
	if ( pTalentInfo == NULL)
		return false;


	if ( m_fMoveSpeedRatio - TALENT_REQUIRED_MOVE_TRUNCATED < pTalentInfo->m_fRequireMoveSpeed)		// 속도가 탤런트 요구 속도에 미치지 못하는 사용을 못한다.
		return false;

	// 슬립이나 루팅상태에서도 탤런트 사용 가능
	if(IsMesmerizable())
	{
		if (pTalentInfo->m_bIgnoreMesmerize)
			return true;

		return false;
	}

	switch ( pTalentInfo->m_nCategory)
	{
	case TC_COMMON :
		if ( IsDisabledTalent()  &&  pTalentInfo->m_bIgnoreMesmerize == false)
			return false;
		break;

	case TC_MELEE :
		if ( IsDisabledMelee()  &&  pTalentInfo->m_bIgnoreMesmerize == false)
			return false;
		break;

	case TC_RANGE :
		if ( IsDisabledRange()  &&  pTalentInfo->m_bIgnoreMesmerize == false)
			return false;
		break;

	case TC_MAGIC :
		if ( IsDisabledMagic()  &&  pTalentInfo->m_bIgnoreMesmerize == false)
			return false;
		break;
	}

	return true;
}

bool XBuffAttribute::IsUsableTalent( int nTalentID)
{
	if ( nTalentID == 0)
		return false;

	if (info.talent == NULL) 
		return false;

	XTalentInfo* pTalentInfo = info.talent->Get( nTalentID);

	if ( pTalentInfo == NULL)
		return false;

	return IsUsableTalent( pTalentInfo);
}
