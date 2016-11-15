#include "stdafx.h"
#include "XReplayAnalyzer.h"
#include "XStrings.h"
#include "MLocale.h"
#include "XTalentInfoMgr.h"

XReplayAnalyzer::XReplayAnalyzer() : m_pReplay(new XReplay())
{

}

XReplayAnalyzer::~XReplayAnalyzer()
{
	SAFE_DELETE(m_pReplay);
}

bool XReplayAnalyzer::Load( const wchar_t* szFileName )
{
	if (m_pReplay->Load(szFileName) == false) return false;

	m_strReplayFileName = szFileName;

	return true;
}


void XReplayAnalyzer::Analyze()
{
	wstring strSaveFileName = m_strReplayFileName + L".xml";

	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild( pDec);

	MXmlElement* pRootElement = new MXmlElement( "ReplayAnalyze");
	xml.LinkEndChild( pRootElement);

	SetReplayInfo(pRootElement, &xml);
	SetCharInfo(pRootElement, &xml);
	SetCommands(pRootElement);

	xml.SaveFile( MLocale::ConvUTF16ToAnsi(strSaveFileName.c_str()).c_str());

	wstring strDPSFile = m_strReplayFileName + L".dps.csv";
	SaveDPSReport(xml, strDPSFile.c_str());
}

void XReplayAnalyzer::SetReplayInfo( MXmlElement* pRootElement, MXml* pXml )
{
	XReplayGameInfo& replayGameInfo = m_pReplay->GetGameInfo();

	MXmlElement* pReplayInfoElement = new MXmlElement( "ReplayInfo");
	pRootElement->LinkEndChild(pReplayInfoElement);

	_SetAttribute(pReplayInfoElement, "start_time", (int)(replayGameInfo.fStartTime));
	_SetAttribute(pReplayInfoElement, "finish_time", (int)(replayGameInfo.fFinishTime));
	_SetAttribute(pReplayInfoElement, "start_field_id", replayGameInfo.nFieldID);
	_SetAttribute(pReplayInfoElement, "start_gametime", MLocale::ConvUTF16ToAnsi(XStrings::GameTime(replayGameInfo.nGameTime)));
	_SetAttribute(pReplayInfoElement, "start_weather", MLocale::ConvUTF16ToAnsi(XStrings::WeatherSimple(replayGameInfo.nWeather)));
}

