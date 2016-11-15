#include "stdafx.h"
#include "XHelp.h"

// XHelpMgr 변경으로 인해 test 전부 새로이 작성해야함	by tripleJ 110415
SUITE( Tutorial )
{
	class TestTutorial : public XHelpMgr
	{
	public:
						TestTutorial()		{}
			virtual		~TestTutorial()		{}

			using		XHelpMgr::InsertHelpData;
			using		XHelpMgr::CheckHelpID;
			using		XHelpMgr::CheckHelpType;
			using		XHelpMgr::FindHelpData;
			using		XHelpMgr::FindEventTriggerHelpData;
			using		XHelpMgr::InitEventTrigger;

			using		XHelpMgr::m_mapHelpData;
			using		XHelpMgr::m_mapEventTriggerHelpData;
			using		XHelpMgr::m_listStartChecker;
			using		XHelpMgr::m_listReserveHelp;
	};

	class TTutorial
	{
	public:
							TTutorial()			{}
							~TTutorial()		{}

			TestTutorial	m_HelpMgr;
	};
	
	// 데이타 초기화 검사
	TEST_FIXTURE( TTutorial, TestTutorialConstructor )
	{
		XHelp help;

		CHECK_EQUAL( 0,				help.m_nHelpID						);
		CHECK_EQUAL( HELPTYPE_MAX,	help.m_eHelpType					);
		CHECK_EQUAL( 0,				help.m_strImg.size()				);
		CHECK_EQUAL( 0,				help.m_strString.size()				);
		CHECK_EQUAL( 0,				help.m_strEventTrigger.size()		);
		CHECK_EQUAL( 0,				help.m_strEventTriggerString.size()	);
		CHECK_EQUAL( 0,				help.m_strFinishEvent.size()		);
		CHECK_EQUAL( 0,				help.m_strFinishEventString.size()	);
		CHECK_EQUAL( 0,				help.m_nLinkHelpID					);
		CHECK_EQUAL( 0,				help.m_nMinLevel					);
		CHECK_EQUAL( 0,				help.m_nMaxLevel					);
		CHECK_EQUAL( 0.0f,			help.m_fTimer						);
		CHECK_EQUAL( 0.0f,			help.m_fFinishTime					);
		CHECK_EQUAL( false,			help.m_bDestroy						);
	}

	// 
	TEST_FIXTURE( TTutorial, TestTutorialHelpInit )
	{
		XHelp help;
	
		help.m_nMaxLevel		= -1;
		CHECK_EQUAL( false, help.InitHelp() );

		help.m_nMaxLevel		= 1;
		CHECK_EQUAL( true, help.InitHelp() );
	}

	// Mgr에 데이타 넣기
	TEST_FIXTURE( TTutorial, TestTutorialInsertData )
	{
		XHelp help;

		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( &help )	);
		CHECK_EQUAL( 1,		m_HelpMgr.m_mapHelpData.size()		);

		help.m_nHelpID		= 1;
		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( &help )	);
		CHECK_EQUAL( 2,		m_HelpMgr.m_mapHelpData.size()		);
	}

	// Mgr에 ID 중복 체크
	TEST_FIXTURE( TTutorial, TestTutorialIDCheck )
	{
		XHelp help;
		CHECK_EQUAL( true, m_HelpMgr.InsertHelpData( &help ) );

		CHECK_EQUAL( false, m_HelpMgr.CheckHelpID( &help ) );

		// 중복검사
		help.m_nHelpID			= 1;
		CHECK_EQUAL( true, m_HelpMgr.CheckHelpID( &help ) );
	}

	// Help Type 체크
	TEST_FIXTURE( TTutorial, TestTutorialTypeCheck )
	{
		XHelp help;

		// 없는거
		CHECK_EQUAL( false, m_HelpMgr.CheckHelpType( L"asdf", &help ) );
		// 있는거
		CHECK_EQUAL( true, m_HelpMgr.CheckHelpType( L"tutorial", &help ) );
	}

	// 종료 검사
	TEST_FIXTURE( TTutorial, TestTutorialFinish )
	{
		XHelp* phelp		= new XHelp;
		phelp->m_nHelpID	= 1;
		CHECK_EQUAL( true, m_HelpMgr.InsertHelpData( phelp ) );
		phelp				= new XHelp;
		phelp->m_nHelpID	= 2;
		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( phelp )	);
		CHECK_EQUAL( 2,		m_HelpMgr.m_mapHelpData.size()		);

		// 종료
		m_HelpMgr.FinishHelpMgr();

		CHECK_EQUAL( 0, m_HelpMgr.m_mapHelpData.size()				);
		CHECK_EQUAL( 0, m_HelpMgr.m_mapEventTriggerHelpData.size()	);
		CHECK_EQUAL( 0, m_HelpMgr.m_listStartChecker.size()			);
		CHECK_EQUAL( 0, m_HelpMgr.m_listReserveHelp.size()			);
	}

	// 검색 검사
	TEST_FIXTURE( TTutorial, TestTutorialFind )
	{
		XHelp help;
		help.m_nHelpID			= 1;
		
		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( &help )	);
		CHECK_EQUAL( 1,		m_HelpMgr.m_mapHelpData.size()		);

		CHECK( NULL != m_HelpMgr.FindHelpData( 1 )								);
	}

	// Data Get 함수
	TEST_FIXTURE( TTutorial, TestTutorial_Get )
	{
		XHelp help;
		help.m_nHelpID			= 1;
		help.m_strImg			= L"TestImg";
		help.m_strString		= L"TestString";
		help.m_strEventTrigger	= L"TestEventTrigger";
		help.m_fTimer			= 1.0f;

		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( &help )	);
		CHECK_EQUAL( 1,		m_HelpMgr.m_mapHelpData.size()		);

		m_HelpMgr.InitEventTrigger();

		CHECK_EQUAL( 0,		lstrcmpi( L"TestImg", m_HelpMgr.GetHelpImg( help.m_nHelpID, 0 ).c_str() )				);
		CHECK_EQUAL( 0,		lstrcmpi( L"TestString", m_HelpMgr.GetHelpMsg( help.m_nHelpID ).c_str() )				);
		CHECK_EQUAL( 0,		lstrcmpi( L"TestEventTrigger", m_HelpMgr.GetEventTrigger( help.m_nHelpID ).c_str() )	);
		CHECK_EQUAL( 1.0f,	m_HelpMgr.GetHelpTimer( help.m_nHelpID )												);
	}

	// StartChecker 
	TEST_FIXTURE( TTutorial, TestStartChecker )
	{
		XHelp help;
		help.m_nHelpID			= 1;
		help.m_strEventTrigger	= L"OnKillEnemy";

		CHECK_EQUAL( true,	m_HelpMgr.InsertHelpData( &help )	);
		CHECK_EQUAL( 1,		m_HelpMgr.m_mapHelpData.size()		);

		m_HelpMgr.InitEventTrigger();
		CHECK_EQUAL( 1, m_HelpMgr.m_listStartChecker.size() );
	}

	// 종료검사 함수 체크 클래스
	class TestTutorialFinishChecker		: public XTutorialFinishChecker
	{
	public:
										TestTutorialFinishChecker()		{}
		virtual							~TestTutorialFinishChecker()	{}

		using	XTutorialFinishChecker::fChecker;
	};

	// 종료 함수 연결 테스트
	TEST_FIXTURE( TestTutorialFinishChecker, SetTutorialFinishChecker )
	{
		XHelp help;
		
		CHECK( NULL == fChecker );

		help.m_strFinishEvent	= L"OnMyPlayerMove";
		SetTutorialFinishChecker( &help );
		CHECK( NULL != fChecker );

		help.m_strFinishEvent	= L"OnNpcInteraction";
		SetTutorialFinishChecker( &help );
		CHECK( NULL != fChecker );
	}

	// History Class
	TEST( HistoryLoadTest )
	{
		XHelpHistory HistoryTest;
		HistoryTest.Load();

		CHECK( true == HistoryTest.IsLoaded() );
	}

	TEST( HistoryExistTest )
	{
		XHelpHistory HistoryTest;
		HistoryTest.Load();
		int nHelpID		= 1;
		
		HistoryTest.InsertHistory( nHelpID );

		CHECK( true == HistoryTest.IsExist( nHelpID ) );
	}
}
