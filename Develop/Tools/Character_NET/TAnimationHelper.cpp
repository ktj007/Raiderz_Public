#include "stdafx.h"
#include "TAnimationHelper.h"
#include "TAnimationResourceController.h"
#include "TPlayMotion.h"
#include "MXml.h"
#include "XMotionMgr.h"
#include "TNaming.h"

TAnimationHelper::TAnimationHelper()
{

}

TAnimationHelper::~TAnimationHelper()
{

}

TALENT_ANIMATION_INFO TAnimationHelper::GetTalentAnimationInfo( CSTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eType )
{
	string strAnimationName = pTalentInfo->m_szUseAnimation;
	
	if(eType == TAT_CASTING)
		strAnimationName = pTalentInfo->m_szCastingAnimation;
	else if(eType == TAT_CASTINGLOOP)
		strAnimationName = pTalentInfo->m_szCastingLoopAnimation;
	else if(eType == TAT_USE)
		strAnimationName = pTalentInfo->m_szUseAnimation;
	else if(eType == TAT_EXTRA)
		strAnimationName = pTalentInfo->m_szExtraActAnimation;
	else if(eType == TAT_EXTRA2)
		strAnimationName = pTalentInfo->m_szExtraActAnimation2;
	else if(eType == TAT_EXTRA3)
		strAnimationName = pTalentInfo->m_szExtraActAnimation3;

	TALENT_ANIMATION_INFO talentAniInfo;

	if(strAnimationName.empty())
		return talentAniInfo;

	if(XMotionMgr::IsTalentMotionName(strAnimationName.c_str()) ||
		g_pMainApp->GetMotion()->GetMotionMgr()->GetMotion(strAnimationName.c_str()) != NULL)
	{
		// 모션용이다.
		talentAniInfo = GetMotionAnimationStart(pTalentInfo, strAnimationName);
	}
	else
	{
		// 그냥 애니메이션이다.
		talentAniInfo = GetAnimtionStart(pTalentInfo, strAnimationName);
	}

	return talentAniInfo;
}

TALENT_ANIMATION_INFO TAnimationHelper::GetMotionAnimationStart( CSTalentInfo* pTalentInfo, string& strTalentAnimationName )
{
	TALENT_ANIMATION_INFO talentAniInfo;
	talentAniInfo.eType = TALT_MOTION;
	talentAniInfo.strMotionName = strTalentAnimationName;

	string motion = "";
	string t = strTalentAnimationName;
	int f = t.find(";");
	if (f != string::npos)
	{
		motion		= t.substr(0, f);
	}
	else
		motion		= t;

	XMotion* pMotion		= g_pMainApp->GetMotion()->GetMotionMgr()->GetMotion(motion.c_str());
	if(pMotion == NULL)
		return talentAniInfo;

	talentAniInfo.bAutoPrefix = pMotion->IsAutoPrefix();

	// 모델 위치 가져오기
	string strMeshPath;
	string strModelName;
	int NPCID = 0;
	bool bNPC = false;

	int nWeaponTypeMax = (int)WEAPON_NONE;
	bool bWeaponAni = pMotion->IsWeaponAni();
	if(bWeaponAni)
		nWeaponTypeMax = (int)WEAPON_MAX;

	if(pTalentInfo->m_setNPCID.size() > 0)
	{
		for(set<int>::iterator it = pTalentInfo->m_setNPCID.begin(); it != pTalentInfo->m_setNPCID.end(); ++it)
		{
			NPCID = *it;
			bNPC = true;
			nWeaponTypeMax = 1;

			GetMotionAnimationInfo(strMeshPath, strModelName, NPCID, nWeaponTypeMax, pTalentInfo, bNPC, pMotion, talentAniInfo);

			// 있으면 끝
			if(talentAniInfo.vecTalentAniInfo.size() > 0)
				return talentAniInfo;

			// 없으면 다음 NPC로 찾는다.
		}
	}
	else
	{
		GetMotionAnimationInfo(strMeshPath, strModelName, 0, nWeaponTypeMax, pTalentInfo, bNPC, pMotion, talentAniInfo);
	}

	return talentAniInfo;
}

