#include "StdAfx.h"
#include "XOverlayAnimationTC.h"
#include "XOverlayAnimationInfo.h"
#include "MLocale.h"

XOverlayAnimationTC::XOverlayAnimationTC( XCharacter* pCharacter )
:XTransformController(pCharacter)
{
	m_vecOverlayAnimationController.clear();
	m_vecReservedOverlayAnimationName.clear();
}

XOverlayAnimationTC::~XOverlayAnimationTC( void )
{
	OverlayAnimationDel();
}

void XOverlayAnimationTC::Update( float fDelta )
{
	vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin();
	while(it != m_vecOverlayAnimationController.end())
	{
		if(it->pOverlayAnimationController->IsAnimationLoop() == false &&
			it->pOverlayAnimationController->IsPlayDone())
		{
			m_pActor->RemoveController(it->pOverlayAnimationController);
			SAFE_DELETE(it->pOverlayAnimationController);
			it = m_vecOverlayAnimationController.erase(it);
			continue;
		}
		else
			it->pOverlayAnimationController->OnUpdate();

		++it;
	}
}

void XOverlayAnimationTC::SetAnimation( tstring& strAniName )
{
	if(m_pActor == NULL)
		return;

	// 같은 이름은 어떻게...? 일단 지켜보자.
	if(m_strCurrentPlayAnimation == strAniName)
		return;

	if( CheckAnimationUsable(strAniName) == false)
	{
		m_vecReservedOverlayAnimationName.push_back(strAniName);
		return;
	}

	// 오버레이 애니메이션 정보 가져오기
	XOverlayAnimationInfo* pInfo = m_pActor->GetOverlayAnimaionInfo();
	OVERLAY_ANIMATION_LIST * pOverlayList = NULL;

	if(pInfo)
		pOverlayList = pInfo->Get(strAniName);

	// 현재 되어 있는 오버레이 애니메이션중에서 중복되지 않는건... 삭제
	vector<OVERLAY_ANIMATION_INFO> vecPlayList;
	GetNewOverlayAnimationList(pOverlayList, vecPlayList);

	// 오버레이 애니메이션 재생
	for(vector<OVERLAY_ANIMATION_INFO>::iterator itInfo = vecPlayList.begin(); itInfo != vecPlayList.end(); ++itInfo)
	{
		SetOverlayAnimation(itInfo->strOverlayAnimation, itInfo->strBoneName);
	}

	m_strCurrentPlayAnimation = strAniName;
}

void XOverlayAnimationTC::SetOverlayAnimation(tstring& strAniName, tstring& strBoneName)
{
	RActorNodeSelection nodeSelection;
	if(nodeSelection.SelectNodeAndChildren(m_pActor, MLocale::ConvTCHARToAnsi(strBoneName.c_str()).c_str(), true) == false)
		return ;

	OVERLAY_ANIMATION_CONTROLLER overlayAnimationController;
	overlayAnimationController.pOverlayAnimationController = new XRAnimationController(RSNCTRL_CUSTOMUPDATE, RSNCTRL_CUSTOMUPDATE);
	m_pActor->AddController(overlayAnimationController.pOverlayAnimationController);

	RPLAYINFO playInfo;
	playInfo.blendMode = RBLENDMODE_BLEND;
	playInfo.pNodeSet = &nodeSelection;

	overlayAnimationController.pOverlayAnimationController->SetAnimation( MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str(), &playInfo );
	overlayAnimationController.pOverlayAnimationController->Play();

	overlayAnimationController.strAniName = strAniName;
	overlayAnimationController.strBoneName = strBoneName;
	m_vecOverlayAnimationController.push_back(overlayAnimationController);
}

void XOverlayAnimationTC::OverlayAnimationPlay()
{
	for(vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin(); it != m_vecOverlayAnimationController.end(); ++it)
	{
		it->pOverlayAnimationController->Play();
	}
}

