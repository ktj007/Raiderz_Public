#include "stdafx.h"
#include "GUTHelper.h"
#include "GUTHelper_Buff.h"
#include "GUTHelper_Talent.h"
#include "GPlayerGameData.h"
#include "MockField.h"
#include "GCommandTable.h"
#include "GMasterServerFacade.h"
#include "MockLink.h"
#include "GCommand.h"
#include "FBaseNetClient.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GPlayerTalent.h"
#include "GPalette.h"
#include "GFieldInfoMgr.h"
#include "GPlayerBattleArena.h"
#include "GPaletteItem.h"
#include "GModuleBuff.h"
#include "GPlayerFactions.h"
#include "GPlayerFaction.h"
#include "GFactionInfo.h"
#include "CSFactionCalculator.h"
#include "GFactionSystem.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GPlayerQObjective.h"
#include "GQuestInfo.h"
#include "GPlayerGameData.h"
#include "GServerGateRouter.h"
#include "GPlayerFieldDynamic.h"
#include "GActorCooltimeChecker.h"
#include "GTestForward.h"
#include "GUTHelper_Field.h"
#include "GPlayerCutscene.h"
#include "GItemHolder.h"
#include "GPlayerRecipe.h"
#include "GTestForward.h"
#include "GEquipmentSlot.h"

