#include "stdafx.h"
#include "VNpcInfoMgr.h"
#include "CSCommonParser.h"

#define NPC_XML_ATTR_ATTACKABLE		"Attackable"

VNpcInfoMgr::VNpcInfoMgr()
{

}

VNpcInfoMgr::~VNpcInfoMgr()
{

}

void VNpcInfoMgr::ParseNPCInfo( MXmlElement* pElement, MXml* pXml )
{
	ParseNPC(pElement, pXml);

	// Modifier Parse
	int nID;
	_Attribute(&nID,						pElement, NPC_XML_ATTR_ID);

	XNPCInfo* pNpcInfo = Get(nID);
	if(pNpcInfo == NULL)
		return;

	_Attribute(&pNpcInfo->MFModifier.Value[MF_BEATEN].fPer,			pElement, NPC_XML_ATTR_BEATEN_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_KNOCKBACK].fPer,		pElement, NPC_XML_ATTR_KNOCKBACK_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_STUN].fPer,			pElement, NPC_XML_ATTR_STUN_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_DEFLECT].fPer,		pElement, NPC_XML_ATTR_DEFLECT_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_THROWUP].fPer,		pElement, NPC_XML_ATTR_THROWUP_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_KNOCKDOWN].fPer,		pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE);
	_Attribute(&pNpcInfo->MFModifier.Value[MF_UPPERED].fPer,		pElement, NPC_XML_ATTR_UPPERED_MOD_VALUE);

	_Attribute(&pNpcInfo->MFModifier.Weight[MF_BEATEN].fPer,		pElement, NPC_XML_ATTR_BEATEN_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_KNOCKBACK].fPer,		pElement, NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_STUN].fPer,			pElement, NPC_XML_ATTR_STUN_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_DEFLECT].fPer,		pElement, NPC_XML_ATTR_DEFLECT_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_THROWUP].fPer,		pElement, NPC_XML_ATTR_THROWUP_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_KNOCKDOWN].fPer,		pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT);
	_Attribute(&pNpcInfo->MFModifier.Weight[MF_UPPERED].fPer,		pElement, NPC_XML_ATTR_UPPERED_MOD_WEIGHT);

	string strValue;
	if (_Attribute(strValue,		pElement, NPC_XML_ATTR_ATTACKABLE, pXml))
	{
		if(strValue == "NONE")
			pNpcInfo->nInitAttackable = NAT_NONE;
		else if(strValue == "ALWAYS")
			pNpcInfo->nInitAttackable = NAT_ALWAYS;
		else if(strValue == "FACTION")
			pNpcInfo->nInitAttackable = NAT_FACTION;
	}

	_Attribute(&pNpcInfo->bValidation,			pElement, "validation");
}
