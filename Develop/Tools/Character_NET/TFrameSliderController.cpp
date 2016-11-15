#include "stdafx.h"
#include "TFrameSliderController.h"

TFrameSliderController::TFrameSliderController()
:m_eAnimationPlayState(APS_STOP)
{

}

TFrameSliderController::~TFrameSliderController()
{

}

void TFrameSliderController::Update_AniFrame(TCharacter* pActor)
{
	Character_NET::Form1^ pMainForm = GlobalObjects::g_pMainForm;

	if(pActor->GetCurAnimation())
	{
		//트랙범위 재설정
		int nMaxFrame						= pActor->GetAnimationMaxFrame();
		int nCurFrame						= pActor->GetAnimationFrame();

		pMainForm->AnitrackBar->Maximum = nMaxFrame;
		pMainForm->AnitrackBar->Value = pActor->GetAnimationFrame();

		float timespeed = g_pMainApp->GetActor()->GetAnimationSpeed() * 1000.f;
		float fCurrentTime = nCurFrame/ 4.8f /timespeed;

		pMainForm->Framelabel->Text = String::Format("{0}({1}) : {2}/{3}frame : {4:F}/{5:F}초)",
			gcnew String(pActor->GetCurAnimation()->GetAliasName().c_str()),
			pActor->IsAnimationLoop() ? String::Format("Loop") : String::Format("once"),
			nCurFrame, nMaxFrame,
			fCurrentTime, nMaxFrame/ 4.8f /timespeed);

		pMainForm->Framelabel2->Text = String::Format("MAX {0}/{1}",
			nCurFrame/160.f, nMaxFrame/160.f);

		pMainForm->Speedlabel->Text = String::Format("Speed : {0}", pMainForm->AniSpeedtrackBar->Value / ANISPEED);

		Update_PlayStopButton(nCurFrame, nMaxFrame);
		ShowNormalAnimationName(pActor);
	}
}

void TFrameSliderController::Update_TalentFrame( TCharacter* pActor, TTalentAnimation* pTalentAnimation )
{
	if( pActor == NULL ||
		pTalentAnimation == NULL) 
		return;

	Character_NET::Form1^ pMainForm = GlobalObjects::g_pMainForm;

	int nMaxFrame						= pTalentAnimation->GetCurTalentFrameMax();
	int nCurFrame						= pTalentAnimation->GetCurTalentCurFrame();

	float fMaxTime						= pTalentAnimation->GetCurTalentMaxFrametime();
	float fCurTime						= pTalentAnimation->GetCurTalentCurFrametime();

	if(nMaxFrame == 0)
		return;
	else if(nCurFrame > nMaxFrame)
	{
		// 이건 문제가 있다.
		mlog("경고!!! 프레임 계산에 문제가 있습니다. (%d / %d)\r", nCurFrame, nMaxFrame);
		nCurFrame = nMaxFrame;
	}
	pMainForm->AnitrackBar->Maximum		= nMaxFrame;
	pMainForm->AnitrackBar->Value		= nCurFrame;

	pMainForm->Framelabel->Text = String::Format("{0}/{1} frame ( {2:F}/{3:F}초 )",
		nCurFrame, nMaxFrame,
		fCurTime,
		fMaxTime);

	float fCurrentTime = 0;
	if(pActor->GetCurAnimation() != NULL)
	{
		float timespeed = pActor->GetAnimationSpeed() * 1000.f;
		fCurrentTime = pActor->GetAnimationFrame()/ 4.8f /timespeed;
		pMainForm->Framelabel2->Text = String::Format("{0}({1}) : {2}/{3}frame : {4:F}/{5:F}초)",
			gcnew String(pActor->GetCurAnimation()->GetAliasName().c_str()),
			pActor->IsAnimationLoop() ? String::Format("Loop") : String::Format("once"),
			pActor->GetAnimationFrame(), pActor->GetAnimationMaxFrame(),
			fCurrentTime, pActor->GetAnimationMaxFrame()/ 4.8f /timespeed);
	}

	Update_PlayStopButton(nCurFrame, nMaxFrame);
	ShowTalentAnimationName(pActor, pTalentAnimation);
	ShowTalentHitCapsule(pTalentAnimation->GetCurTalentCurFrametime());
}

void TFrameSliderController::FrameSliderScroll( TCharacter* pActor, TTalentAnimation* pTalentAnimation, int nMax, int nValue )
{
	RSceneNodeUpdateVisitor visitor;
	if( pActor == NULL ||
		pTalentAnimation == NULL) 
		return;

	if( g_pMainApp->m_bTalentAnimation)
	{
		pTalentAnimation->SetTalentAnimationFrame(nValue);
		pActor->PauseAnimation();

		if (visitor.OnPreOrderVisit(pActor))
			visitor.OnPostOrderVisit(pActor);
	}
	else
	{
		double dblCurPosition;
		dblCurPosition = (double)(nValue / (double)nMax) * pActor->GetAnimationMaxFrame();
		//g_pMainApp->SetFrame((DWORD)dblCurPosition);
		pActor->PauseAnimation();
		pActor->SetAnimationFrame(nValue);
		if (visitor.OnPreOrderVisit(pActor))
			visitor.OnPostOrderVisit(pActor);
	}
}

