#include "stdafx.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XDef.h"
#include "XScript.h"
#include "XGlueGameEvent.h"
#include "MCsvParser.h"
#include "XInteractionInfo.h"
#include "XGame.h"
#include "XHelp.h"

// Maiet XML
#define HELP_XML_TAG_MAIET							"maiet"
#define HELP_XML_TAG_HELP_COMMON					"HELPCMN"
#define HELP_XML_TAG_HELP							"HELP"

#define HELP_XML_ATTR_CMN_MAXLEVEL					"HelpMaxLevel"
#define HELP_XML_ATTR_CMN_CYCLETIME					"PopupHelpCicle"

#define HELP_XML_ATTR_ID							"id"
#define HELP_XML_ATTR_TYPE							"Type"
#define HELP_XML_ATTR_IMAGE							"Img"
#define HELP_XML_ATTR_IMAGE2						"Img2"
#define HELP_XML_ATTR_STRING						"String"
#define HELP_XML_ATTR_EVENT_TRIGGER					"EventTrigger"
#define HELP_XML_ATTR_EVENT_TRIGGER_STRING			"EventTriggerString"
#define HELP_XML_ATTR_FINISH_EVENT					"FinishEvent"
#define HELP_XML_ATTR_FINISH_EVENT_STRING			"FinishEventString"
#define HELP_XML_ATTR_REQUIRED_QUEST				"RequiredQuest"
#define HELP_XML_ATTR_LINK_HELPID					"LinkHelpID"
#define HELP_XML_ATTR_MIN_LEVEL						"MinLevel"
#define HELP_XML_ATTR_MAX_LEVEL						"MaxLevel"
#define HELP_XML_ATTR_TIMER							"Timer"
#define HELP_XML_ATTR_FINISH_TIMER					"FinishTime"

#define AUTOHELP_CYCLE_TIME							180000.0f
#define HELP_HISTORYTYPE_HISTORY					L"HelpHistory"
#define HELP_HISTORYTYPE_RELOAD						L"HelpHistoryReload"

/**********************************************************
* Desc			: XHelp Class
***********************************************************/
XHelp::XHelp()
: m_nHelpID( 0 ), m_eHelpType( HELPTYPE_MAX ), m_nLinkHelpID( 0 ), m_nMinLevel( 0 ), m_nMaxLevel( 0 ), m_fTimer( 0.0f ),
m_bDestroy( false ), m_fFinishTime( 0.0f ), m_nRequiredQuest( 0 )
{
	
}

XHelp::~XHelp()
{

}

bool XHelp::InitHelp()
{
	m_bDestroy			= false;

	// 레벨 체크
	if( 0 != m_nMinLevel )
	{
		if( gvar.MyInfo.nLevel < m_nMinLevel )
			return false;
	}
	
	if( 0 != m_nMaxLevel )
	{
		if( gvar.MyInfo.nLevel > m_nMaxLevel )
			return false;
	}
	
	// 소유 퀘스트 체크
	if( 0 == m_nRequiredQuest )
		return true;

	XPlayerQuests* pQuest	= &gvar.MyInfo.Quests;
	if( NULL == pQuest )
		return false;

	if( NULL == pQuest->Get( m_nRequiredQuest ) )
		return false;

	return true;
}

void XHelp::FinishHelp()
{
	m_bDestroy			= true;
}

void XHelp::UpdateHelp( float fDeleta )
{
}

/**********************************************************
* Desc			: XTutorialFinishChecker Class
***********************************************************/
XTutorialFinishChecker::XTutorialFinishChecker()
: fChecker( NULL ), m_pHelp( NULL ), m_eSpecialActionCmd( SPECIAL_KEY_NONE ), m_eFinishCheckerState( FINISHCHECKERSTATE_MAX )
{

}

XTutorialFinishChecker::~XTutorialFinishChecker()
{

}

void XTutorialFinishChecker::FinishTutorial()
{
	m_pHelp					= NULL;
	fChecker				= NULL;
	m_eFinishCheckerState	= FINISHCHECKERSTATE_MAX;
}

void XTutorialFinishChecker::SetTutorialFinishChecker( XHelp* pHelp )
{
	if( NULL == pHelp )
	{
		FinishTutorial();
		return ;
	}

	if( m_pHelp )
		m_pHelp	= NULL;

	m_pHelp					= pHelp;
	m_eFinishCheckerState	= FINISHCHECKERSTATE_NONE;

	// 종료 처리 함수 연결
	if( 0 == lstrcmpi ( L"OnMyPlayerMove", m_pHelp->m_strFinishEvent.c_str() ) )			// 캐릭터 이동
		fChecker			= &XTutorialFinishChecker::OnMyPlayerMove;
	else if( 0 == lstrcmpi ( L"OnNpcInteraction", m_pHelp->m_strFinishEvent.c_str() ) )		// 특정 Npc 인터렉션
		fChecker			= &XTutorialFinishChecker::OnNpcInteraction;
	else if( 0 == lstrcmpi ( L"OnMouseVisible", m_pHelp->m_strFinishEvent.c_str() ) )		// 마우스 활성화
		fChecker			= &XTutorialFinishChecker::OnMouseVisible;
	else if( 0 == lstrcmpi ( L"OnShowMap", m_pHelp->m_strFinishEvent.c_str() ) )			// 지도 활성화
		fChecker			= &XTutorialFinishChecker::OnShowMap;
	else if( 0 == lstrcmpi ( L"OnBattleStance", m_pHelp->m_strFinishEvent.c_str() ) )		// 전투 스텐스 전환
		fChecker			= &XTutorialFinishChecker::OnBattleStance;
	else if( 0 == lstrcmpi ( L"OnNomalAttack", m_pHelp->m_strFinishEvent.c_str() ) )		// 일반 공격
		fChecker			= &XTutorialFinishChecker::OnNomalAttack;
	else if( 0 == lstrcmpi ( L"OnLooting", m_pHelp->m_strFinishEvent.c_str() ) )			// 몬스터 루팅
		fChecker			= &XTutorialFinishChecker::OnLooting;
	else if( 0 == lstrcmpi ( L"OnMyPlayerDodge", m_pHelp->m_strFinishEvent.c_str() ) )		// 회피
		fChecker			= &XTutorialFinishChecker::OnMyPlayerDodge;
	else if( 0 == lstrcmpi ( L"OnMyPlayerGuard", m_pHelp->m_strFinishEvent.c_str() ) )		// 가드
		fChecker			= &XTutorialFinishChecker::OnMyPlayerGuard;
	else if( 0 == lstrcmpi ( L"OnShowTalent", m_pHelp->m_strFinishEvent.c_str() ) )			// 탤런트 창 활성화
		fChecker			= &XTutorialFinishChecker::OnShowTalent;
	else if( 0 == lstrcmpi( L"OnUseTalent", m_pHelp->m_strFinishEvent.c_str() ) )			// 탤런트 사용
		fChecker			= &XTutorialFinishChecker::OnUseTalent;
	else 
		FinishTutorial();
}

