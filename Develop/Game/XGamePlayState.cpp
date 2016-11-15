#include "stdafx.h"
#include "XGamePlayState.h"
#include "XPost_Loot.h"
#include "XNPCInfo.h"
#include "XBattleArena.h"
#include "XPvPAreaLogic.h"
#include "XModuleNetControl.h"
#include "XModuleNonControl.h"
#include "XModuleMyControl.h"
#include "XNonPlayer.h"
#include "XBPartNonPlayer.h"
#include "XMyPlayer.h"
#include "XMagicArea.h"
#include "XSoulObject.h"
#include "XFeedbackObject.h"
#include "XFeedbackSystem.h"
#include "XBaseApplication.h"
#include "MockWorld.h"
#include "XItemManager.h"
#include "XController.h"
#include "XCameraManager.h"
#include "XEffectManager.h"
#include "XResourceManager.h"
#include "XSoundField.h"
#include "XGamePlayController.h"
#include "XCharacter.h"
#include "XGameStateMgr.h"
#include "XEventID.h"
#include "XSensorManager.h"
#include "XWorldChanger.h"
#include "XInteractionInfo.h"
#include "XPost_Comm.h"
#include "XStrings.h"
#include "XScreenFader.h"

XGamePlayState::XGamePlayState( XGameStateMgr * pStateMgr, bool bRealGameNotTest /*= true*/ )
: XGameState(pStateMgr)
{
	m_nUpdateObjectCacheTime		= 0;
	m_bEnableUpdateObjectCache		= true;
	m_bRealGameNotTest				= bRealGameNotTest;

	m_pBattleArena					= new XBattleArena();
	m_pPvPAreaLogic					= new XPvPAreaLogic();

	if(bRealGameNotTest)
	{
		m_pWorld = new XWorld();
	}
	else
	{
		m_pWorld = new MockWorld();
	}

	m_pController = new XGamePlayController();
	m_pScreenFader = new XScreenFader;

	m_pWorldChanger = new XWorldChanger();
}

XGamePlayState::~XGamePlayState()
{
	SAFE_DELETE(m_pScreenFader);
	SAFE_DELETE(m_pWorldChanger);
	SAFE_DELETE(m_pPvPAreaLogic);
	SAFE_DELETE(m_pBattleArena);

	Destroy();
}

bool XGamePlayState::Create()
{
	if(_CreateWorld() == false)
		return false;

	// create my object
	_CreateMyPlayer();

	return XGameState::Create();
}

bool XGamePlayState::Ready( XGameState* pPreState, int nParam /*= 0*/ )
{
	return XGameState::Ready(pPreState, nParam);
}

void XGamePlayState::LoadResource()
{
	// 카메라 설정
	global.camera->SetPrimaryCamera(CAMERA_MAIN_BACKVIEW);

	// 내 캐릭터 생성
	_CreateMyPlayer();

	// 배경 사운드 재생
	PlayFieldBGMSound();
}

void XGamePlayState::Enter()
{
	m_pOwnerStateMgr->m_ResourceMgr.SubObjectResourceReferenCount();
}

void XGamePlayState::Exit()
{
	gvar.World.SetNextMapInfo(gvar.Game.pMyPlayer->GetPosition(), gvar.Game.pMyPlayer->GetDirection());

	if ( global.sound)		global.sound->StopAll();

	gvar.Game.pMyPlayer = NULL;

	XGameState::Exit();
}

void XGamePlayState::Update( float fDelta )
{
	XGameState::Update(fDelta);

	m_pPvPAreaLogic->Update(fDelta, gvar.MyInfo.FactionInfo);
}

void XGamePlayState::Destroy()
{
	XGameState::Destroy();
}

