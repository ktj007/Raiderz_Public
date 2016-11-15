#include "stdafx.h"
#include "XValidateLogger.h"
#include "XGlobal.h"
#include "RSceneNode.h"
//#include "MTextArea.h"

void XValidateLogListener::OnLog( const char* szLog )
{
	const char* VALIDATOR_OUTPUT_TEXTAREA = "Login_ValidatorOutputTextArea";

	if (global.mint)
	{
		MWidget* pWidget = global.mint->FindWidget(VALIDATOR_OUTPUT_TEXTAREA);
//		if (pWidget && !strcmp(pWidget->GetClassName(), MINT_TEXTAREA))
//		{
// 			MTextArea* pTextArea = static_cast<MTextArea*>(pWidget);
// 			pTextArea->AddText(szLog);
//		}
	}

	mlog3(szLog);
	mlog3("\n");
}

XValidateLogger::XValidateLogger()
{
	SetListener(&m_Listener);
}

XValidateLogger::~XValidateLogger()
{

}

