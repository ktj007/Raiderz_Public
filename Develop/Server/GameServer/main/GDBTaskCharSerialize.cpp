#include "stdafx.h"
#include "GDBTaskCharSerialize.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GItemExpirer.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GPlayerTalent.h"
#include "GPalette.h"
#include "GFatigueSystem.h"
#include "GFieldInfoMgr.h"
#include "GItemManager.h"
#include "GPaletteSystem.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GSystem.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GCalculator.h"
#include "GPlayerBattleArena.h"
#include "GGlobal.h"
#include "GBattleArenaMgr.h"
#include "CCommandTable_Login.h"
#include "GPlayerFactions.h"
#include "GFactionInfoMgr.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerFieldDynamic.h"
#include "GActorCooltimeChecker.h"
#include "GPlayerCutscene.h"
#include "GPlayerRecipe.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "GDBManager.h"
#include "GGuildSystem.h"
#include "GGuildMgr.h"
#include "CSBasicAttributeCalculator.h"
#include "GPlayerEmblem.h"
#include "GPlayerMailBox.h"
#include "GServer.h"
#include "GItemDBCache.h"
#include "GCharacterDBCache.h"
#include "GPlayerDBCache.h"
#include "GDBCacheSystem.h"
#include "GQObjectiveInfo.h"
#include "GClientMoveServerRouter.h"
#include "GDBTaskDataConnection.h"
#include "GEquipmentSlot.h"
#include "GConfig.h"


GDBTaskCharSerialize::GDBTaskCharSerialize(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB_SELECT_CHAR, SDBTID_CHARSERIALIZE, GDBTaskCharSerialize::END)
{
	m_Data.uidPlayer					= uidReqPlayer;
	
	m_Data.CharInfo.ActivePaletteID		= PALETTENUM_1;
	m_Data.CharInfo.PrimaryPaletteID	= PALETTENUM_MAX;
	m_Data.CharInfo.SecondaryPaletteID	= PALETTENUM_MAX;
}

GDBTaskCharSerialize::~GDBTaskCharSerialize()
{

}

void GDBTaskCharSerialize::Input(const int64 nCID, const UIID& nUIID, bool bReload)
{
	m_Data.nCID		= nCID;
	m_Data.nUIID	= nUIID;	
	m_Data.bReload	= bReload;
}

void GDBTaskCharSerialize::OnExecute(mdb::MDatabase& rfDB)
{
	if (!GetCharInfo(rfDB))
		return;
	
	if (!GetCharItemList(rfDB))
		return;
	
	if (!GetCharTalent(rfDB))
		return;
	
	if (!GetCharPalette(rfDB))
		return;
	
	if (!GetEffectRemaindSecondsList(rfDB))
		return;
	
	if (!GetQuestListDoing(rfDB))
		return;

	if (!GetQuestListDontDoing(rfDB))
		return;

	if (!GetFactionList(rfDB))
		return;

	if (!GetSawnCutsceneList(rfDB))
		return;

	if (!GetRecipeList(rfDB))
		return;

	if (!GetEmblem(rfDB))
		return;

	if (!GetMailSummaryInfo(rfDB))
		return;
}

