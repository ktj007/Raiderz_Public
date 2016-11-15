#include "StdAfx.h"
#include "SaveNotifier.h"
#include "TTalentEffectMgr.h"
#include "TTalentEventMgr.h"
#include "FileInfo.h"

System::Windows::Forms::ToolStripStatusLabel^ SaveNotifier::RefreshSaveNoticeLabel( System::Windows::Forms::ToolStripStatusLabel^ label, bool bSave, System::String^ Text )
{
	if (bSave)
	{
		if(label == nullptr)
		{
			label = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			GlobalObjects::g_pMainForm->statusStrip->Items->Add(label);
			label->Name = Text + "Label";
			label->Size = System::Drawing::Size(80, 20);
			label->Margin.Left = 10;
			label->Padding.Left = 3;
			label->Padding.Right = 3;
			label->BackColor = System::Drawing::Color::Red;
			label->Text = Text;
			label->BorderSides = ToolStripStatusLabelBorderSides::All;
			label->BorderStyle = Border3DStyle::Flat;
		}
		return label;
	}
	else
	{
		if(label != nullptr)
		{
			GlobalObjects::g_pMainForm->statusStrip->Items->Remove(label);
			return nullptr;
		}
		return label;
	}
	return label;
}

System::Void SaveNotifier::SaveTalentPosInfoNotice(bool bsave)
{
	TalentPosInfoStatusLabel = RefreshSaveNoticeLabel(TalentPosInfoStatusLabel, bsave, L"talent_pos_info.xml");
}
System::Void SaveNotifier::SaveTalentExtNotice(bool bsave)
{
	TalentExtStatusLabel = RefreshSaveNoticeLabel(TalentExtStatusLabel, bsave, L"talent_ext.xml");
}

System::Void SaveNotifier::SaveTalentEffectNotice( bool bsave )
{
	TalentEffectStatusLabel = RefreshSaveNoticeLabel(TalentEffectStatusLabel, bsave, L"talent_Effect_info.xml");
}

System::Void SaveNotifier::SaveTalentHitInfoNotice( bool bsave )
{
	TalentHitInfoStatusLabel = RefreshSaveNoticeLabel(TalentHitInfoStatusLabel, bsave, L"talent_Hit_info.xml");
}

System::Void SaveNotifier::SaveMeshInfoNotice( bool bsave )
{
	MeshInfoStatusLabel = RefreshSaveNoticeLabel(MeshInfoStatusLabel, bsave, L"mesh_info.xml");
}

System::Void SaveNotifier::SaveChangeMtrlItemNotice( bool bsave )
{
	ChangeMtrlItemStatusLabel = RefreshSaveNoticeLabel(ChangeMtrlItemStatusLabel, bsave, L"xitem_changemtrl.xml");
}

System::Void SaveNotifier::SaveChangeMtrlNPCNotice( bool bsave )
{
	ChangeMtrlNPCStatusLabel = RefreshSaveNoticeLabel(ChangeMtrlNPCStatusLabel, bsave, L"npc_changemtrl.xml");
}

System::Void SaveNotifier::SaveAnimationNotice( bool bsave )
{
	AnimaionLabel = RefreshSaveNoticeLabel(AnimaionLabel, bsave, L"elu.animation.xml");
}