SUITE(PlayerGameData)
{
	struct Fixture: public FBaseNetClient
	{
		Fixture()
		{
			// 마스터서버 무시 커맨드 추가 - 필드에 플레이어 추가/삭제 시, 커맨드 발생
			m_pNetAgent->IgnoreCmd(MMC_PLAYER_IN_FIELD_NOTIFY);


			// 플레이어 생성
			MUID uidPlayer = SUnitTestUtil::NewUID();
			vec3 vPos = vec3(100, 100, 100);

			m_pFieldInfo = GUTHelper_Field::NewFieldInfo(107);
			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
			m_pEntityPlayer = m_helper.NewEntityPlayerForGameData(m_pField, vPos, uidPlayer);
		}
		
		~Fixture()
		{
			// 사용 오브젝트 정리
			m_pField->Destroy();
			SAFE_DELETE(m_pField);

			gmgr.pFieldInfoMgr->Clear();
		}

		GUTHelper m_helper;
		GFieldInfo* m_pFieldInfo;
		MockField* m_pField;
		GEntityPlayer* m_pEntityPlayer;

		DECLWRAPPER_FieldInfoMgr;
	};

	class TestPlayerGameData: public GPlayerGameData
	{
	public:
		using GPlayerGameData::PackMovePlayerInfo;
		using GPlayerGameData::UnpackMovePlayerInfo;

		using GPlayerGameData::PackGateInfo;
		using GPlayerGameData::UnpackGateInfo;

		using GPlayerGameData::PackCharaInfo;
		using GPlayerGameData::UnpackCharaInfo_Pre;
		using GPlayerGameData::UnpackCharaInfo_Post;

		using GPlayerGameData::PackItemList;
		using GPlayerGameData::UnpackItemList;

		using GPlayerGameData::PackTalent;
		using GPlayerGameData::UnpackTalent;

		using GPlayerGameData::PackPaletteList;
		using GPlayerGameData::UnpackPaletteList;

		using GPlayerGameData::PackTalentRemainTime;
		using GPlayerGameData::UnpackTalentRemainTime;

		using GPlayerGameData::PackQuestInstance;
		using GPlayerGameData::UnpackQuestInstance;

		using GPlayerGameData::PackFaction;	
		using GPlayerGameData::UnpackFaction;

		using GPlayerGameData::PackSawnCutscene;	
		using GPlayerGameData::UnpackSawnCutscene;

		using GPlayerGameData::PackRecipe;
		using GPlayerGameData::UnpackRecipe;
	};

	TEST_FIXTURE(Fixture, TestMakeMoveGameServerCommand)
	{
		// 서버이동 커맨드 전달
		int nTargetGameServerID = 999;
		
		GPlayerGameData playerGameData;
		playerGameData.PackFromEntityPlayer(m_pEntityPlayer);

		MUID uidRequestPlayer = m_pEntityPlayer->GetUID();
		GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
		pServerRouter->RequestReserveMoveGameServer(uidRequestPlayer, nTargetGameServerID, &playerGameData);	


		// 커맨드 확인
		MCommand& cmd = m_pNetAgent->GetSendCommand(0);
		CHECK_EQUAL(cmd.GetID(), MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(cmd.GetParameterCount(), 17);
		

		// 요청 파라미터 확인
		int nTargetGameServerIDParam;
		MUID uidRequestPlayerParam;		
		if (!cmd.GetParameter(&nTargetGameServerIDParam,	0, MPT_INT))	{ CHECK(!"Wrong parameter");	return;	}
		if (!cmd.GetParameter(&uidRequestPlayerParam,		1, MPT_UID))	{ CHECK(!"Wrong parameter");	return;	}		
		
		CHECK_EQUAL(nTargetGameServerIDParam, nTargetGameServerID);
		CHECK_EQUAL(uidRequestPlayerParam, m_pEntityPlayer->GetUID());


		// 캐릭터정보 파라미터 타입 확인
		for(int i = 2; i < cmd.GetParameterCount(); i++)
		{
			MCommandParameter* pParameter = cmd.GetParameter(i);
			CHECK_EQUAL(pParameter->GetType(), MPT_BLOB);
		}
	}

	TEST_FIXTURE(Fixture, TestPackMovePlayerInfo)
	{
		// 이동 플레이어 정보 Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackMovePlayerInfo(m_pEntityPlayer);


		// 이동 플레이어 정보 Unpack - UnpackMovePlayerInfo는 EntityPlayer에 값을 설정하지 않는다.
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackMovePlayerInfo(&unpackPlayer);


		// 정보 비교
		MUID uidPlayer = m_pEntityPlayer->GetUID();
		GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
		ACCOUNT_INFO& AccountInfo = pPlayerObject->GetAccountInfo();
		
		CHECK_EQUAL(playerGameData.GetEnterKey(), 0);			// ClientIP, 현재는 0
		CHECK_EQUAL(playerGameData.GetAID(), AccountInfo.nAID);
	}

	TEST_FIXTURE(Fixture, TestPackCharaInfo_Pre)
	{
		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackGateInfo(m_pEntityPlayer);	// dependency
		playerGameData.PackCharaInfo(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;		
		playerGameData.UnpackGateInfo(&unpackPlayer);	// dependecy
		playerGameData.UnpackCharaInfo_Pre(&unpackPlayer);


		// 정보비교 - PlayerInfo
		PLAYER_INFO* pPackPlayerInfo = m_pEntityPlayer->GetPlayerInfo();
		PLAYER_INFO* pUnpackPlayerInfo = unpackPlayer.GetPlayerInfo();
	
		CHECK_EQUAL(pUnpackPlayerInfo->nSex, pPackPlayerInfo->nSex);
		CHECK_EQUAL(pUnpackPlayerInfo->nCID, pPackPlayerInfo->nCID);
		CHECK_EQUAL(pUnpackPlayerInfo->nLevel, pPackPlayerInfo->nLevel);
		CHECK_EQUAL(pUnpackPlayerInfo->nXP, pPackPlayerInfo->nXP);
		CHECK_EQUAL(pUnpackPlayerInfo->GetMoney(), pPackPlayerInfo->GetMoney());
		CHECK_EQUAL(pUnpackPlayerInfo->nGID, pPackPlayerInfo->nGID);
		CHECK_EQUAL(pUnpackPlayerInfo->nFeatureHair, pPackPlayerInfo->nFeatureHair);
		CHECK_EQUAL(pUnpackPlayerInfo->nFeatureFace, pPackPlayerInfo->nFeatureFace);
		CHECK_EQUAL(pUnpackPlayerInfo->nFeatureHairColor, pPackPlayerInfo->nFeatureHairColor);
		CHECK_EQUAL(pUnpackPlayerInfo->nFeatureSkinColor, pPackPlayerInfo->nFeatureSkinColor);
		CHECK_EQUAL(pUnpackPlayerInfo->nEyeColor, pPackPlayerInfo->nEyeColor);

		CHECK_EQUAL(pUnpackPlayerInfo->nMakeUp, pPackPlayerInfo->nMakeUp);
		CHECK_EQUAL(pUnpackPlayerInfo->nTattooType, pPackPlayerInfo->nTattooType);
		CHECK_EQUAL(pUnpackPlayerInfo->nTattooPosX, pPackPlayerInfo->nTattooPosX);
		CHECK_EQUAL(pUnpackPlayerInfo->nTattooPosY, pPackPlayerInfo->nTattooPosY);
		CHECK_EQUAL(pUnpackPlayerInfo->nTattooScale, pPackPlayerInfo->nTattooScale);

		CHECK_EQUAL(pUnpackPlayerInfo->nSoulBindingID, pPackPlayerInfo->nSoulBindingID);
		CHECK_EQUAL(pUnpackPlayerInfo->nInnRoomID, pPackPlayerInfo->nInnRoomID);		
		CHECK(0 == wcscmp(pUnpackPlayerInfo->szName, pPackPlayerInfo->szName));
		CHECK_EQUAL(pUnpackPlayerInfo->nFatigueQuantity, pPackPlayerInfo->nFatigueQuantity);
		CHECK_EQUAL(pUnpackPlayerInfo->nBlankTimeSec, pPackPlayerInfo->nBlankTimeSec);
		CHECK_EQUAL(pUnpackPlayerInfo->nPlayerGrade, pPackPlayerInfo->nPlayerGrade);
		CHECK_EQUAL(pUnpackPlayerInfo->nPlayTimeSec, pPackPlayerInfo->nPlayTimeSec);
		CHECK_EQUAL(pUnpackPlayerInfo->nElapsedDeadTimeSec, pPackPlayerInfo->nElapsedDeadTimeSec);
		

		// 정보비교 - Talent
		GPlayerTalent& packPlayerTalent = m_pEntityPlayer->GetTalent();
		GPlayerTalent& unpackPlayerTalent = unpackPlayer.GetTalent();		
		
		CHECK_EQUAL(unpackPlayerTalent.GetTP(), packPlayerTalent.GetTP());
		

		// 정보비교 - Palette
		GPalette& packPlayerPalette = m_pEntityPlayer->GetPalette();
		GPalette& unpackPlayerPalette = unpackPlayer.GetPalette();

		CHECK_EQUAL((int)unpackPlayerPalette.GetSelectedPalette(), (int)packPlayerPalette.GetSelectedPalette());
		CHECK_EQUAL((int)unpackPlayerPalette.GetPrimary(), (int)packPlayerPalette.GetPrimary());
		CHECK_EQUAL((int)unpackPlayerPalette.GetSecondary(), (int)packPlayerPalette.GetSecondary());


		// 정보비교 - FieldInfo	
		GPlayerField& packPlayerField = m_pEntityPlayer->GetPlayerField();
		GPlayerField& unpackPlayerField = unpackPlayer.GetPlayerField();
		
		MUID uidDynamicfieldGroupUID = packPlayerField.GetDynamicFieldGroupUID();
		if (uidDynamicfieldGroupUID.IsValid())
		{
			const MARKER_INFO& packPlayerEnterMarkerInfo = packPlayerField.GetFieldDynamic().GetEnterPos();
			const MARKER_INFO& unpackPlayerEnterMarkerInfo = unpackPlayerField.GetFieldDynamic().GetEnterPos();

			CHECK_EQUAL(unpackPlayerEnterMarkerInfo.nFieldID, packPlayerEnterMarkerInfo.nFieldID);
			CHECK_EQUAL(unpackPlayerEnterMarkerInfo.vPoint, packPlayerEnterMarkerInfo.vPoint);
			
			CHECK_EQUAL(unpackPlayerField.GetDynamicFieldGroupUID(), packPlayerField.GetDynamicFieldGroupUID());


			const MARKER_INFO& packPlayerCheckMarkerInfo = packPlayerField.GetFieldDynamic().GetCheckpoint();
			const MARKER_INFO& unpackPlayerCheckMarkerInfo = unpackPlayerField.GetFieldDynamic().GetCheckpoint();

			CHECK_EQUAL(unpackPlayerCheckMarkerInfo.nFieldID, packPlayerCheckMarkerInfo.nFieldID);
			CHECK_EQUAL(packPlayerCheckMarkerInfo.vPoint, packPlayerCheckMarkerInfo.vPoint);
		}
		

		// 정보비교 - Party and Arena
		GPlayerBattleArena& packPlayerBattleArena = m_pEntityPlayer->GetPlayerBattleArena();
		GPlayerBattleArena& unpackPlayerBattleArena = unpackPlayer.GetPlayerBattleArena();

		CHECK_EQUAL(unpackPlayerBattleArena.GetBattleArenaUID(), packPlayerBattleArena.GetBattleArenaUID());
		CHECK_EQUAL(unpackPlayer.GetPartyUID(), m_pEntityPlayer->GetPartyUID());
	}

	TEST_FIXTURE(Fixture, TestPackCharaInfo_Post)
	{
		//
		m_helper.EquipNewItem(m_pEntityPlayer, 1, ITEMSLOT_LWEAPON);
		m_helper.EquipNewItem(m_pEntityPlayer, 2, ITEMSLOT_RWEAPON);

		// 보조무기 장비 후, 보조무기 사용
		GItem* pLWeapon2 = m_helper.GiveNewItem(m_pEntityPlayer, 3, 1);
		pLWeapon2->m_pItemData->m_nItemSlot = ITEMSLOT_LWEAPON;

		GItem* pRWeapon2 = m_helper.GiveNewItem(m_pEntityPlayer, 4, 1);
		pRWeapon2->m_pItemData->m_nItemSlot = ITEMSLOT_RWEAPON;

		m_helper.EquipItem(m_pEntityPlayer, pLWeapon2, ITEMSLOT_LWEAPON2);
		m_helper.EquipItem(m_pEntityPlayer, pRWeapon2, ITEMSLOT_RWEAPON2);
		m_pEntityPlayer->GetItemHolder()->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);

		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackCharaInfo(m_pEntityPlayer);
		playerGameData.PackItemList(m_pEntityPlayer);		///< Dependency


		// Unpack
		GEntityPlayer unpackPlayer;
		unpackPlayer.Create(SUnitTestUtil::NewUID());
		playerGameData.UnpackCharaInfo_Pre(&unpackPlayer);	///< Dependency
		playerGameData.UnpackItemList(&unpackPlayer);		///< Dependency
		playerGameData.UnpackCharaInfo_Post(&unpackPlayer);

		// 정보비교 - CharacterStatus
		PLAYER_INFO* pPackPlayerInfo = m_pEntityPlayer->GetPlayerInfo();
		PLAYER_INFO* pUnpackPlayerInfo = unpackPlayer.GetPlayerInfo();

		CHECK_EQUAL(pUnpackPlayerInfo->nSTR, pPackPlayerInfo->nSTR);
		CHECK_EQUAL(pUnpackPlayerInfo->nDEX, pPackPlayerInfo->nDEX);
		CHECK_EQUAL(pUnpackPlayerInfo->nINT, pPackPlayerInfo->nINT);
		CHECK_EQUAL(pUnpackPlayerInfo->nCHA, pPackPlayerInfo->nCHA);
		CHECK_EQUAL(pUnpackPlayerInfo->nCON, pPackPlayerInfo->nCON);

		CHECK_EQUAL(pUnpackPlayerInfo->nMaxHP, pPackPlayerInfo->nMaxHP);
		CHECK_EQUAL(pUnpackPlayerInfo->nMaxEN, pPackPlayerInfo->nMaxEN);
		CHECK_EQUAL(pUnpackPlayerInfo->nMaxSTA, pPackPlayerInfo->nMaxSTA);


		CHR_STATUS* pPackPlayerChrStatus = m_pEntityPlayer->GetChrStatus();
		CHR_STATUS* pUnpackPlayerChrStatus = unpackPlayer.GetChrStatus();

		CHECK_EQUAL(pUnpackPlayerChrStatus->nHP, pPackPlayerChrStatus->nHP);
		CHECK_EQUAL(pUnpackPlayerChrStatus->nEN, pPackPlayerChrStatus->nEN);
		CHECK_EQUAL(pUnpackPlayerChrStatus->nSTA, pPackPlayerChrStatus->nSTA);


		// 정보비교 - WeaponSet
		GItemHolder* pPackItemHolder = m_pEntityPlayer->GetItemHolder();
		GItemHolder* pUnpackItemHolder = unpackPlayer.GetItemHolder();

		CHECK_EQUAL(pUnpackItemHolder->GetEquipment().GetWeaponSet(), pPackItemHolder->GetEquipment().GetWeaponSet());

		// 플레이어 정리
		unpackPlayer.Destroy();
	}

	TEST_FIXTURE(Fixture, TestPackEquipAndItem)
	{
		// 더미 아이템 설정
		m_helper.GiveNewItem(m_pEntityPlayer, 1, 2);
		m_helper.GiveNewItem(m_pEntityPlayer, 2, 5);
		m_helper.GiveNewItem(m_pEntityPlayer, 2, 5);
		m_helper.EquipNewItem(m_pEntityPlayer, 3, ITEMSLOT_HEAD);
		m_helper.EquipNewItem(m_pEntityPlayer, 4, ITEMSLOT_FEET);
		m_helper.EquipNewItem(m_pEntityPlayer, 5, ITEMSLOT_LWEAPON);
		m_helper.EquipNewItem(m_pEntityPlayer, 6, ITEMSLOT_RWEAPON);
		test.item.GiveNewItemToStorage(m_pEntityPlayer, 1, 1);
		test.item.GiveNewItemToStorage(m_pEntityPlayer, 1, 2);

		// 보조무기 장비 후, 보조무기 사용
		GItem* pLWeapon2 = m_helper.GiveNewItem(m_pEntityPlayer, 7, 1);
		pLWeapon2->m_pItemData->m_bLeftWeaponEquipable = true;
		pLWeapon2->m_pItemData->m_nItemSlot = ITEMSLOT_LWEAPON;
		for(int i = 0; i < ENCHANT_MAX_COUNT; ++i)
			pLWeapon2->m_nEnchants[i] = 1000 + i;

		GItem* pRWeapon2 = m_helper.GiveNewItem(m_pEntityPlayer, 8, 1);
		pRWeapon2->m_pItemData->m_nItemSlot = ITEMSLOT_RWEAPON;
		for(int i = 0; i < ENCHANT_MAX_COUNT; ++i)
			pRWeapon2->m_nEnchants[i] = 2000 + i;

		m_helper.EquipItem(m_pEntityPlayer, pLWeapon2, ITEMSLOT_LWEAPON2);
		m_helper.EquipItem(m_pEntityPlayer, pRWeapon2, ITEMSLOT_RWEAPON2);		
		m_pEntityPlayer->GetItemHolder()->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);



		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackCharaInfo(m_pEntityPlayer);		// dependency
		playerGameData.PackItemList(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		unpackPlayer.Create(SUnitTestUtil::NewUID());
		playerGameData.UnpackCharaInfo_Pre(&unpackPlayer);		// dependency
		playerGameData.UnpackItemList(&unpackPlayer);

		// 정보비교
		GItemHolder* pPackItemHolder = m_pEntityPlayer->GetItemHolder();
		GItemHolder* pUnpackItemHolder = unpackPlayer.GetItemHolder();

		CHECK_EQUAL(pUnpackItemHolder->GetItemCount(SLOTTYPE_EQUIP), pPackItemHolder->GetItemCount(SLOTTYPE_EQUIP));
		CHECK_EQUAL(pUnpackItemHolder->GetItemCount(SLOTTYPE_INVENTORY), pPackItemHolder->GetItemCount(SLOTTYPE_INVENTORY));
		CHECK_EQUAL(pUnpackItemHolder->GetItemCount(SLOTTYPE_STORAGE), pPackItemHolder->GetItemCount(SLOTTYPE_STORAGE));

		vector<GItem*> vecItem;
		pPackItemHolder->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
		pPackItemHolder->CollectAllItem(SLOTTYPE_INVENTORY, vecItem);
		pPackItemHolder->CollectAllItem(SLOTTYPE_STORAGE, vecItem);
		
		for each(GItem* pPackItem in vecItem)
		{
			GItem* pUnpackItem = pUnpackItemHolder->GetItem(pPackItem->m_nSlotType, pPackItem->m_nIUID);
			if (pUnpackItem == NULL)
			{	
				CHECK(!"No exist item.");	continue;	
			}

			CHECK_EQUAL(pUnpackItem->m_pItemData->m_nID, pPackItem->m_pItemData->m_nID);

			CHECK_EQUAL(pUnpackItem->m_nIUID, pPackItem->m_nIUID);			
			CHECK_EQUAL(pUnpackItem->GetAmount(), pPackItem->GetAmount());
			CHECK_EQUAL(pUnpackItem->m_nDurability, pPackItem->m_nDurability);	
			CHECK_EQUAL(pUnpackItem->m_bBind, pPackItem->m_bBind);
			CHECK_EQUAL(pUnpackItem->m_nSoulCount, pPackItem->m_nSoulCount);
			CHECK_EQUAL(pUnpackItem->m_nDyedColor, pPackItem->m_nDyedColor);
			CHECK_EQUAL(pUnpackItem->m_UsagePeriod.IsStartedCalculatingUsagePeriod(), pPackItem->m_UsagePeriod.IsStartedCalculatingUsagePeriod());
			CHECK_EQUAL(pUnpackItem->m_UsagePeriod.IsPeriod(), pPackItem->m_UsagePeriod.IsPeriod());
			CHECK_EQUAL(pUnpackItem->m_UsagePeriod.GetPeriodSec(), pPackItem->m_UsagePeriod.GetPeriodSec());
			CHECK_EQUAL(pUnpackItem->m_UsagePeriod.GetUsageStartCharPlayTimeSec(), pPackItem->m_UsagePeriod.GetUsageStartCharPlayTimeSec());
			CHECK_EQUAL(pUnpackItem->m_ExpirationPeriod.IsStartedCalculatingUsagePeriod(), true);	// 접속 시, 무조건 감소
			CHECK_EQUAL(pUnpackItem->m_ExpirationPeriod.IsPeriod(), pPackItem->m_ExpirationPeriod.IsPeriod());
			CHECK_EQUAL(pUnpackItem->m_ExpirationPeriod.GetPeriodSec(), pPackItem->m_ExpirationPeriod.GetPeriodSec());
			CHECK_EQUAL(pUnpackItem->m_ExpirationPeriod.GetUsageStartCharPlayTimeSec(), pPackItem->m_ExpirationPeriod.GetUsageStartCharPlayTimeSec());
	
			CHECK_EQUAL(pUnpackItem->m_bEquiped, pPackItem->m_bEquiped);

			for(int i = 0; i < ENCHANT_MAX_COUNT; ++i)
			{
				CHECK_EQUAL(pUnpackItem->m_nEnchants[i], pPackItem->m_nEnchants[i]);
			}
		}


		PLAYER_INFO* pPackPlayerInfo = m_pEntityPlayer->GetPlayerInfo();
		PLAYER_INFO* pUnpackPlayerInfo = unpackPlayer.GetPlayerInfo();

		for(int i = 0; i < ITEMSLOT_MAX; i++)
		{			
			const GItem* pPlayerItem = pPackItemHolder->GetItem(SLOTTYPE_EQUIP, SH_ITEM_SLOT(i));
			const GItem* pUpackPlayerItem = pUnpackItemHolder->GetItem(SLOTTYPE_EQUIP, SH_ITEM_SLOT(i));

			if (NULL == pPlayerItem || NULL == pUpackPlayerItem)
			{
				continue;
			}

			CHECK_EQUAL(pPlayerItem->m_nIUID, pUpackPlayerItem->m_nIUID);
			
			GItem* pPackPlayerEquipItem = pPackItemHolder->GetItem(SLOTTYPE_EQUIP, (SH_ITEM_SLOT)i);
			GItem* pUnpackPlayerEquipItem = pUnpackItemHolder->GetItem(SLOTTYPE_EQUIP, (SH_ITEM_SLOT)i);

			if (pPackPlayerEquipItem == NULL && pUnpackPlayerEquipItem == NULL)
			{
				continue;
			}

			if (pPackPlayerEquipItem != NULL && pUnpackPlayerEquipItem != NULL)
			{
				CHECK_EQUAL(pUnpackPlayerEquipItem->m_nIUID, pPackPlayerEquipItem->m_nIUID);
			}
			else
			{
				CHECK(!"Exist not equal equipment.");
			}
		}

		// 플레이어 정리
		unpackPlayer.Destroy();
	}

	TEST_FIXTURE(Fixture, TestPackEquipAndItem_BaseOverSizeItem)
	{
		for(int i = 0; i < BASE_INVENTORY_SIZE + 10; i++)
		{
			m_helper.GiveNewItem(m_pEntityPlayer);
		}

		TestPlayerGameData playerGameData;
		playerGameData.PackItemList(m_pEntityPlayer);

		CHECK_EQUAL(playerGameData.GetItemInstance().size(), BASE_INVENTORY_SIZE);
	}

	TEST_FIXTURE(Fixture, TestPackTalent)
	{
		// 텔런트 설정
		m_helper.LearnNewTalent(m_pEntityPlayer, 100, 1);
		m_helper.LearnNewTalent(m_pEntityPlayer, 100, 2);
		m_helper.LearnNewTalent(m_pEntityPlayer, 100, 3);
		m_helper.LearnNewTalent(m_pEntityPlayer, 200, 1);
		m_helper.LearnNewTalent(m_pEntityPlayer, 300, 1);


		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackTalent(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackTalent(&unpackPlayer);


		// 정보비교
		GPlayerTalent& packPlayerTalent = m_pEntityPlayer->GetTalent();
		GPlayerTalent& unpackPlayerTalent = unpackPlayer.GetTalent();

		SET_TALENTID& setPackPlayerTalentID = packPlayerTalent.GetContainer();
		SET_TALENTID& setUnpackPlayerTalentID = unpackPlayerTalent.GetContainer();

		CHECK_EQUAL(setUnpackPlayerTalentID.size(), setPackPlayerTalentID.size());

		for each(int nTalentID in setPackPlayerTalentID)
		{
			SET_TALENTID::iterator it = setUnpackPlayerTalentID.find(nTalentID);
			CHECK(it != setUnpackPlayerTalentID.end());
		}
	}

	TEST_FIXTURE(Fixture, TestPackPaletteList)
	{
		// 팔레트 설정
		m_pEntityPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_ITEM, 10);
		m_pEntityPlayer->GetPalette().Set(PALETTENUM_2, PALETTESLOT_2, PIT_ITEM, 11);
		m_pEntityPlayer->GetPalette().Set(PALETTENUM_3, PALETTESLOT_3, PIT_TALENT, 12);
		m_pEntityPlayer->GetPalette().Set(PALETTENUM_4, PALETTESLOT_4, PIT_TALENT, 13);


		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackPaletteList(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackPaletteList(&unpackPlayer);		



		// 정보비교
		GPalette& packPlayerPalette = m_pEntityPlayer->GetPalette();
		GPalette& unpackPlayerPalette = unpackPlayer.GetPalette();

		VEC_PALETTE_ITEM& vecPackPlayerPaletteItem = packPlayerPalette.GetContainer();
		VEC_PALETTE_ITEM& vecUnpackPlayerPaletteItem = unpackPlayerPalette.GetContainer();

		CHECK_EQUAL(vecUnpackPlayerPaletteItem.size(), vecPackPlayerPaletteItem.size());

		if (vecUnpackPlayerPaletteItem.size() != vecPackPlayerPaletteItem.size())
		{
			return;
		}

		for(unsigned int i = 0; i < vecPackPlayerPaletteItem.size(); i++)
		{
			CHECK_EQUAL(vecUnpackPlayerPaletteItem[i].IsEmpty(), vecPackPlayerPaletteItem[i].IsEmpty());

			if (vecUnpackPlayerPaletteItem[i].IsEmpty())	continue;

			CHECK_EQUAL((int)vecUnpackPlayerPaletteItem[i].m_nType, (int)vecPackPlayerPaletteItem[i].m_nType);
			CHECK_EQUAL(vecUnpackPlayerPaletteItem[i].m_nItemIDorTalentID, vecPackPlayerPaletteItem[i].m_nItemIDorTalentID);
		}
	}

	TEST_FIXTURE(Fixture, TestPackTalentRemainTime)
	{
		// Talent 설정
		m_helper.NewTalentInfo(1, 100, 1);
		m_helper.NewTalentInfo(2, 100, 2);
		m_helper.NewTalentInfo(3, 200, 1);
		m_helper.NewTalentInfo(4, 300, 1);

		vector<pair<int, float>> vecTestTalentData;
		vecTestTalentData.push_back(make_pair(1, 1000.0f));
		vecTestTalentData.push_back(make_pair(2, 2000.0f));
		vecTestTalentData.push_back(make_pair(3, 3000.0f));
		vecTestTalentData.push_back(make_pair(4, 4000.0f));

		vector<REMAIN_BUFF_TIME> vecTestBuffData;

		REMAIN_BUFF_TIME remainBuffTime;
		remainBuffTime.nID = 1;
		remainBuffTime.fRemainDurationSeconds = 1000.0f;
		vecTestBuffData.push_back(remainBuffTime);

		remainBuffTime.nID = 2;
		remainBuffTime.fRemainDurationSeconds = 2000.0f;
		vecTestBuffData.push_back(remainBuffTime);

		remainBuffTime.nID = 3;
		remainBuffTime.fRemainDurationSeconds = 3000.0f;
		vecTestBuffData.push_back(remainBuffTime);
		
		remainBuffTime.nID = 4;		
		remainBuffTime.fRemainDurationSeconds = 4000.0f;
		vecTestBuffData.push_back(remainBuffTime);


		m_pEntityPlayer->GetActorCooltimeChecker().InsertTalentRemainCoolTime(vecTestTalentData);
		m_pEntityPlayer->InsertBuffRemainTime(vecTestBuffData);


		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackTalentRemainTime(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		unpackPlayer.Create(SUnitTestUtil::NewUID());
		playerGameData.UnpackTalentRemainTime(&unpackPlayer);


		// 정보비교		
		vector<pair<int,float>> vecPackPlayerTalentTime;
		vector<pair<int,float>> vecUnpackPlayerTalentTime;
		m_pEntityPlayer->GetActorCooltimeChecker().GetTalentRemainCoolTime(vecPackPlayerTalentTime);
		unpackPlayer.GetActorCooltimeChecker().GetTalentRemainCoolTime(vecUnpackPlayerTalentTime);

		CHECK(vecUnpackPlayerTalentTime == vecPackPlayerTalentTime);
		

		vector<REMAIN_BUFF_TIME> vecPackPlayerBuffTime;
		vector<REMAIN_BUFF_TIME> vecUnpackPlayerBuffTime;
		m_pEntityPlayer->GetBuffRemainTime(vecPackPlayerBuffTime);
		unpackPlayer.GetBuffRemainTime(vecUnpackPlayerBuffTime);

		CHECK(vecUnpackPlayerTalentTime == vecPackPlayerTalentTime);


		// 플레이어 정리
		unpackPlayer.Destroy();
	}

	TEST_FIXTURE(Fixture, TestPackQuestInstance)
	{
		// 퀘스트 설정
		test.quest.GiveNewPlayerQuest(m_pEntityPlayer);		
		test.quest.GiveNewPlayerDoneQuest(m_pEntityPlayer);		
		test.quest.GiveNewPlayerQuest(m_pEntityPlayer, 0, 10, true);

		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackQuestInstance(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackQuestInstance(&unpackPlayer);


		// 정보비교
		GPlayerQuests& packPlayerQuests = m_pEntityPlayer->GetQuests();
		GPlayerQuests& unpackPlayerQuests = unpackPlayer.GetQuests();

		MAP_PLAYER_QUEST& mapPackPlayerQuest = packPlayerQuests.GetQuestContainer();
		MAP_PLAYER_QUEST& mapUnpackPlayerQuest = unpackPlayerQuests.GetQuestContainer();

		CHECK_EQUAL(mapUnpackPlayerQuest.size(), mapPackPlayerQuest.size());

		for each(MAP_PLAYER_QUEST::value_type quest in mapPackPlayerQuest)
		{
			MAP_PLAYER_QUEST::iterator it = mapUnpackPlayerQuest.find(quest.first);

			CHECK(it != mapUnpackPlayerQuest.end());
			if (it == mapUnpackPlayerQuest.end())	continue;

			CHECK_EQUAL(it->second->GetQuestVar(), quest.second->GetQuestVar());
			CHECK_EQUAL(it->second->GetState(), quest.second->GetState());
			CHECK_EQUAL(it->second->GetAcceptTime(), quest.second->GetAcceptTime());
			CHECK_EQUAL(unpackPlayerQuests.GetDoneCount(it->first), packPlayerQuests.GetDoneCount(quest.first));


			for(int i = 1; i <= 5; i++)
			{
				if (NULL == quest.second->Get(i))
				{
					CHECK(NULL == it->second->Get(i));
					continue;
				}

				if (NULL == it->second->Get(i))
				{
					CHECK(NULL == quest.second->Get(i));
					continue;
				}

				CHECK_EQUAL(it->second->Get(i)->GetProgress(), quest.second->Get(i)->GetProgress());
				CHECK_EQUAL(it->second->Get(i)->IsComplete(), quest.second->Get(i)->IsComplete());
			}
		}


		MAP_DONE_COUNT& mapPackPlayerDoneQuest = packPlayerQuests.GetDoneCountContainer();
		MAP_DONE_COUNT& mapUnpackPlayerDoneQuest = unpackPlayerQuests.GetDoneCountContainer();

		CHECK_EQUAL(mapUnpackPlayerDoneQuest.size(), mapPackPlayerDoneQuest.size());

		for each(MAP_DONE_COUNT::value_type doneCount in mapPackPlayerDoneQuest)
		{	
			CHECK_EQUAL(unpackPlayerQuests.GetDoneCount(doneCount.first), doneCount.second);
			CHECK_EQUAL(unpackPlayerQuests.GetDoneCount(doneCount.first), packPlayerQuests.GetDoneCount(doneCount.first));			
		}
	}

	TEST_FIXTURE(Fixture, TestPackFaction)
	{
		// Faction 설정
		GFactionInfo* pFationInfo1 = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		gsys.pFactionSystem->Increase(m_pEntityPlayer, pFationInfo1->m_nID, 1000);
		
		GFactionInfo* pFationInfo2 = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		gsys.pFactionSystem->Increase(m_pEntityPlayer, pFationInfo2->m_nID, 2000);

		
		// Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackFaction(m_pEntityPlayer);


		// Unpack
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackFaction(&unpackPlayer);


		// 정보비교
		GPlayerFactions& packPlayerFactions = m_pEntityPlayer->GetPlayerFactions();
		GPlayerFactions& unpackPlayerFactions = unpackPlayer.GetPlayerFactions();
		MAP_PLAYERFACTION& mapPackPlayerFaction = packPlayerFactions.GetContainer();
		MAP_PLAYERFACTION& mapUnpackPlayerFaction = unpackPlayerFactions.GetContainer();

		CHECK_EQUAL(mapUnpackPlayerFaction.size(), mapPackPlayerFaction.size());

		for each(MAP_PLAYERFACTION::value_type faction in mapPackPlayerFaction)
		{
			MAP_PLAYERFACTION::iterator it = mapUnpackPlayerFaction.find(faction.first);

			CHECK(it != mapUnpackPlayerFaction.end());
			if (it == mapUnpackPlayerFaction.end())	continue;

			CHECK_EQUAL(it->second->GetInfo()->m_nID, faction.second->GetInfo()->m_nID);
			CHECK_EQUAL(it->second->GetQuantity(), faction.second->GetQuantity());
		}
	}

	TEST_FIXTURE(Fixture, TestPackSawnCutsceneInfo)
	{
		// 본 컷씬 리스트 정보 설정
		int nTestSawnCutscene1 = 101;
		int nTestSawnCutscene2 = 102;

		std::vector<int> vecSawnCutscene;
		vecSawnCutscene.push_back(nTestSawnCutscene1);
		vecSawnCutscene.push_back(nTestSawnCutscene2);

		GPlayerCutscene& refPlayerCutscene = m_pEntityPlayer->GetCutscene();
		refPlayerCutscene.AssignSawnCutscene(vecSawnCutscene);


		// 이동 플레이어 정보 Pack
		TestPlayerGameData playerGameData;
		playerGameData.PackSawnCutscene(m_pEntityPlayer);


		// 이동 플레이어 정보 Unpack - UnpackMovePlayerInfo는 EntityPlayer에 값을 설정하지 않는다.
		GEntityPlayer unpackPlayer;
		playerGameData.UnpackSawnCutscene(&unpackPlayer);


		// 정보 비교
		GPlayerCutscene& refUnpackPlayerCutscene = unpackPlayer.GetCutscene();

		CHECK_EQUAL(2, refPlayerCutscene.GetContainer().size());
		CHECK_EQUAL(2, refUnpackPlayerCutscene.GetContainer().size());
		CHECK_EQUAL(refPlayerCutscene.GetContainer().size(), refUnpackPlayerCutscene.GetContainer().size());


		SET_SAWN_CUTSCENE& refSawnCutscene_Player = refPlayerCutscene.GetContainer();
		SET_SAWN_CUTSCENE& refSawnCutscene_UnpackPlayer = refUnpackPlayerCutscene.GetContainer();

		SET_SAWN_CUTSCENE::iterator iter;
		iter = refUnpackPlayerCutscene.GetContainer().find(nTestSawnCutscene1);
		CHECK(iter != refUnpackPlayerCutscene.GetContainer().end());

		iter = refUnpackPlayerCutscene.GetContainer().find(nTestSawnCutscene2);
		CHECK(iter != refUnpackPlayerCutscene.GetContainer().end());


		SET_SAWN_CUTSCENE::iterator iterPlayer = refPlayerCutscene.GetContainer().begin();
		SET_SAWN_CUTSCENE::iterator iterUnpackPlayer = refUnpackPlayerCutscene.GetContainer().begin();

		CHECK_EQUAL((*iterPlayer), (*iterUnpackPlayer));

		iterPlayer++;
		iterUnpackPlayer++;
		CHECK_EQUAL((*iterPlayer), (*iterUnpackPlayer));
	}

	TEST_FIXTURE(Fixture, TestPackRecipe)
	{
		for(int i = 0; i < 3; i++)
		{
			int nRecipeID = GUnitTestUtil::NewID();
			m_pEntityPlayer->GetPlayerRecipe().Insert(nRecipeID);
		}


		TestPlayerGameData playerGameData;
		playerGameData.PackRecipe(m_pEntityPlayer);

		GEntityPlayer unpackPlayer;
		playerGameData.UnpackRecipe(&unpackPlayer);


		const SET_RECIPEID& setPackedRecipe = m_pEntityPlayer->GetPlayerRecipe().GetContainer();
		const SET_RECIPEID& setUnpackRecipe = unpackPlayer.GetPlayerRecipe().GetContainer();

		CHECK(setPackedRecipe == setUnpackRecipe);		
	}
}
