#include "stdafx.h"
#include "TTalentEffectViewer.h"
#include "Xmlseral.h"
#include "TTalentEffectMgr.h"

//////////////////////////////////////////////////////////////////////////
TTalentEffectViewer::TTalentEffectViewer()
{
	m_pEffectMgr								= NULL;

	// 탤런트 이펙트 함수 포인터
	// 캐스트
	TalentEffectFuncToProperty[0]				= &TTalentEffectViewer::SetTalentEffectCastToProperty;
	TalentEffectFuncToTalentEffectInfo[0]		= &TTalentEffectViewer::SetPropertyToTalentEffectCast;

	// 사용
	TalentEffectFuncToProperty[1]				= &TTalentEffectViewer::SetTalentEffectUseToProperty;
	TalentEffectFuncToTalentEffectInfo[1]		= &TTalentEffectViewer::SetPropertyToTalentEffectUse;

	// 피격
	TalentEffectFuncToProperty[2]				= &TTalentEffectViewer::SetTalentEffectAttackToProperty;
	TalentEffectFuncToTalentEffectInfo[2]		= &TTalentEffectViewer::SetPropertyToTalentEffectAttack;

	// 실패
	TalentEffectFuncToProperty[3]				= &TTalentEffectViewer::SetTalentEffectMissToProperty;
	TalentEffectFuncToTalentEffectInfo[3]		= &TTalentEffectViewer::SetPropertyToTalentEffectMiss;

	// 충격
	TalentEffectFuncToProperty[4]				= &TTalentEffectViewer::SetTalentEffectShockToProperty;
	TalentEffectFuncToTalentEffectInfo[4]		= &TTalentEffectViewer::SetPropertyToTalentEffectShock;

	// 지속
	TalentEffectFuncToProperty[5]				= &TTalentEffectViewer::SetTalentEffectDurationToProperty;
	TalentEffectFuncToTalentEffectInfo[5]		= &TTalentEffectViewer::SetPropertyToTalentEffectDuration;

	// 방어
	TalentEffectFuncToProperty[6]				= &TTalentEffectViewer::SetTalentEffectDefenseToProperty;
	TalentEffectFuncToTalentEffectInfo[6]		= &TTalentEffectViewer::SetPropertyToTalentEffectDefense;

	// 캐스트 종료
	TalentEffectFuncToProperty[7]				= &TTalentEffectViewer::SetTalentEffectCastEndToProperty;
	TalentEffectFuncToTalentEffectInfo[7]		= &TTalentEffectViewer::SetPropertyToTalentEffectCastEnd;

}

PropertyGrid^ TTalentEffectViewer::GetEffectViewerPropertGrid()
{
	return GlobalObjects::g_pMainForm->TalentEffectAttGrid;
}

void TTalentEffectViewer::SetTextEffectType( string& strBuffer, TALENT_EFFECT_TYPE nEffectType )
{
	if(nEffectType == CAST_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_CAST_EFFECT;
	}
	else if(nEffectType == USE_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_USE_EFFECT;
	}
	else if(nEffectType == ATTACK_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_ATTACK_EFFECT;
	}
	else if(nEffectType == MISS_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_MISS_EFFECT;
	}
	else if(nEffectType == SHOCK_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_SHOCK_EFFECT;
	}
	else if(nEffectType == DURATION_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_DURATION_EFFECT;
	}
	else if(nEffectType == DEFENSE_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_DEFENSE_EFFECT;
	}
	else if(nEffectType == CAST_END_EFFECT)
	{
		strBuffer = EFFECTYPE_STRING_CAST_END_EFFECT;
	}
	else
	{
		// 모름.
		strBuffer = EFFECTYPE_STRING_NOT;
	}
}

void TTalentEffectViewer::SetTextImpactPositionList( string& strBuffer, IMPACTPOS nImapctPos )
{
	if(nImapctPos == HIT_POS)
	{
		strBuffer = IMPACTPOSITION_STRING_HIT_POS;
	}
	else if(nImapctPos == BONE_POS)
	{
		strBuffer = IMPACTPOSITION_STRING_BONE_POS;
	}
	else if(nImapctPos == SHOCK_GROUND_POS)
	{
		strBuffer = IMPACTPOSITION_STRING_SHOCK_GROUND_POS;
	}
	else if(nImapctPos == TALENT_HIT_CAPSULE_POS)
	{
		strBuffer = IMPACTPOSITION_STRING_TALENT_HIT_CAPSUL_POS;
	}
	else //if(nImapctPos == MODEL_POS)
	{
		strBuffer = IMPACTPOSITION_STRING_MODEL_POS;
	}
}