bool GDBTaskCharSerialize::GetCharInfo(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(CHAR_INFO)))
	{
		return false;
	}

	if (0 == rs.GetFetchedCount())
	{
		return false;
	}

	m_Data.CharInfo.nCID					= rs.FieldW(L"CHAR_ID").AsInt64();
	m_Data.CharInfo.nSex					= static_cast<SEX>(rs.FieldW(L"SEX").AsByte());
	m_Data.CharInfo.nFeatureHair			= rs.FieldW(L"FEAT_HAIR").AsByte();
	m_Data.CharInfo.nFeatureFace			= rs.FieldW(L"FEAT_FACE").AsByte();
	m_Data.CharInfo.nFeatureHairColor		= rs.FieldW(L"FEAT_HAIR_COLOR").AsByte();
	m_Data.CharInfo.nFeatureSkinColor		= rs.FieldW(L"FEAT_SKIN_COLOR").AsByte();

	m_Data.CharInfo.nTattooType				= rs.FieldW(L"TATOO").AsByte();
	m_Data.CharInfo.nTattooPosX				= rs.FieldW(L"TATOO_POS_X").AsShort();
	m_Data.CharInfo.nTattooPosY				= rs.FieldW(L"TATOO_POS_Y").AsShort();
	m_Data.CharInfo.nTattooScale				= rs.FieldW(L"TATOO_SCALE").AsByte();
	m_Data.CharInfo.nMakeUp					= rs.FieldW(L"MAKEUP").AsByte();

	m_Data.CharInfo.nLevel					= rs.FieldW(L"LEV").AsInt();	
	m_Data.CharInfo.nXP						= rs.FieldW(L"EXP").AsInt();
	m_Data.CharInfo.nHP						= rs.FieldW(L"HP").AsInt();
	m_Data.CharInfo.nEN						= rs.FieldW(L"EN").AsInt();
	m_Data.CharInfo.nSTA					= rs.FieldW(L"STA").AsInt();
	m_Data.CharInfo.nRemaindTP				= rs.FieldW(L"REMAIN_TP").AsInt();
	m_Data.CharInfo.nMoney					= rs.FieldW(L"MONEY").AsInt();
	m_Data.CharInfo.nSoulBindingID			= rs.FieldW(L"SOUL_BINDING_ID").AsShort();
	m_Data.CharInfo.nFatigueQuantity		= rs.FieldW(L"FATIGUE").AsShort();
	m_Data.CharInfo.nInnRoomID				= rs.FieldW(L"INN_ROOM_ID").AsByte();
	m_Data.CharInfo.nGID					= rs.FieldW(L"GUILD_ID").AsInt();
	m_Data.CharInfo.nBlankTimeSec			= rs.FieldW(L"BLANK_TIME").AsInt();
	m_Data.CharInfo.nWeaponSet				= static_cast<SH_ITEM_SWITCH_WEAPON>(rs.FieldW(L"WEAPON_SET").AsInt());
	m_Data.CharInfo.strName					= rs.FieldW(L"NAME").AsWString();

	m_Data.CharInfo.fPosX					= rs.FieldW(L"POS_X").AsFloat();
	m_Data.CharInfo.fPosY					= rs.FieldW(L"POS_Y").AsFloat();
	m_Data.CharInfo.fPosZ					= rs.FieldW(L"POS_Z").AsFloat();
	m_Data.CharInfo.nSharedFieldID			= rs.FieldW(L"SHR_FIELD_ID").AsInt();
	m_Data.CharInfo.fEnterPosX				= rs.FieldW(L"ENT_POS_X").AsFloat();
	m_Data.CharInfo.fEnterPosY				= rs.FieldW(L"ENT_POS_Y").AsFloat();
	m_Data.CharInfo.fEnterPosZ				= rs.FieldW(L"ENT_POS_Z").AsFloat();
	m_Data.CharInfo.uidDynamicFieldGroup	= rs.FieldW(L"DYN_FIELD_GROUP_UID").AsInt64();
	m_Data.CharInfo.nDynamicFieldID			= rs.FieldW(L"DYN_FIELD_ID").AsInt();
	m_Data.CharInfo.nCheckPointFieldID		= rs.FieldW(L"CHK_POINT_FIELD_ID").AsInt();
	m_Data.CharInfo.nCheckPointMarkerID		= rs.FieldW(L"CHK_POINT_MKR_ID").AsInt();

	m_Data.CharInfo.uidParty				= rs.FieldW(L"PARTY_UID").AsInt64();

	m_Data.CharInfo.uidArena				= rs.FieldW(L"ARENA_UID").AsInt64();
	m_Data.CharInfo.uidArenaTeam			= rs.FieldW(L"ARENA_TEAM_UID").AsInt64();

	m_Data.CharInfo.ActivePaletteID			= static_cast<PALETTE_NUM>(rs.FieldW(L"ACT_PALETTE_ID").AsByte());		
	m_Data.CharInfo.PrimaryPaletteID		= static_cast<PALETTE_NUM>(rs.FieldW(L"PRI_PALETTE_ID").AsByte());
	m_Data.CharInfo.SecondaryPaletteID		= static_cast<PALETTE_NUM>(rs.FieldW(L"SEC_PALETTE_ID").AsByte());

	m_Data.CharInfo.nPlayTimeSec			= rs.FieldW(L"CHAR_PTM").AsInt();
	m_Data.CharInfo.nPlayerGrade			= rs.FieldW(L"PLAYER_GRADE").AsByte();	

	m_Data.CharInfo.nElapsedDeadTimeSec	= -1;
	if (!rs.FieldW(L"ELAPS_DEAD_TIME").IsNull())
		m_Data.CharInfo.nElapsedDeadTimeSec = rs.FieldW(L"ELAPS_DEAD_TIME").AsInt();

	m_Data.CharInfo.nStorageMoney			= rs.FieldW(L"STORAGE_MONEY").AsInt();	
	
	rs.Close();

	return true;
}


bool GDBTaskCharSerialize::GetCharItemList(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(ITEM)))
	{
		return false;
	}

	return m_Data.qItemInst.GetFromDB(rs);
}


bool GDBTaskCharSerialize::GetCharTalent(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(TALENT)))
	{
		return false;
	}

	if (0 == rs.GetFetchedCount())
	{
		return true;
	}

	const size_t nTIDHash = rs.MakeHashValueW(L"TALENT_ID");

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		m_Data.qTalent.push_back(rs.FieldHash(nTIDHash).AsInt());
	}

	rs.Close();

	return true;
}