TALENT_ANIMATION_INFO TAnimationHelper::GetAnimtionStart( CSTalentInfo* pTalentInfo, string& strTalentAnimationName )
{
	TALENT_ANIMATION_INFO talentAniInfo;
	talentAniInfo.eType = TALT_ANIMATION;

	// 모델 위치 가져오기
	string strMeshPath;
	string strModelName;
	int NPCID = 0;
	bool bNPC = false;

	int nWeaponTypeMax = (int)WEAPON_MAX;

	if(pTalentInfo->m_setNPCID.size() > 0)
	{
		for(set<int>::iterator it = pTalentInfo->m_setNPCID.begin(); it != pTalentInfo->m_setNPCID.end(); ++it)
		{
			NPCID = *it;
			bNPC = true;
			nWeaponTypeMax = 1;

			GetAnimationInfo(strMeshPath, strModelName, NPCID, nWeaponTypeMax, pTalentInfo, bNPC, strTalentAnimationName, talentAniInfo);

			// 있으면 끝
			if(talentAniInfo.vecTalentAniInfo.size() > 0)
				return talentAniInfo;

			// 없으면 다음 NPC로 찾는다.
		}
	}
	else
	{
		GetAnimationInfo(strMeshPath, strModelName, 0, nWeaponTypeMax, pTalentInfo, bNPC, strTalentAnimationName, talentAniInfo);
	}

	return talentAniInfo;
}

void TAnimationHelper::GetModelPathName( string& strOutMeshPath, string& strOutModelName, int nNPCID /*= 0*/ )
{
	if(nNPCID > 0)
	{
		TNPCInfo* pNPCInfo = g_pMainApp->m_NPC.GetNPCInfo(nNPCID);
		if(pNPCInfo == NULL)
			return;

		strOutMeshPath = PATH_MODEL_TOOL + pNPCInfo->strMeshPath;
		strOutModelName = pNPCInfo->strMeshName;

		return;
	}

	// Player이네...
	// 현재 보여주는 모델 보여주기
	string strModelName = TNaming::GetMeshAliasName(g_pMainApp->GetActor());
	if(strModelName == "")
	{
		// 없으면... 여자로 기본
		strOutMeshPath = PATH_MODEL_TOOL + string("Player/hf/");
		strOutModelName = "hf";
	}
	else
	{
		strOutMeshPath = PATH_MODEL_TOOL + string("Player/") + strModelName + string("/");
		strOutModelName = strModelName;
	}
}

bool TAnimationHelper::GetMotionAnimation( vector<ANIMATION_INFO>& vecOutAniInfo, string strMeshPath, string strModelName, XMotion* pMotion, int nWeaponType )
{
	if(pMotion == NULL)
		return false;

	for(vector<XMotionAniSequence*>::iterator itMotion = pMotion->m_vecAniSequence[MT_DEFAULT].begin(); itMotion != pMotion->m_vecAniSequence[MT_DEFAULT].end(); itMotion++)
	{
		ANIMATION_INFO aniInfo;		
		aniInfo.fAnimationSpeed			= (*itMotion)->GetSpeed();
		aniInfo.strAnimationName		= strModelName;
		aniInfo.nFrameOver				= (*itMotion)->GetFrameOver();

		if(nWeaponType == -1)
			aniInfo.strAnimationName	= (*itMotion)->GetAniName();
		else
			aniInfo.strAnimationName	= (*itMotion)->GetAniName((WEAPON_TYPE)nWeaponType);

		// 애니메이션 정보 가져오기
		SIMPLEANIMATION_INFO stData;
		if(GetAnimationInfo(stData, strMeshPath, strModelName, aniInfo.strAnimationName) == false)
			continue;

		aniInfo.strAnimationPathName	= stData.strAnimationPathName;
		aniInfo.strAnimationFileName	= stData.strAnimationFileName;

		aniInfo.fAnimationSpeed			= stData.fAnimationSpeed;
		if((*itMotion)->GetSpeed() != 1.0f)
			aniInfo.fAnimationSpeed = (*itMotion)->GetSpeed();

		aniInfo.loopType				= stData.loopType;

		vecOutAniInfo.push_back(aniInfo);
	}

	if(vecOutAniInfo.empty())
		return false;

	return true;
}