TALENT_EFFECT_TYPE TTalentEffectViewer::GetEffectType( string& strBuffer )
{
	TALENT_EFFECT_TYPE eType = CAST_EFFECT;				// 기본값

	if(strBuffer == EFFECTYPE_STRING_CAST_EFFECT)
	{
		eType = CAST_EFFECT;	
	}
	else if(strBuffer == EFFECTYPE_STRING_USE_EFFECT)
	{
		eType = USE_EFFECT;
	}
	else if(strBuffer == EFFECTYPE_STRING_ATTACK_EFFECT)
	{
		eType = ATTACK_EFFECT;
	}
	else if(strBuffer == EFFECTYPE_STRING_MISS_EFFECT)
	{
		eType = MISS_EFFECT;
	}
	else if(strBuffer == EFFECTYPE_STRING_SHOCK_EFFECT)
	{
		eType = SHOCK_EFFECT;
	}
	else if(strBuffer ==  EFFECTYPE_STRING_DURATION_EFFECT)
	{
		eType = DURATION_EFFECT;
	}
	else if(strBuffer == EFFECTYPE_STRING_DEFENSE_EFFECT)
	{
		eType = DEFENSE_EFFECT;
	}
	else if(strBuffer == EFFECTYPE_STRING_CAST_END_EFFECT)
	{
		eType = CAST_END_EFFECT;
	}

	return eType;
}

IMPACTPOS TTalentEffectViewer::GetEffectImpactPositionList( string& strBuffer )
{
	IMPACTPOS eImpactPos = MODEL_POS;

	if(strBuffer == IMPACTPOSITION_STRING_MODEL_POS)
	{
		eImpactPos = MODEL_POS;	
	}
	else if(strBuffer == IMPACTPOSITION_STRING_HIT_POS)
	{
		eImpactPos = HIT_POS;
	}
	else if(strBuffer == IMPACTPOSITION_STRING_BONE_POS)
	{
		eImpactPos = BONE_POS;
	}	
	else if(strBuffer == IMPACTPOSITION_STRING_SHOCK_GROUND_POS)
	{
		eImpactPos = SHOCK_GROUND_POS;
	}
	else if(strBuffer == IMPACTPOSITION_STRING_TALENT_HIT_CAPSUL_POS)
	{
		eImpactPos = TALENT_HIT_CAPSULE_POS;
	}

	return eImpactPos;
}

void TTalentEffectViewer::SetEffectInfoFromPropertyGrid(XTalentEffectInfo* pEffectInfo)
{
	// 이펙트 정보
	if(pEffectInfo == NULL)
	{
		m_eTalentEffectType = TALENT_EFFECT_TYPE_MAX;
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = nullptr;
		return;
	}

	// 프로퍼티에게 셋팅
	int nEffectTypeIndex = 0;
	XmlSerializable::TALENTEFFECT^ xeffect	= (XmlSerializable::TALENTEFFECT^)(GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject);

	// 프로퍼티에서 타입을  가져온다.
	nEffectTypeIndex = GetEffectType((string)MStringToCharPointer(xeffect->EffectType));

	if(nEffectTypeIndex > 7)
		return;

	// 이펙트 정보에서 타입을 가져온다.
	m_eTalentEffectType = pEffectInfo->m_nEffectType;
	int nEffectTypeFromInfo		= (int)pEffectInfo->m_nEffectType;

	// 타입이 다른지 비교
	if(nEffectTypeFromInfo != nEffectTypeIndex)
	{
		// 다르다
		// 우선 다뀐 타입으로 새로 셋팅 한다.
		pEffectInfo->m_nEffectType	= (TALENT_EFFECT_TYPE)nEffectTypeIndex;
		(this->*TalentEffectFuncToProperty[nEffectTypeIndex])(pEffectInfo);

		pEffectInfo->Init();
	}

	(this->*TalentEffectFuncToTalentEffectInfo[nEffectTypeIndex])(pEffectInfo);
}