bool GDBTaskCharSerialize::GetCharPalette(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(PALETTE)))
	{
		return false;
	}

	if (0 == rs.GetFetchedCount())
	{
		return true;
	}

	const size_t nOrderIDHash	= rs.MakeHashValueW(L"SLOT_ID");
	const size_t nTypeHash		= rs.MakeHashValueW(L"ID_TYPE");
	const size_t nIDHash		= rs.MakeHashValueW(L"ID");

	GDBT_PALETTE pl;
	for (; !rs.IsEOF(); rs.MoveNext())
	{
		pl.nOrderID	= rs.FieldHash(nOrderIDHash).AsByte();
		pl.nType	= static_cast<PALETTE_ITEM_TYPE>(rs.FieldHash(nTypeHash).AsByte());
		pl.nID		= rs.FieldHash(nIDHash).AsInt();

		m_Data.qPalette.push_back(pl);
	}

	rs.Close();

	return true;
}


bool GDBTaskCharSerialize::GetEffectRemaindSecondsList(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(REMAIND_TIME)))
	{
		return false;
	}

	if (0 == rs.GetFetchedCount())
	{
		return true;
	}

	const size_t nIDHash		= rs.MakeHashValueW(L"ID");
	const size_t nTypeHash		= rs.MakeHashValueW(L"EFF_TYPE");
	const size_t nSecondHash	= rs.MakeHashValueW(L"REMAIN_TIME");	

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		const int	nID		= rs.FieldHash(nIDHash).AsInt();
		const float fTime	= static_cast<float>(rs.FieldHash(nSecondHash).AsInt());
		const int	nType	= rs.FieldHash(nTypeHash).AsInt();

		switch (nType)
		{
		case 0 :
			{
				m_Data.vecTalentCollTimeSec.push_back(TalentRemaindSecVec::value_type(nID, fTime));
			}
			break;

		case 1 :
			{
				REMAIN_BUFF_TIME remainBuffTime;

				remainBuffTime.nID = nID;
				remainBuffTime.fRemainDurationSeconds = fTime;

				m_Data.vecBuffRemaindSec.push_back(remainBuffTime);
			}
			break;

		case 2 :
			{

			}
			break;

		default :
			_ASSERT(false && L"정의도지 않은 타입");
			continue;
		}		
	}
	
	rs.Close();

	return true;
}

bool GDBTaskCharSerialize::GetQuestListDoing( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(QLIST_DOING)))
		return false;

	const size_t nSlotIDHash	= rs.MakeHashValueW(L"SLOT_ID");
	const size_t nQIDHash		= rs.MakeHashValueW(L"QUEST_ID");
	const size_t nVarHash		= rs.MakeHashValueW(L"VAR");
	const size_t nDoneHash		= rs.MakeHashValueW(L"STATE");
	const size_t nElapsedSec	= rs.MakeHashValueW(L"ELAPS_TIME");
	const size_t nAcceptDt		= rs.MakeHashValueW(L"ACCP_DATE");
	const size_t nExpiDt		= rs.MakeHashValueW(L"EXPI_DATE");
	const size_t nObj1ProgHash	= rs.MakeHashValueW(L"OBJ1_PROG");
	const size_t nObj2ProgHash	= rs.MakeHashValueW(L"OBJ2_PROG");
	const size_t nObj3ProgHash	= rs.MakeHashValueW(L"OBJ3_PROG");
	const size_t nObj4ProgHash	= rs.MakeHashValueW(L"OBJ4_PROG");
	const size_t nObj5ProgHash	= rs.MakeHashValueW(L"OBJ5_PROG");	

	GDBT_QUESTINSTANCE qi;

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		qi.nSlotID			= rs.FieldHash(nSlotIDHash).AsByte();
		qi.nQID				= rs.FieldHash(nQIDHash).AsInt();
		qi.nVar				= rs.FieldHash(nVarHash).AsInt();
		qi.nDone			= rs.FieldHash(nDoneHash).AsByte();
		qi.dwElapsedSec		= static_cast<DWORD>(rs.FieldHash(nElapsedSec).AsInt());
		qi.strAcceptDt		= rs.FieldHash(nAcceptDt).AsWString();
		qi.strExpiDt		= rs.FieldHash(nExpiDt).AsWString();
		qi.nObj1Progress	= rs.FieldHash(nObj1ProgHash).AsShort();
		qi.nObj2Progress	= rs.FieldHash(nObj2ProgHash).AsShort();
		qi.nObj3Progress	= rs.FieldHash(nObj3ProgHash).AsShort();
		qi.nObj4Progress	= rs.FieldHash(nObj4ProgHash).AsShort();
		qi.nObj5Progress	= rs.FieldHash(nObj5ProgHash).AsShort();		

#ifdef _DEBUG
		QuestQ::iterator it = m_Data.qQuestInstance.begin();
		for (;m_Data.qQuestInstance.end() != it; ++it)
		{
			_ASSERT((*it).nQID != qi.nQID);
		}
