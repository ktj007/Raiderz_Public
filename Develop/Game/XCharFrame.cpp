#include "stdafx.h"
#include "XCharFrame.h"
#include "XCampaign.h"
#include "XWidgetNameDef.h"
#include "XGameFrameUILoader.h"
#include "XGameInitializer.h"
#include "XBaseApplication.h"
#include "XItemManager.h"
#include "XEffectManager.h"
#include "XModuleMotion.h"
#include "XCameraManager.h"


vec3 DEFAULT_PLAYER_POS (0.0f, 5.0f, 0.0f);//( 2812.435f, 952.378f, -125.592f); //( 655.0f, -88.0f, -130.0f); //Needs updating
vec3 DEFAULT_PLAYER_DIR( 0.0f, -1.0f, 0.0f);
//vec3 DEFAULT_MOCKCHAR_POS ( 22735.951f, 85.614f, 5.686f); //( 20622.0f, -1853.0f, 0.0f);
//vec3 DEFAULT_MOCKCHAR_DIR( 20.0f, 20.0f, 20.0f);



// XCharFrame
XCharFrame::XCharFrame() 
: XBaseFrame(), m_bUpdateOnce(false), m_bLoaded(false), m_nButtonDown( 0), m_dwMotionTimer(0)
{
	DEFAULT_PLAYER_DIR.z = 0.0;
	DEFAULT_PLAYER_DIR.Normalize();

	m_bLoadCampaign = false;

	for ( int i = 0;  i < MAX_ACCOUNT_CHAR_COUNT;  i++)		m_pPlayers[ i] = NULL;
	for ( int i = 0;  i < SEX_MAX;  i++)					m_pMockPlayers[ i] = NULL;
	m_pCurrPlayer = NULL;
}


// ~XCharFrame
XCharFrame::~XCharFrame()
{
	// Delete players
	DeletePlayers();

	// Unload loading campaign
	XGetLoginCampaign()->Destroy();
}


// Open
bool XCharFrame::Open( int nPrevFrameID, const TCHAR* szArgs)
{
	dlog( "Enter frame : Character\n");

	m_bUpdateOnce = false;
	m_bLoaded = false;
	m_bLoadCampaign = false;

	global.app->GetInitializer()->Init_Pre_LoadingLoginCampaign();

	// Load UI
	XGameFrameUILoader ui_loader;
	ui_loader.Load( UI_CHAR_SHEET_NAME);
	global.ui->ShowSheet( UI_CHAR_SHEET_NAME);

	// Call event
	if ( global.script)		global.script->GetGlueGameEvent().OnCharacterEvent( "UI", "ENTER");

	if (nPrevFrameID == LOGIN_FRAME_ID) Load();
	else if ( global.script)		global.script->GetGlueGameEvent().OnCharacterEvent( "UI", "LOADING");

	if(gg.omgr == NULL)
	{
		gg.omgr = &m_obj;
	}

	return true;
}

void XCharFrame::Load()
{
	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();

	// Load loading campaign
	m_bLoadCampaign = XGetLoginCampaign()->Create();

	if ( global.script && !m_bLoadCampaign)		global.script->GetGlueGameEvent().OnCharacterEvent( "UI", "LOADED");

	global.camera->GetCamera()->SetPosition(0.0f, -300.0, 120.0f);
	global.camera->GetCamera()->SetDirection(vec3(0.0f, 1.0f, -0.1f));

	m_bLoaded = true;
}

// Close
void XCharFrame::Close( int nNextFrameID)
{
	dlog( "Leave frame : Character\n");

	// Update에서 Init_While_LoadingLoginCampaign 함수가 안불릴 수 있으므로 여기서 확인차 다시 재호출
	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();


	// Call event
	if ( global.script  &&  global.ui->IsExistSheet( UI_CHAR_SHEET_NAME) == true)
		global.script->GetGlueGameEvent().OnCharacterEvent( "UI", "LEAVE");


	// Check next frame ID
	if ( nNextFrameID != LOGIN_FRAME_ID)
	{
		// Delete sheet
		global.ui->DeleteSheet( UI_LOGIN_SHEET_NAME);
		global.ui->DeleteSheet( UI_CHAR_SHEET_NAME);

		// Delete players
		DeletePlayers();

		// Unload loading campaign
		XGetLoginCampaign()->Destroy();
	}

	if(gg.omgr == &m_obj)
		gg.omgr = NULL;

	// Effect
	global.emgr->Destroy();
}