void XReplayAnalyzer::SetCharInfo( MXmlElement* pRootElement, MXml* pXml )
{
	MXmlElement* pCharInfoElement = new MXmlElement( "CharInfo");
	pRootElement->LinkEndChild(pCharInfoElement);

	for (XObserverCommandList::iterator itorItem = m_pReplay->GetGameInfo().ReplayCommandList.begin();
		itorItem != m_pReplay->GetGameInfo().ReplayCommandList.end(); ++itorItem)
	{
		XObserverCommandItem* pItem = (*itorItem);
		MCommand* pCommand = pItem->pCommand;

		switch (pCommand->GetID())
		{
		case MC_CHAR_MYINFO:
			{
				TD_MYINFO vecTDMyInfo;
				vector<TD_ITEM>	vecTDInventory;
				vector<TD_ITEM> vecTDEquipment;
				vector<int> vecTalent;
				vector<TD_PLAYERQUEST> vecTDPlayerQuest;
				vector<TD_PALETTE> vecTDPalette;
				vector<TD_FACTION> vecTDFaction;
				vector<int> vecRecipeID;

				if (pCommand->GetSingleBlob(vecTDMyInfo, 0)==false) continue;
				if (pCommand->GetBlob(vecTDInventory, 1)==false) continue;
				if (pCommand->GetBlob(vecTDEquipment, 2)==false) continue;
				if (pCommand->GetBlob(vecTalent, 3)==false) continue;
				if (pCommand->GetBlob(vecTDPlayerQuest, 4)==false) continue;
				if (pCommand->GetBlob(vecTDPalette, 5)==false) continue;
				if (pCommand->GetBlob(vecTDFaction, 6)==false) continue;
				if (pCommand->GetBlob(vecRecipeID, 7)==false) continue;

				m_MyInfo.SetFrom_TD_MYINFO(vecTDMyInfo, 
					vecTDInventory, 
					vecTDEquipment, 
					vecTalent, 
					vecTDPlayerQuest, 
					vecTDPalette[0], 
					vecTDFaction, 
					vecRecipeID);
			}
			break;
		}
	}

	_SetAttribute(pCharInfoElement, "name", MLocale::ConvUTF16ToAnsi(m_MyInfo.ChrInfo.szName));
	_SetAttribute(pCharInfoElement, "level", m_MyInfo.ChrInfo.nLevel);

	_SetAttribute(pCharInfoElement, "uiid", (int)m_pReplay->GetGameInfo().nUIID);
	_SetAttribute(pCharInfoElement, "uid_high", (int)m_pReplay->GetGameInfo().MyUID.High);
	_SetAttribute(pCharInfoElement, "uid_low", (int)m_pReplay->GetGameInfo().MyUID.Low);

	// talent

	vector<int> vecTalentID;
	m_MyInfo.Talent.GetAllTalentID(vecTalentID);
	for (size_t i = 0; i < vecTalentID.size(); i++)
	{
		XTalentInfo* pTalentInfo = info.talent->Get(vecTalentID[i]);
		if (pTalentInfo)
		{
			MXmlElement* pTalentElement = new MXmlElement( "Talent");
			pCharInfoElement->LinkEndChild(pTalentElement);

			_SetAttribute(pTalentElement, "id", pTalentInfo->m_nID);
			_SetAttribute(pTalentElement, "name", MLocale::ConvUTF16ToAnsi(pTalentInfo->GetName()));
		}
	}
}