void TFrameSliderController::Update_PlayStopButton( int nCurFrame, int nMaxFrame )
{
	if(nMaxFrame <= (nCurFrame + 10))
	{
		GlobalObjects::g_pMainForm->pausebutton->Hide();
		GlobalObjects::g_pMainForm->playbutton->Show();

		m_eAnimationPlayState = APS_STOP;
	}
}

void TFrameSliderController::pressPlayButton()
{
	GlobalObjects::g_pMainForm->playbutton->Hide();
	GlobalObjects::g_pMainForm->pausebutton->Show();

	m_eAnimationPlayState = APS_PLAY;
}

void TFrameSliderController::pressStopButton()
{
	GlobalObjects::g_pMainForm->pausebutton->Hide();
	GlobalObjects::g_pMainForm->playbutton->Show();

	m_eAnimationPlayState = APS_STOP;
}

void TFrameSliderController::pressPauseButton()
{
	GlobalObjects::g_pMainForm->pausebutton->Hide();
	GlobalObjects::g_pMainForm->playbutton->Show();
	
	m_eAnimationPlayState = APS_PAUSE;
}

void TFrameSliderController::ShowTalentAnimationName( TCharacter* pActor, TTalentAnimation* pTalentAnimation )
{
	if(pTalentAnimation == NULL)
		return;

	GlobalObjects::g_pMainForm->TalentAniNameBar->Show();

	int i				= 0;
	int nMaxFrame		= pTalentAnimation->GetCurTalentFrameMax();
	int nNextStart		= 0;
	Size nameBarSize	= GlobalObjects::g_pMainForm->TalentAniNameBar->Size;

	System::Drawing::Graphics^ formGraphics		= GlobalObjects::g_pMainForm->TalentAniNameBar->CreateGraphics();
	System::Drawing::SolidBrush^ fontBrush		= GetBrush(-1);
	System::Drawing::Font^ drawFont				= gcnew System::Drawing::Font("굴림", 10);
	System::Drawing::StringFormat^ drawFormat	= gcnew System::Drawing::StringFormat;
	drawFormat->Alignment						= System::Drawing::StringAlignment::Center;
	drawFormat->LineAlignment					= System::Drawing::StringAlignment::Center;

	for(int i = 0; i < pTalentAnimation->GetTalentAnimationMaxIndex(); ++i)
	{
		System::Drawing::SolidBrush^ myBrush = GetBrush(i);

		int nAniMaxFrame = pTalentAnimation->GetTalentFrame(i);
		int nCurWidth = (int)((float)(nAniMaxFrame * nameBarSize.Width) / (float)nMaxFrame);

		RectangleF drawRect = RectangleF((float)nNextStart, 0, (float)nCurWidth, (float)nameBarSize.Height);
		formGraphics->FillRectangle(myBrush, drawRect);

		formGraphics->DrawString(gcnew String(pTalentAnimation->GetTalentAniName(i).c_str()), drawFont, fontBrush, drawRect, drawFormat);

		nNextStart += nCurWidth;
	}

	//for(vector<XMotionAniSequence*>::iterator itAni = pPlayMotion->GetMotion()->m_vecAniSequence[pPlayMotion->GetCurMotionType()].begin();
	//	itAni != pPlayMotion->GetMotion()->m_vecAniSequence[pPlayMotion->GetCurMotionType()].end();
	//	itAni++)
	//{
	//	XMotionAniSequence* pAniSequence = (*itAni);
	//	System::Drawing::SolidBrush^ myBrush = GetBrush(i);

	//	int nAniMaxFrame = pPlayMotion->GetTalentFrame(i);
	//	int nCurWidth = (int)((float)(nAniMaxFrame * nameBarSize.Width) / (float)nMaxFrame);
	//
	//	RectangleF drawRect = RectangleF((float)nNextStart, 0, (float)nCurWidth, (float)nameBarSize.Height);
	//	formGraphics->FillRectangle(myBrush, drawRect);

	//	formGraphics->DrawString(gcnew String(pAniSequence->GetAniName()), drawFont, fontBrush, drawRect, drawFormat);

	//	nNextStart += nCurWidth;
	//	i++;
	//}

	delete fontBrush;
	delete drawFont;
	delete drawFormat;
}

System::Drawing::SolidBrush^ TFrameSliderController::GetBrush( int n )
{
	switch(n)
	{
	case 0:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::Aqua);
	case 1:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::YellowGreen);
	case 2:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::BlueViolet);
	case 3:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::OrangeRed);
	case 4:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::Coral);
	case 5:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::CornflowerBlue);
	case 6:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::SaddleBrown);
	case 7:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::DarkGray);
	case 99:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::White);
	default:
		return gcnew System::Drawing::SolidBrush(System::Drawing::Color::Black);
	}
	
	return nullptr;
}

void TFrameSliderController::ShowNormalAnimationName( TCharacter* pActor )
{
	GlobalObjects::g_pMainForm->TalentAniNameBar->Hide();
}

void TFrameSliderController::ShowTalentHitCapsule( float fTime )
{
	GlobalObjects::g_pMainForm->m_TalentTabPage->m_TalentHitTabController->Play(fTime);
}