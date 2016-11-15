#include "StdAfx.h"
#include "GFakeLoginLogic.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "SCharacterInfoChecker.h"
#include "SCmdRouter_Login.h"
#include "GFakeLoginPlayerAcceptor_FromGameServerToLoginServer.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GPlayerAcceptManager.h"
#include "CCommandResultTable.h"
#include "GCommandCenter.h"
#include "CSStringFilter.h"
#include "MLocale.h"
#include "CSBasicAttributeCalculator.h"
#include "CSNameStringChecker.h"
#include "GDBTaskCharData.h"
#include "GClientMoveServerRouter.h"
#include "GConst.h"
#include "CSDef_Fatigue.h"


MUIDGenerator GFakeLoginLogic::m_AcceptorUIDGenerator;

bool GFakeLoginLogic::RequestAccountCharList(MUID uidPlayer)
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL)	return false;	
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;

	ACCOUNT_INFO& accountInfo =  pPlayer->GetAccountInfo();
	return gsys.pDBManager->CharGetLookList(uidPlayer, accountInfo.nAID);
}

bool GFakeLoginLogic::InsertCharacter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	// 요청자 확인
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;


	// 인자값 유효성 검사
	if (!CheckInsertCharacter_Account(pPlayer))	return false;
	if (!CheckInsertCharacter_Parameter(uidPlayer, pInsertCharInfo))	return false;	


	// 캐릭터 추가
	ACCOUNT_INFO& accountInfo = pPlayer->GetAccountInfo();
	AID nAID = (int)accountInfo.nAID;

	if (!ApplyInsertCharacter(uidPlayer, nAID, pInsertCharInfo))		return false;

	return true;
}

bool GFakeLoginLogic::DeleteCharacter(MUID uidPlayer, int8 nCharIndex)
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL)	return false;
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;

	SAccountCharFieldInfo* pCharFieldInfo = pPlayer->GetCharFieldInfo(nCharIndex);
	if (pCharFieldInfo == NULL)	return false;

	GDBT_CHAR_DELETE data(pPlayer->GetAccountInfo().nAID, uidPlayer, pCharFieldInfo->nCID, nCharIndex);

	gsys.pDBManager->CharDelete(data);
	return true;
}

bool GFakeLoginLogic::SelMyChar(MUID uidPlayer, int8 nCharIndex)
{
	// 요청 플레이어 상태 확인
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayer)
	{
		return CR_TRUE;
	}

	SAccountCharFieldInfo* pCharFieldInfo = pPlayer->GetCharFieldInfo(nCharIndex);
	if (pCharFieldInfo == NULL)	return CR_TRUE;

	// ReservePlayerEnter --
	TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo;
	wcsncpy_s(tdLoginAccountInfo.szUserID, pPlayer->GetAccountInfo().szUserID, _TRUNCATE);
	tdLoginAccountInfo.nAID = (int)pPlayer->GetAccountInfo().nAID; 
	tdLoginAccountInfo.nCID = pCharFieldInfo->nCID;
	tdLoginAccountInfo.nEnterKey = 0;

	GPlayerAcceptor_FromLoginServer* pPlayerAcceptor = new GPlayerAcceptor_FromLoginServer();
	pPlayerAcceptor->MakeFakeReserveLogin(m_AcceptorUIDGenerator.Generate(), &tdLoginAccountInfo);

	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	if (pCharFieldInfo->nCID == 0 || pAcceptManager->Add(pPlayerAcceptor, GPlayerAcceptor::FROM_LOGIN_SERVER) == false)
	{
		pPlayerAcceptor->PostResponseSelMyChar(CR_FAIL_LOGIN_INVALID_CHAR_INFO, uidPlayer);
		SAFE_DELETE(pPlayerAcceptor);
		return CR_TRUE;
	}

	int nDestFIeldID = 0;
	if (pCharFieldInfo->uidExitedDynamicFieldGroup.IsValid())
		nDestFIeldID = pCharFieldInfo->nExitedDynamicFieldID;
	else
	{
		nDestFIeldID = pCharFieldInfo->nExitedSharedFieldID;
		if (0 >= nDestFIeldID)
			nDestFIeldID = GConst::GetFirstEnterFieldID();
	}

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.PreparingToLoad(uidPlayer, nDestFIeldID);

	pPlayerAcceptor->PostResponseSelMyChar(CR_SUCCESS, uidPlayer);

	return true;
}


bool GFakeLoginLogic::CheckInsertCharacter_Account(GPlayerObject* pPlayerObject)
{
	int nCharCount = pPlayerObject->GetCharFieldInfoCount();

	SCharacterInfoChecker checker;	

	if (!checker.IsRemainCharSlot(nCharCount))
	{		
		MUID uidPlayer = pPlayerObject->GetUID();

		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseInsertChar(uidPlayer, CR_FAIL_CHAR_LIMIT_CHAR_SLOT_COUNT);

		return false;
	}

	return true;
}