void StandAloneTest_AddMyItems()
{
#ifdef _PUBLISH
	return;
#endif

	int idx = 0;
	// xconst.xml에서 장비할 아이템 목록을 가져온다.
	wchar_t* token = wcstok(XCONST::STANDALONE_EQUIP_ITEMS, L" ");
	while (token != NULL)
	{
		idx++;
		int item_id = _wtoi(token);
		XItem* pNewItem = info.item->MakeNewItem(item_id);
		if (pNewItem == NULL) 
		{
			token = wcstok( NULL, L" " ); // C4996
			continue;
		}

		pNewItem->m_nDurability = 1;
		pNewItem->m_nAmount = 1;		
		pNewItem->m_nSlotID = idx - 1;
		pNewItem->m_pItemData = info.item->GetItemData(item_id);

		gvar.MyInfo.Inventory.PushItem(idx - 1, pNewItem);

		token = wcstok( NULL, L" " ); // C4996

		// 장착
		if (pNewItem->m_pItemData)
		{
			// EquipItem의 반환값은 이전에 장비하고 있던 아이템
			gvar.MyInfo.EquipmentSlot.EquipItem(pNewItem, pNewItem->m_pItemData->m_nItemSlot);
		}
	}
	if (global.ui)
		global.ui->InventoryUIRefresh();
}

XPlayer* XGamePlayState::InPlayer( MUID& uid, TD_UPDATE_CACHE_PLAYER* pPlayerInfo, TD_PLAYER_FEATURE_TATTOO* pTattooInfo, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	if (m_pObjectManager->Find(uid) != NULL) 
	{
		XPlayer* pThisPlayer = m_pObjectManager->FindPlayer(uid);
		if (pThisPlayer)
		{
			XModuleNetControl* pModuleNetControl = pThisPlayer->GetModuleNetControl();
			if (pModuleNetControl)
			{
				pModuleNetControl->OnReInPlayer(pPlayerInfo);
			}
		}
		return pThisPlayer;
	}

	XNetPlayer* pPlayer = AsNetPlayer(CreateObject(XOBJ_NET_PLAYER, uid, pPlayerInfo->nUIID));
	if (pPlayer)
	{
		if(bLoadingAsync == false && CheckModelLoadingAsync(pPlayerInfo->Feature.nSex) == true)
		{
			bLoadingAsync = true;
			bAppearEffect = true;
		}

		pPlayer->InPlayer(pPlayerInfo, pTattooInfo, bAppearEffect, bLoadingAsync);

		if (m_pBattleArena->IsInBattleArena())
		{
			m_pBattleArena->InNetPlayer(uid);
		}

		if (m_pPvPAreaLogic->IsInPvPArea()  &&  m_pPvPAreaLogic->IsEnemy( pPlayer) == true)
		{
			wstring strEnemyName = m_pPvPAreaLogic->MakeEnemyName( pPlayer);
			pPlayer->ChangeNameForPvP( strEnemyName);
			pPlayer->SetCharacterPane_VSNetPlayer();
		}


		//내가 pvp 중이면
		if (XGetInteractionInfo().VSPlayerUID == uid ||
			pPlayer->IsAlly(gvar.Game.pMyPlayer) == false)
		{
			pPlayer->SetCharacterPane_VSNetPlayer();
			pPlayer->SetDuel(true, XGetMyUID());
		}
	}

	return pPlayer;
}

void XGamePlayState::OutPlayer( MUID uid )
{
	XObject* pObject = m_pObjectManager->Find(uid);
	if (pObject == NULL) return;

	OutPlayer(pObject);
}

void XGamePlayState::OutPlayer( UIID nPlayerUIID )
{
	XActor* pPlayer = m_pObjectManager->FindActor_UIID(nPlayerUIID);
	if (pPlayer == NULL) return;

	OutPlayer(pPlayer);
}

void XGamePlayState::OutPlayer( XObject* pObject )
{
	XEvent msgDisAppear( XEVTD_DISAPPEAR_ME);
	pObject->PostDelayEvent( msgDisAppear, 0);

	m_pObjectManager->ReserveDelete(pObject->GetUID(), 1.0f);
}

void XGamePlayState::InNPCs( TD_UPDATE_CACHE_NPC* pNpcNode, int nNPCCount, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	for (int i = 0; i < nNPCCount; i++)
	{
		TD_UPDATE_CACHE_NPC* pMonsterInfo = &pNpcNode[i];

		XNPCInfo* pNPCInfo = info.npc->Get(pMonsterInfo->nNPCID);
		if (pNPCInfo == NULL) continue;

		InNPC(pNPCInfo, pMonsterInfo, bAppearEffect, bLoadingAsync);
	}
}