// 캐스팅
bool TTalentEffectViewer::SetTalentEffectCastToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	// 탤런트 이펙트 정보를 프로퍼티에게 셋팅
	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEFFECT^ xeffect = gcnew XmlSerializable::TALENTEFFECT;
	string strBuff;

	xeffect->TalentEffectInfoName = gcnew String(pTalentEffectInfo->m_strTalentEffectInfoName.c_str());

	SetTextEffectType(strBuff, pTalentEffectInfo->m_nEffectType);
	xeffect->EffectType		= gcnew String(strBuff.c_str());
	xeffect->EffectName		= gcnew String(pTalentEffectInfo->m_strEffectName.c_str());
	xeffect->본이름			= gcnew String(pTalentEffectInfo->m_strBoneName.c_str());
	xeffect->Parameter		= gcnew String(pTalentEffectInfo->m_strParameter.c_str());

	// 방향
	SetTextEffectDirectionList(strBuff, pTalentEffectInfo->m_nEffectDirection);
	xeffect->방향			= gcnew String(strBuff.c_str());

	//데이타 바인딩 ?
	GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = xeffect;

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectCast( XTalentEffectInfo * pTalentEffectInfo )
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅
	XmlSerializable::TALENTEFFECT^ xeffect	= (XmlSerializable::TALENTEFFECT^)(GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject);
	if(xeffect == nullptr)
		return false;

	pTalentEffectInfo->m_strTalentEffectInfoName = string(MStringToCharPointer(xeffect->TalentEffectInfoName));
	pTalentEffectInfo->m_nEffectType		= GetEffectType((string)MStringToCharPointer(xeffect->EffectType));

	pTalentEffectInfo->m_strEffectName		= string(MStringToCharPointer(xeffect->EffectName));
	pTalentEffectInfo->m_strBoneName		= string(MStringToCharPointer(xeffect->본이름));
	pTalentEffectInfo->m_strParameter		= string(MStringToCharPointer(xeffect->Parameter));

	// 방향
	pTalentEffectInfo->m_nEffectDirection =  GetEffectDirectionList((string)MStringToCharPointer(xeffect->방향));

	return true;
}

// 사용
bool TTalentEffectViewer::SetTalentEffectUseToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	SetTalentEffectAttackToProperty(pTalentEffectInfo);
	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectUse( XTalentEffectInfo * pTalentEffectInfo )
{
	SetPropertyToTalentEffectAttack(pTalentEffectInfo);
	return true;
}

// 피격
bool TTalentEffectViewer::SetTalentEffectAttackToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	// 탤런트 이펙트 정보를 프로퍼티에게 셋팅
	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEFFECT_EX^ xeffect = gcnew XmlSerializable::TALENTEFFECT_EX;
	string strBuff;

	xeffect->TalentEffectInfoName = gcnew String(pTalentEffectInfo->m_strTalentEffectInfoName.c_str());

	SetTextEffectType(strBuff, pTalentEffectInfo->m_nEffectType);
	xeffect->EffectType		= gcnew String(strBuff.c_str());
	xeffect->EffectName		= gcnew String(pTalentEffectInfo->m_strEffectName.c_str());
	xeffect->본이름			= gcnew String(pTalentEffectInfo->m_strBoneName.c_str());
	xeffect->Parameter		= gcnew String(pTalentEffectInfo->m_strParameter.c_str());

	SetTextImpactPositionList(strBuff, (IMPACTPOS)pTalentEffectInfo->m_nImpactPos);
	xeffect->충돌위치사용	= gcnew String(strBuff.c_str());

	// 방향
	SetTextEffectDirectionList(strBuff, pTalentEffectInfo->m_nEffectDirection);
	xeffect->방향			= gcnew String(strBuff.c_str());

	//데이타 바인딩 ?
	GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = xeffect;

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectAttack( XTalentEffectInfo * pTalentEffectInfo )
{
	XmlSerializable::TALENTEFFECT_EX^ xeffect	= (XmlSerializable::TALENTEFFECT_EX^)(GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject);
	if(xeffect == nullptr)
		return false;

	string strBuff;

	pTalentEffectInfo->m_strTalentEffectInfoName = string(MStringToCharPointer(xeffect->TalentEffectInfoName));
	pTalentEffectInfo->m_nEffectType		= GetEffectType((string)MStringToCharPointer(xeffect->EffectType));

	pTalentEffectInfo->m_strEffectName		= string(MStringToCharPointer(xeffect->EffectName));
	pTalentEffectInfo->m_strBoneName		= string(MStringToCharPointer(xeffect->본이름));
	pTalentEffectInfo->m_strParameter		= string(MStringToCharPointer(xeffect->Parameter));

	pTalentEffectInfo->m_nImpactPos			= GetEffectImpactPositionList((string)MStringToCharPointer(xeffect->충돌위치사용));

	// 이펙트 타입이 피지, 충격이라면...
	if(	pTalentEffectInfo->m_nEffectType == SHOCK_EFFECT)
	{
		// 충돌 위치를 지형 충돌로 변경한다.
		pTalentEffectInfo->m_nImpactPos = SHOCK_GROUND_POS;
		SetTextImpactPositionList(strBuff, (IMPACTPOS)pTalentEffectInfo->m_nImpactPos);
		xeffect->충돌위치사용	= gcnew String(strBuff.c_str());
	}

	//if( pTalentEffectInfo->m_nImpactPos == HIT_POS && pTalentEffectInfo->m_strBoneName.empty())
	//{
	//	pTalentEffectInfo->m_strBoneName = "Bip01";
	//	SetTalentEffectAttackToProperty(pTalentEffectInfo);
	//}

	// 방향
	pTalentEffectInfo->m_nEffectDirection =  GetEffectDirectionList((string)MStringToCharPointer(xeffect->방향));

	return true;
}