// CreatePlayers
void XCharFrame::CreatePlayers()
{
}


// DeletePlayers
void XCharFrame::DeletePlayers()
{
	// Delete account players
	for ( int i = 0;  i < MAX_ACCOUNT_CHAR_COUNT;  i++)
	{
		if ( m_pPlayers[ i] != NULL)
		{
			m_obj.Erase(m_pPlayers[ i]);

			m_pPlayers[ i]->Destroy();
			SAFE_DELETE( m_pPlayers[ i]);
		}
	}

	// Delete mock players
	for ( int i = 0;  i < SEX_MAX;  i++)
	{
		if ( m_pMockPlayers[ i] != NULL)
		{
			m_pMockPlayers[ i]->Destroy();
			SAFE_DELETE( m_pMockPlayers[ i]);
		}
	}


	// Clear current player
	m_pCurrPlayer = NULL;
}


// OnEvent
bool XCharFrame::OnEvent( MEvent& e)
{
	if ( e.nMessage == MWM_LBUTTONUP  ||  e.nMessage == MWM_RBUTTONUP )
	{
		m_nButtonDown = 0;
	}


	// Process UI event
	if ( global.ui->OnEvent( e) == true)				return true;

	// Dispatch event
	return DispatchEvent( e);
}


// DispatchEvent
bool XCharFrame::DispatchEvent( MEvent& e)
{
	if ( m_pCurrPlayer == NULL  ||  ( m_pCurrPlayer != m_pMockPlayers[ SEX_MALE]  &&  m_pCurrPlayer != m_pMockPlayers[ SEX_FEMALE]))
		return false;


	if ( e.nMessage == MWM_LBUTTONDOWN  ||  e.nMessage == MWM_RBUTTONDOWN)
	{
		m_nButtonDown = e.nMessage;
		m_ptButtonDown = e.Pos;
	}
	else if ( e.nMessage == MWM_MOUSEMOVE)
	{
		float _diff_x = (float)( e.Pos.x - m_ptButtonDown.x);
		float _diff_y = (float)( e.Pos.y - m_ptButtonDown.y);
		m_ptButtonDown = e.Pos;

		if ( m_nButtonDown == MWM_LBUTTONDOWN)
		{
			// Rotate character
			float _rot = _diff_x * 0.01f;
			vec3 _dir_char = m_pCurrPlayer->GetDirection();
			vec3 _result_dir( _dir_char.x * cos( _rot) - _dir_char.y * sin( _rot),  _dir_char.y * cos( _rot) + _dir_char.x * sin( _rot),  0.0f);
			XMath::NormalizeZ0( _result_dir);
			m_pCurrPlayer->SetDirection( _result_dir);
		}
		else if ( m_nButtonDown == MWM_RBUTTONDOWN)
		{
			// Up/Down
			if ( global.camera  &&  global.camera->GetPrimaryCamera())
			{
				RCameraSceneNode* pCamera = global.camera->GetPrimaryCamera();
				vec3 _pos_char = m_pCurrPlayer->GetPosition();
				vec3 _pos_cam = pCamera->GetPosition();

				vec3 _pos_head = _pos_char;
				if ( m_pCurrPlayer->GetActor()->GetActorNodePos( L"Bip01 Head", _pos_head, E_TS_WORLD) == NULL)		_pos_head.z += 100.0;

				float _inout = _diff_y * 0.2f;
				_pos_cam.z = min( _pos_head.z, max( _pos_char.z + 30.0f, _pos_cam.z + _inout));
				pCamera->SetPosition( _pos_cam);
			}
		}
	}
	else if ( e.nMessage == MWM_MOUSEWHEEL)
	{
		// In/Out
		if ( global.camera  &&  global.camera->GetPrimaryCamera())
		{
			RCameraSceneNode* pCamera = global.camera->GetPrimaryCamera();
			vec3 _pos_char = m_pCurrPlayer->GetPosition();

			vec3 _pos_cam = pCamera->GetPosition();

			vec3 _pos_head = _pos_char;
			if ( m_pCurrPlayer->GetActor()->GetActorNodePos( L"Bip01 Head", _pos_head, E_TS_WORLD) == NULL)			_pos_head.z += 100.0;

			vec3 _pos_pelvis = _pos_char;
			if ( m_pCurrPlayer->GetActor()->GetActorNodePos( L"Bip01 Pelvis", _pos_pelvis, E_TS_WORLD) == NULL)		_pos_pelvis.z += 50.0;

			vec3 _diff = _pos_cam - _pos_char;
			_diff.z = 0.0f;
			float _distance = sqrt( _diff.x * _diff.x + _diff.y * _diff.y);
			vec3 _dir = _diff;
			_dir.Normalize();

			float _inout = -(float)e.nDelta * 0.2f;
			_distance = min( 300.0f, max( 50.0f, _distance + _inout));

			_pos_cam.x = _pos_char.x + _dir.x * _distance;
			_pos_cam.y = _pos_char.y + _dir.y * _distance;
			
			if ( _inout < 0.0)
				_pos_cam.z += -_inout * 0.25f;
			else
			{
				float _delta = _pos_pelvis.z - _pos_cam.z;
				float _up = ( _delta == 0.0)  ?  0.0f  :  _delta / abs( _delta);
				float _move = _up * _inout * 0.25f;

				if ( abs( _move) > abs( _delta))		_pos_cam.z = _pos_pelvis.z;
				else									_pos_cam.z += _move;
			}

			_pos_cam.z = max( _pos_char.z, min( _pos_head.z, _pos_cam.z));
			pCamera->SetPosition( _pos_cam);
		}
	}

	return false;
}