void XReplayAnalyzer::SetCommands( MXmlElement* pRootElement )
{
	XReplayGameInfo& replayGameInfo = m_pReplay->GetGameInfo();

	MXmlElement* pCommandsElement = new MXmlElement( "Commands");
	pRootElement->LinkEndChild(pCommandsElement);

	for (XObserverCommandList::iterator itorItem = m_pReplay->GetGameInfo().ReplayCommandList.begin();
		itorItem != m_pReplay->GetGameInfo().ReplayCommandList.end(); ++itorItem)
	{
		XObserverCommandItem* pItem = (*itorItem);
		MCommand* pCommand = pItem->pCommand;
		float fCommandTime = pItem->fTime;

		switch (pCommand->GetID())
		{
		case MC_ACTION_MOVE:
			{
				MUID uidPlayer;
				if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) continue;

				if (uidPlayer == replayGameInfo.MyUID)
				{
					MCommandParameter* pParam = pCommand->GetParameter(1);
					if(pParam->GetType()!=MPT_BLOB) continue;
					TD_PC_MOVE* pMI = (TD_PC_MOVE*)pParam->GetPointer();

					MXmlElement* pCommandElement = new MXmlElement( "Move");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "pos_x", pMI->vTarPos.x);
					_SetAttribute(pCommandElement, "pos_y", pMI->vTarPos.y);
					_SetAttribute(pCommandElement, "pos_z", pMI->vTarPos.z);
				}

			}
			break;
		case MC_ACTION_USE_TALENT:
			{
				MUID uidPlayer;
				int nTalentID;
				vec3 vDir, vPos;
				if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) continue;
				if (pCommand->GetParameter(&nTalentID,	1, MPT_INT)==false) continue;
				if (pCommand->GetParameter(&vDir,		2, MPT_VEC)==false) continue;
				if (pCommand->GetParameter(&vPos,		3, MPT_VEC)==false) continue;

				if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "UseTalent");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "talent", nTalentID);
					_SetAttribute(pCommandElement, "pos_x", vPos.x);
					_SetAttribute(pCommandElement, "pos_y", vPos.y);
					_SetAttribute(pCommandElement, "pos_z", vPos.z);
				}
			}
			break;
		case MC_ACTION_ACT_TALENT:
			{
				MUID uidPlayer;
				int nTalentID;

				if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) continue;
				if (pCommand->GetParameter(&nTalentID,	1, MPT_INT)==false) continue;

				if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "ActTalent");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "talent", nTalentID);
				}

			}
			break;
		case MC_ACTION_DIE:
			{
				UIID nVictimUIID;
				if ( pCommand->GetParameter( &nVictimUIID, 0, MPT_USHORT) == false) continue;

				if (nVictimUIID == replayGameInfo.nUIID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "Die");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
				}
			}
			break;
		case MC_ACTION_TALENT_HIT:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_SINGLE_BLOB) continue;
				TD_TALENT_HIT* tdtalentHit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "TalentHit");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "attacker_uiid", (int)tdtalentHit->nAttackerUIID);
					_SetAttribute(pCommandElement, "victim_uiid", (int)tdtalentHit->nVictimUIID);
					_SetAttribute(pCommandElement, "talent", (int)tdtalentHit->nTalentID);
					_SetAttribute(pCommandElement, "damage", (int)tdtalentHit->nDamage);
					_SetAttribute(pCommandElement, "mf_weight", (int)tdtalentHit->nMFWeight);
					_SetAttribute(pCommandElement, "mf_state", (int)tdtalentHit->nMFState);
				}
			}
			break;
		case MC_ACTION_TALENT_HIT_NO_MF:
			{
				UIID nVictimUIID, nAttackerUIID;
				int nTalentID;

				if (pCommand->GetParameter(&nVictimUIID,	0, MPT_USHORT)==false) continue;
				if (pCommand->GetParameter(&nAttackerUIID,	1, MPT_USHORT)==false) continue;
				if (pCommand->GetParameter(&nTalentID,		2, MPT_INT)==false) continue;

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "TalentHitNoMF");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "attacker_uiid", (int)nAttackerUIID);
					_SetAttribute(pCommandElement, "victim_uiid", (int)nVictimUIID);
					_SetAttribute(pCommandElement, "talent", (int)nTalentID);
				}
			}
			break;
		case MC_ACTION_TALENT_HIT_KNOCKBACK_MF:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_SINGLE_BLOB) continue;
				TD_TALENT_HIT* tdtalentHit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

				MCommandParameter* pParam1 = pCommand->GetParameter(1);
				if(pParam1->GetType()!=MPT_SINGLE_BLOB) continue;
				TD_TALENT_HIT_EX_KNOCKBACK* tdtalentHitKnockback = static_cast<TD_TALENT_HIT_EX_KNOCKBACK*>(pParam1->GetPointer());

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "TalentHit");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "attacker_uiid", (int)tdtalentHit->nAttackerUIID);
					_SetAttribute(pCommandElement, "victim_uiid", (int)tdtalentHit->nVictimUIID);
					_SetAttribute(pCommandElement, "talent", (int)tdtalentHit->nTalentID);
					_SetAttribute(pCommandElement, "damage", (int)tdtalentHit->nDamage);
					_SetAttribute(pCommandElement, "mf_weight", (int)tdtalentHit->nMFWeight);
					_SetAttribute(pCommandElement, "mf_state", (int)tdtalentHit->nMFState);
				}
			}
			break;
		case MC_ACTION_BUFF_HIT:
			{
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if(pParam->GetType()!=MPT_SINGLE_BLOB) continue;
				TD_BUFF_HIT* tdBuffHit = (TD_BUFF_HIT*)pParam->GetPointer();

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "BuffHit");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "victim_uiid", (int)tdBuffHit->nVictimUIID);
					_SetAttribute(pCommandElement, "buff", (int)tdBuffHit->nBuffID);
					_SetAttribute(pCommandElement, "damage", (int)tdBuffHit->nDamage);
					_SetAttribute(pCommandElement, "mf_weight", (int)tdBuffHit->nMFWeight);
					_SetAttribute(pCommandElement, "mf_state", (int)tdBuffHit->nMFState);
				}
			}
			break;
		case MC_ACTION_BUFF_HIT_NO_MF:
			{
				UIID nEntityUIID;
				int nTalentID;

				if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) continue;
				if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) continue;

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "BuffHitNoMF");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "victim_uiid", (int)nEntityUIID);
					_SetAttribute(pCommandElement, "talent", (int)nTalentID);
				}
			}
			break;
		case MC_ACTION_TALENT_HIT_MISS:
			{
				UIID nVictimUIID, nAttackerUIID;
				int nAttackTalentID;

				if (pCommand->GetParameter(&nVictimUIID,		0, MPT_USHORT)==false) continue;
				if (pCommand->GetParameter(&nAttackerUIID,		1, MPT_USHORT)==false) continue;
				if (pCommand->GetParameter(&nAttackTalentID,	2, MPT_INT)==false) continue;

				//if (uidPlayer == replayGameInfo.MyUID)
				{
					MXmlElement* pCommandElement = new MXmlElement( "TalentHitDodge");
					pCommandsElement->LinkEndChild(pCommandElement);

					_SetAttribute(pCommandElement, "time", fCommandTime);
					_SetAttribute(pCommandElement, "attacker_uiid", (int)nAttackerUIID);
					_SetAttribute(pCommandElement, "victim_uiid", (int)nVictimUIID);
					_SetAttribute(pCommandElement, "talent", (int)nAttackTalentID);
				}
			}
			break;

		} // switch
	} // for
}