#endif

		m_Data.qQuestInstance.push_back(qi);
	}

	return true;
}

bool GDBTaskCharSerialize::GetQuestListDontDoing( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(QLIST_DONTDOING)))
		return false;

	const size_t nQIDHash		= rs.MakeHashValueW(L"QUEST_ID");
	
	GDBT_QUESTHISTORY qidd;

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		qidd.nQID		= rs.FieldHash(nQIDHash).AsInt();
		qidd.nDoneCnt	= 1; // 히스토리에 등록되면 최소 한번 완료한 퀘스트.

		m_Data.qQuestInstanceDontDoing.push_back(qidd);
	}

	return true;
}


mdb::MDB_THRTASK_RESULT GDBTaskCharSerialize::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskCharSerialize::GetFactionList( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(FACTION_LIST)))
		return false;

	GDBT_FACTION fac;

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		fac.nFactionID	= static_cast<uint8>(rs.FieldW(L"FACT_ID").AsShort());
		fac.nFaction	= static_cast<uint16>(rs.FieldW(L"VAL").AsInt());

		m_Data.qFaction.push_back(fac);
	}

	return true;
}

bool GDBTaskCharSerialize::GetSawnCutsceneList(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SAWN_CUTSCENE_LIST)))
		return false;

	const size_t nCutsceneIDHash = rs.MakeHashValueW(L"CUTSCN_ID");

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		m_Data.vecSawnCutscene.push_back(rs.FieldHash(nCutsceneIDHash).AsInt());
	}

	rs.Close();

	return true;
}

bool GDBTaskCharSerialize::GetRecipeList( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	
	if (!ExecuteW(rs, GetSQLW(RECIPE_LIST)))
		return false;

	const size_t nRecpIDHash = rs.MakeHashValueW(L"RECP_ID");
	
	for (; !rs.IsEOF(); rs.MoveNext())
	{
		m_Data.qRecipe.push_back(rs.FieldHash(nRecpIDHash).AsInt());
	}

	return true;
}

bool GDBTaskCharSerialize::GetEmblem( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(QUALIFY_LIST)))
		return false;

	const size_t nEmblemIDhash = rs.MakeHashValueW(L"EMBLEM_ID");
	for (; !rs.IsEOF(); rs.MoveNext())
	{
		m_Data.qEmblem.push_back(rs.FieldHash(nEmblemIDhash).AsInt());
	}

	return true;
}

bool GDBTaskCharSerialize::GetMailSummaryInfo(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(MAIL_SUMMARY)))
		return false;

	m_Data.MailSummary.nUnreadMailCount	= rs.FieldW(L"UNREAD_MAIL_COUNT").AsInt();
	m_Data.MailSummary.nTotalMailCount	= rs.FieldW(L"TOTAL_MAIL_COUNT").AsInt();
	m_Data.MailSummary.nTopMailUID		= rs.FieldW(L"TOP_MAIL_ID").AsInt64();
	m_Data.MailSummary.nBottomMailUID	= rs.FieldW(L"BOTTOM_MAIL_ID").AsInt64();

	return true;
}

mdb::MDB_THRTASK_RESULT GDBTaskCharSerialize::_OnFailed()
{
	gsys.pServer->Disconnect(m_Data.uidPlayer);

	return mdb::MDBTR_SUCESS;
}

GDBTaskCharSerialize::Completer::Completer( GCHAR_SERIAL_DATA& Data ) : m_Data(Data)
{

}

void GDBTaskCharSerialize::Completer::Do()
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayer)
		return;
	
	GEntityPlayer* pEntityPlayer = pPlayer->GetEntity();
	if (NULL == pEntityPlayer)
	{
		gsys.pServer->Disconnect(m_Data.uidPlayer);
		return;
	}

	// gsys.pDBCacheSystem->DeletePlayerCache(m_Data.uidPlayer);
	if ((NULL != gmgr.pPlayerDBCacheMgr->Find(m_Data.uidPlayer)) && !m_Data.bReload)
	{
		// 이미 같은 해시값을 사용중이면 접속 실패 처리한다... 이런 경우가 생기면 std::map을 고려해 보자...
		MLog3("Player DB Cache hash key is already used...  can't connect!!!. WORLD:%d, CHAR_ID:%I64d.\n"
			, GConfig::m_nMyWorldID, m_Data.CharInfo.nCID);
		gsys.pServer->Disconnect(m_Data.uidPlayer);
		return;
	}

	Clear(pEntityPlayer);
	AddTalentList(pEntityPlayer);
	AddItemList(pEntityPlayer);
	RemoveExpiredItem(pEntityPlayer);
	SetActiveWeaponSet(pEntityPlayer);	
	AddEffectRemainSecList(pEntityPlayer);
	AddDoingQuestList(pEntityPlayer);
	AddDoneCount(pEntityPlayer);	
	AddFaction(pEntityPlayer);
	AddSawnCutscene(pEntityPlayer);
	AddRepice(pEntityPlayer);
	AddEmblem(pEntityPlayer);
	SetMailSummaryInfo(pEntityPlayer);
	SetInfo(pEntityPlayer);

	gmgr.pPlayerObjectManager->AddCID(pPlayer);

	PostResponseLoginGameServer();
	SerializeGuld(pEntityPlayer);

	
	// 접속 로그를 상세화 할 필요가 있다.
	gsys.pDBManager->CharSelectLog(
		GDBT_CHAR_SELECT_LOG(pPlayer->GetAccountInfo().nAID
			, m_Data.nCID
			, m_Data.CharInfo.nPlayTimeSec
			, m_Data.CharInfo.nMoney
			, m_Data.CharInfo.nSharedFieldID));
}