eFinishCheckerState XTutorialFinishChecker::OnMyPlayerMove()
{
	if ( gvar.Game.pMyPlayer == NULL)
		return FINISHCHECKERSTATE_MAX;

	XModuleMyControl* pMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pMyControl == NULL)
		return FINISHCHECKERSTATE_MAX;
	if ( pMyControl->IsMoving() == false)
		return FINISHCHECKERSTATE_MAX;
	if ( pMyControl->IsAutoRun() == true)
		return FINISHCHECKERSTATE_MAX;

	m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;
	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnNpcInteraction()
{
	XNonPlayer* pNonPlayer = gg.omgr->FindNPC( XGetInteractionInfo().InteractionTargetUID);
	if( NULL == pNonPlayer )
		return FINISHCHECKERSTATE_MAX;

	if( 0 == lstrcmpi ( pNonPlayer->GetName(), m_pHelp->m_strFinishEventString.c_str() ) )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnMouseVisible()
{
	if( true == global.ui->IsMouseVisible() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnShowMap()
{
	string strUIName		= "MAP";
	if( true == IsShowUI( strUIName ) )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnBattleStance()
{
	if( NULL == gvar.Game.pMyPlayer )
		return FINISHCHECKERSTATE_MAX;
	
	if( CS_BATTLE == gvar.Game.pMyPlayer->GetStance() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnNomalAttack()
{
	if( NULL == gvar.Game.pMyPlayer )
		return FINISHCHECKERSTATE_MAX;

	XModuleMyControl* pMyControl	= gvar.Game.pMyPlayer->GetModuleMyControl();
	if( NULL == pMyControl )
		return FINISHCHECKERSTATE_MAX;

	if( ACTION_STATE_NORMAL_ATTACK == pMyControl->GetCurrentActionStateID() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnLooting()
{
	if( NULL == gvar.Game.pMyPlayer )
		return FINISHCHECKERSTATE_MAX;

	XModuleMyControl* pMyControl	= gvar.Game.pMyPlayer->GetModuleMyControl();
	if( NULL == pMyControl )
		return FINISHCHECKERSTATE_MAX;

	if( ACTION_STATE_LOOTINGITEM == pMyControl->GetCurrentActionStateID() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnMyPlayerDodge()
{
	if( ( SPECIAL_KEY_DODGE_FRONT <= m_eSpecialActionCmd ) && ( SPECIAL_KEY_DODGE_RIGHT >= m_eSpecialActionCmd ) )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnMyPlayerGuard()
{
	if( NULL == gvar.Game.pMyPlayer )
		return FINISHCHECKERSTATE_MAX;

	XModuleMyControl* pMyControl	= gvar.Game.pMyPlayer->GetModuleMyControl();
	if( NULL == pMyControl )
		return FINISHCHECKERSTATE_MAX;

	if( true == pMyControl->IsCurrentGuard() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnShowTalent()
{
	string strUIName		= "TALENT";
	if( true == IsShowUI( strUIName ) )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::OnUseTalent()
{
	if( NULL == gvar.Game.pMyPlayer )
		return FINISHCHECKERSTATE_MAX;

	XModuleMyControl* pMyControl	= gvar.Game.pMyPlayer->GetModuleMyControl();
	if( NULL == pMyControl )
		return FINISHCHECKERSTATE_MAX;

	if( ACTION_STATE_USE_TALENT == pMyControl->GetCurrentActionStateID() )
		m_eFinishCheckerState	= FINISHCHECKERSTATE_CHECKED;

	return m_eFinishCheckerState;
}

eFinishCheckerState XTutorialFinishChecker::UpdateTutorialFinishChecker()
{
	if( ( fChecker ) && IsUpdate() )
		return ( this->*fChecker )();

	return FINISHCHECKERSTATE_MAX;
}

bool XTutorialFinishChecker::IsShowUI( string& strUIName )
{
	return global.script->GetGlueGameEvent().IsShowUI( strUIName );
}

bool XTutorialFinishChecker::IsUpdate()
{
	return global.script->GetGlueGameEvent().IsUpdateTutorial();
}

/**********************************************************
* Desc			: XTutorialFinishChecker Class
***********************************************************/
XTutorialStartChecker::XTutorialStartChecker()
:m_bStart( false ), m_pHelp( NULL )
{

}

XTutorialStartChecker::~XTutorialStartChecker()
{

}

bool XTutorialStartChecker::InitStartChecker( XHelp* pHelp )
{
	if( NULL == pHelp )
		return false;

	m_pHelp		= pHelp;
	return true;
}

void XTutorialStartChecker::FinishStartChecker()
{
	m_pHelp		= NULL;
	m_bStart	= false;
}

int XTutorialStartChecker::GetHelpID()
{
	if( NULL == m_pHelp )
		return 0;

	return m_pHelp->m_nHelpID;
}

/**********************************************************
* Desc			: XKillEnemyStartChecker Class
***********************************************************/
XKillEnemyStartChecker::XKillEnemyStartChecker()
: m_nkillEnemy( 0 ), m_nKilledEpicEnemy( 0 ), m_nKillCondition( 0 )
{

}

XKillEnemyStartChecker::~XKillEnemyStartChecker()
{

}

bool XKillEnemyStartChecker::InitStartChecker( XHelp* pHelp )
{
	if( false == XTutorialStartChecker::InitStartChecker( pHelp ) )
		return false;

	m_nkillEnemy			= gvar.MyInfo.nKilledEnemy;
	m_nKilledEpicEnemy		= gvar.MyInfo.nKilledEpicEnemy;

	m_nKillCondition		= _wtoi64( pHelp->m_strEventTriggerString.c_str() );

	return true;
}

void XKillEnemyStartChecker::FinishStartChecker()
{
	XTutorialStartChecker::FinishStartChecker();
}

bool XKillEnemyStartChecker::UpdateStartChecker()
{
	if( true == m_bStart )
		return m_bStart;

	if( ( m_nkillEnemy + m_nKillCondition ) <= gvar.MyInfo.nKilledEnemy )
		m_bStart	= true;
	else if( ( m_nKilledEpicEnemy + m_nKillCondition ) <= gvar.MyInfo.nKilledEpicEnemy )
		m_bStart	= true;

	return XTutorialStartChecker::UpdateStartChecker();
}

/**********************************************************
* Desc			: XStateHitStartChecker Class
***********************************************************/
XStateHitStartChecker::XStateHitStartChecker()
: m_nHitCurr( 0 ), m_nHitCondition( 0 ), m_eActionState( ACTION_STATE_NONE )
{
}

XStateHitStartChecker::~XStateHitStartChecker()
{

}

bool XStateHitStartChecker::InitStartChecker( XHelp* pHelp )
{
	if( false == XTutorialStartChecker::InitStartChecker( pHelp ) )
		return false;

	m_nHitCondition				= _wtoi64( pHelp->m_strEventTriggerString.c_str() );

	return true;
}

void XStateHitStartChecker::FinishStartChecker()
{
	XTutorialStartChecker::FinishStartChecker();
}

bool XStateHitStartChecker::UpdateStartChecker()
{
	if( true == m_bStart )
		return m_bStart;

	if( m_nHitCurr >= m_nHitCondition )
		m_bStart = true;

	return XTutorialStartChecker::UpdateStartChecker();
}

void XStateHitStartChecker::MyControlEvent( const XEvent& msg )
{
	if( XEVTD_TALENT_HIT == msg.m_nID )
		++m_nHitCurr;

	XTutorialStartChecker::MyControlEvent( msg );
}

/**********************************************************
* Desc			: XHelpHistory Class
***********************************************************/
void XHelpHistory::InsertHistory( int nHelpID )
{
	m_Data.insert(nHelpID);

	XAccountData* pAccountData = gvar.Char.m_AccountInfo.GetAccountInfo();
	if (!pAccountData) return;

	XHelpHistoryExporter exporter;
	exporter.Export( HELP_HISTORYTYPE_HISTORY, pAccountData, m_Data );
}

void XHelpHistory::EraseHistory( int nHelpID )
{
	m_Data.erase(nHelpID);

	XAccountData* pAccountData = gvar.Char.m_AccountInfo.GetAccountInfo();
	if (!pAccountData) return;

	XHelpHistoryExporter exporter;
	exporter.Export( HELP_HISTORYTYPE_HISTORY, pAccountData, m_Data );
}

void XHelpHistory::InsertReloadHistory( int nHelpID )
{
	m_ReLoadData.insert(nHelpID);

	XAccountData* pAccountData = gvar.Char.m_AccountInfo.GetAccountInfo();
	if (!pAccountData) return;

	XHelpHistoryExporter exporter;
	exporter.Export( HELP_HISTORYTYPE_RELOAD, pAccountData, m_ReLoadData );
}

void XHelpHistory::EraseReloadHistory( int nHelpID )
{
	m_ReLoadData.erase(nHelpID);

	XAccountData* pAccountData = gvar.Char.m_AccountInfo.GetAccountInfo();
	if (!pAccountData) return;

	XHelpHistoryExporter exporter;
	exporter.Export( HELP_HISTORYTYPE_RELOAD, pAccountData, m_ReLoadData );
}

bool XHelpHistory::IsExist( int nHelpID )
{
	set<int>::iterator itor = m_Data.find(nHelpID);
	if (itor != m_Data.end())
	{
		return true;
	}
	return false;
}

void XHelpHistory::Load()
{
	m_bLoaded = true;

	XAccountData* pAccountData = gvar.Char.m_AccountInfo.GetAccountInfo();
	if (!pAccountData) return;

	XHelpHistoryImporter importer;
	importer.Import( HELP_HISTORYTYPE_HISTORY, pAccountData, m_Data			);
	importer.Import( HELP_HISTORYTYPE_RELOAD, pAccountData, m_ReLoadData	);

	// 봤던 도움말중에 다시 나와야하는것들을 찾아서 제거해주고 ReloadData는 지워주자
	set<int>::iterator itor = m_ReLoadData.begin();
	int nHelpID				= 0;
	for( ; itor != m_ReLoadData.end() ; ++itor )
	{
		nHelpID		= (*itor);
		if( true == IsExist( nHelpID ) )
			EraseHistory( nHelpID );
	}

	m_ReLoadData.clear();
	XHelpHistoryExporter exporter;
	exporter.Export( HELP_HISTORYTYPE_RELOAD, pAccountData, m_ReLoadData );
}

void XHelpHistory::Clear()
{
	m_Data.clear();
}

void XHelpHistory::Finish()
{
	Clear();
	m_bLoaded		= false;
}

/**********************************************************
* Desc			: XPopupHelpAutoQueue Class
***********************************************************/
XPopupHelpAutoQueue::XPopupHelpAutoQueue() : m_fQueueElapsedTime(0.0f), m_fAutoCycle( 0.0f ),
m_bAutoPopupHelp( false )
{

}

XPopupHelpAutoQueue::~XPopupHelpAutoQueue()
{

}

void XPopupHelpAutoQueue::InitAutoQueue( float fCycleTime )
{
	m_bAutoPopupHelp	= true;
	
	m_fAutoCycle		= ( fCycleTime * 0.001f );	//1/1000 초
}

void XPopupHelpAutoQueue::ClearAll()
{
	m_Queue.clear();
	m_fQueueElapsedTime		= 0.0f;
}

void XPopupHelpAutoQueue::Push_Back( XHelp* pHelp )
{
	if( false == m_bAutoPopupHelp )
		return;

	m_Queue.push_back( pHelp );
}

void XPopupHelpAutoQueue::Push_Front( XHelp* pHelp )
{
	if( false == m_bAutoPopupHelp )
		return;

	m_Queue.push_front( pHelp );
}

void XPopupHelpAutoQueue::ResetTime()
{
	m_fQueueElapsedTime = 0.0f;
}

int XPopupHelpAutoQueue::Update( float fDelta )
{
	if( true == gvar.Game.IsCombatingNow() )
		return 0;

	if( false == m_bAutoPopupHelp )
		return 0;

	if (m_Queue.empty()) return 0;

	m_fQueueElapsedTime += fDelta;

	if( m_fQueueElapsedTime > m_fAutoCycle )
	{
		list<XHelp*>::iterator itorBegin = m_Queue.begin();
		XHelp* pHelp = *itorBegin;

		m_fQueueElapsedTime		= 0.0f;
		m_Queue.pop_front();

		return pHelp->m_nHelpID;
	}

	return 0;
}

/**********************************************************
* Desc			: XHelpHistoryExporter Class
***********************************************************/
void XHelpHistoryExporter::Export( wstring strHistroyType, XAccountData* pAccountData, set<int>& setData )
{
	wstring strValue;

	for (set<int>::iterator itor = setData.begin(); itor != setData.end(); ++itor)
	{
		int nHelpID = (*itor);

		wchar_t szNumber[64];
		_itow_s(nHelpID, szNumber, 10);

		strValue += wstring(szNumber);
		strValue += L",";
	}

	pAccountData->SetParameter( strHistroyType, wstring(L"data"), strValue );
}

/**********************************************************
* Desc			: XHelpHistoryImporter Class
***********************************************************/
void XHelpHistoryImporter::Import( wstring strHistroyType, XAccountData* pAccountData, set<int>& setData )
{
	const wchar_t* szValue = pAccountData->GetParameter( strHistroyType, wstring(L"data"));
	if (szValue == NULL) return;

	MCsvParser csvParser;

	csvParser.LoadFromStream(MLocale::ConvUTF16ToAnsi(szValue).c_str());
	int nColCount = csvParser.GetColCount(0);

	char szBuf[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csvParser.GetData(nCol, 0, szBuf, 1024))
		{
			int nHelpID = atoi(szBuf);
			if (nHelpID != 0)
			{
				setData.insert(nHelpID);
			}
		}
	}

}

/**********************************************************
* Desc			: XHelpMgr Class
***********************************************************/
XHelpMgr::XHelpMgr()
:m_bEnabled( false ), m_pCurrHelp( NULL ), m_eSpecialActionCmd( SPECIAL_KEY_NONE )
,m_pFinishDelayHelp( NULL ), m_fFinishDelayTime( 0.0f ), m_pCurrPopHelp( NULL )
{
	m_mapHelpData.clear();
	m_mapEventTriggerHelpData.clear();
	m_listStartChecker.clear();
	m_listReserveHelp.clear();
}

XHelpMgr::~XHelpMgr()
{

}

void XHelpMgr::FinishHelpMgr()
{
	if( false == m_mapHelpData.empty() )
	{
		XHelp* pHelp	= NULL;
		itHelpData it	= m_mapHelpData.begin();
		for( ; it != m_mapHelpData.end() ; ++it )
		{
			pHelp		= ( XHelp* )( *it ).second;
			DeleteHelpData( pHelp );
		}
#ifdef	HELP_DEV
		dlog( "[ HELP_DEV / tripleJ ] == XHelpMgr::FinishHelpMgr() mapHelpData Clear ==\n" );
#endif	
		m_mapHelpData.clear();
	}

	if( false == m_mapEventTriggerHelpData.empty() )
		m_mapEventTriggerHelpData.clear();
#ifdef	HELP_DEV
	dlog( "[ HELP_DEV / tripleJ ] == XHelpMgr::FinishHelpMgr() ==\n" );
#endif
	
	DestroyStartChecker();

	m_listReserveHelp.clear();
	m_PopupHelpAuto.ClearAll();

	m_pCurrHelp			= NULL;
	m_pFinishDelayHelp	= NULL;
	m_pCurrPopHelp		= NULL;

	m_TutorialFinishChecker.FinishTutorial();
}

void XHelpMgr::DestroyStartChecker()
{
	if( false == m_listStartChecker.empty() )
	{
		XTutorialStartChecker* pChecker	= NULL;
		itStartChecker it				= m_listStartChecker.begin();
		for( ; it != m_listStartChecker.end() ; ++it )
		{
			pChecker					= *it;
			DeleteStartChecker( pChecker );
		}

		m_listStartChecker.clear();
	}
}

bool XHelpMgr::LoadHelpMgr()
{
	// Help Data Load
	if( false == LoadHelpData() )
		return false;

	// 최초 생성은 해두고, 사용은 금지. 사용가능 시점에서 활성화해주자
	m_bEnabled			= false;

	return true;
}

bool XHelpMgr::InitHelpMgr()
{
#ifdef	HELP_DEV
	dlog( "[ HELP_DEV / tripleJ ] == XHelpMgr::InitHelpMgr() ==\n" );
#endif

	DestroyHelpMgr();
	
	m_History.Load();

	InitEventTrigger();

	InitPopupHelpAuto();

	m_bEnabled			= true;
	
	return true;
}

void XHelpMgr::DestroyHelpMgr()
{
	DestroyStartChecker();

	OnHelpTriggerClose();
	m_pCurrHelp			= NULL;
	m_pFinishDelayHelp	= NULL;
	m_listReserveHelp.clear();

	m_History.Finish();

}

bool XHelpMgr::LoadHelpData()
{
	MXml xml;
	if ( false == xml.LoadFile( MLocale::ConvUTF16ToAnsi( FILENAME_HELP ).c_str() ) )
		return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(HELP_XML_TAG_MAIET).FirstChildElement().Element();
	for( ; NULL != pElement ; pElement = pElement->NextSiblingElement() )
	{
		if ( 0 == _stricmp( pElement->Value(), HELP_XML_TAG_HELP ) )
		{
			ParseHelpData( pElement, &xml );
		}
	}

	return true;
}

void XHelpMgr::ParseHelpData( MXmlElement* pElement, MXml* pXml )
{
	wstring		strValue;
	XHelp*		pNewHelp = new XHelp;
	if( NULL == pNewHelp )
		return ;

	// ID Check
	_Attribute( &pNewHelp->m_nHelpID,			pElement, HELP_XML_ATTR_ID );
	if( false == CheckHelpID( pNewHelp ) )
	{
		DeleteHelpData( pNewHelp );
		return ;
	}

	// Type 설정
	_Attribute( strValue, pElement, HELP_XML_ATTR_TYPE );
	if( false == CheckHelpType( strValue, pNewHelp ) )
	{
		DeleteHelpData( pNewHelp );
		return ;
	}

	_Attribute( pNewHelp->m_strImg,					pElement, HELP_XML_ATTR_IMAGE,					pXml );
	_Attribute( pNewHelp->m_strImg2,				pElement, HELP_XML_ATTR_IMAGE2,					pXml );
	_Attribute( pNewHelp->m_strString,				pElement, HELP_XML_ATTR_STRING,					pXml );
	_Attribute( pNewHelp->m_strEventTrigger,		pElement, HELP_XML_ATTR_EVENT_TRIGGER,			pXml );
	_Attribute( pNewHelp->m_strEventTriggerString,	pElement, HELP_XML_ATTR_EVENT_TRIGGER_STRING,	pXml );
	_Attribute( pNewHelp->m_strFinishEvent,			pElement, HELP_XML_ATTR_FINISH_EVENT,			pXml );
	_Attribute( pNewHelp->m_strFinishEventString,	pElement, HELP_XML_ATTR_FINISH_EVENT_STRING,	pXml );
	_Attribute( &pNewHelp->m_nLinkHelpID,			pElement, HELP_XML_ATTR_LINK_HELPID		);
	_Attribute( &pNewHelp->m_nRequiredQuest,		pElement, HELP_XML_ATTR_REQUIRED_QUEST	);
	_Attribute( &pNewHelp->m_nMinLevel,				pElement, HELP_XML_ATTR_MIN_LEVEL		);
	_Attribute( &pNewHelp->m_nMaxLevel,				pElement, HELP_XML_ATTR_MAX_LEVEL		);
	_Attribute( &pNewHelp->m_fTimer,				pElement, HELP_XML_ATTR_TIMER			);
	_Attribute( &pNewHelp->m_fFinishTime,			pElement, HELP_XML_ATTR_FINISH_TIMER	);

	if( false == InsertHelpData( pNewHelp ) )
	{
		DeleteHelpData( pNewHelp );
		return ;
	}
}

void XHelpMgr::UpdateReserveHelp()
{
	if( m_pCurrHelp )		// 현재 무엇인가 떠있다 패스
		return ;

	if( true == m_listReserveHelp.empty() )
		return ;

	XHelp* pHelp		= m_listReserveHelp.front();
	m_listReserveHelp.pop_front();

	if( true == OnGlueEventHelpTrigger( pHelp ) )
	{
		m_History.InsertHistory( pHelp->m_nHelpID );
		// 조건 종료 일경우 일단 Reload에 등록해두고 종료시 조건에 맞으면 빼주자
		if( FINISHCHECKERSTATE_NONE == m_TutorialFinishChecker.GetCheckState() )
			m_History.InsertReloadHistory( pHelp->m_nHelpID );
	}
}

void XHelpMgr::UpdateFinishHelp()
{
	if( m_pFinishDelayHelp )
	{
		if( ( XGetNowTime() - m_fFinishDelayTime ) > m_pFinishDelayHelp->m_fFinishTime )
		{
			OnHelpTriggerClose( m_pFinishDelayHelp );
			m_pFinishDelayHelp	= NULL;
		}
		return ;
	}
	// 종료 검사
	if( m_pCurrHelp && ( FINISHCHECKERSTATE_CHECKED == m_TutorialFinishChecker.UpdateTutorialFinishChecker() ) )
	{
		m_History.EraseReloadHistory( m_pCurrHelp->m_nHelpID );
		SetFinishHelp();
	}
}

void XHelpMgr::UpdatePopupHelpAuto( float fDeleta )
{
	if( true == gvar.Game.IsCombatingNow() )
	{
		// 전투 중이라면 떠있는 Popup중 자동으로 시작된 놈이라면 삭제를 시켜주자
		if( ( m_pCurrPopHelp ) && ( HELPTYPE_POPUPHELP_AUTO == m_pCurrPopHelp->m_eHelpType ) )
		{
			// 현재 떠있는놈이 Auto인데 전투 때문에 사라지는 것이라면 다시 AutoQueue 앞에 넣어주자
			m_PopupHelpAuto.Push_Front( m_pCurrPopHelp );
			// 중복등록을 피하기 위해서 Curr를 강제로 없애주자
			m_pCurrPopHelp->FinishHelp();
			m_History.EraseHistory( m_pCurrPopHelp->m_nHelpID );
			m_pCurrPopHelp		= NULL;
			OnHelpTriggerClose( "POPUPHELP" );
		}
		return ;
	}
	int nHelpID		= m_PopupHelpAuto.Update( fDeleta );
	if( 0 != nHelpID )
		OnHelpTrigger( nHelpID );
}

void XHelpMgr::UpdateStartChecker()
{
	if( true == m_listStartChecker.empty() )
		return ;

	XTutorialStartChecker* pChecker	= NULL;
	itStartChecker it				= m_listStartChecker.begin();
	for( ; it != m_listStartChecker.end() ; )
	{
		pChecker					= *it;
		if( pChecker->UpdateStartChecker() )
		{
			OnHelpTrigger( pChecker->GetHelpID() );
			it = m_listStartChecker.erase( it );
			DeleteStartChecker( pChecker );
		}
		else
			++it;
	}
}

bool XHelpMgr::CheckHelpID( XHelp* pHelp )
{
	// ID check
	if( 0 == pHelp->m_nHelpID )			// ID Error
		return false;

	// 중복 Check
	if( true == m_mapHelpData.empty() )
		return true;

	itHelpData it		= m_mapHelpData.find( pHelp->m_nHelpID );
	if( it != m_mapHelpData.end() )
		return false;

	return true;
}

bool XHelpMgr::CheckHelpType( wstring strVal, XHelp* pHelp )
{
	if( L"popup" == strVal )
		pHelp->m_eHelpType		= HELPTYPE_POPUPHELP;
	else if( L"tutorial" == strVal )
		pHelp->m_eHelpType		= HELPTYPE_TUTORIAL;
	else if( L"s_tutorial" == strVal )
		pHelp->m_eHelpType		= HELPTYPE_SPECIAL_TUTORIAL;
	else if( L"popup_auto" == strVal )
		pHelp->m_eHelpType		= HELPTYPE_POPUPHELP_AUTO;
	else
		return false;

	return true;
}

bool XHelpMgr::InsertHelpData( XHelp* pHelp )
{
	if( NULL == pHelp )
		return false;

	// m_mapHelpData에 중복 검사( ID )는 이미 시작전에 검사했으므로 안전하다 판단하고 넣는다
	m_mapHelpData.insert( mapHelpData::value_type( pHelp->m_nHelpID, pHelp ) );

	return true;
}

void XHelpMgr::InitEventTrigger()
{
	if( true == m_mapHelpData.empty() )
		return ;

	XHelp* pHelp		= NULL;
	itHelpData it		= m_mapHelpData.begin();
	for( ; it != m_mapHelpData.end() ; ++it )
	{
		pHelp				= ( XHelp* )( *it ).second;

		// Checker 확인
		if( true == SetEventChecker( pHelp ) )
			continue;

		// Checker 없음 m_mapEventTriggerHelpData에 등록
		if( ( false == m_mapEventTriggerHelpData.empty() ) && ( false == pHelp->m_strEventTrigger.empty() ) )
		{
			itStringTriggerHelpData it		= m_mapEventTriggerHelpData.find( pHelp->m_strEventTrigger );
			if( it != m_mapEventTriggerHelpData.end() )
				continue;
		}

		m_mapEventTriggerHelpData.insert( mapStringTriggerHelpData::value_type( pHelp->m_strEventTrigger, pHelp ) );
	}
}

void XHelpMgr::InitPopupHelpAuto()
{
	m_PopupHelpAuto.ClearAll();

	m_PopupHelpAuto.InitAutoQueue( AUTOHELP_CYCLE_TIME );

	if( true == m_mapHelpData.empty() )
		return ;

	XHelp* pHelp		= NULL;
	itHelpData it		= m_mapHelpData.begin();
	for( ; it != m_mapHelpData.end() ; ++it )
	{
		pHelp				= ( XHelp* )( *it ).second;

		if( HELPTYPE_POPUPHELP_AUTO == pHelp->m_eHelpType )
			m_PopupHelpAuto.Push_Back( pHelp );
	}
}

bool XHelpMgr::SetEventChecker( XHelp* pHelp )
{
	// History에 있는건 굳이 다시 할 필요가 없..
	if ( true == m_History.IsExist( pHelp->m_nHelpID ) )
		return false;

	XTutorialStartChecker* pChecker		= NULL;

	if( 0 == lstrcmpi ( L"OnKillEnemy", pHelp->m_strEventTrigger.c_str() ) )			// 몬스터를 죽였을 때 
		pChecker		= new XKillEnemyStartChecker;
	else if( 0 == lstrcmpi ( L"OnStateHit", pHelp->m_strEventTrigger.c_str() ) )		// 맞았을 때
		pChecker		= new XStateHitStartChecker;

	if( NULL == pChecker )
		return false;

	pChecker->InitStartChecker( pHelp );
	m_listStartChecker.push_back( pChecker );

	return true;
}

void XHelpMgr::DeleteHelpData( XHelp* pHelp )
{
	if( pHelp )
	{
		pHelp->FinishHelp();
		delete pHelp;
	}
}

void XHelpMgr::DeleteStartChecker( XTutorialStartChecker* pChecker )
{
	if( pChecker )
	{
		pChecker->FinishStartChecker();
		delete pChecker;
	}
}

void XHelpMgr::UpdateHelpMgr( float fDeleta )
{
	if( false == IsEnabled() )
		return ;

	// 예약 검사
	UpdateReserveHelp();

	// 종료 검사
	UpdateFinishHelp();

	// 시작 검사
	UpdateStartChecker();

	// 자동 팝업도움말 
	UpdatePopupHelpAuto( fDeleta );

}

bool XHelpMgr::IsEnabled()
{
	if( false == m_bEnabled )								// 외부에서 강제 사용 금지시
		return false;

	if( HELP_ENABLED_MAX_LEVEL < gvar.MyInfo.nLevel )		// 레벨 제한
		return false;

	if( false == XGETCFG_GAME_SHOWHELPMESSAGE )				// 옵션 선택인듯
		return false;

	return true;
}

wstring XHelpMgr::GetHelpImg( int nHelpID, int nImgIndex )
{
	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return L"";

	if( 0 == nImgIndex )
		return pHelp->m_strImg;
	else if( 1 == nImgIndex )
		return pHelp->m_strImg2;

	return L"";
}

wstring XHelpMgr::GetHelpMsg( int nHelpID )
{
	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return L"";

	return pHelp->m_strString;
}

wstring XHelpMgr::GetEventTrigger( int nHelpID )
{
	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return L"";

	return pHelp->m_strEventTrigger;
}

float XHelpMgr::GetHelpTimer( int nHelpID )
{
	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return NULL;

	return pHelp->m_fTimer;
}

bool XHelpMgr::OnEvent( MEvent& e )
{
	switch( e.nMessage )
	{
	case MWM_KEYDOWN:
		{
			switch ( e.nKey )
			{
			case VK_ESCAPE:
				{
					if( m_pCurrHelp )
					{
						OnHelpTriggerClose( m_pCurrHelp );
						return true;
					}
				}
				break;
			}
			break;
		}
	}

	return false;
}

void XHelpMgr::SpecialActionCmd( const SPECIAL_ACTION_COMMAND& eCmd )
{
	m_eSpecialActionCmd = eCmd; 

	// EventTrigger에 신호 보내기

	// FinishTrigger에 신호 보내기
	m_TutorialFinishChecker.SpecialActionCmd( eCmd );
}

void XHelpMgr::MyControlEvent( const XEvent& msg )
{
	XTutorialStartChecker* pChecker	= NULL;
		
	// EventTrigger에 신호 보내기
	if( false == m_listStartChecker.empty() )
	{
		itStartChecker it		= m_listStartChecker.begin();
		for( ; it != m_listStartChecker.end() ; ++it )
		{
			pChecker					= *it;
			pChecker->MyControlEvent( msg );
		}
	}
	
	// FinishTrigger에 신호 보내기
}

bool XHelpMgr::OnHelpTrigger( int nHelpID )
{
	// 중복 체크
	if ( false == m_History.IsLoaded() )
		m_History.Load();

	if ( true == m_History.IsExist( nHelpID ) )
		return false;

	if( ( m_pCurrHelp ) && ( nHelpID == m_pCurrHelp->m_nHelpID ) )
		return false;

	if( ( m_pCurrPopHelp ) && ( nHelpID == m_pCurrPopHelp->m_nHelpID ) )
		return false;

	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return false;

	if( false == pHelp->InitHelp() )			// 가능 유무 검사
	{
		pHelp->FinishHelp();
		return false;
	}
	
	bool bResult		= false;
	// Popup
	// Tutorial
	switch( pHelp->m_eHelpType )
	{
	case HELPTYPE_POPUPHELP:
	case HELPTYPE_POPUPHELP_AUTO:
		{
			if( false == OnGlueEventPopupHelpTrigger( pHelp ) )
				return false;

			m_History.InsertHistory( pHelp->m_nHelpID );
		}
		break;

	case HELPTYPE_TUTORIAL:
	case HELPTYPE_SPECIAL_TUTORIAL:
		{
			XFieldInfo* pFieldInfo	= gg.currentgamestate->GetWorld()->GetInfo();
			if( ( pFieldInfo ) && ( true == pFieldInfo->m_bTutorial ) )
				SetReserveHelp( pHelp );
		}
		break;
	}
	
	return true;
}

bool XHelpMgr::OnHelpTriggerCmd( int nHelpID )
{
	XHelp* pHelp	= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return false;

	bool bResult		= false;
	switch( pHelp->m_eHelpType )
	{
	case HELPTYPE_POPUPHELP:
	case HELPTYPE_POPUPHELP_AUTO:
		bResult		= OnGlueEventPopupHelpTrigger( pHelp );
		break;

	case HELPTYPE_TUTORIAL:
	case HELPTYPE_SPECIAL_TUTORIAL:
		{
			SetReserveHelp( pHelp );
			bResult		= true;
		}
		break;
	}

	return bResult;
}

void XHelpMgr::OnHelpTriggerClose( XHelp* pHelp )
{
	string strTutorialType;
	if( pHelp )
		GetHelpTypeString( pHelp->m_eHelpType, strTutorialType );
	// 현재 열려있는 Help를 닫기 신호를 보낸다
	global.script->GetGlueGameEvent().OnTutorialTriggerClose( strTutorialType );
}

void XHelpMgr::OnHelpTriggerClose( const string& strTutorialType )
{
	global.script->GetGlueGameEvent().OnTutorialTriggerClose( strTutorialType );
}

bool XHelpMgr::OnHelpTriggerString( const wstring& strEventName )
{
	XHelp* pHelp	= FindEventTriggerHelpData( strEventName );
	if( pHelp )
		return OnHelpTrigger( pHelp->m_nHelpID );	
	
	return false;
}

void XHelpMgr::FinishedHelp( int nHelpID )
{
	XHelp* pHelp			= FindHelpData( nHelpID );
	if( NULL == pHelp )
		return ;

	int nLinkHelpID			= 0; 
	switch( pHelp->m_eHelpType )
	{
	case HELPTYPE_TUTORIAL:
	case HELPTYPE_SPECIAL_TUTORIAL:
		{
			if( m_pCurrHelp )
			{
				nLinkHelpID			= m_pCurrHelp->m_nLinkHelpID;
				m_pCurrHelp->FinishHelp();
				m_pCurrHelp			= NULL;
				m_pFinishDelayHelp	= NULL;

			}
		}
		break;

	case HELPTYPE_POPUPHELP:
	case HELPTYPE_POPUPHELP_AUTO:
		{
			if( m_pCurrPopHelp )
			{
				nLinkHelpID			= m_pCurrPopHelp->m_nLinkHelpID;
				m_pCurrPopHelp->FinishHelp();
				m_pCurrPopHelp		= NULL;
			}
		}
		break;
	}

	if( 0 != nLinkHelpID )
		OnHelpTrigger( nLinkHelpID );
}

void XHelpMgr::ResetForTest()
{
	InitHelpMgr();
}

XHelp* XHelpMgr::FindHelpData( int nHelpID )
{
	itHelpData it		= m_mapHelpData.find( nHelpID );
	if( it == m_mapHelpData.end() )
		return NULL;

	XHelp* pNewHelp		= ( XHelp* )( *it ).second;

	return pNewHelp;
}

XHelp* XHelpMgr::FindEventTriggerHelpData( const wstring& strEventName )
{
	itStringTriggerHelpData it	= m_mapEventTriggerHelpData.find( strEventName );
	if( it == m_mapEventTriggerHelpData.end() )
		return NULL;

	XHelp* pNewHelp				= ( XHelp* )( *it ).second;
	
	return pNewHelp;
}

bool XHelpMgr::OnGlueEventHelpTrigger( XHelp* pHelp )
{
	if( NULL == global.script )
		return false;

	string strType;
	GetHelpTypeString( pHelp->m_eHelpType, strType );

	m_pCurrHelp		= pHelp;

	if( true == global.script->GetGlueGameEvent().OnTutorialTrigger( strType, pHelp->m_nHelpID ) )
	{
		m_eSpecialActionCmd		= SPECIAL_KEY_NONE;
		m_TutorialFinishChecker.SetTutorialFinishChecker( pHelp );

		return true;
	}
	return false;
}

bool XHelpMgr::OnGlueEventPopupHelpTrigger( XHelp* pHelp )
{
	if( NULL == global.script )
		return false;

	string strType;
	GetHelpTypeString( pHelp->m_eHelpType, strType );

	if( true == global.script->GetGlueGameEvent().OnTutorialTrigger( strType, pHelp->m_nHelpID ) )
	{
		m_pCurrPopHelp		= pHelp;
		return true;
	}

	return false;
}

void XHelpMgr::GetHelpTypeString( XHelpType eType, string& strType )
{
	switch( eType )
	{
	case HELPTYPE_POPUPHELP:
	case HELPTYPE_POPUPHELP_AUTO:
		strType			= "POPUPHELP";
		break;

	case HELPTYPE_TUTORIAL:
	case HELPTYPE_SPECIAL_TUTORIAL:
		strType			= "TUTORIAL";
		break;
		
	default:
		strType			= "";
	}
}

void XHelpMgr::SetReserveHelp( XHelp* pHelp )
{
	if( NULL == pHelp )
		return ;

	if( HELPTYPE_SPECIAL_TUTORIAL == pHelp->m_eHelpType )
	{
		// 현재 활성화중인 튜토리얼을 가장 앞의 예약으로 밀어 넣고 이놈을 활성화해주자
		if( m_pCurrHelp )
		{
			OnHelpTriggerClose( m_pCurrHelp );

			// 종료대기타임중일수도 있다.
			m_pFinishDelayHelp		= NULL;
			//m_listReserveHelp.push_front( m_pCurrHelp );	// 현재 활성화를 뒤로 밀어 넣기 삭제 _ by tripleJ 110509
		}

		m_listReserveHelp.push_front( pHelp );

		return ;
	}

	// 예약시스템 추가
	m_listReserveHelp.push_back( pHelp );
}

void XHelpMgr::SetFinishHelp()
{
	if( NULL == m_pCurrHelp )
		return ;

	if( 0.0f == m_pCurrHelp->m_fFinishTime )
	{
		// 바로 종료
		OnHelpTriggerClose( m_pCurrHelp );
		m_pFinishDelayHelp		= NULL;
	}
	else
	{
		// 딜레이 종료
		m_pFinishDelayHelp		= m_pCurrHelp;
		m_fFinishDelayTime		= XGetNowTime();
	}
}