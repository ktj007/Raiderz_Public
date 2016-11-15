/// @file C_ControlPopupActionImpl.h
#pragma once

#include "M_ToolFunctor.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_KeyEventHandler.h"

#include "M_ToolSceneManager.h"
#include "M_ToolCamera.h"

#include "VR_EnvWeather.h"

#include "WS_Field.h"

#include "C_XMLUserConfig.h"

class CRunGameFunctor : public CCoreToolFunctor
{
public:
	CRunGameFunctor(CControlMediator* _pControlMediator){ m_pControlMediator_ = _pControlMediator; }
	virtual ~CRunGameFunctor(){}

	virtual bool Run()
	{
		char szParams[512] = {0, };
		rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();

		rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();

		RVector vMousePos;

		RPICKINFO pickInfo;

		const rs3::RVector* pMousePos = m_pControlMediator_->GetHelperAction()->GetCurrentPickingPosition();
		rs3::RVector dir(pCamera->GetDirection());
		if(pMousePos == NULL) {
			pickInfo.inPickOrigin = pCamera->GetPosition();
			pickInfo.inPickTo = pCamera->GetPosition() + pCamera->GetDirection()*10000;
			bool bPicked = pToolSceneManager->PickSceneNode(pickInfo);
			if(!bPicked)
				return false;

			vMousePos = pickInfo.outPosition;
			vMousePos.z += 2000;
		}else
			vMousePos = *pMousePos;

		CControlMediator::MAP_INFO& map_info = m_pControlMediator_->m_mapInfo;
		if (!map_info.m_bLoaded)
			return false;

		if (map_info.m_bSceneEditableMap)
		{
			// 존 편집일 경우 ---
			string strZoneName = map_info.m_strSceneFile;
			int nIdx = strZoneName.length() - 9;	// ".zone.xml"
			if (nIdx >= 0) strZoneName.erase(nIdx);

			int nGameTime = 0;
			int nGameWeather = 0;
			std::string strEnvSet;

			CRollupEnvWeather* pEditWeather = m_pControlMediator_->GetViewPtr<CRollupEnvWeather>();
			if (pEditWeather)
			{
				int enable_view;
				pEditWeather->GetWeatherInfo(enable_view, nGameTime, nGameWeather, strEnvSet);

				if ( nGameTime < 0 ) nGameTime = 1;
				if ( nGameWeather < 0 )	nGameWeather = 0;
			}
			sprintf(szParams, "zonetest %s %.2f %.2f %.2f %.2f %.2f %.2f %d %d", strZoneName.c_str(),
				vMousePos.x, vMousePos.y, vMousePos.z, dir.x, dir.y, dir.z, nGameTime, nGameWeather);
		}
		else
		{
			// 필드 편집일 경우 ---

			int nFieldID = 0;
			char szFieldID[512] = {0, };
			strncpy(szFieldID, m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->m_fieldInfo.m_strFieldFile.c_str(), 12);
			nFieldID = atoi(szFieldID);	// FieldID를 알아내는 방법이 파일로부터 알아내는 방법밖에 없을까요? -_- - birdkr(2007-03-09)

			std::string strLoginID = CXMLUserConfig::GetReference().GetTextLoginID();
			std::string strIP = CXMLUserConfig::GetReference().GetTextLoginIP();

			sprintf(szParams, "login %s %s %d %.2f %.2f %.2f", strLoginID.c_str(), strIP.c_str(), nFieldID, vMousePos.x, vMousePos.y, vMousePos.z);
		}

		bool bPause = true;
		if(!m_pControlMediator_->GetHelperAction()->IsPause())
		{
			bPause = false; //bPause는 거짓이었다.(예전에 bPause 아님)
			m_pControlMediator_->GetHelperAction()->SetPauseToggle();
		}

		// 맵툴을 최소화시킴..
		AfxGetMainWnd()->ShowWindow(SW_SHOWMINIMIZED);

		string strCurrentDir = MGetCurrentDirectory();
		strCurrentDir += "..\\..\\";

		// 게임 실행
		SHELLEXECUTEINFO ShExecInfo = {0};
		ZeroMemory(&ShExecInfo, sizeof(ShExecInfo));

		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = "";
		ShExecInfo.lpFile = "Raiderz.exe";
		ShExecInfo.lpParameters = szParams;
		ShExecInfo.lpDirectory = strCurrentDir.c_str();
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		mlog("%s%s %s\n", ShExecInfo.lpDirectory, ShExecInfo.lpFile, ShExecInfo.lpParameters);
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

		// 게임 종료후 복구
		AfxGetMainWnd()->ShowWindow(SW_RESTORE);

		if(!bPause) //Pause가 거짓이였다면
			//거짓이었을때 체크했던 SetPauseToggle 활성화
			m_pControlMediator_->GetHelperAction()->SetPauseToggle();

		return true;
	}

	virtual const char* GetFuncName()
	{
		return "Run Game\tCtrl+G";
	}

private:
	CControlMediator* m_pControlMediator_;

};

class CSoulHuntKeyboardHelper : public CKeyEventHandlerHelper
{
public:
	CSoulHuntKeyboardHelper(CRunGameFunctor* _pRunGameFunctor)
	{
		m_pRunGameFunctor = _pRunGameFunctor;
	}
	virtual ~CSoulHuntKeyboardHelper(){}

	virtual void OnPressedCtrlCompositeHotKey(UINT _charParam, CControlMediator* _pMediator)
	{
		// Navigation mesh 만들기 ( ctrl + \ )
		if( 220 == _charParam && _pMediator->m_mapInfo.m_bEditSceneObject)
		{
			CRollupEtcPathFind* pPathFind = _pMediator->GetViewPtr<CRollupEtcPathFind>();
			if (pPathFind)
				pPathFind->OnBnClickedButtonPathCreate();
		}

		if ('G' == _charParam)
		{
			m_pRunGameFunctor->Run();
		}

		if ('E' == _charParam)
		{
		}

		if ('R' == _charParam)
		{
		}
	}

	virtual void OnPressedNoneCompositeHotKey(UINT _charParam, CControlMediator* _pMediator)
	{

		//네비게이션메시 토글
		if( 220 == _charParam)
		{
			CRollupEtcPathFind* pPathFind = _pMediator->GetViewPtr<CRollupEtcPathFind>();
			if (pPathFind)
			{
				BOOL b = (pPathFind->m_btnCheckNaviActivity.GetCheck() ? FALSE : TRUE);
				pPathFind->m_btnCheckNaviActivity.SetCheck(b);
				pPathFind->SetViewNavigationMesh(b);
			}
		}

		// toggle cl2
		if (VK_F12 == _charParam)
		{
			CRollupEtcCollision* pCollision = _pMediator->GetViewPtr<CRollupEtcCollision>();
			if (pCollision)
				pCollision->Cl2ViewToggle();
		}

	}

private:
	CRunGameFunctor* m_pRunGameFunctor;
};