XNonPlayer* XGamePlayState::InNPC( XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_NPC* pMonsterInfo, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	MUID uid = pMonsterInfo->uid;

	if (m_pObjectManager->Find(uid) != NULL) 
	{
		XNonPlayer* pThisNPC = m_pObjectManager->FindNPC(uid);
		if (pThisNPC)
		{
			XModuleNonControl* pModuleNonControl = pThisNPC->GetModuleNonControl();
			if (pModuleNonControl)
			{
				pModuleNonControl->OnReInNonPlayer(pMonsterInfo);
			}
		}
		return pThisNPC;
	}


	XNonPlayer* pObject = AsNPC(CreateObject(XOBJ_NPC, pMonsterInfo->uid, pMonsterInfo->nUIID));
	if (pObject)
	{
		wstring strResourceName = m_pOwnerStateMgr->m_ResourceMgr.GetObjectResourceName(pNPCInfo->strMeshName + L".elu");
		if(bLoadingAsync == false && CheckModelLoadingAsync(strResourceName) == true)
		{
			bLoadingAsync = true;
			bAppearEffect = true;
		}

		pObject->InNPC(pNPCInfo, pMonsterInfo, bAppearEffect, bLoadingAsync);
	}
	return pObject;
}

void XGamePlayState::OutNPC( UIID nNPCUIID )
{
	// * BPartNonPlayer도 Out을 여기서한다.

	XActor* pObject = m_pObjectManager->FindActor_UIID(nNPCUIID);
	if (pObject == NULL) return;

	// NPC 오브젝트에만 해당사항이다. BPart NPC는 아니다.
	// 죽은 상태이면 자동으로 없어진다.
	if (pObject->GetType() == XOBJ_NPC &&
		pObject->IsDead()) return;

	XNonPlayer* pNpc = AsNPC(pObject);
	pNpc->Despawn();
}

void XGamePlayState::OutNPC( MUID uidNPC )
{
	// * BPartNonPlayer도 Out을 여기서한다.

	XActor* pNPC = m_pObjectManager->FindActor(uidNPC);
	if (pNPC)
	{
		OutNPC(pNPC->GetUIID());
	}
}

XMagicArea* XGamePlayState::InMagicArea( TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, bool bStarted /*= false*/ )
{
	if (m_pObjectManager->Find(pMagicArea->UID) != NULL)
		return NULL;

	XMagicArea* pNewMagicArea = static_cast<XMagicArea*>(CreateObject(XOBJ_MAGIC_AREA, pMagicArea->UID));
	if(pNewMagicArea == NULL)
		return NULL;

	pNewMagicArea->Setup(pMagicArea->vPos, pMagicArea->nBuffID, pMagicArea->fReaminTime, bStarted);

	return pNewMagicArea;
}

void XGamePlayState::InMagicAreas( TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, int nCount )
{
	for (int i = 0; i < nCount; i++)
	{
		InMagicArea(pMagicArea, true);
	}
}

void XGamePlayState::OutMagicArea( MUID& uid )
{
	XMagicArea* pMagicArea = m_pObjectManager->FindMagicArea(uid);
	if (pMagicArea == NULL) return;

	pMagicArea->Despawn();
}

void XGamePlayState::TrapTriggered( MUID& uid )
{
	XMagicArea* pMagicArea = m_pObjectManager->FindMagicArea(uid);
	if (pMagicArea == NULL) return;

	pMagicArea->TrapAction();
}

XObject* XGamePlayState::CreateObject( XObjectID nObjectID, MUID uid, UIID nUIID )
{
	XObject* pObject = m_pObjectManager->CreateObject(nObjectID, uid, nUIID);

	if(pObject == NULL)
		return NULL;

	pObject->Create();
	if (!m_pObjectManager->Add(pObject))
	{
		pObject->Destroy();
		SAFE_DELETE(pObject);
	}

	return pObject;
}

void XGamePlayState::CreateSoulObject( const MUID& uidOwnerNPC, const MUID& uidTarget, int nType, unsigned int nWaitTime /*=0*/ )
{
	XObject* pOwnerNpc = m_pObjectManager->Find( uidOwnerNPC);
	if ( pOwnerNpc == NULL)
	{
		ASSERT( 0);
		return;
	}

	if ( uidTarget == MUID::ZERO)
	{
		ASSERT( 0);
		return;
	}

	for ( int i = 0;  i < 4;  i++)
	{
		MUID uidSoulObject = global.system->GenerateLocalUID();
		XSoulObject* pSoulObject = new XSoulObject(uidSoulObject);
		pSoulObject->Create( pOwnerNpc, uidTarget, nType, nWaitTime + i * 100);

		if ( !m_pObjectManager->Add( pSoulObject))
		{
			mlog( "Cannot create soul object.\n");

			ASSERT( 0);
			SAFE_DELETE( pSoulObject);
		}
	}
}

