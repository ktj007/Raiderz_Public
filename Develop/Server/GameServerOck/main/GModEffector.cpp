#include "StdAfx.h"
#include "GModEffector.h"
#include "GEntityPlayer.h"

void GModEffector::ApplyInstantModifier(GEntityActor* pTarget, CSInstantModifier& Modifier)
{
	PFI_BLOCK_THISFUNC(9021);

	if (!Modifier.IsModified())
		return;	// 수정사항 없음	

	CSInstantModifier& ChrModifier = pTarget->GetChrStatus()->InstantModifier;
	ChrModifier += Modifier;

	int nHP = pTarget->GetHP();
	int nEN = pTarget->GetEN();
	int nSTA = pTarget->GetSTA();
	ChrModifier.nHP.Modify(nHP);
	ChrModifier.nEN.Modify(nEN);	
	ChrModifier.nSTA.Modify(nSTA);

	pTarget->SetHP(nHP);
	pTarget->SetEN(nEN);
	pTarget->SetSTA(nSTA);

	ChrModifier.Clear();
}

void GModEffector::RefreshStatus(GEntityActor* pTarget)
{
	if (pTarget->GetHP() > pTarget->GetMaxHP())
	{
		pTarget->SetHP(pTarget->GetMaxHP());
	}

	if (pTarget->GetEN() > pTarget->GetMaxEN())
	{
		pTarget->SetEN(pTarget->GetMaxEN());
	}

	if (pTarget->GetSTA() > pTarget->GetMaxSTA())
	{
		pTarget->SetSTA(pTarget->GetMaxSTA());
	}
}
