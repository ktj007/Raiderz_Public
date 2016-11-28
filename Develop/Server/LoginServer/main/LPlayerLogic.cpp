#include "StdAfx.h"
#include "LPlayerLogic.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LGlobal.h"
#include "LCommandCenter.h"
#include "SCharacterInfoChecker.h"
#include "CSNameStringChecker.h"
#include "LCmdRouter_Player.h"
#include "SCmdRouter_Login.h"
#include "LDBManager.h"
#include "LMasterClient.h"
#include "LServer.h"
#include "PmDef.h"
#include "PmRegionStat.h"
#include "LPMSSystem.h"
#include "LDBParamData.h"
#include "LPlayerLoginGameServerProcManager.h"
#include "CSBasicAttributeCalculator.h"
#include "LPlayerAcceptManager.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LTimeoutManager.h"
#include "CSDef_Fatigue.h"


bool LPlayerLogic::RequestAccountCharList(MUID uidPlayer)
{
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;

	ACCOUNT_INFO& accountInfo = pPlayer->GetAccountInfo();
	AID nAID = accountInfo.nAID;

	if (!gsys.pDBManager->GetAccountCharList(uidPlayer, nAID))
		return false;

	return true;
}

bool LPlayerLogic::InsertCharacter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	// 요청자 확인
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;
	
	
	// 인자값 유효성 검사
	if (!CheckInsertCharacter_Account(pPlayer))	return false;
	if (!CheckInsertCharacter_Parameter(uidPlayer, pInsertCharInfo))	return false;
	
	
	// 캐릭터 추가
	ACCOUNT_INFO& accountInfo = pPlayer->GetAccountInfo();
	AID nAID = accountInfo.nAID;
	
	if (!ApplyInsertCharacter(uidPlayer, nAID, pInsertCharInfo))		return false;
	
	
	return true;
}

bool LPlayerLogic::DeleteCharacter(MUID uidPlayer, int8 nCharIndex)
{
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	if (pPlayer->GetState() != POS_SELECT_CHAR) return false;

	SAccountCharFieldInfo* pCharFieldInfo = pPlayer->GetCharFieldInfo(nCharIndex);
	if (pCharFieldInfo == NULL)	return false;

	return gsys.pDBManager->DeleteCharacter(uidPlayer, pPlayer->GetAccountInfo().nAID, nCharIndex);
}

bool LPlayerLogic::SelMyChar(MUID uidPlayer, int8 nCharIndex)
{
	// 요청 플레이어 상태 확인
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return false;
	if (POS_SELECT_CHAR != pPlayer->GetState() &&		
		POS_LOGIN_GAME_SERVER != pPlayer->GetState())	///< 자동 재시도할 때 상태.
		return false;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 캐릭터 인덱스가 유효한지 확인
	SAccountCharFieldInfo* pCharFieldInfo = pPlayer->GetCharFieldInfo(nCharIndex);
	if (NULL == pCharFieldInfo)
	{
		cmdRouter.ResponseSelMyChar(pPlayer->GetUID(), CR_FAIL_LOGIN_INVALID_CHAR_INFO);
		return false;
	}

	// 선택 캐릭터 게임서버 로그인 요청 Queuing.
	pPlayer->OnLoginGameServer(nCharIndex);
	gsys.pServer->GetPlayerLoginGameServerProcManager()->Add(uidPlayer, nCharIndex);

	return true;
}

void LPlayerLogic::DuplicatedPlayerLogin(MUID uidPlayer, MUID uidConnectionKey)
{
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer != NULL)
	{
		SCmdRouter_Login router(gsys.pCommandCenter);
		router.NotifyKick(uidPlayer, CR_FAIL_DUPLICATED_PLAYER_LOGIN);

		/// 클라이언트와의 연결 종료.
		gsys.pServer->HardDisconnectPlayer(uidPlayer);

		return;
	}

	// 서비이동 중일 경우 Acceptor 가 있을 수 있다.
	LPlayerAcceptor_FromGameServer* pAcceptor = gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey);
	if (pAcceptor != NULL)
	{
		LCmdRouter_Player::PostDeletePlayer(pAcceptor->GetAID(), pAcceptor->GetUID());

		gmgr.pPlayerAcceptManager->Del(uidConnectionKey);

		return;
	}
}