bool TAnimationHelper::GetAnimation( vector<ANIMATION_INFO>& vecOutAniInfo, string strMeshPath, string strModelName, string strUseAnimation, int nWeaponType )
{
	ANIMATION_INFO aniInfo;

	if(nWeaponType == -1)
		aniInfo.strAnimationName	= strUseAnimation;
	else
		aniInfo.strAnimationName	= string(TStrings::WeaponType(WEAPON_TYPE(nWeaponType))) + string("_") + strUseAnimation;

	// 애니메이션 정보 가져오기
	SIMPLEANIMATION_INFO stData;
	if(GetAnimationInfo(stData, strMeshPath, strModelName, aniInfo.strAnimationName) == false)
		return false;

	aniInfo.strAnimationPathName	= stData.strAnimationPathName;
	aniInfo.strAnimationFileName	= stData.strAnimationFileName;

	aniInfo.fAnimationSpeed			= stData.fAnimationSpeed;

	aniInfo.loopType				= stData.loopType;

	vecOutAniInfo.push_back(aniInfo);

	return true;
}

bool TAnimationHelper::GetAnimationInfo( SIMPLEANIMATION_INFO& stOutData, string strMeshPath, string strModelName, string strAniName )
{
	string strAnimationXMLFileName = strMeshPath + strModelName + ".elu.animation.xml";

	return GetAnimationInfoInXml(stOutData, strAnimationXMLFileName, strAniName, string(""));
}

void TAnimationHelper::GetAnimationInfo( string strMeshPath, string strModelName, int NPCID, int nWeaponTypeMax, CSTalentInfo* pTalentInfo, bool bNPC, string& strTalentAnimationName, TALENT_ANIMATION_INFO &talentAniInfo )
{
	GetModelPathName(strMeshPath, strModelName, NPCID);

	// Player Talent용
	for(int nType = 0; nType < nWeaponTypeMax; nType++)
	{
		ONLY_ONE_WEAPON_TYPE_RESULT eResult = CheckAnimationByOnlyOneWeaponType(pTalentInfo, (WEAPON_TYPE)nType);
		if(eResult == OOWTR_NO)
			continue;

		int nWeaponTypeIndex = nType;
		if(bNPC)
			nWeaponTypeIndex = -1;

		WEAPON_ANIMATION_INFO weaponAniInfo;
		weaponAniInfo.eWeaponType = (WEAPON_TYPE)nType;

		// 애니메이션 가져오기
		if(GetAnimation(weaponAniInfo.vecAniInfo, strMeshPath, strModelName, strTalentAnimationName, nWeaponTypeIndex))
		{
			talentAniInfo.vecTalentAniInfo.push_back(weaponAniInfo);
		}
	}
}