// Update
void XCharFrame::Update( float fDelta)
{
	// Show cursor
	if ( global.ui  &&  global.ui->IsMouseVisible() == false)		global.ui->ShowMouse( true);

	// 리소스 읽기 - 한번 로딩화면을 그려준 후에 실행하기 위하여 여기에서 Load
	if (UpdateOnce(fDelta) == true) return;

	// Check loading completed
	if ( m_bLoadCampaign == true  &&  XGetLoginCampaign()->IsLoadingCompleted() == true)
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnCharacterEvent( "UI", "LOADED");
		m_bLoadCampaign = false;
	}


	// Change occasional motion
	DWORD _curr = timeGetTime();
	if ( _curr > ( m_dwMotionTimer + 10000))
	{
		m_dwMotionTimer = _curr;

		if ( m_pCurrPlayer != NULL)
		{
			for ( int i = 0;  i < MAX_ACCOUNT_CHAR_COUNT;  i++)
			{
				if ( m_pCurrPlayer == m_pPlayers[ i])
				{
					m_pCurrPlayer->GetModuleMotion()->ChangeMotion( L"occasional_char", MT_DEFAULT, true);
					break;
				}
			}
		}
	}


	// Update base
	XBaseFrame::Update( fDelta);


	// Update login campaign
	XGetLoginCampaign()->Update( fDelta);


	// Update player
	if ( m_pCurrPlayer)		m_pCurrPlayer->Update( fDelta);

	// Update Effect
	global.emgr->OnUpdate(fDelta);
}


// OnRender
void XCharFrame::OnRender()
{
	if ( !m_bLoaded || XGetLoginCampaign()->IsLoadingCompleted() == false)
	{
		global.mint->Draw();
		return;
	}


	// Render base
	XBaseFrame::OnRender();


	// Render campaign
	XGetLoginCampaign()->Render();
}



