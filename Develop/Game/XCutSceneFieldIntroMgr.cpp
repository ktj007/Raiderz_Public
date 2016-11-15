#include "stdafx.h"
#include "XCutSceneFieldIntroMgr.h"
#include "RSceneManager.h"
#include "XCutSceneObject.h"
#include "XNPCInfo.h"
#include "XEffectInfo.h"
#include "XEffectInvoker.h"
#include "XPost_GM.h"
#include "XMyPlayer.h"
#include "XModuleEntity.h"

XCutSceneFieldIntroMgr::XCutSceneFieldIntroMgr()
{
	Reset();
}

void XCutSceneFieldIntroMgr::Reset()
{
	m_nBlockStartCnt = 0;
	m_nBlockEndCnt = 0;
	m_nPreIntroCutSceneID = INVALID_ID;
	m_bObjectLoading = false;
	m_mapIntroCutScene.clear();
	vecMyPlayerPos.Set(0.0f, 0.0f, 0.0f);
}

void XCutSceneFieldIntroMgr::ReleaseResource()
{
	for (std::vector<rs3::RResource*>::iterator itr = m_vecPreLoadEluRs3.begin(); itr != m_vecPreLoadEluRs3.end(); ++itr)
	{
		rs3::REngine::GetSceneManager().ReleaseResource(*itr);
	}
	m_vecPreLoadEluRs3.clear();
}

bool XCutSceneFieldIntroMgr::SetIntroCutSceneEnv(int* nTime, int* nWeather)
{
	int nFieldID = gvar.World.GetFieldID();
	int nIntroCutSceneID = gvar.World.GetIntroCutSceneID();

	if(nIntroCutSceneID == INVALID_ID)
		return false;

	map<int, SIntroCutSceneInfo >::iterator itr = m_mapIntroCutScene.find(nFieldID);
	if (itr != m_mapIntroCutScene.end())
	{
		SIntroCutSceneInfo& info = itr->second;

		if(info.m_bIntroCutSceneEnv == false)
		{
			info.m_bIntroCutSceneEnv = true;
			*nTime = info.m_nTime;
			*nWeather = info.m_nWeather;

			return true;
		}
	}

	return false;
}

void XCutSceneFieldIntroMgr::FillResourceData(SIntroCutSceneInfo& introCutSceneInfo)
{
	int nIntroCutSceneID = gvar.World.GetIntroCutSceneID();

	wchar_t strFileName[256] = {0, };
	swprintf_s(strFileName, L"%s%d%s", PATH_CUTSCENE, nIntroCutSceneID, FILEEXT_CUTSCENE);

	MXml aXml;
	if (!aXml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName).c_str()))
		return;

	MXmlElement* pRootElement = aXml.DocHandle().FirstChildElement("maiet").Element();
	if (!pRootElement)
		return;

	//Event(time, Weather)
	MXmlElement* pEventElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_EVENT)));
	if(pEventElement)
	{
		MXmlElement* pEnvElement = pEventElement->FirstChildElement("env");
		if(pEnvElement)
		{
			MXmlElement* pEnvKeyElement = pEnvElement->FirstChildElement("envkey");
			if(pEnvKeyElement)
			{
				std::tstring strType;
				_Attribute(strType, pEnvKeyElement, "envtime");
				introCutSceneInfo.m_nTime = _ttoi( strType.c_str() );
				_Attribute(strType, pEnvKeyElement, "envweather");
				introCutSceneInfo.m_nWeather = _ttoi( strType.c_str() );
			}
		}
	}

	//Event(Effect)
	pEventElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_EVENT)));
	if(pEventElement)
	{
		MXmlElement* pEffectElement = pEventElement->FirstChildElement("effect");
		while(pEffectElement)
		{
			MXmlElement* pEffectKeyElement = pEffectElement->FirstChildElement("effectkey");
			while(pEffectKeyElement)
			{
				std::tstring strType;
				_Attribute(strType, pEffectKeyElement, "effectname");

				XEffectAdvenceInvoker advenceInvoker;
				advenceInvoker.Invoke(strType);

				pEffectKeyElement = pEffectKeyElement->NextSiblingElement("effectkey");
			}
			pEffectElement = pEffectElement->NextSiblingElement("effect");
		}
	}



	//Actor(NPC)
	MXmlElement* pActorElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_ACTOR)));

	while (pActorElement)
	{
		std::tstring strType;
		_Attribute(strType, pActorElement, "type");

		int nNPCID;
		if (strType.find(_T("npc_")) != std::tstring::npos)
		{
			// npc_ : 4
			nNPCID = _ttoi( strType.substr(4).c_str() );
			std::tstring strNPCPath = PATH_MODEL + info.npc->Get(nNPCID)->strMeshPath + info.npc->Get(nNPCID)->strMeshName + L".elu" ;

			bool bCompareData = false;
			for (std::vector<wstring>::iterator itr = introCutSceneInfo.m_vecPreLoadElu.begin(); itr != introCutSceneInfo.m_vecPreLoadElu.end(); ++itr)
			{
				if(itr->compare(strNPCPath) == 0)
				{
					bCompareData = true;
					break;
				}
			}

			if(bCompareData == false)
			{
				introCutSceneInfo.m_vecPreLoadElu.push_back(strNPCPath);
			}
		}

		pActorElement = pActorElement->NextSiblingElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_ACTOR)));
	}
}

