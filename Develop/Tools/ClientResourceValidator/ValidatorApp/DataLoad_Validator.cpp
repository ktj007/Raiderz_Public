#include "StdAfx.h"
#include "DataLoad_Validator.h"


void ValidatorApp::DataLoad_Validator::AddErrorMsg( VErrorMsg& errorMsg )
{
	for(vector<stERRORMSG>::iterator it = errorMsg.m_strErrorMsg.begin(); it != errorMsg.m_strErrorMsg.end(); ++it)
	{
		LoadingErrorResult_listBox->Items->Add(gcnew String(it->strErrorMsg.c_str()));
	}
}