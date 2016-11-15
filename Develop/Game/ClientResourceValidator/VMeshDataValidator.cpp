#include "stdafx.h"
#include "VMeshDataValidator.h"


VMeshDataValidator::VMeshDataValidator()
:VDataValidator()
{
	m_nMeshCount = 0;
}

VMeshDataValidator::~VMeshDataValidator()
{

}

void VMeshDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_nMeshCount = vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.size();
}

void VMeshDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{
	for(map<string, VMeshInfo*>::iterator it = vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.begin(); it != vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.end(); ++it)
	{
		// animation.xml
		CheckAnimationXMl(it->second);

		// animationevent.xml
		CheckAnimationEvent(it->second, vInfoMgr);

		// animationinfo.xml
		CheckAnimationInfo(it->second);
	}
}

void VMeshDataValidator::CheckAnimationXMl( VMeshInfo* pMeshInfo )
{
	// 애니메이션 파일 존재 체크
	VAnimationList* pAnimationList = &(pMeshInfo->m_AnimationInfoMgr.m_AnimationList);
	for(vector<ANIMATION_INFO>::iterator it = pAnimationList->m_vecAnimationList.begin(); it != pAnimationList->m_vecAnimationList.end(); ++it)
	{
		if(ExistFile(it->strAnimationFileFullName.c_str()) == false)
		{
			string strErrorMsg = pMeshInfo->m_strMeshName + " : " + it->strAnimationName + " 의 파일이 존재하지 않습니다.";
			m_AnimationListValidatorError.Add(EMT_FILE, strErrorMsg);
		}
	}
}

void VMeshDataValidator::CheckAnimationEvent( VMeshInfo* pMeshInfo, VValidatorInfoMgr* vInfoMgr )
{
	// 이벤트 체크
	VAnimationEventInfo* pAnimationEvent = &(pMeshInfo->m_AnimationInfoMgr.m_AnimationEventInfo);
	for(vector<ANIMATION_EVENT_INFO>::iterator it = pAnimationEvent->m_vecAnimationEvents.begin(); it != pAnimationEvent->m_vecAnimationEvents.end(); ++it)
	{
		for(vector<RAnimationEvent*>::iterator itEvent = it->vecAnimationEvent.begin(); itEvent != it->vecAnimationEvent.end(); ++itEvent)
		{
			string strErrorPreMsg = pMeshInfo->m_strMeshName + " : " + it->strAnimationName;

			RAnimationEvent* pEvent = *itEvent;
			if(pEvent->m_strEvent == "sh_custom")
			{
				if(vInfoMgr->m_pEffectInfoMgr->GetEffectInfo(pEvent->m_strParam1) == NULL)
				{
					string strError = strErrorPreMsg + " 의 [sh_custom] 이벤트에서 " + pEvent->m_strParam1 + " 이펙트가 effect.xml에 없습니다.";
					m_AnimationEventValidatorError.Add(EMT_LINK, strError);
				}
			}
			else if(pEvent->m_strEvent == "sh_sound")
			{
				if(vInfoMgr->m_pSoundInfoMgr->GetSampleInfo(pEvent->m_strParam1) == NULL)
				{
					string strError = strErrorPreMsg + " 의 [sh_sound] 이벤트에서 " + pEvent->m_strParam1 + " 사운드가 sound.xml에 없습니다.";
					m_AnimationEventValidatorError.Add(EMT_LINK, strError);
				}
			}
			else if(pEvent->m_strEvent == "sh_effect")
			{
				if(vInfoMgr->m_pEffectInfoMgr->GetEffectFileInfo(pEvent->m_strParam1) == NULL)
				{
					string strError = strErrorPreMsg + " 의 [sh_effect] 이벤트에서 " + pEvent->m_strParam1 + ".effect.scene.xml 파일이 Data/SFX 폴더에 없습니다.";
					m_AnimationEventValidatorError.Add(EMT_FILE, strError);
				}
			}
			else if(pEvent->m_strEvent == "sh_camera")
			{
				if(vInfoMgr->m_pCameraInfoMgr->GetCameraFileInfo(pEvent->m_strParam1) == NULL)
				{
					string strError = strErrorPreMsg + " 의 [sh_camera] 이벤트에서 " + pEvent->m_strParam1 + ".elu.ani 파일이 Data/SFX/Camera 폴더에 없습니다.";
					m_AnimationEventValidatorError.Add(EMT_FILE, strError);
				}
			}
		}
	}
}

void VMeshDataValidator::CheckAnimationInfo( VMeshInfo* pMeshInfo )
{
	vector<BLEND_INFO_TOOL>* pvecAnimationBlendInfo = &(pMeshInfo->m_AnimationInfoMgr.m_vecAnimationBlendInfoForTool);
	VAnimationList* pAnimationList = &(pMeshInfo->m_AnimationInfoMgr.m_AnimationList);

	string strErrorPreMsg = pMeshInfo->m_strMeshName + " : ";
	for(vector<BLEND_INFO_TOOL>::iterator it = pvecAnimationBlendInfo->begin(); it != pvecAnimationBlendInfo->end(); ++it)
	{
		if(pAnimationList->GetAnimationInfo(it->m_strSource) == NULL)
		{
			string strError = strErrorPreMsg + "source의 " + it->m_strSource + " 이(가) animation.xml에 없습니다.";
			m_AnimationInfoValidatorError.Add(EMT_LINK, strError);
		}

		for(vector<string>::iterator itTarget = it->m_vecTarget.begin(); itTarget != it->m_vecTarget.end(); ++itTarget)
		{
			if(pAnimationList->GetAnimationInfo(*itTarget) == NULL)
			{
				string strError = strErrorPreMsg + it->m_strSource + " 블렌딩 정보에서 target의 " + *itTarget + " 이(가) animation.xml에 없습니다.";
				m_AnimationInfoValidatorError.Add(EMT_LINK, strError);
			}
		}
	}
}