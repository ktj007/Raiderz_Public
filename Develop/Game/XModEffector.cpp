#include "StdAfx.h"
#include "XModEffector.h"

void XModEffector::ApplyInstantModidier(CSInstantModifier& Modifier)
{
	if (!Modifier.IsModified())
		return;	// 수정사항 없음

	int nHP = gvar.MyInfo.GetHP();
	int nEN = gvar.MyInfo.GetEN();
	int nSTA = gvar.MyInfo.GetSTA();

	CSInstantModifier& ChrModifier = gvar.MyInfo.ChrStatus.InstantModifier;
	ChrModifier += Modifier;

	ChrModifier.nHP.Modify(nHP);	
	ChrModifier.nEN.Modify(nEN);	
	ChrModifier.nSTA.Modify(nSTA);	

	gvar.MyInfo.SetHP(nHP);
	gvar.MyInfo.SetEN(nEN);
	gvar.MyInfo.SetSTA(nSTA);

	ChrModifier.nHP.Clear();
	ChrModifier.nEN.Clear();
	ChrModifier.nSTA.Clear();
}

void XModEffector::RefreshStatus()
{
	int nHP = gvar.MyInfo.GetHP();
	int nEN = gvar.MyInfo.GetEN();
	int nSTA = gvar.MyInfo.GetSTA();

	gvar.MyInfo.SetHP(nHP);
	gvar.MyInfo.SetEN(nEN);
	gvar.MyInfo.SetSTA(nSTA);
}
