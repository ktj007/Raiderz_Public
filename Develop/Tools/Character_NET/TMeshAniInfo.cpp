#include "stdafx.h"
#include "TMeshAniInfo.h"
#include "MXml.h"
#include "RMesh.h"
#include "RAnimationManager.h"
#include "RAnimationResource.h"

using namespace rs3;

TMeshAniInfoMgr::TMeshAniInfoMgr(TMeshInfoMgr* pMeshInfo)
{
	// 애니메이션 시간을 저장할 애니메이션 리스트
	// 필요시 추가적으로 입력하세요.

	m_vecAnimationTimeList.push_back("MF_KD3");
	m_vecAnimationTimeList.push_back("1hb_MF_KD3");
	m_vecAnimationTimeList.push_back("1hp_MF_KD3");
	m_vecAnimationTimeList.push_back("1hs_MF_KD3");
	m_vecAnimationTimeList.push_back("2hd_MF_KD3");
	m_vecAnimationTimeList.push_back("2hb_MF_KD3");
	m_vecAnimationTimeList.push_back("arc_MF_KD3");
	m_vecAnimationTimeList.push_back("none_MF_KD3");
	m_vecAnimationTimeList.push_back("sta_MF_KD3");
	m_vecAnimationTimeList.push_back("dwd_MF_KD3");

	m_pMeshInfo = pMeshInfo;
}

bool TMeshAniInfoMgr::RefreshMeshAniInfo( string strModelName, string strModelPath )
{
	bool bChange = false;

	strModelName = CSStrings::StringToLower(strModelName);

	CSMeshInfo* meshInfo = m_pMeshInfo->GetMeshInfo(strModelName);
	if(meshInfo == NULL)
	{
		m_pMeshInfo->CreateMeshInfo(strModelName);
		meshInfo = m_pMeshInfo->GetMeshInfo(strModelName);
	}

	if(meshInfo)
	{
		// 데이터 경로
		string strAnimationPath = strModelPath + strModelName + "/";

		for(vector<string>::iterator it = m_vecAnimationTimeList.begin(); it != m_vecAnimationTimeList.end(); it++)
		{
			TMeshAniInfoMgr::stANIMATIONXMLDATA stAnimationData = CheckExistAnimation(strAnimationPath, strModelName, *it);
			if(!stAnimationData.fileName.empty())
			{
				CSAniInfo* pAniInfo = GetAnimationInfo(meshInfo, *it);
				if(pAniInfo)
				{
					float fAniTime = GetAnimationTime(stAnimationData);
					if(pAniInfo->m_fAnimationTime + 0.001f < fAniTime ||
						pAniInfo->m_fAnimationTime - 0.001f > fAniTime)
					{
						AnimationTimeChangeView((*it).c_str(), pAniInfo->m_fAnimationTime, fAniTime);
						pAniInfo->m_fAnimationTime = fAniTime;
						bChange = true;
					}
				}
			}
		}
	}

	return bChange;
}

bool TMeshAniInfoMgr::RefreshMeshAniInfo( RActor* pActor )
{
	bool bChange = false;

	if(pActor)
	{
		string n = pActor->GetMeshName();
		string nn = n.substr(0, n.rfind("."));

		CSMeshInfo* meshInfo = m_pMeshInfo->GetMeshInfo(nn);
		if(meshInfo == NULL)
		{
			m_pMeshInfo->CreateMeshInfo(nn);
			meshInfo = m_pMeshInfo->GetMeshInfo(nn);
		}

		if(meshInfo)
		{
			for(vector<string>::iterator it = m_vecAnimationTimeList.begin(); it != m_vecAnimationTimeList.end(); it++)
			{
				RAnimation* pAnimation = CheckExistAnimation(pActor, *it);
				if(pAnimation)
				{
					CSAniInfo* pAniInfo = GetAnimationInfo(meshInfo, *it);
					if(pAniInfo)
					{
						float fAniTime = GetAnimationTime(pAnimation, pActor->GetSpeed());
						if(pAniInfo->m_fAnimationTime + 0.001f < fAniTime ||
							pAniInfo->m_fAnimationTime - 0.001f > fAniTime)
						{
							AnimationTimeChangeView((*it).c_str(), pAniInfo->m_fAnimationTime, fAniTime);
							pAniInfo->m_fAnimationTime = fAniTime;
							bChange = true;
						}
					}
				}
			}
		}
	}

	return bChange;
}