void GDBTaskCharSerialize::Completer::Clear(GEntityPlayer* pEntityPlayer)
{
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();
	pItemHolder->Clear();
}


void GDBTaskCharSerialize::Completer::SetInfo(GEntityPlayer* pEntityPlayer)
{
	SetPlayerInfo(pEntityPlayer);
	SetState(pEntityPlayer);

	pEntityPlayer->GetTalent().SetTP(m_Data.CharInfo.nRemaindTP);
	SetPalette(pEntityPlayer);

	GItemHolder* pHolder = pEntityPlayer->GetItemHolder();
	pHolder->GetEquipment().SwitchingWeaponSet(m_Data.CharInfo.nWeaponSet);

	pEntityPlayer->SetParty(m_Data.CharInfo.uidParty);

	gsys.pFatigueSystem->IncFatigueByBlankTime(pEntityPlayer, m_Data.CharInfo.nBlankTimeSec/3600/*1시간 단위*/);

	SetGateInfo(pEntityPlayer);	
	SetArenaUID(pEntityPlayer);
}

void GDBTaskCharSerialize::Completer::SetPlayerInfo( GEntityPlayer* pEntityPlayer )
{
	PLAYER_INFO*	pPlayerInfo	= pEntityPlayer->GetPlayerInfo();
	GDBT_CHARINFO&	DBCharInfo	= m_Data.CharInfo;

	pPlayerInfo->nCID				= (int)DBCharInfo.nCID;
	pPlayerInfo->nSex				= DBCharInfo.nSex;
	pPlayerInfo->nFeatureHair		= DBCharInfo.nFeatureHair;
	pPlayerInfo->nFeatureFace		= DBCharInfo.nFeatureFace;
	pPlayerInfo->nFeatureHairColor	= DBCharInfo.nFeatureHairColor;
	pPlayerInfo->nFeatureSkinColor	= DBCharInfo.nFeatureSkinColor;
	pPlayerInfo->nEyeColor			= DBCharInfo.nEyeColor;
	pPlayerInfo->nMakeUp			= DBCharInfo.nMakeUp;
	pPlayerInfo->nTattooType			= DBCharInfo.nTattooType;
	pPlayerInfo->nTattooPosX			= DBCharInfo.nTattooPosX;
	pPlayerInfo->nTattooPosY			= DBCharInfo.nTattooPosY;
	pPlayerInfo->nTattooScale		= DBCharInfo.nTattooScale;
	pPlayerInfo->nLevel				= DBCharInfo.nLevel;
	pPlayerInfo->nXP				= DBCharInfo.nXP;
	pPlayerInfo->SetMoney(DBCharInfo.nMoney);
	pPlayerInfo->SetStorageMoney(DBCharInfo.nStorageMoney);
	pPlayerInfo->nSoulBindingID		= DBCharInfo.nSoulBindingID;
	pPlayerInfo->nFatigueQuantity	= DBCharInfo.nFatigueQuantity;
	pPlayerInfo->nInnRoomID			= DBCharInfo.nInnRoomID;
	pPlayerInfo->nGID				= DBCharInfo.nGID;
	pPlayerInfo->m_nWeaponSet		= DBCharInfo.nWeaponSet;
	pPlayerInfo->nBlankTimeSec		= DBCharInfo.nBlankTimeSec;
	pPlayerInfo->nPlayTimeSec		= DBCharInfo.nPlayTimeSec;
	pPlayerInfo->nConnPlayTimeSec	= DBCharInfo.nPlayTimeSec;
	pPlayerInfo->nElapsedDeadTimeSec= DBCharInfo.nElapsedDeadTimeSec;
	
	if (0 < DBCharInfo.nElapsedDeadTimeSec)
	{
		pPlayerInfo->nDeadTime = static_cast<int>(gsys.pSystem->GetNowTime() - (DBCharInfo.nElapsedDeadTimeSec * 1000));
	}


	// 권한 및 이름 설정, GM 권한의 경우 [GM] 접두어를 붙인다.
	pPlayerInfo->nPlayerGrade		= static_cast<GPlayerGrade>(DBCharInfo.nPlayerGrade);

	wstring strName = DBCharInfo.strName;

	if (pPlayerInfo->nPlayerGrade == PLAYER_GRADE_GM)
	{
		strName = L"[GM]" + strName;
	}

	wcsncpy_s(pPlayerInfo->szName, strName.c_str(), strName.length());
}