// SelectChar
void XCharFrame::SelectChar( int nIndex)
{
	if ( nIndex >= (int)gvar.Char.m_CharInfos.size())							return;
	if ( m_pCurrPlayer != NULL  &&  m_pPlayers[ nIndex] == m_pCurrPlayer)		return;

	CharBuffEnchantAllLost(m_pCurrPlayer);

	XPlayerInfoFeature& _feature = gvar.Char.m_CharInfos[ nIndex].Feature;

	XCHARACTER_CREATEINFO ci;
	ci.nSex = _feature.nSex;
	ci.nStyle = TS_NONE;
	ci.nFace = _feature.nFace;
	ci.nHair = _feature.nHair;
	ci.nSkinColor = _feature.nSkinColor;
	ci.nHairColor = _feature.nHairColor;
	ci.nEyeColor = _feature.nEyeColor;
	ci.nMakeUp = _feature.nMakeUp;
	ci.nTattoo = _feature.nTattoo;
	ci.fTattooScale = _feature.fTattooScale;
	ci.vTattooPos = _feature.vTattooPos;
	ci.nEquipments = 0;
	ci.nEquipmentsCol = 0;
	ci.bBattleStance = false;

	TD_UPDATE_CACHE_PLAYER _info = GetDefaultCharData( ci);
	_info.SimpleInfo.vPos = DEFAULT_PLAYER_POS;
	_info.SimpleInfo.svDir = DEFAULT_PLAYER_DIR;

	global.camera->GetCamera()->SetPosition(0.0f, -300.0, 120.0f);
	global.camera->GetCamera()->SetDirection(vec3(0.0f, 1.0f, -0.1f));

	TD_PLAYER_FEATURE_TATTOO _tattoo;
	_tattoo.nTattooType = ci.nTattoo;
	_tattoo.nTattooScale = (uint8)( ci.fTattooScale * 50.0f);
	_tattoo.nTattooPosX = (short)( ci.vTattooPos.x * 100.0f);
	_tattoo.nTattooPosY = (short)( ci.vTattooPos.y * 100.0f);

	TD_PLAYER_BUFF_LIST _buffList;
	for (int j = 0; j < MAX_OWN_BUFF_NUMBER; j++)
		_buffList.nBuffID[j] = 0;
	_buffList.nUIID = 0;

	for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
	{
		_info.ExtraInfo.Feature.nItemID[i] = _feature.nEquipedItemID[i];
		_info.ExtraInfo.Feature.nItemID_DyedColor[i] = _feature.nEquipedItemColor[i];
		_info.ExtraInfo.Feature.nItemID_EnchantBuff[i] = _feature.nEquipedItemEnchantBuff[i];
	}

	if ( m_pPlayers[ nIndex] != NULL)
	{
		gg.omgr->Erase(m_pPlayers[ nIndex]);

		m_pPlayers[ nIndex]->Destroy();
		SAFE_DELETE( m_pPlayers[ nIndex]);
	}

	m_pPlayers[ nIndex] = new XNetPlayer( global.system->GenerateLocalUID(), 0);
	m_pPlayers[ nIndex]->Create();
	m_pPlayers[ nIndex]->ActiveModule( XMID_MOVABLE, false);
	m_pPlayers[ nIndex]->RemoveFromSceneManager();
	m_pPlayers[ nIndex]->InPlayer( &_info, &_tattoo, NULL, false);
	m_pPlayers[ nIndex]->ChangeStance( CS_BATTLE, true);
	m_pPlayers[ nIndex]->AddToSceneManager();
	m_pPlayers[ nIndex]->GetModuleMotion()->ChangeMotion( L"select_char", MT_DEFAULT, true);

	gg.omgr->Add(m_pPlayers[ nIndex]);


	if ( m_pCurrPlayer != NULL)			m_pCurrPlayer->RemoveFromSceneManager();
	m_pCurrPlayer = m_pPlayers[ nIndex];


	m_dwMotionTimer = timeGetTime();
}


// DeleteCurrChar
void XCharFrame::DeleteCurrChar()
{
	if ( m_pCurrPlayer == NULL)			return;

	m_pCurrPlayer->RemoveFromSceneManager();
	m_pCurrPlayer = NULL;
}


