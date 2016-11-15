#include "stdafx.h"
#include "XResourceValidator.h"

bool XResourceValidator::CheckAll()
{
	bool ret = true;

	if (CheckTalentInfo() == false) ret = false;
	if (CheckNPCInfo() == false) ret = false;
	if (CheckAnimaionEvent() == false) ret = false;
	if (CheckISAnimaion() == false) ret = false;
	if (CheckItem() == false) ret = false;
	if (CheckSound() == false) ret = false;

	InitOutputWidget(ret);

	return ret;
}

void XResourceValidator::InitOutputWidget( bool ret )
{
	if (!ret)
	{
		const char* VALIDATOR_OUTPUT_FRAME = "Login_ValidatorOutputFrame";

		if (global.mint)
		{
			MWidget* pWidget = global.mint->FindWidget(VALIDATOR_OUTPUT_FRAME);
			if (pWidget)
			{
				pWidget->Show();
			}
		}
	}
}

bool XResourceValidator::CheckTalentInfo()
{
	return m_TalentInfoValidator.Check();
}

bool XResourceValidator::CheckNPCInfo()
{
	return m_NPCInfoValidator.Check();
}

bool XResourceValidator::CheckAnimaionEvent()
{
	return m_AnimationValidator.Check();
}

bool XResourceValidator::CheckISAnimaion()
{
	return m_ISAnimationValidator.Check();
}

bool XResourceValidator::CheckItem()
{
	return m_ItemValidator.Check();
}

bool XResourceValidator::CheckSound()
{
//	return m_SoundValidator.Check();			<<<rsound3
	return true;
}