float TMeshAniInfoMgr::GetAnimationTime( RAnimation* pAnimation, float fSpeed )
{
	int nMaxFrame = pAnimation->GetResource()->GetMaxFrame();
	return  (nMaxFrame / 4.8f / (fSpeed * 1000.f));
}

float TMeshAniInfoMgr::GetAnimationTime( stANIMATIONXMLDATA& animationData )
{
	return (animationData.nMaxFrame / 4.8f / (animationData.fSpeed * 1000.f));
}

CSAniInfo * TMeshAniInfoMgr::GetAnimationInfo( CSMeshInfo * pMeshInfo, string strAnimationName )
{
	CSAniInfo * pAniInfo = NULL;

	if(pMeshInfo)
	{
		pAniInfo = pMeshInfo->FindAniInfo(strAnimationName);

		if(pAniInfo == NULL)
		{
			CSAniInfo aniInfo;
			aniInfo.m_strAnimationName = strAnimationName;
			pMeshInfo->m_vecAniInfo.push_back(aniInfo);
			pAniInfo = pMeshInfo->FindAniInfo(strAnimationName);
		}
	}

	return pAniInfo;
}

TMeshAniInfoMgr::stANIMATIONXMLDATA TMeshAniInfoMgr::CheckExistAnimation( const string& strDirPath, const string& strModelName, const string& strAnimationName )
{
	string strAnimationXMLFileName = strDirPath + strModelName + ".elu.animation.xml";
	//string strTempAnimationFileName;
	stANIMATIONXMLDATA stData;

//	MXml xml;
//	if(!xml.LoadFile(strAnimationXMLFileName.c_str()))
//	{
//		return stData;
//	}
//
//	MXmlElement* pElement = xml.DocHandle().FirstChild("XML").FirstChildElement().Element();
//	if(pElement)
//	{
//		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
//		{
//			if (!_stricmp(pElement->Value(), "AddAnimation"))
//			{
//				string strTempAnimationName;
//				_Attribute(strTempAnimationName, pElement , "name");
//
//				if(strTempAnimationName == strAnimationName)
//				{
//					stData.animationName = strTempAnimationFileName;
//					_Attribute(stData.fileName, pElement , "filename");
//					_Attribute(&stData.fSpeed , pElement, "speed");
//					break;
//				}
//			}
//		}
//
//		string strAnimationFileName = strDirPath + stData.fileName;
//
//		RAnimationResource* node = REngine::GetSceneManager().CreateResource < RAnimationResource > ( strAnimationFileName.c_str() );
//		if( node->BuildAll() )
//			stData.nMaxFrame = node->GetMaxFrame();
//		REngine::GetSceneManager().ReleaseResource( node );
//
//// 		RAnimationFile animationFileSystem;
//// 		if(animationFileSystem.LoadAni(strAnimationFileName.c_str()))
////		{
////			stData.nMaxFrame = animationFileSystem.m_nMaxFrame;
////		}
//	}

	SIMPLEANIMATION_INFO simpleAniInfo;
	if(TAnimationHelper::GetAnimationInfoInXml(simpleAniInfo, strAnimationXMLFileName, strAnimationName, string("")))
	{
		stData.animationName = strAnimationName;
		stData.fileName = simpleAniInfo.strAnimationFileName;
		stData.fSpeed = simpleAniInfo.fAnimationSpeed;

		string strAnimationFileName = simpleAniInfo.strAnimationPathName + simpleAniInfo.strAnimationFileName;
		RAnimationResource* node = REngine::GetSceneManager().CreateResource < RAnimationResource > ( strAnimationFileName.c_str() );
		if( node->BuildAll() )
			stData.nMaxFrame = node->GetMaxFrame();
		REngine::GetSceneManager().ReleaseResource( node );
	}

	return stData;
}

RAnimation* TMeshAniInfoMgr::CheckExistAnimation( RActor* pActor, const string& strAnimationName )
{
	return pActor->GetAnimation(strAnimationName.c_str());
}

void TMeshAniInfoMgr::AnimationTimeChangeView( string strAnimation, float fPreTime, float fCurrentTime )
{
	if(fPreTime == 0.0f)
	{
		mlog("정보 : Animation = %s Time %f : 애니메이션 시간이 추가 되었습니다.. \r", strAnimation.c_str(), fCurrentTime);
	}
	else
	{
		mlog("정보 : Animation = %s Time %f -> %f : 애니메이션 시간이 변경이 되었습니다.. \r", strAnimation.c_str(), fPreTime, fCurrentTime);
	}
}