void XReplayAnalyzer::SaveDPSReport( MXml& xmlAnalyze, const wchar_t* szFileName )
{
	FILE *pFile = fopen(MLocale::ConvUTF16ToAnsi(szFileName).c_str(), "w+");
	if (!pFile) return;

	fprintf(pFile, "시간(ms), 데미지, 탤런트 ID, 탤런트 이름\n");

	float fStartTime = 0;
	float fFinishTime = 0;
	int nMyUIID = 0;

	MXmlHandle docHandle = xmlAnalyze.DocHandle();

	MXmlElement* pElement = docHandle.FirstChild( "ReplayAnalyze").FirstChildElement().Element();


	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "ReplayInfo"))
		{
			_Attribute(&fStartTime,		pElement, "start_time");
			_Attribute(&fFinishTime,		pElement, "finish_time");
		}

		else if (!_stricmp(pElement->Value(), "CharInfo"))
		{
			_Attribute(&nMyUIID,		pElement, "uiid");
		}

		else if (!_stricmp(pElement->Value(), "Commands"))
		{
			MXmlElement* pChild = pElement->FirstChildElement();

			for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
			{
				float fTime = 0.0f;
				int nUIID = 0;
				int nDamage = 0;
				int nTalentID = 0;

				if (!_stricmp(pChild->Value(), "TalentHit"))
				{
					_Attribute(&fTime,			pChild, "time");
					_Attribute(&nUIID,			pChild, "attacker_uiid");		
					_Attribute(&nDamage,		pChild, "damage");		
					_Attribute(&nTalentID,		pChild, "talent");		
				}
				//else if (!_stricmp(pChild->Value(), "TalentHitNoMF"))
				//{
				//	_Attribute(&fTime,			pChild, "time");
				//	_Attribute(&nUIID,			pChild, "attacker_uiid");		
				//	_Attribute(&nDamage,		pChild, "damage");		
				//	_Attribute(&nTalentID,		pChild, "talent");		
				//}

				if (fTime != 0.0f)
				{
					fTime -= fStartTime;

					XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
					if (nMyUIID == nUIID && pTalentInfo)
					{
						fprintf(pFile, "%d, %d, %d, %s\n", (int)(fTime * 1000), nDamage, nTalentID, MLocale::ConvUTF16ToAnsi(pTalentInfo->GetName()).c_str());
					}

				}
			}

		}
	}

	fclose(pFile);
}