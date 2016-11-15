#include "stdafx.h"
#include "VNpcDataValidator.h"

VNpcDataValidator::VNpcDataValidator()
:VDataValidator()
{
	vector<string> vecBeten;
	vecBeten.push_back("MF_BT1");
	vecBeten.push_back("MF_BT2");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_BEATEN, vecBeten));

	vector<string> vecKnockBack;
	vecKnockBack.push_back("MF_KB1");
	vecKnockBack.push_back("MF_KB2");
	vecKnockBack.push_back("MF_KB3");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_KNOCKBACK, vecKnockBack));

	//vector<string> vecStun;
	//vecStun.push_back("MF_BT1");
	//vecStun.push_back("sleep");
	//m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_STUN, vecStun));

	vector<string> vecDeflect;
	vecDeflect.push_back("MF_DT");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_DEFLECT, vecDeflect));

	vector<string> vecThrowup;
	vecThrowup.push_back("MF_TU1");
	vecThrowup.push_back("MF_TU2");
	vecThrowup.push_back("MF_KD3");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_THROWUP, vecThrowup));

	vector<string> vecKnockDown;
	vecKnockDown.push_back("MF_KD1");
	vecKnockDown.push_back("MF_KD2");
	vecKnockDown.push_back("MF_KD3");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_KNOCKDOWN, vecKnockDown));

	vector<string> vecUppered;
	vecUppered.push_back("MF_KB1");
	m_mapMotionFactorAnimation.insert(map<MF_STATE, MF_ANIMATION>::value_type(MF_UPPERED, vecUppered));
}

VNpcDataValidator::~VNpcDataValidator()
{

}

void VNpcDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nNpcCount = vInfoMgr->m_pNpcInfoMgr->size();
}

void VNpcDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	for(map<int, CSNPCInfo*>::iterator it = vInfoMgr->m_pNpcInfoMgr->begin(); it != vInfoMgr->m_pNpcInfoMgr->end(); ++it)
	{
		XNPCInfo* pNpcInfo = vInfoMgr->m_pNpcInfoMgr->Get(it->second->nID);

		CheckNpcInfo(pNpcInfo, vInfoMgr);
	}
}

void VNpcDataValidator::CheckNpcInfo( XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr )
{
	if (!pNpcInfo->bValidation) return;

	string strNpcError = (string)pNpcInfo->GetName() + "(" + TransToStringFromNumber(pNpcInfo->nID) + ") : ";

	CheckMesh(pNpcInfo, vInfoMgr, strNpcError);

	CheckMotionFactor(pNpcInfo, vInfoMgr, strNpcError);
}

void VNpcDataValidator::CheckMotionFactor( XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr, string strNpcError )
{
	if(pNpcInfo->nInitAttackable == NAT_NONE)			// 공격 안하거나
		return;											// 모션팩터 체크 안한다.

	for(int i = 0; i < MF_VALUABLE_SIZE; ++i)
	{
		// 1. 모션 팩터가 걸리는데 모션 팩터 애니메이션이 없으면 안된다.
		if(pNpcInfo->MFModifier.Value[i].fPer > 0 && pNpcInfo->MFModifier.Weight[i].fPer > 0)
		{
			// 애니메이션 검사
			VMeshInfo* pMeshInfo = vInfoMgr->m_pMeshInfoMgr->GetMeshInfo(pNpcInfo->strMeshName);
			if(pMeshInfo == NULL)
				continue;

			MF_ANIMATION * pMotion = GetMotionFactorAnimationName((MF_STATE)i);
			if(pMotion == NULL)
				continue;

			for(vector<string>::iterator itAni = pMotion->begin(); itAni != pMotion->end(); ++itAni)
			{
				string strAni = pNpcInfo->strAniPrefix + *itAni;
				if(pMeshInfo->m_AnimationInfoMgr.m_AnimationList.GetAnimationInfo(strAni) == NULL)
				{
					string strError = strNpcError + "모션펙터(" + MF_STATE_NAME[i] + ") " + strAni + " 애니메이션이 없습니다.";
					m_NpcMeshValidatorError.Add(EMT_LINK, strError);
				}
			}
		}
	}
}

void VNpcDataValidator::CheckMesh( XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr, string strNpcError )
{
	VMeshInfo* pMeshInfo = vInfoMgr->m_pMeshInfoMgr->GetMeshInfo(pNpcInfo->strMeshName);
	if(pMeshInfo == NULL)
	{
		string strError = strNpcError + pNpcInfo->strMeshName + " 메쉬 이름을 가진 파일이 없습니다.";
		m_NpcMeshValidatorError.Add(EMT_NAME, strError);
		return;
	}

	string strNPCFileName = V_PATH_MODEL_TOOL + pNpcInfo->strMeshPath + pNpcInfo->strMeshName + ".elu";
	string strMeshFileName = pMeshInfo->m_strPathName + pMeshInfo->m_strMeshFileName;

	_strlwr((char*)strNPCFileName.c_str());
	_strlwr((char*)strMeshFileName.c_str());

	if(CheckBackSlash(strNPCFileName))
	{
		string strError = strNpcError + "역슬레쉬(\\)를 슬레쉬(/)로 변경해 주세요.";
		m_NpcMeshValidatorError.Add(EMT_STRING, strError);
	}
	else if(strNPCFileName != strMeshFileName)
	{
		string strError = strNpcError + strNPCFileName + " 파일이 없습니다.";
		m_NpcMeshValidatorError.Add(EMT_FILE, strError);
	}
}

VNpcDataValidator::MF_ANIMATION * VNpcDataValidator::GetMotionFactorAnimationName( MF_STATE eMF )
{
	map<MF_STATE, MF_ANIMATION>::iterator it = m_mapMotionFactorAnimation.find(eMF);
	if(it != m_mapMotionFactorAnimation.end())
		return &(it->second);

	return NULL;
}

bool VNpcDataValidator::CheckBackSlash( string strPath )
{
	return VFileName::stristr(strPath, "\\");
}