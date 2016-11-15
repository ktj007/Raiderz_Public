#include "stdafx.h"
#include "TNormalAnimation.h"
#include "TAnimationHelper.h"

TNormalAnimation::TNormalAnimation()
: m_pActor(NULL)
{
	m_checkDummyLocResult = CDLR_NO_ANIMATION;
	InitNormalAnimation();
}

TNormalAnimation::~TNormalAnimation()
{

}

void TNormalAnimation::Init( TPlayMotion* pMotion )
{
	m_pMotion = pMotion;
}

bool TNormalAnimation::SetAnimation( string strAniName )
{
	g_pMainApp->m_pEffectManager->Clear();

	m_bUseAnimationLinkListTest = false;

	CheckDummyLoc(strAniName);


	m_pActor->InitMeshNodeNoiseRef();
	return false;
}

void TNormalAnimation::Update( float fDelta )
{
	CheckAnimationState();
	CheckAnimationEvent();
	CheckAnimationLinkListTest();
}

void TNormalAnimation::CheckAnimationEvent()
{
	if(m_pActor == NULL)
		return;

	if(m_pActor->GetAnimationState() != RPS_PAUSE)
		return;

	unsigned int nCurFrame = m_pActor->GetCurFrame();

	CheckAnimationEvent(nCurFrame);
}

void TNormalAnimation::CheckAnimationEvent( int nFrame )
{
	if(m_pActor == NULL)
		return;

	m_pActor->ResetAnimationEventByFrame(nFrame);
}

bool TNormalAnimation::AnimationPlay( bool bPrePause, bool bPlayDone )
{
	if(m_pActor == NULL ||
		m_bUseAnimationLinkListTest ||
		m_pActor->GetCurAnimation() == NULL)
		return false;

	g_pMainApp->m_MagicArea.OnDestroy();

	if(bPrePause)
	{
		g_pMainApp->UpdateModelScale();
		m_pActor->PlayAnimation();
	}
	else if(bPlayDone)
	{
		//if(GlobalObjects::g_pMainForm->AnitrackBar->Value == (GlobalObjects::g_pMainForm->AnitrackBar->Maximum-1))
		//{
		//	g_pMainApp->SetAnimation(m_pActor->GetAnimation());

		//	float s = GlobalObjects::g_pMainForm->AniSpeedtrackBar->Value / ANISPEED;
		//	m_pActor->SetAnimationSpeed(s);

		//	m_bUseAnimationLinkListTest = false;
		//}
		//else
		{
			AnimationStop();

			g_pMainApp->UpdateModelScale();
			m_pActor->PlayAnimation();
		}
	}
	else
		return false;

	return true;
}

void TNormalAnimation::AnimationStop()
{
	if(m_pActor == NULL ||
		m_bUseAnimationLinkListTest)
		return;

	//m_pActor->PauseAnimation();
	//m_pActor->InitCurrentAnimation();
	m_pActor->StopAnimation();

	RMatrix mat;
	mat.MakeIdentity();
	m_pActor->SetTransform(mat);
	
	m_pActor->InitMeshNodeNoiseRef();

	if(g_pMainApp->m_pEffectManager)
		g_pMainApp->m_pEffectManager->StopAnimationEffect();
}

void TNormalAnimation::AnimationPause()
{
	if(m_pActor == NULL)
		return;

	m_pActor->PauseAnimation();
}

bool TNormalAnimation::CalAnimaitonPos()
{
	return TAnimationHelper::CalAnimaitonPos(m_pActor, m_pMotion, m_vecDummyLocLast);
}

void TNormalAnimation::InitNormalAnimation()
{
	m_bUseAnimationLinkListTest = false;
	m_checkDummyLocResult = CDLR_NO_ANIMATION;

	if(g_pMainApp && g_pMainApp->m_pEffectManager)
		g_pMainApp->m_pEffectManager->StopAnimationEffect();
}

void TNormalAnimation::AnimationLinkListTestPlay( ListBox^ AnimationLinkList )
{
	int nCount = AnimationLinkList->Items->Count;

	if(m_pActor == NULL ||
		nCount == 0)
		return;

	m_bUseAnimationLinkListTest = true;

	if(m_pMotion->GetCurAniMaxSequence() == nCount &&
		m_bUseAnimationLinkListTest)
	{
		// 이미 플레이중인데... 멈춘거라면...
		g_pMainApp->UpdateModelScale();
		m_pActor->PlayAnimation();
		return;
	}

	bool bChange = false;
	vector<string> vecAnimationName;

	for(int i = 0; i < nCount; i++)
	{
		String^ itemString = AnimationLinkList->Items[i]->ToString();

		int fndpos = itemString->IndexOf("(");
		if (fndpos != -1)
			itemString = itemString->Substring(0, fndpos)->Trim();

		vecAnimationName.push_back(MStringToCharPointer(itemString));
	}

	m_pMotion->ChangeCustomAnimationList(vecAnimationName);
	m_pMotion->CalAnimationDummyLocLastMatrixList(m_pActor, m_vecDummyLocLast);
}

void TNormalAnimation::CheckAnimationLinkListTest()
{
	if(m_bUseAnimationLinkListTest == false ||
		GlobalObjects::g_pMainForm->m_AnimationTabPage->GetTestState() != LTS_PLAY)
		return;

	int nCurrentSequence = m_pMotion->GetCurAniSequence();

	GlobalObjects::g_pMainForm->m_AnimationTabPage->SetCurrentAnimationLinkListIndex(nCurrentSequence);
}

void TNormalAnimation::CheckDummyLoc(string strAniName)
{
	m_checkDummyLocResult = g_pMainApp->m_TabAnimation.CheckAnimationDummyLocNode(strAniName.c_str());
}

void TNormalAnimation::CheckAnimationState()
{
	if(m_pActor == NULL)
		return;
}

void TNormalAnimation::UpperAnimationTestPlay( string strUpperAni, string strLowerAni, TCharacter* pActor )
{
	if(pActor != NULL)
		m_pActor = pActor;

	if(m_pActor == NULL ||
		strUpperAni.empty() ||
		strLowerAni.empty())
		return;

	g_pMainApp->SetAnimation(strLowerAni.c_str());
	m_pActor->SetUpperAnimation(strUpperAni);
}

void TNormalAnimation::AnimationLinkListTestStop()
{
	if(m_pActor == NULL)
		return;

	m_pMotion->SetSequence(0);
	m_pActor->InitMeshNodeNoiseRef();
	m_pActor->StopAnimation();
}

void TNormalAnimation::AnimationLinkListTestSpeed( float fSpeed )
{
	m_pMotion->ChangeCustomAnimationListSpeed(fSpeed);
}

void TNormalAnimation::UpperAnimationTestStop()
{
	m_pActor->InitMeshNodeNoiseRef();
	m_pActor->StopAnimation();
}

void TNormalAnimation::AnimationLinkListTestEndTime(int nIndex, float fEndTime)
{
	m_pMotion->SetCustomAnimationListEndTime(nIndex, fEndTime);
}
