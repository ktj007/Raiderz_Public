#include "StdAfx.h"
#include "ProgressBarForm.h"


void Character_NET::ProgressBarForm::SetProGressBarValue( int nPercent )
{
	progressBar->Value = nPercent;
	progressBar->Text = String::Format("{0}%", nPercent);
}

void Character_NET::ProgressBarForm::SetProGressBarData( int nCount )
{
	float fValue = ((float)nCount / (float)m_nFullCount) * 100.0f;

	if(fValue > 100.0f)
		fValue = 100.0f;

	SetProGressBarValue((int)fValue);
}

void Character_NET::ProgressBarForm::Init( int nFullCount )
{
	m_nFullCount = nFullCount;
}