MUID XGamePlayState::CreateFeedbackObject( vec3 vPos, vec3 vDir, wstring strName, wstring strComment )
{
	MUID uid = global.system->GenerateLocalUID();
	XFeedbackObject* pFeedbackObject = new XFeedbackObject(uid);
	pFeedbackObject->Create();

	pFeedbackObject->Init(vPos, vDir, strName, strComment);

	if ( !m_pObjectManager->Add( pFeedbackObject))
	{
		mlog( "Cannot create feedback object.\n");

		ASSERT( 0);
		SAFE_DELETE( pFeedbackObject);
	}

	return uid;
}

bool XGamePlayState::_CreateWorld()
{
	XWorldInitParam worldInitParam;
	worldInitParam.SetField(gvar.World.GetFieldID(), true);

	bool bInitWorldRet = m_pWorld->Init(worldInitParam);

	if (bInitWorldRet == false)
		return false;

	return true;
}

void XGamePlayState::_CreateMyPlayer()
{
	if (gvar.Game.pMyPlayer != NULL)
	{
		// 내 오브젝트는 Game 시작 제일 처음에만 생성해준다.
//		_ASSERT(0);
		return;
	}

	XObject* pObject = CreateObject(XOBJ_MY_PLAYER, XGetMyUID(), XGetMyUIID());
	if(pObject == NULL) return;

	XMyPlayer* pMyPlayer = pObject->AsMyPlayer();
	gvar.Game.pMyPlayer = pMyPlayer;

	gvar.MyInfo.EquipmentSlot.RefreshUI();
	if (global.ui)
		global.ui->InventoryUIRefresh();

	global.ui->SetSilver(gvar.MyInfo.ChrInfo.GetMoney());

	//*
	pMyPlayer->InMyInfo(this);


	if (global.app->GetMode() != EM_GAME)
	{
		StandAloneTest_AddMyItems();	// 서버에 접속없이 클라이언트 실행할 경우 아이템 넣어주기
	}

//	global.ui->SetFocusMainInput();
}

void XGamePlayState::OnWorldLoadingComplete()
{
	XGameState::OnWorldLoadingComplete();

	global.res->DropFieldResource();

	global.ui->OnWorldLoadingComplete();

	XPostField_LoadingComplete();


	XPlayer* pMyPlayer = static_cast<XPlayer*>(gg.omgr->Find(XGetMyUID()));
	if ( pMyPlayer)
	{
		XModuleMyControl* pMyControl = pMyPlayer->GetModuleMyControl();
		if ( pMyControl)
			pMyControl->SetPause( false);
	}

	if ( gg.controller) 
	{
		gg.controller->SetEnabled( true);
	}

	//접속종료전 경기중이던 투기장 정보 세팅
	if (gvar.MyInfo.BattleArenaInfo.IsInBattleArena())
	{
		GetBattleArena()->ReJoin();
	}

	//if (global.feedback)
	//	global.feedback->SetShowFeedback(true);
}

void XGamePlayState::ChangeField( int nNewFieldID, vec3 vMyPos, vec3 vMyDir )
{
	m_pWorldChanger->ChangeField(m_pObjectManager, m_pWorld, m_pSensorManager, nNewFieldID, vMyPos, vMyDir);
}

void XGamePlayState::PlayFieldBGMSound()
{
	XFieldInfo* pFieldInfo = m_pWorld->GetInfo();
	if (pFieldInfo && gvar.Game.pMyPlayer)
	{
		if ( global.sound)
			global.sound->Play( MLocale::ConvUTF16ToAnsi(pFieldInfo->strBGMSoundName[ 0].c_str()).c_str());
	}
}

void XGamePlayState::UpdateEnvSound()
{
	if ( m_pWorld != NULL)
	{
		XGetSoundField()->Update( m_pWorld->GetInfo());
	}
}