void XOverlayAnimationTC::OverlayAnimationStop()
{
	for(vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin(); it != m_vecOverlayAnimationController.end(); ++it)
	{
		it->pOverlayAnimationController->SetFrame(0);
		it->pOverlayAnimationController->ResetEvent();
		it->pOverlayAnimationController->Pause();
	}
}

void XOverlayAnimationTC::OverlayAnimationPause()
{
	for(vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin(); it != m_vecOverlayAnimationController.end(); ++it)
	{
		it->pOverlayAnimationController->Pause();
	}
}

void XOverlayAnimationTC::OverlayAnimationDel()
{
	for(vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin(); it != m_vecOverlayAnimationController.end(); ++it)
	{
		it->pOverlayAnimationController->Stop();
		m_pActor->RemoveController(it->pOverlayAnimationController);
		SAFE_DELETE(it->pOverlayAnimationController);
	}

	m_vecOverlayAnimationController.clear();
}

void XOverlayAnimationTC::GetNewOverlayAnimationList( OVERLAY_ANIMATION_LIST * pList, vector<OVERLAY_ANIMATION_INFO>& outList )
{
	// 중복된건 놔두고... 그외는 삭제하고 새로운것만 리스트에 넣는다.
	if(pList)
	{
		// 중복되지 않는 애니 삭제
		vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin();
		while(it != m_vecOverlayAnimationController.end())
		{
			bool bDuplication = false;
			for(vector<OVERLAY_ANIMATION_INFO>::iterator itInfo = pList->vecOverlayAnimationList.begin(); itInfo != pList->vecOverlayAnimationList.end(); ++itInfo)
			{
				if(itInfo->strOverlayAnimation == it->strAniName &&
					itInfo->strBoneName == it->strBoneName)
				{
					bDuplication = true;
					break;
				}
			}

			if(bDuplication == false)
			{
				it->pOverlayAnimationController->Stop();
				m_pActor->RemoveController(it->pOverlayAnimationController);
				SAFE_DELETE(it->pOverlayAnimationController);
				it = m_vecOverlayAnimationController.erase(it);
				continue;
			}

			++it;
		}

		// 새로운거 넣기
		for(vector<OVERLAY_ANIMATION_INFO>::iterator itNewInfo = pList->vecOverlayAnimationList.begin(); itNewInfo != pList->vecOverlayAnimationList.end(); ++itNewInfo)
		{
			bool bNew = true;
			for(it = m_vecOverlayAnimationController.begin();it != m_vecOverlayAnimationController.end(); ++it)
			{
				if(itNewInfo->strOverlayAnimation == it->strAniName &&
					itNewInfo->strBoneName == it->strBoneName)
				{
					bNew = false;
				}
			}

			if(bNew)
				outList.push_back(*itNewInfo);
		}
	}
	else
	{
		OverlayAnimationDel();
	}
}

void XOverlayAnimationTC::CheckReservedAnimation()
{
	if(m_vecReservedOverlayAnimationName.empty())
		return;

	for(vector<tstring>::iterator it = m_vecReservedOverlayAnimationName.begin(); it != m_vecReservedOverlayAnimationName.end(); ++it)
	{
		// 모델이나 애니메이션이 로딩 되었는가?
		if(CheckAnimationUsable(*it))
			SetAnimation( *it );
	}

	m_vecReservedOverlayAnimationName.clear();
}

void XOverlayAnimationTC::PreUpdate( float fDelta )
{
	vector<OVERLAY_ANIMATION_CONTROLLER>::iterator it = m_vecOverlayAnimationController.begin();
	while(it != m_vecOverlayAnimationController.end())
	{
		if(it->pOverlayAnimationController->IsAnimationLoop() == false &&
			it->pOverlayAnimationController->IsPlayDone())
		{
			m_pActor->RemoveController(it->pOverlayAnimationController);
			SAFE_DELETE(it->pOverlayAnimationController);
			it = m_vecOverlayAnimationController.erase(it);
			continue;
		}
		else
		{
			it->pOverlayAnimationController->OnPreUpdate();
			it->pOverlayAnimationController->OnUpdate();
		}

		++it;
	}
}