void GDBTaskCharSerialize::Completer::SetArenaUID( GEntityPlayer* pEntityPlayer )
{
	MUID uidBattleArena = m_Data.CharInfo.uidArena;
	if (gmgr.pBattleArenaMgr->FindBattleArena(uidBattleArena))
	{
		pEntityPlayer->GetPlayerBattleArena().SetBattleArenaUID(uidBattleArena);
	}
	// 여기에 아레나 팀 UID를 셋팅하도록 하자.
}


void GDBTaskCharSerialize::Completer::SetState( GEntityPlayer* pEntityPlayer )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->GetPlayerInfo();

	// str 등 설정
	pPlayerInfo->nSTR		= PLAYER_DEFAULT_STR;
	pPlayerInfo->nDEX		= PLAYER_DEFAULT_DEX;
	pPlayerInfo->nINT		= PLAYER_DEFAULT_INT;
	pPlayerInfo->nCHA		= PLAYER_DEFAULT_CHA;
	pPlayerInfo->nCON		= PLAYER_DEFAULT_CON;

	// 탤런트 효과 적용
	pEntityPlayer->ApplyAllPassiveTalents();

	// MaxHP 설정
	pPlayerInfo->nMaxHP		= CSBasicAttributeCalculator::CalcMaxHP(pPlayerInfo->nLevel, pEntityPlayer->GetCON());
	pPlayerInfo->nMaxEN		= CSBasicAttributeCalculator::CalcMaxEN(pPlayerInfo->nLevel, pEntityPlayer->GetINT());
	pPlayerInfo->nMaxSTA	= CSBasicAttributeCalculator::CalcMaxSP(pPlayerInfo->nLevel, pEntityPlayer->GetDEX());

	pEntityPlayer->SetHP(m_Data.CharInfo.nHP);
	pEntityPlayer->SetEN(m_Data.CharInfo.nEN);
	pEntityPlayer->SetSTA(m_Data.CharInfo.nSTA);

	if (true == pEntityPlayer->IsDie())
	{
		pEntityPlayer->SetDie();
	}
}

void GDBTaskCharSerialize::Completer::SetPalette( GEntityPlayer* pEntityPlayer )
{
	pEntityPlayer->GetPalette().SelectPalette(m_Data.CharInfo.ActivePaletteID);
	pEntityPlayer->GetPalette().SetPrimary(m_Data.CharInfo.PrimaryPaletteID);
	pEntityPlayer->GetPalette().SetSecondary(m_Data.CharInfo.SecondaryPaletteID);

	AddPalette(pEntityPlayer);
}

void GDBTaskCharSerialize::Completer::AddPalette( GEntityPlayer* pEntityPlayer )
{
	PALETTE_NUM		nNum;
	PALETTE_SLOT	nSlot;

	for (PaletteQ::iterator it = m_Data.qPalette.begin()
		; it != m_Data.qPalette.end()
		; ++it)
	{
		gsys.pPaletteSystem->IndexToNumAndSlot((*it).nOrderID, nNum, nSlot);
		pEntityPlayer->GetPalette().Set(nNum, nSlot, (*it).nType, (*it).nID);
	}
}

void GDBTaskCharSerialize::Completer::SetGateInfo( GEntityPlayer* pEntityPlayer )
{
	GGateInfo		gateInfo;
	GDBT_CHARINFO&	DBCharInfo	= m_Data.CharInfo;

	// pos
	gateInfo.vPosition = vec3(DBCharInfo.fPosX, DBCharInfo.fPosY, DBCharInfo.fPosZ);
	gateInfo.vDirection = vec3(1.0f, 1.0f, 0.0f);

	// field info
	if (true == DBCharInfo.uidDynamicFieldGroup.IsValid())
	{
		// GateInfo
		gateInfo.nFieldID = DBCharInfo.nDynamicFieldID;
		gateInfo.bDynamicField = true;
		gateInfo.uidDynamicFieldGroup = DBCharInfo.uidDynamicFieldGroup;

		MARKER_INFO markerEnter;
		markerEnter.nFieldID = DBCharInfo.nSharedFieldID;
		markerEnter.vPoint.x = DBCharInfo.fEnterPosX;
		markerEnter.vPoint.y = DBCharInfo.fEnterPosY;
		markerEnter.vPoint.z = DBCharInfo.fEnterPosZ;
		pEntityPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerEnter);

		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(DBCharInfo.nCheckPointFieldID);
		if (NULL != pFieldInfo)
		{
			const MARKER_INFO* pMarkerCheckpoint = pFieldInfo->FindMarker(DBCharInfo.nCheckPointMarkerID);
			if (NULL != pMarkerCheckpoint)
			{
				pEntityPlayer->GetPlayerField().GetFieldDynamic().SetCheckpoint(*pMarkerCheckpoint);
			}			
		}		
	}
	else
	{
		gateInfo.nFieldID = DBCharInfo.nSharedFieldID;
		gateInfo.bDynamicField = false;
	}

	pEntityPlayer->GetPlayerField().SetGateInfo(gateInfo);
}