bool TAnimationHelper::GetAnimationInfoInXml( SIMPLEANIMATION_INFO& stOutData, string strAnimationXMLFileName, string strCompareAniName, string strFilter )
{
	MXml xml;
	if(!xml.LoadFile(strAnimationXMLFileName.c_str()))
	{
		return false;
	}

	string strCompareAniNameWithFilter = strFilter + strCompareAniName;

	MXmlElement* pElement = xml.DocHandle().FirstChild("XML").FirstChildElement().Element();
	if(pElement)
	{
		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp( pElement->Value(), "CopyFromXml"))
			{
				string strCopyPath, strFilter;
				_Attribute(strCopyPath, pElement, "filename");
				_Attribute(strFilter, pElement, "filter");

				string path = MGetPathFileNameFromRelativePath( strAnimationXMLFileName, strCopyPath);
				GetAnimationInfoInXml(stOutData, path, strCompareAniName, strFilter);
			}
			else if (!_stricmp(pElement->Value(), "AddAnimation"))
			{
				string strTempAnimationName;
				_Attribute(strTempAnimationName, pElement , "name");

				if(strTempAnimationName == strCompareAniNameWithFilter)
				{
					stOutData.strAnimationName = strTempAnimationName;
					stOutData.strAnimationPathName = MGetPurePath(strAnimationXMLFileName);
					_Attribute(stOutData.strAnimationFileName, pElement , "filename");
					_Attribute(&stOutData.fAnimationSpeed , pElement, "speed");

					string strLoopType;
					_Attribute(strLoopType, pElement, "motion_loop_type");
					if(strLoopType == "loop") 
					{
						stOutData.loopType = RAniLoopType_Loop;
					}
					else if(strLoopType == "lastframe") 
					{
						stOutData.loopType = RAniLoopType_HoldLastFrame;
					}
					break;
				}
			}
		}
	}

	return true;
}

bool TAnimationHelper::GetAnimationInfoInXml( vector<SIMPLEANIMATION_INFO>& vecOutData, string strAnimationXMLFileName, string strFilter /*= ""*/ )
{
	MXml xml;
	if(!xml.LoadFile(strAnimationXMLFileName.c_str()))
	{
		return false;
	}

	MXmlElement* pElement = xml.DocHandle().FirstChild("XML").FirstChildElement().Element();
	if(pElement)
	{
		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp( pElement->Value(), "CopyFromXml"))
			{
				string strCopyPath, strFilter;
				_Attribute(strCopyPath, pElement, "filename");
				_Attribute(strFilter, pElement, "filter");

				string path = MGetPathFileNameFromRelativePath( strAnimationXMLFileName, strCopyPath);
				GetAnimationInfoInXml(vecOutData, path, strFilter);
			}
			else if (!_stricmp(pElement->Value(), "AddAnimation"))
			{
				SIMPLEANIMATION_INFO stData;
				_Attribute(stData.strAnimationName, pElement , "name");

				stData.strAnimationPathName = MGetPurePath(strAnimationXMLFileName);
				_Attribute(stData.strAnimationFileName, pElement , "filename");
				_Attribute(&stData.fAnimationSpeed , pElement, "speed");

				string strLoopType;
				_Attribute(strLoopType, pElement, "motion_loop_type");
				if(strLoopType == "loop") 
				{
					stData.loopType = RAniLoopType_Loop;
				}
				else if(strLoopType == "lastframe") 
				{
					stData.loopType = RAniLoopType_HoldLastFrame;
				}

				vecOutData.push_back(stData);
			}
		}
	}

	return true;
}

string TAnimationHelper::GetModelFilePathFullName( int nNPCID /*= 0*/ )
{
	string strPathName;
	string strMeshName;

	GetModelPathName(strPathName, strMeshName, nNPCID);

	return strPathName + strMeshName;
}

bool TAnimationHelper::SetupActor( rs3::RActor &actor, CSTalentInfo* pTalentInfo )
{
	int NPCID = 0;
	if(pTalentInfo != NULL &&
		pTalentInfo->m_setNPCID.size() != 0)
	{
		NPCID = *pTalentInfo->m_setNPCID.begin();
	}

	string strMeshFileName = TAnimationHelper::GetModelFilePathFullName(NPCID) + ".elu";

	if( !ExistFile(strMeshFileName.c_str()))
	{
		mlog("에러 : 파일이 존재하지 않습니다. - %s \r", strMeshFileName.c_str());
		return false;
	}

	if (actor.Create(strMeshFileName.c_str()) == false)
	{
		return false;
	}

	TNPCInfo* pNPCInfo = g_pMainApp->m_NPC.GetNPCInfo(NPCID);
	if(pNPCInfo)
	{
		actor.SetScale(vec3(pNPCInfo->fScale,pNPCInfo->fScale,pNPCInfo->fScale));
	}

	if (actor.m_pMesh == NULL) 
		return false;

	return true;
}