// 실패
bool TTalentEffectViewer::SetTalentEffectMissToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	SetTalentEffectAttackToProperty(pTalentEffectInfo);

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectMiss( XTalentEffectInfo * pTalentEffectInfo )
{
	SetPropertyToTalentEffectAttack(pTalentEffectInfo);

	return true;
}

// 충격
bool TTalentEffectViewer::SetTalentEffectShockToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	// 탤런트 이펙트 정보를 프로퍼티에게 셋팅
	SetTalentEffectAttackToProperty(pTalentEffectInfo);

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectShock( XTalentEffectInfo * pTalentEffectInfo )
{
	SetPropertyToTalentEffectAttack(pTalentEffectInfo);

	return true;
}

// 지속
bool TTalentEffectViewer::SetTalentEffectDurationToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	SetTalentEffectCastToProperty(pTalentEffectInfo);

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectDuration( XTalentEffectInfo * pTalentEffectInfo )
{
	SetPropertyToTalentEffectCast(pTalentEffectInfo);

	return true;
}

// 방어
bool TTalentEffectViewer::SetTalentEffectDefenseToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	SetTalentEffectAttackToProperty(pTalentEffectInfo);

	return true;
}

bool TTalentEffectViewer::SetPropertyToTalentEffectDefense( XTalentEffectInfo * pTalentEffectInfo )
{
	SetPropertyToTalentEffectAttack(pTalentEffectInfo);

	return true;
}

void TTalentEffectViewer::SetPropertyGridFromEffectInfo( XTalentEffectInfo* pEffectInfo )
{
	// 이펙트 정보
	if(pEffectInfo == NULL)
	{
		m_eTalentEffectType = TALENT_EFFECT_TYPE_MAX;
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = nullptr;
		return;
	}

	int nEffectTypeIndex = 0;

	// 이펙트 정보에서 타입을 가져온다.
	m_eTalentEffectType = pEffectInfo->m_nEffectType;
	nEffectTypeIndex = (int)pEffectInfo->m_nEffectType;
	if(nEffectTypeIndex > 7)
	{
		GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = nullptr;
		return;
	}

	(this->*TalentEffectFuncToProperty[nEffectTypeIndex])(pEffectInfo);

}

void TTalentEffectViewer::SetHitEffectPosDirInfoFromPropertyGrid( XTalentHitEffectPosDir* pHitEffectPosDirInfo )
{
	XmlSerializable::TALENTEFFECT_HITEFFECT_POSDIR^ xeffect	= (XmlSerializable::TALENTEFFECT_HITEFFECT_POSDIR^)(GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject);
	if(xeffect == nullptr)
		return;

	pHitEffectPosDirInfo->vHitEffectPos.x	= xeffect->X;
	pHitEffectPosDirInfo->vHitEffectPos.y	= xeffect->Y;
	pHitEffectPosDirInfo->vHitEffectPos.z	= xeffect->Z;

	return;
}

void TTalentEffectViewer::SetPropertyGridFromHitEffectPosDirInfo( XTalentHitEffectPosDir* pHitEffectPosDirInfo )
{
	// 탤런트 이펙트 정보를 프로퍼티에게 셋팅
	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEFFECT_HITEFFECT_POSDIR^ xeffect = gcnew XmlSerializable::TALENTEFFECT_HITEFFECT_POSDIR;

	xeffect->Init_Dir(pHitEffectPosDirInfo->vSwordTrailHitUp.x, pHitEffectPosDirInfo->vSwordTrailHitUp.y, pHitEffectPosDirInfo->vSwordTrailHitUp.z);
	xeffect->X				= pHitEffectPosDirInfo->vHitEffectPos.x;
	xeffect->Y				= pHitEffectPosDirInfo->vHitEffectPos.y;
	xeffect->Z				= pHitEffectPosDirInfo->vHitEffectPos.z;

	//데이타 바인딩 ?
	GlobalObjects::g_pMainForm->TalentEffectAttGrid->SelectedObject = xeffect;
}

bool TTalentEffectViewer::SetTalentEffectCastEndToProperty( XTalentEffectInfo * pTalentEffectInfo )
{
	return SetTalentEffectCastToProperty(pTalentEffectInfo);
}

bool TTalentEffectViewer::SetPropertyToTalentEffectCastEnd( XTalentEffectInfo * pTalentEffectInfo )
{
	return SetPropertyToTalentEffectCast(pTalentEffectInfo);
}