void GDBTaskCharSerialize::Completer::AddItemList( GEntityPlayer* pEntityPlayer )
{
	m_Data.qItemInst.SetToServer(pEntityPlayer);	
}

void GDBTaskCharSerialize::Completer::RemoveExpiredItem( GEntityPlayer* pEntityPlayer )
{
	pEntityPlayer->GetItemExpirer()->RunImmediately();
}

void GDBTaskCharSerialize::Completer::SetActiveWeaponSet( GEntityPlayer* pEntityPlayer )
{
	PLAYER_INFO* pPlayerInfo	= pEntityPlayer->GetPlayerInfo();
	GItemHolder* pItemHolder	= pEntityPlayer->GetItemHolder();

	SH_ITEM_SWITCH_WEAPON nWeaponSet = pPlayerInfo->m_nWeaponSet;
	if (nWeaponSet == ITEMSWITCH_SECONDARY)
	{
		if (pItemHolder->GetEquipment().IsEquipItem(ITEMSLOT_RWEAPON2) == false &&
			pItemHolder->GetEquipment().IsEquipItem(ITEMSLOT_LWEAPON2) == false)
		{
			nWeaponSet = ITEMSWITCH_PRIMARY;
		}
	}

	pItemHolder->GetEquipment().SwitchingWeaponSet(nWeaponSet);
}

void GDBTaskCharSerialize::Completer::AddTalentList( GEntityPlayer* pEntityPlayer )
{
	for (TalentQ::iterator it = m_Data.qTalent.begin()
		; it != m_Data.qTalent.end()
		; ++it)
	{
		pEntityPlayer->GetTalent().Insert((*it), false);
	}
}

void GDBTaskCharSerialize::Completer::AddEffectRemainSecList( GEntityPlayer* pEntityPlayer )
{
	pEntityPlayer->GetActorCooltimeChecker().InsertTalentRemainCoolTime(m_Data.vecTalentCollTimeSec);		
	pEntityPlayer->InsertBuffRemainTime(m_Data.vecBuffRemaindSec);
}

void GDBTaskCharSerialize::Completer::AddDoingQuestList( GEntityPlayer* pEntityPlayer )
{
	GPlayerQuests& playerQuests = pEntityPlayer->GetQuests();

	const uint32 ServerNowTime = gsys.pSystem->GetNowTime();

	for (QuestQ::iterator it = m_Data.qQuestInstance.begin()
		; m_Data.qQuestInstance.end() != it;
		++it)
	{
		GDBT_QUESTINSTANCE& qi = (*it);

		GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(qi.nQID);
		if (pQuestInfo == NULL) 
		{
			playerQuests.SlotMarking(qi.nSlotID);
			dlog("QUEST ID NOT EXIST IN quest.xml(%d).", qi.nQID);
			continue;
		}

		const uint32 nAcceptTime = ServerNowTime - (qi.dwElapsedSec * 1000);
		GPlayerQuest* pNewPlayerQuest = new GPlayerQuest(pQuestInfo, pEntityPlayer, qi.nSlotID, nAcceptTime, qi.strAcceptDt, qi.strExpiDt);
		//GPlayerQuest* pNewPlayerQuest = pEntityPlayer->GetQuests().Insert(qi.nSlotID, qi.nQID, qi.strAcceptDt, qi.strExpiDt);
		if (NULL == pNewPlayerQuest)
			continue;

		pNewPlayerQuest->UpdateQuestVar(qi.nVar);	
		if (QST_FAIL == qi.nDone)
			pNewPlayerQuest->Fail();
		
		pair<MAP_PLAYER_QUEST::iterator, bool> ret;
		ret = playerQuests.GetQuestContainer().insert(MAP_PLAYER_QUEST::value_type(pQuestInfo->nID, pNewPlayerQuest));
		_ASSERT(ret.second);
		if (!ret.second)
		{
			delete pNewPlayerQuest;
			continue;
		}

		AddQuestObjective(pEntityPlayer, pNewPlayerQuest, 1, qi.nObj1Progress);
		AddQuestObjective(pEntityPlayer, pNewPlayerQuest, 2, qi.nObj2Progress);
		AddQuestObjective(pEntityPlayer, pNewPlayerQuest, 3, qi.nObj3Progress);
		AddQuestObjective(pEntityPlayer, pNewPlayerQuest, 4, qi.nObj4Progress);
		AddQuestObjective(pEntityPlayer, pNewPlayerQuest, 5, qi.nObj5Progress);

		if (true == pNewPlayerQuest->IsAllQObjectiveComplete())
		{
			pNewPlayerQuest->Complete();
		}

		playerQuests.SlotMarking(qi.nSlotID);
	}	
}