TAnimationHelper::ONLY_ONE_WEAPON_TYPE_RESULT TAnimationHelper::CheckAnimationByOnlyOneWeaponType( CSTalentInfo* pTalentInfo, WEAPON_TYPE eWeaponType )
{
	if(pTalentInfo->m_WeaponAllowedInfo.m_setWeapons.empty())
	{
		// 아무런 무기 타입이 없다.
		return OOWTR_NONE;
	}

	// 탤런트에 맞는 무기인가?
	if(pTalentInfo->m_WeaponAllowedInfo.IsAllowed(eWeaponType, true) == false) 
		return OOWTR_NO;
		
	return OOWTR_YES;
}

bool TAnimationHelper::CalAnimaitonPos(TCharacter* pActor, TPlayMotion* pMotion, vector<RMatrix>& vecDummyLocLast)
{
	if(pActor &&
		pMotion)
	{
		MMatrix matScale;
		matScale.MakeIdentity();
		matScale.SetScale(pActor->GetScale());
		if(pActor->IsExistActorNode("dummy_loc"))
		{
			vec3 vAnimPos = vec3::ZERO;
			vec3 vAnimDir = pActor->GetDirection();

			pActor->GetMovingDummyPosition(vAnimPos);
			pActor->GetMovingDummyDirection(vAnimDir);

			MMatrix tm;
			tm.SetLocalMatrix(vAnimPos, -vAnimDir, vec3::AXISZ);

			MMatrix matResult = pMotion->GetAnimationDummyLocLastMatrix(pMotion->GetCurAniSequence(), vecDummyLocLast);
			pActor->SetTransform(matResult * tm * matScale);

			pActor->UpdateTransform();
			pActor->UpdateVisibility();

			return true;
		}
		else if(vecDummyLocLast.size() > 0)
		{
			MMatrix matResult = pMotion->GetAnimationDummyLocLastMatrix(pMotion->GetCurAniSequence(), vecDummyLocLast);
			pActor->SetTransform(matResult * matScale);

			pActor->UpdateTransform();
			pActor->UpdateVisibility();

			return true;
		}
	}

	return false;
}

void TAnimationHelper::GetMotionAnimationInfo( string strMeshPath, string strModelName, int NPCID, int nWeaponTypeMax, CSTalentInfo* pTalentInfo, bool bNPC, XMotion* pMotion, TALENT_ANIMATION_INFO &talentAniInfo )
{
	GetModelPathName(strMeshPath, strModelName, NPCID);

	for(int i = 0; i < nWeaponTypeMax; i++)
	{
		ONLY_ONE_WEAPON_TYPE_RESULT eResult = CheckAnimationByOnlyOneWeaponType(pTalentInfo, (WEAPON_TYPE)i);
		if(eResult == OOWTR_NO)
			continue;

		int nWeaponTypeIndex = i;
		if(bNPC)
			nWeaponTypeIndex = -1;

		WEAPON_ANIMATION_INFO weaponAniInfo;
		weaponAniInfo.eWeaponType = (WEAPON_TYPE)i;

		// 애니메이션 가져오기
		if(GetMotionAnimation(weaponAniInfo.vecAniInfo, strMeshPath, strModelName, pMotion, nWeaponTypeIndex))
		{
			talentAniInfo.vecTalentAniInfo.push_back(weaponAniInfo);
		}
	}
}