void XGamePlayState::GetDataForSoundListener( vec3& vPos, vec3& vDir )
{
	if ( global.camera  &&  global.camera->GetPrimaryCamera())
	{
		if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW) == true)
		{
			if (gvar.Game.pMyPlayer)
			{
//				vPos = global.camera->GetPrimaryCamera()->GetWorldPosition();				// 카메라 위치 기준
				vPos = gvar.Game.pMyPlayer->GetPosition();									// 내 캐릭터 위치 기준
				vDir = global.camera->GetPrimaryCamera()->GetWorldDirection();
			}
			else
			{
				vPos = global.camera->GetPrimaryCamera()->GetWorldPosition();
				vDir = global.camera->GetPrimaryCamera()->GetWorldDirection();
			}
		}
		else
		{
			vPos = global.camera->GetPrimaryCamera()->GetWorldPosition();
			vDir = global.camera->GetPrimaryCamera()->GetWorldDirection();
		}
	}

}

void XGamePlayState::FieldStartMessage()
{
	if (global.script)
	{
		XFieldInfo* pFieldInfo = m_pWorld->GetInfo();
		if ( pFieldInfo == NULL)	return;

		wstring strMessage = CSFormatString::Format( XGetStr( L"FIELD_ENTERED"), FSParam(pFieldInfo->GetName()));
		global.ui->OnPresentationUpper(strMessage);
	}
}

void XGamePlayState::InBPartNPCs( TD_UPDATE_CACHE_PBART* pBPartNpcNode, int BPartNpcCount, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	for (int i = 0; i < BPartNpcCount; i++)
	{
		TD_UPDATE_CACHE_PBART* pBPartInfo = &pBPartNpcNode[i];

		XNPCInfo* pNPCInfo = info.npc->Get(pBPartInfo->nNPCID);
		if (pNPCInfo == NULL) continue;

		InBPartNPC(pNPCInfo, pBPartInfo, bAppearEffect, bLoadingAsync);
	}
}

XBPartNonPlayer* XGamePlayState::InBPartNPC( XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_PBART* pBPartInfo, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	MUID uid = pBPartInfo->uid;

	if (m_pObjectManager->Find(uid) != NULL) 
	{
		XBPartNonPlayer* pThisNPC = m_pObjectManager->FindBPartNPC(uid);
		if (pThisNPC)
		{
			XModuleNonControl* pModuleNonControl = pThisNPC->GetModuleNonControl();
			if (pModuleNonControl)
			{
				TD_UPDATE_CACHE_NPC tempInfo;
				tempInfo.vPos = pBPartInfo->vPos;
				tempInfo.svDir = pBPartInfo->svDir;
				pModuleNonControl->OnReInNonPlayer(&tempInfo);
			}
		}
		return pThisNPC;
	}


	XBPartNonPlayer* pObject = AsBPartNPC(CreateObject(XOBJ_BPART_NPC, pBPartInfo->uid, pBPartInfo->nUIID));
	if (pObject)
	{
		wstring strResourceName = m_pOwnerStateMgr->m_ResourceMgr.GetObjectResourceName(pNPCInfo->strMeshName + L".elu");
		if(bLoadingAsync == false && CheckModelLoadingAsync(strResourceName) == true)
		{
			bLoadingAsync = true;
			bAppearEffect = true;
		}

		pObject->m_vServerPos = pBPartInfo->vPos;
		pObject->InBPartNPC(pNPCInfo, pBPartInfo, bAppearEffect, bLoadingAsync);
	}
	return pObject;
}

void XGamePlayState::Render()
{
	if (m_pScreenFader)	m_pScreenFader->Draw();
}


void XGamePlayState::InBPartNPC( TD_UPDATE_CACHE_PBART* pBPartInfo, MMatrix matWorld, vec3 vOrgPos )
{
	// 파츠에서 BPart NPC 생성
	XNPCInfo* pNPCInfo = info.npc->Get(pBPartInfo->nNPCID);
	if (pNPCInfo == NULL)
		return;

	XBPartNonPlayer* pObject = AsBPartNPC(CreateObject(XOBJ_BPART_NPC, pBPartInfo->uid, pBPartInfo->nUIID));
	if (pObject)
	{
		pObject->m_vServerPos = vOrgPos;
		pObject->InBPartNPC(pNPCInfo, pBPartInfo, false, true, &matWorld);
	}	
}