void LPlayerLogic::OnDisconnectPlayer(MUID uidPlayer)
{
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return;

	if (POS_MOVE_SERVER == pPlayer->GetState())
	{
		/// 서버 이동 Timeout 제거
		gmgr.pTimeoutManager->StopWatchingMoveServerTimeout(uidPlayer);
	}
	else
	{
		/// 서버 이동으로 인한 연결 종료가 아닐 경우, 플레이어 제거 통지.
		LCmdRouter_Player::PostDeletePlayer(pPlayer->GetAccountInfo().nAID, pPlayer->GetUID());
	}

	gmgr.pPlayerObjectManager->DeleteObject(uidPlayer);
}

bool LPlayerLogic::CheckInsertCharacter_Account(LPlayerObject* pPlayerObject)
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

bool LPlayerLogic::CheckInsertCharacter_Parameter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	/// 정보 체크
	SCharacterInfoChecker checker;
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 생성 이름이 유효한지 확인
	CSNameStringChecker illegalword_checker(pInsertCharInfo->szName);
	CSNameStringChecker::ERROR_CODE nErrorCode = illegalword_checker.Check();
	if (CSNameStringChecker::OK != nErrorCode)
	{
		cmdRouter.ResponseInsertChar(uidPlayer, nErrorCode);
		return false;
	}

	// 파라미터 확인
	SEX sex = (pInsertCharInfo->nSex == SEX_FEMALE ? SEX_FEMALE : SEX_MALE);
	
	if (false == checker.CheckAllParam_InsertCharacter(*pInsertCharInfo))
	{
		mlog3("Invalid Param - InsertCharacter(Name:%S, Sex:%d, Hair:%d, Face:%d, Voice:%d, HairColor:%d, SkinColor:%d, EyeColor:%d, TalentStyle:%d, MakeUp:%d, TattooType:%d, TattooScale:%d, TattooColor:%d, EquipmentColor:%d)\n"
			, pInsertCharInfo->szName, (int)pInsertCharInfo->nSex, (int)pInsertCharInfo->nHair, (int)pInsertCharInfo->nFace, (int)pInsertCharInfo->nVoice
			, (int)pInsertCharInfo->nHairColor, (int)pInsertCharInfo->nSkinColor, pInsertCharInfo->nEyeColor, pInsertCharInfo->nTalentStyle
			, pInsertCharInfo->nMakeUp, pInsertCharInfo->nTattooType, pInsertCharInfo->nTattooScale, pInsertCharInfo->nTattooColor, pInsertCharInfo->nEquipmentColorIndex);

		cmdRouter.ResponseInsertChar(uidPlayer, CR_FAIL_CHAR_INVALID_PARAMETER);
		return false;
	}

	return true;
}

bool LPlayerLogic::ApplyInsertCharacter(MUID uidPlayer, AID nAID, TD_INSERT_CHARACTER_INFO* pInsertCharInfo)
{
	DBP_CHAR_INSERT param;
	param.uidPlayer = uidPlayer;
	param.nAID = nAID;
	param.Assign(pInsertCharInfo);
	
	// Level, XP, Silver 기본값
	param.nLevel = 1;
	param.nXP = 0;
	param.nSilver = 0;

	// HP, EN, STA, TP 기본값
	param.nTotalTP = PLAYER_DEFAULT_TP;
	param.nHP = CSBasicAttributeCalculator::CalcMaxHP(param.nLevel, PLAYER_DEFAULT_CON);
	param.nEN = CSBasicAttributeCalculator::CalcMaxEN(param.nLevel, PLAYER_DEFAULT_INT);
	param.nSTA = CSBasicAttributeCalculator::CalcMaxSP(param.nLevel, PLAYER_DEFAULT_DEX);


	param.nFatigue = NORMAL_FATIGUE;
	param.nSoulBindingID = PLAYER_DEFAULT_SOULBINDINGID;
	param.nInnRoomID = PLAYER_DEFAULT_INNROOMID;
	param.nPrimaryPalettedID = PLAYER_DEFAULT_PRIPALETTE;
	param.nSecondaryPalettedID = PLAYER_DEFAULT_SECPALETTE;

	if (!gsys.pDBManager->InsertCharacter(param))
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseInsertChar(uidPlayer, CR_FAIL_CHAR_POST_INSERT_DB);

		return false;
	}

	return true;
}