void GDBTaskCharSerialize::Completer::AddQuestObjective(GEntityPlayer* pEntityPlayer, GPlayerQuest* pPlayerQuest, int nObjectiveID, int nDBProgress)
{
	GPlayerQObjective* pPlayerQObjective;
	pPlayerQObjective = pPlayerQuest->Get(nObjectiveID);
	if (NULL != pPlayerQObjective) 
	{
		int nProgress = nDBProgress;
		// 캐싱된 아이템이 DB에 반영되지 않았을때를 대비해서 다시 계산해준다.
		if (QOT_COLLECT == pPlayerQObjective->GetInfo()->nType &&
			false == pPlayerQObjective->GetInfo()->vecParam1.empty())
		{
			int nItemID = pPlayerQObjective->GetInfo()->vecParam1.front();
			nProgress = pEntityPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, nItemID);
		}

		pPlayerQObjective->UpdateProgress(nProgress);
	}
}

void GDBTaskCharSerialize::Completer::AddDoneCount( GEntityPlayer* pEntityPlayer )
{
	GPlayerQuests& playerQuests = pEntityPlayer->GetQuests();

	for (QusetHistoryQ::iterator it = m_Data.qQuestInstanceDontDoing.begin()
		; m_Data.qQuestInstanceDontDoing.end() != it;
		++it)
	{
		GDBT_QUESTHISTORY&	qi = (*it);

		if (false == gmgr.pQuestInfoMgr->IsExist(qi.nQID))
			continue;

		playerQuests.GetDoneCountContainer().insert(MAP_DONE_COUNT::value_type(qi.nQID, qi.nDoneCnt));
	}
}

void GDBTaskCharSerialize::Completer::PostResponseLoginGameServer()
{
	GClientMoveServerRouter router;
	router.RouteResponseLoginGameServer(m_Data.uidPlayer, CR_SUCCESS, m_Data.uidPlayer, m_Data.nUIID);
}

void GDBTaskCharSerialize::Completer::AddFaction(GEntityPlayer* pEntityPlayer)
{
	GPlayerFactions& playerFactions = pEntityPlayer->GetPlayerFactions();

	for each (const GDBT_FACTION& dbTaskFaction in m_Data.qFaction)
	{
		GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(dbTaskFaction.nFactionID);
		if (NULL == pFactionInfo) continue;

		playerFactions.Insert(pFactionInfo, dbTaskFaction.nFaction);
	}
}

void GDBTaskCharSerialize::Completer::AddSawnCutscene(GEntityPlayer* pEntityPlayer)
{
	GPlayerCutscene& refPlayerCutscene = pEntityPlayer->GetCutscene();

	refPlayerCutscene.AssignSawnCutscene(m_Data.vecSawnCutscene);
}

void GDBTaskCharSerialize::Completer::AddRepice( GEntityPlayer* pEntityPlayer )
{
	for (size_t i = 0; i < m_Data.qRecipe.size(); ++i)
	{
		pEntityPlayer->GetPlayerRecipe().Insert(m_Data.qRecipe[i]);
	}
}

void GDBTaskCharSerialize::Completer::AddEmblem( GEntityPlayer* pEntityPlayer )
{
	pEntityPlayer->GetEmblem().LoadFromDB(m_Data.qEmblem);
}

void GDBTaskCharSerialize::Completer::SetMailSummaryInfo(GEntityPlayer* pEntityPlayer)
{
	GPlayerMailBox& refMailbox = pEntityPlayer->GetMailbox();
	refMailbox.SetSummaryInfo(m_Data.MailSummary.nTotalMailCount, m_Data.MailSummary.nUnreadMailCount);

	bool bRet = refMailbox.GetDBMailUID().Init(m_Data.MailSummary.nTopMailUID, m_Data.MailSummary.nBottomMailUID);
	if (bRet == false)
		mlog3("Error! GDBTaskCharSerialize::Completer::SetMailSummaryInfo() - TotalMailCount(%d), UnreadMailCount(%d) TopMailUID(%I64d), BottomMailUID(%I64d)\n"
			, m_Data.MailSummary.nTotalMailCount, m_Data.MailSummary.nUnreadMailCount, m_Data.MailSummary.nTopMailUID, m_Data.MailSummary.nBottomMailUID);
}

void GDBTaskCharSerialize::Completer::SerializeGuld( GEntityPlayer* pEntityPlayer )
{
	gsys.pGuildSystem->Serialize(pEntityPlayer);
}