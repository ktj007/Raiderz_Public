#include "stdafx.h"
#include "WS_Script.h"

#include "VR_CoreToolRS3.h"
#include "MXml.h"

CWorkSpaceScript::CWorkSpaceScript()
{
}

CWorkSpaceScript::~CWorkSpaceScript()
{
}

void CWorkSpaceScript::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	// minimal ui
	int nDefaultTabIdx = _rUiSetter.AddTab("CoreToolRS3", WORKBENCH_STATE::ETC_MAP_VIEW);
	_rUiSetter.AddRollup(nDefaultTabIdx, CRollupCoreToolRS3::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, false);

	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nDefaultTabIdx, CRollupCoreToolRS3::GetID());
}

MAP_EDIT_TYPE CWorkSpaceScript::OnOpenFile(const char* _pSzFileName)
{
	MXml aXml;
	if (!aXml.LoadFile(_pSzFileName))
		return MAP_EDIT_TYPE_EXIT_FAILD;

	MXmlElement* pRoot = aXml.DocHandle().FirstChildElement("coretool_script").Element();
	if (!pRoot)
		return MAP_EDIT_TYPE_EXIT_FAILD;

	bool bExitAfterOpenMap = false;
	_Attribute(bExitAfterOpenMap, pRoot, "exit_after_run");

	// call ruby script
	MXmlElement* pRubyScriptElement = pRoot->FirstChildElement("ruby");
	while (pRubyScriptElement)
	{
		pRubyScriptElement = pRubyScriptElement->NextSiblingElement("ruby");
	}

	// call built-in func script
	MXmlElement* pBuiltInFuncElement = pRoot->FirstChildElement("builtin");
	std::string call_func;
	std::string param_name;
	std::string param_value;

	int nFailCnt = 0;
	while (pBuiltInFuncElement)
	{
		call_func.clear();
		_Attribute(call_func, pBuiltInFuncElement, "call_func");

		CCoreToolFunctor* pFunctor = CRubyManager::GetReference().GetBuiltInFunc(call_func);
		if (pFunctor)
		{
			MXmlElement* pParam = pBuiltInFuncElement->FirstChildElement("param");
			while (pParam)
			{
				param_name.clear();
				param_value.clear();
				_Attribute(param_name, pParam, "name");
				_Attribute(param_value, pParam, "value");
				pFunctor->AddParam(param_name, param_value);

				pParam = pParam->NextSiblingElement("param");
			}

			if (!pFunctor->Run())
				++nFailCnt;
		}
		pBuiltInFuncElement = pBuiltInFuncElement->NextSiblingElement("builtin");
	}


	MAP_EDIT_TYPE eOpenType = MAP_EDIT_TYPE_LOGIC;
	if (bExitAfterOpenMap)
	{
		if (nFailCnt > 0)
		{
			eOpenType = MAP_EDIT_TYPE_EXIT_FAILD;
		}
		else
		{
			eOpenType = MAP_EDIT_TYPE_EXIT_SUCCESSED;
		}

		std::string& errorMsg = m_pControlMediator_->GetErrorString();
		if (!errorMsg.empty())
		{
			mlog("Error Msg Begin:\n");
			mlog("%s", errorMsg.c_str());
			mlog("Error Msg End\n");
			m_pControlMediator_->ClearErrorString();
		}
	}
	return eOpenType;
}

std::string CWorkSpaceScript::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	return "NOTE : Script workspace does not support save action";
}

void CWorkSpaceScript::OnResetResourceAfterSceneCleared(bool bOnDestroy)
{
}

void CWorkSpaceScript::OnBetweenCoreToolUpdateAndRS3Render()
{
}

void CWorkSpaceScript::OnWBDevAction()
{
}

void CWorkSpaceScript::OnWBHelperRender()
{
}