bool GFakeLoginLogic::CheckInsertCharacter_Parameter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 생성 이름이 유효한지 확인
	CSNameStringChecker nameChecker(pInsertCharInfo->szName);
	CSNameStringChecker::ERROR_CODE nErrorCode = nameChecker.Check();
	if (CSNameStringChecker::OK != nErrorCode)
	{
		cmdRouter.ResponseInsertChar(uidPlayer, nErrorCode);
		return false;
	}

	// 파라미터 확인
	SEX sex = (pInsertCharInfo->nSex == SEX_FEMALE ? SEX_FEMALE : SEX_MALE);

	SCharacterInfoChecker infoChecker;
	if (false == infoChecker.CheckAllParam_InsertCharacter(*pInsertCharInfo))
	{
		dlog("Invalid Param - InsertCharacter(Name:%s, Sex:%d, Hair:%d, Face:%d, Voice:%d, HairColor:%d, SkinColor:%d, EyeColor:%d, TalentStyle:%d, MakeUp:%d, TattooType:%d, TattooScale:%d)\n"
			, pInsertCharInfo->szName, (int)pInsertCharInfo->nSex, (int)pInsertCharInfo->nHair, (int)pInsertCharInfo->nFace, (int)pInsertCharInfo->nVoice
			, (int)pInsertCharInfo->nHairColor, (int)pInsertCharInfo->nSkinColor, pInsertCharInfo->nEyeColor
			, pInsertCharInfo->nMakeUp, pInsertCharInfo->nTattooType, pInsertCharInfo->nTattooPosX, pInsertCharInfo->nTattooPosY, pInsertCharInfo->nTattooScale);

		cmdRouter.ResponseInsertChar(uidPlayer, CR_FAIL_CHAR_INVALID_PARAMETER);
		return false;
	}

	return true;
}

bool GFakeLoginLogic::ApplyInsertCharacter(MUID uidPlayer, AID nAID, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	// Level, XP, Money 기본값
	const uint8 nLevel	= 1;
	const int	nXP		= 0;
	const int	nMoney = 0;

	int nHP = CSBasicAttributeCalculator::CalcMaxHP(nLevel, PLAYER_DEFAULT_CON);
	int nEN = CSBasicAttributeCalculator::CalcMaxEN(nLevel, PLAYER_DEFAULT_INT);
	int nSTA = CSBasicAttributeCalculator::CalcMaxSP(nLevel, PLAYER_DEFAULT_DEX);

	if (!gsys.pDBManager->CharInsert(uidPlayer, nAID, pInsertCharInfo->szName
		, nLevel, nXP, nMoney
		, 0, pInsertCharInfo->nSex
		, int(pInsertCharInfo->nHair), int(pInsertCharInfo->nFace), pInsertCharInfo->nHairColor, pInsertCharInfo->nSkinColor, pInsertCharInfo->nEyeColor, int(pInsertCharInfo->nVoice)
		, pInsertCharInfo->nMakeUp, pInsertCharInfo->nTattooType, pInsertCharInfo->nTattooPosX, pInsertCharInfo->nTattooPosY, pInsertCharInfo->nTattooScale
		, nHP, nEN, nSTA, PLAYER_DEFAULT_TP
		, NORMAL_FATIGUE
		, PLAYER_DEFAULT_SOULBINDINGID
		, PLAYER_DEFAULT_INNROOMID
		, PLAYER_DEFAULT_PRIPALETTE
		, PLAYER_DEFAULT_SECPALETTE
		, pInsertCharInfo->nTalentStyle		
		, pInsertCharInfo->nEquipmentIndex, pInsertCharInfo->nEquipmentColorIndex		
		))
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseInsertChar(uidPlayer, CR_FAIL_CHAR_POST_INSERT_DB);

		return false;
	}

	return true;
}

bool GFakeLoginLogic::MoveToLoginServer(const MUID& uidPlayer)
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	
	MUID uidConnectionKey = m_AcceptorUIDGenerator.Generate();
	AID nAID = (AID)pPlayer->GetAccountInfo().nAID;
	wstring strUserID = pPlayer->GetAccountInfo().szUserID;

	GFakeLoginPlayerAcceptor_FromGameServerToLoginServer* pPlayerAcceptor = new GFakeLoginPlayerAcceptor_FromGameServerToLoginServer();
	pPlayerAcceptor->ReserveLogin(uidConnectionKey, nAID, strUserID);

	GClientMoveServerRouter router;

	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	if (pAcceptManager->Add(pPlayerAcceptor, GPlayerAcceptor::FAKE_LOGIN_FROM_GAME_SERVER_TO_LOGIN_SERVER) == false)
	{
		router.RouteStartMoveLoginServerRes(uidPlayer, CR_FAIL, uidConnectionKey);
		SAFE_DELETE(pPlayerAcceptor);
		return CR_TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	pPlayer->SetMoveLoginServerConnectionKey(uidConnectionKey);

	return true;
}