bool XCutSceneFieldIntroMgr::PreLoadIntroCutScene()
{
	int nFieldID = gvar.World.GetFieldID();
	int nIntroCutSceneID = gvar.World.GetIntroCutSceneID();

	if(nIntroCutSceneID == INVALID_ID)
		return false;

	//게임 시작시 인트로 컷씬을 보여줘야 된다면 처음 한번 인트로 컷씬 맵 생성
	if(m_mapIntroCutScene.empty())
	{
		m_mapIntroCutScene[nFieldID] = SIntroCutSceneInfo( nIntroCutSceneID);

		FillResourceData(m_mapIntroCutScene[nFieldID]);

		SIntroCutSceneInfo& info = m_mapIntroCutScene[nFieldID];
		// 리소스 미리 생성
		for (std::vector<wstring>::iterator itr = info.m_vecPreLoadElu.begin(); itr != info.m_vecPreLoadElu.end(); ++itr)
		{
			rs3::RMesh* pResource = rs3::REngine::GetSceneManager().CreateResource<rs3::RMesh>( MLocale::ConvUTF16ToAnsi(itr->c_str()).c_str() );
			pResource->BuildAll();
			m_vecPreLoadEluRs3.push_back(pResource);
		}
	}

	return true;
}

bool XCutSceneFieldIntroMgr::beginIntroCutScene()
{
	int nIntroCutSceneID = gvar.World.GetIntroCutSceneID();
	
	if(nIntroCutSceneID != INVALID_ID && m_nPreIntroCutSceneID != nIntroCutSceneID)
	{
		//인트로 컷씬 플레이시 캐릭터를 원점으로 옮겨 다른 플레이어가 볼수 없도록 한다.
		vecMyPlayerPos = gvar.Game.pMyPlayer->GetModuleEntity()->GetPosition();
		vec3 vecMovePos(1.0f, 1.0f, 1.0f);
		XPostGM_Move(gvar.World.GetFieldID(), vecMovePos);

		m_nPreIntroCutSceneID = nIntroCutSceneID;
		XPostCutscene_BeginReq(nIntroCutSceneID);

		if ( global.sound)
			global.sound->SetSilence( true);

		return true;
	}

	return false;
}

void XCutSceneFieldIntroMgr::RenderMaskForCutScene()
{
	if (m_nBlockStartCnt < m_nBlockEndCnt)
	{
		++m_nBlockStartCnt;
		this->RenderBlackScreen();
	}
	else if(m_bObjectLoading == true)
	{
		this->RenderBlackScreen();
	}
}

void XCutSceneFieldIntroMgr::RenderBlackScreen()
{
	int width = global.ui->GetMint()->GetWorkspaceWidth();
	int height = global.ui->GetMint()->GetWorkspaceHeight();

	global.ui->GetDC()->BeginDraw();
	global.ui->GetDC()->SetColor( MCOLOR( 0, 0, 0, 255));
	global.ui->GetDC()->FillRectangle( 0, 0, width, height);
	global.ui->GetDC()->EndDraw();
}

void XCutSceneFieldIntroMgr::MoveMyPlayerPos()
{
	XPostGM_Move(gvar.World.GetFieldID(), vecMyPlayerPos);
}