// GetDefaultPlayerData
TD_UPDATE_CACHE_PLAYER XCharFrame::GetDefaultCharData( const XCHARACTER_CREATEINFO& ci)
{
	// Set player info
	TD_UPDATE_CACHE_PLAYER ret;
	ret.SimpleInfo.nUIID = 0;
	ret.SimpleInfo.vPos = vec3( 0.0f, 0.0f, 0.0f);
	ret.SimpleInfo.svDir = vec3( 0.0f, 1.0f, 0.0f);
	ret.ExtraInfo.szName[ 0] = 0;
	ret.SimpleInfo.nStance = CS_NORMAL;
	ret.ExtraInfo.Feature.nSex = ci.nSex;
	ret.ExtraInfo.Feature.nHair = ci.nHair;
	ret.ExtraInfo.Feature.nFace = ci.nFace;
	ret.ExtraInfo.Feature.nHairColor = ci.nHairColor;
	ret.ExtraInfo.Feature.nSkinColor = ci.nSkinColor;
	ret.ExtraInfo.Feature.nEyeColor = ci.nEyeColor;
	ret.ExtraInfo.Feature.nMakeUp = ci.nMakeUp;
	ret.ExtraInfo.Feature.nWeaponSet = 0;

	for ( int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
	{
		ret.ExtraInfo.Feature.nItemID[ i] = 0;
		ret.ExtraInfo.Feature.nItemID_DyedColor[ i] = 0;
	}

	if ( ci.nEquipments >= 0)
	{
		for ( int i = 0;  i < XCONST::DEFAULT_EQUIPMENTS[ ci.nEquipments].count;  i++)
		{
			int nItemID = XCONST::DEFAULT_EQUIPMENTS[ ci.nEquipments].items[ i];
			if ( nItemID == 0)			continue;

			XItemData* pItemData = info.item->GetItemData( nItemID);
			if ( pItemData == NULL)		continue;

			SH_FEATURE_ITEMSLOT nItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot( pItemData->m_nItemSlot);
			ret.ExtraInfo.Feature.nItemID[ nItemSlot] = nItemID;

			if ( nItemSlot == FEATURE_ITEMSLOT_LWEAPON  ||  nItemSlot == FEATURE_ITEMSLOT_RWEAPON  ||
				nItemSlot == FEATURE_ITEMSLOT_LWEAPON2  ||  nItemSlot == FEATURE_ITEMSLOT_RWEAPON2 ||
				ci.nEquipments >= 3)
			{
				if ( pItemData->m_bUseTexColor == true)
					ret.ExtraInfo.Feature.nItemID_DyedColor[ nItemSlot] = pItemData->m_nTexColor;
			}
			else
				ret.ExtraInfo.Feature.nItemID_DyedColor[ nItemSlot] = ci.nEquipmentsCol;
		}
	}

	if ( ci.nStyle != TS_NONE)
	{
		int nConvStyle = -1;
		switch ( ci.nStyle)
		{
		case TS_DEFENDER :		nConvStyle = 0;		break;
		case TS_BERSERKER :		nConvStyle = 1;		break;
		case TS_CLERIC :		nConvStyle = 2;		break;
		case TS_SORCERER :		nConvStyle = 3;		break;
		case TS_ASSASSIN :		nConvStyle = 4;		break;
		}

		if ( nConvStyle >= 0)
		{
			for ( int i = 0;  i < XCONST::DEFAULT_STYLES[ nConvStyle].count;  i++)
			{
				int nItemID = XCONST::DEFAULT_STYLES[ nConvStyle].items[ i];
				if ( nItemID == 0)			continue;

				XItemData* pItemData = info.item->GetItemData( nItemID);
				if ( pItemData == NULL)		continue;

				SH_FEATURE_ITEMSLOT nItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot( pItemData->m_nItemSlot);
				if ( ret.ExtraInfo.Feature.nItemID[ nItemSlot] > 0)		continue;

				ret.ExtraInfo.Feature.nItemID[ nItemSlot] = nItemID;

				if ( nItemSlot == FEATURE_ITEMSLOT_LWEAPON  ||  nItemSlot == FEATURE_ITEMSLOT_RWEAPON  ||
					nItemSlot == FEATURE_ITEMSLOT_LWEAPON2  ||  nItemSlot == FEATURE_ITEMSLOT_RWEAPON2 ||
					ci.nEquipments >= 3)
				{
					if ( pItemData->m_bUseTexColor == true)
						ret.ExtraInfo.Feature.nItemID_DyedColor[ nItemSlot] = pItemData->m_nTexColor;
				}
				else
					ret.ExtraInfo.Feature.nItemID_DyedColor[ nItemSlot] = ci.nEquipmentsCol;
			}
		}
	}
	return ret;
}


// ChangeMockChar
void XCharFrame::ChangeMockChar( const XCHARACTER_CREATEINFO& ci)
{
	global.emgr->Destroy();


	bool bChangeMotion = false;
	if ( m_pCurrPlayer == NULL  ||
		( m_pCurrPlayer != m_pMockPlayers[ ci.nSex])  ||
		( m_pCurrPlayer->GetStance() == CS_NORMAL  &&  ci.bBattleStance == true)  ||
		( m_pCurrPlayer->GetStance() == CS_BATTLE  &&  ci.bBattleStance == false) )
		bChangeMotion = true;

	vec3 _dir = DEFAULT_PLAYER_DIR;
	if ( m_pCurrPlayer != NULL)
	{
		for ( int i = (int)SEX_MALE;  i < (int)SEX_MAX;  i++)
		{
			if ( m_pCurrPlayer == m_pMockPlayers[ i])
			{
				_dir = m_pCurrPlayer->GetDirection();
				break;
			}
		}

		m_pCurrPlayer->RemoveFromSceneManager();
	}

	TD_UPDATE_CACHE_PLAYER _info = GetDefaultCharData( ci);
	_info.SimpleInfo.vPos = DEFAULT_PLAYER_POS;
	_info.SimpleInfo.svDir = DEFAULT_PLAYER_DIR;

	global.camera->GetCamera()->SetPosition(0.0f, -300.0, 120.0f);
	global.camera->GetCamera()->SetDirection(vec3(0.0f, 1.0f, -0.1f));

	_info.SimpleInfo.nStance = ci.bBattleStance ? CS_BATTLE : CS_NORMAL;

	TD_PLAYER_FEATURE_TATTOO _tattoo;
	_tattoo.nTattooType = ci.nTattoo;
	_tattoo.nTattooScale = (uint8)( ci.fTattooScale * 50.0f);
	_tattoo.nTattooPosX = (short)( ci.vTattooPos.x * 100.0f);
	_tattoo.nTattooPosY = (short)( ci.vTattooPos.y * 100.0f);

	if ( m_pMockPlayers[ ci.nSex] == NULL)
	{
		m_pMockPlayers[ ci.nSex] = new XNetPlayer( 0, 0);
		m_pMockPlayers[ ci.nSex]->Create();
		m_pMockPlayers[ ci.nSex]->ActiveModule( XMID_MOVABLE, false);
		m_pMockPlayers[ ci.nSex]->RemoveFromSceneManager();
	}
	m_pMockPlayers[ ci.nSex]->InPlayer( &_info, &_tattoo, false);
	m_pMockPlayers[ ci.nSex]->AddToSceneManager();

	
	if ( bChangeMotion == true)
		m_pMockPlayers[ ci.nSex]->GetModuleMotion()->ChangeMotion( L"login_idle", MT_DEFAULT, true);

	m_pCurrPlayer = m_pMockPlayers[ ci.nSex];
}


void XCharFrame::ChangeMockCharStanceToBattle( bool bBattle)
{
	if ( m_pCurrPlayer == NULL)		return;

	for ( int i = (int)SEX_MALE;  i < (int)SEX_MAX;  i++)
	{
		if ( m_pCurrPlayer == m_pMockPlayers[ i])
		{
			m_pCurrPlayer->ChangeStance( bBattle ? CS_BATTLE : CS_NORMAL, false);
			break;
		}
	}
}


bool XCharFrame::IsMockCharSanceBattle()
{
	if ( m_pCurrPlayer == NULL)		return false;

	for ( int i = (int)SEX_MALE;  i < (int)SEX_MAX;  i++)
	{
		if ( m_pCurrPlayer == m_pMockPlayers[ i])
		{
			if ( m_pCurrPlayer->GetStance() == CS_BATTLE)
				return true;

			break;
		}
	}

	return false;
}


void XCharFrame::RotateMockChar( float fRotate)
{
	for ( int i = (int)SEX_MALE;  i <= (int)SEX_FEMALE;  i++)
	{
		if ( m_pMockPlayers[ i] == NULL)	continue;

		vec3 dir = m_pMockPlayers[ i]->GetDirection();
		vec3 vec( dir.x * cos( fRotate) - dir.y * sin( fRotate),  dir.y * cos( fRotate) + dir.x * sin( fRotate),  0.0f);
		XMath::NormalizeZ0( vec);

		m_pMockPlayers[ i]->SetDirection( vec);
	}
}


bool XCharFrame::UpdateOnce( float fDelta )
{
	if (!m_bUpdateOnce)
	{
		m_bUpdateOnce = true;

		// Update base
		XBaseFrame::Update( fDelta);
		global.camera->GetCamera()->SetPosition(0.0f, -300.0, 120.0f);
		global.camera->GetCamera()->SetDirection(vec3(0.0f, 1.0f, -0.1f));
		return true;
	}

	if (!m_bLoaded) 
	{
		Load();
	}

	return false;

}

void XCharFrame::CharBuffEnchantAllLost(XNetPlayer* pCurrPlayer)
{
	if(pCurrPlayer == NULL)
		return;

	pCurrPlayer->DelVisualEnchantBuffEffect();
}