bool SaveNotifier::SaveTalentEventInfo()
{
	DWORD dwTime = GetTickCount();
	if(g_pMainApp->m_pEventMgr->Save())
	{
		SetSaveTalentEvent(false);
		SetSaveTalentTime(false);
		mlog("정보 : talent_ext.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
		return true;
	}
	else
	{
		mlog("에러 : 탈렌트이벤트정보가 저장이 되지않습니다. \r");
		return false;
	}
}

bool SaveNotifier::SaveTalentHitInfo()
{
	DWORD dwTime = GetTickCount();
	if (g_pMainApp->m_TTalentInfoMgr.SaveTalentHitInfo())
	{
		SetSaveTalentHitInfo(false);
		mlog("정보 : talent_Hit_info.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);		
		return true;
	}
	else
	{
		mlog("에러 : 탈렌트히트정보가 저장이 되지않습니다. \r");
		return false;
	}
}

bool SaveNotifier::SaveTalentEffectInfo()
{
	DWORD dwTime = GetTickCount();
	if(g_pMainApp->m_pTalentEffectMgr->Save())
	{
		SetSaveTalentEffect(false);
		mlog("정보 : talent_Effect_info.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
		return true;
	}
	else
	{
		mlog("에러 : 탈렌트이펙트정보가 저장이 되지않습니다. \r");
		return false;
	}
}

void SaveNotifier::SaveTalentPosInfo()
{
	DWORD dwTime = GetTickCount();
	g_pMainApp->m_TTalentInfoMgr.SaveTalentPosInfo();
	SetSaveTalentPosInfo(false);
	mlog("정보 : talent_Pos_info.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
}

void SaveNotifier::SaveMeshInfo()
{
	DWORD dwTime = GetTickCount();
	TMeshInfoMgr::GetInstance().Save();
	SetSaveMeshInfo(false);
	mlog("정보 : mesh_info.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
}

void SaveNotifier::SaveChangeMtrlItem()
{
	DWORD dwTime = GetTickCount();
	g_pMainApp->m_Mtrl.Export_Item();
	SetSaveChangeMtrlItem(false);
	mlog("정보 : xitem_changemtrl.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
}

void SaveNotifier::SaveChangeMtrlNPC()
{
	DWORD dwTime = GetTickCount();
	g_pMainApp->m_Mtrl.Export_NPC();
	SetSaveChangeMtrlNPC(false);
	mlog("정보 : npc_changemtrl.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
}

void SaveNotifier::SaveAnimation()
{
	DWORD dwTime = GetTickCount();
	g_pMainApp->SaveModelAnimation();
	SetSaveAnimation(false);
	mlog("정보 : animation.xml 저장완료 : %d ms\r", GetTickCount() - dwTime);
}

void SaveNotifier::SetSaveTalentTime( bool val )
{
	m_bSaveTalentTime = val;
	SaveTalentExtNotice(val);
}

void SaveNotifier::SetSaveTalentEvent( bool val )
{
	m_bSaveTalentEvent = val;
	SaveTalentExtNotice(val);
}

void SaveNotifier::SetSaveTalentEffect( bool val )
{
	m_bSaveTalentEffect = val;
	SaveTalentEffectNotice(val);
}

void SaveNotifier::SetSaveTalentHitInfo( bool val )
{
	m_bSaveTalentHitInfo = val;
	SaveTalentHitInfoNotice(val);
}

void SaveNotifier::SetSaveTalentPosInfo( bool val )
{
	m_bSaveTalentPosInfo = val;
	SaveTalentPosInfoNotice(val);
}

void SaveNotifier::SetSaveMeshInfo( bool val )
{
	m_bSaveMeshInfo = val;
	SaveMeshInfoNotice(val);
}

void SaveNotifier::SetSaveChangeMtrlItem( bool val )
{
	m_bSaveChangeMtrlItem = val;
	SaveChangeMtrlItemNotice(val);
}

void SaveNotifier::SetSaveChangeMtrlNPC( bool val )
{
	m_bSaveChangeMtrlNPC = val;
	SaveChangeMtrlNPCNotice(val);
}

void SaveNotifier::SetSaveAnimation(bool val)
{
	m_bSaveAnimation = val;
	SaveAnimationNotice(val);
}

bool SaveNotifier::IsSaveDone()
{
	if( m_bSaveTalentTime
		|| m_bSaveTalentEvent
		|| m_bSaveTalentEffect
		|| m_bSaveTalentHitInfo
		|| m_bSaveTalentPosInfo
		|| m_bSaveMeshInfo
		|| m_bSaveChangeMtrlItem
		|| m_bSaveChangeMtrlNPC
		|| m_bSaveAnimation )
	{
		return false;
	}

	return true;
}

void SaveNotifier::Init()
{
	m_bSaveTalentTime	= false;
	m_bSaveTalentEvent	= false;
	m_bSaveTalentEffect	= false;
	m_bSaveTalentHitInfo= false;
	m_bSaveTalentPosInfo= false;
	m_bSaveMeshInfo		= false;
	m_bSaveChangeMtrlItem = false;
	m_bSaveChangeMtrlNPC  = false;
	m_bSaveAnimation	= false;
}

void SaveNotifier::SaveAll()
{
	if(IsSaveDone()) return;

	mlog("정보 : ============ 전체저장시작\r");

	DWORD dwStartTime = GetTickCount();
	g_pMainApp->SetProgressBarValue();

	if (m_bSaveTalentEvent || m_bSaveTalentTime)
	{
		SaveTalentEventInfo();
	}
	if(m_bSaveTalentEffect)
	{
		SaveTalentEffectInfo();
	}
	if (m_bSaveTalentHitInfo)
	{
		SaveTalentHitInfo();
	}
	if (m_bSaveTalentPosInfo)
	{
		SaveTalentPosInfo();
	}
	if (m_bSaveMeshInfo)
	{
		SaveMeshInfo();
	}
	if (m_bSaveChangeMtrlItem)
	{
		SaveChangeMtrlItem();
	}
	if (m_bSaveChangeMtrlNPC)
	{
		SaveChangeMtrlNPC();
	}
	if (m_bSaveAnimation)
	{
		SaveAnimation();
	}

	SaveCeckSum();

	g_pMainApp->SetProgressBarValue(100);

	mlog("정보 : ============ 전체저장완료 : %d ms\r", GetTickCount() - dwStartTime);
}

void SaveNotifier::SaveCeckSum()
{
	vector<tstring>	FileList;
	FileList.push_back(FILENAME_TALENT_EXT_TOOL);
	FileList.push_back(FILENAME_TALENT_HIT_INFO);
	FileList.push_back(FILENAME_TOOL_MESH_INFO);
	//FileList.push_back(FILENAME_BUFFEFFECTINFO_TOOL);
	FileList.push_back(FILENAME_ITEM_CHANGEMTRL_TOOL);
	FileList.push_back(FILENAME_NPC_CHANGEMTRL_TOOL);
	FileList.push_back(FILENAME_TOOL_MESH_INFO);
	//FileList.push_back(FILENAME_TALENT_EFFECTINFO_TOOL);
	FileList.push_back(SERVER_FILENAME_TALENT_POS_INFO);


	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaietElement = new MXmlElement("maiet");

	for (vector<tstring>::iterator itor = FileList.begin();
		itor != FileList.end();itor++)
	{
		string filename = *itor;
		if (IsExist(filename.c_str()))
		{
			string name = GetRemovePathName(filename);
			DWORD dCheckSum = GetFileCheckSum((char*)filename.c_str());

			char szBuf[128];
			sprintf_s(szBuf, "%x", dCheckSum);

			MXmlElement* pMeshElement = new MXmlElement("File");
			_SetAttribute(pMeshElement, "name", name);
			_SetAttribute(pMeshElement, "checksum", szBuf);

			pMaietElement->LinkEndChild(pMeshElement);
		}
	}

	root->LinkEndChild(pMaietElement);

	xml.SaveFile("../../data/system/FileChecksum.xml");

	File::Copy("../../data/system/FileChecksum.xml", "../../Dev/GameServer/system/FileChecksum.xml", true);

	mlog("정보 : ====== 체크섬저장 ======\r");
}

void SaveNotifier::ResetModelSave()
{
	SetSaveAnimation(false);
	SetSaveMeshInfo(false);
}

void SaveNotifier::ResetSaveAll()
{
	Init();
}