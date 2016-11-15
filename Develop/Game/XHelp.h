#pragma once

#include <string>
#include <vector>
using namespace std;

#include "XActionKeyDef.h"

// by tripleJ 110414	_기존 Help 관련 수정

//#define HELP_DEV		// by tripleJ 110414	_개발용 Define

// 20레벨 이하에서만 헬프 튜토리얼 기능이 작동한다.
#define HELP_ENABLED_MAX_LEVEL			20

enum XHelpType
{
	HELPTYPE_POPUPHELP	= 0,
	HELPTYPE_POPUPHELP_AUTO,
	HELPTYPE_TUTORIAL,
	HELPTYPE_SPECIAL_TUTORIAL,
	HELPTYPE_MAX,
};

// Help( PopUpHelper, Tutorial )
class XHelp
{
public:
							XHelp();
							~XHelp();

			bool			InitHelp();
			void			FinishHelp();

			void			UpdateHelp( float fDeleta );

			bool			IsDestroy()						{ return m_bDestroy; }
			void			SetDestroy( bool bDestroy )		{ m_bDestroy = bDestroy; }

public:
			int				m_nHelpID;				// ID
			XHelpType		m_eHelpType;			// Help 타입
			wstring			m_strImg;				// Help에 삽입될 이미지
			wstring			m_strImg2;				// Help에 삽입될 이미지2
			wstring			m_strString;			// Help 내용(?) 문자열
			wstring			m_strEventTrigger;		// EventTrigger
			wstring			m_strEventTriggerString;// EventTrigger Text
			wstring			m_strFinishEvent;		// Help Finish Event
			wstring			m_strFinishEventString;	// Help Finish Event String
			int				m_nLinkHelpID;			// 종료 후 다음에 바로 연결될 Help ID
			int				m_nRequiredQuest;		// 소유 퀘스트
			int				m_nMinLevel;			// 최소 레벨
			int				m_nMaxLevel;			// 최대 레벨
			float			m_fTimer;				// 유지시간
			float			m_fFinishTime;			// 종료시 딜레이 타임

			bool			m_bDestroy;				// 죽기 예약?
};

// 튜토리얼 종료 이벤트 상태
enum eFinishCheckerState
{
	FINISHCHECKERSTATE_NONE,
	FINISHCHECKERSTATE_CHECKED,
	FINISHCHECKERSTATE_MAX,
};

// 튜토리얼 종료 이벤트 검사 클래스
class XTutorialFinishChecker
{
public:
									XTutorialFinishChecker();
	virtual							~XTutorialFinishChecker();
			
			void					SetTutorialFinishChecker( XHelp* pHelp );
			void					FinishTutorial();

			eFinishCheckerState		UpdateTutorialFinishChecker();

			bool					IsShowUI( string& strUIName );
			bool					IsUpdate();

			eFinishCheckerState		GetCheckState()										{ return m_eFinishCheckerState; }

			void					SpecialActionCmd( SPECIAL_ACTION_COMMAND eCmd )		{ m_eSpecialActionCmd = eCmd; }
			eFinishCheckerState		( XTutorialFinishChecker::*fChecker )();

protected:
			eFinishCheckerState		OnMyPlayerMove();
			eFinishCheckerState		OnNpcInteraction();
			eFinishCheckerState		OnMouseVisible();
			eFinishCheckerState		OnShowMap();
			eFinishCheckerState		OnBattleStance();
			eFinishCheckerState		OnNomalAttack();
			eFinishCheckerState		OnLooting();
			eFinishCheckerState		OnMyPlayerDodge();
			eFinishCheckerState		OnMyPlayerGuard();
			eFinishCheckerState		OnShowTalent();
			eFinishCheckerState		OnUseTalent();

			XHelp*					m_pHelp;

			SPECIAL_ACTION_COMMAND	m_eSpecialActionCmd;
			eFinishCheckerState		m_eFinishCheckerState;
};

// 튜토리얼 발생 검사 클래스
class XTutorialStartChecker
{
public:
								XTutorialStartChecker();
	virtual						~XTutorialStartChecker();

	virtual	bool				InitStartChecker( XHelp* pHelp );
	virtual	void				FinishStartChecker();

	virtual	bool				UpdateStartChecker()					{ return m_bStart; }
	virtual	void				MyControlEvent( const XEvent& msg )		{}

			int					GetHelpID();

protected:	
			bool				m_bStart;
			XHelp*				m_pHelp;
};

class XKillEnemyStartChecker	: public XTutorialStartChecker
{
public:
								XKillEnemyStartChecker();
	virtual						~XKillEnemyStartChecker();

	virtual	bool				InitStartChecker( XHelp* pHelp );
	virtual	void				FinishStartChecker();

	virtual	bool				UpdateStartChecker();
	virtual	void				MyControlEvent( const XEvent& msg )		{}

protected:
			int					m_nkillEnemy;
			int					m_nKilledEpicEnemy;
			int					m_nKillCondition;
			
};

class XStateHitStartChecker		: public XTutorialStartChecker
{
public:
								XStateHitStartChecker();
	virtual						~XStateHitStartChecker();

	virtual	bool				InitStartChecker( XHelp* pHelp );
	virtual	void				FinishStartChecker();

	virtual	bool				UpdateStartChecker();
	virtual	void				MyControlEvent( const XEvent& msg );

protected:
			ACTION_STATE		m_eActionState;
			int					m_nHitCurr;
			int					m_nHitCondition;
};

// 봤던 도움말 저장소
class XHelpHistory
{
private:
			set<int>			m_Data;								// 봤던 도움말
			set<int>			m_ReLoadData;						// 다시 볼수 있는 도움말( 맵이동등 History가 리셋될경우 )
			bool				m_bLoaded;
public:
								XHelpHistory() : m_bLoaded(false) {}
			void				InsertHistory( int nHelpID );
			void				EraseHistory( int nHelpID );
			void				InsertReloadHistory( int nHelpID );
			void				EraseReloadHistory( int nHelpID );


			void				Load();
			void				Finish();
			void				Clear();
			bool				IsExist(int nHelpID);
			bool				IsLoaded() { return m_bLoaded; }
};

class XHelpHistoryExporter
{
public:
	void Export( wstring strHistroyType, XAccountData* pAccountData, set<int>& setData );
};

class XHelpHistoryImporter
{
public:
	void Import( wstring strHistroyType, XAccountData* pAccountData, set<int>& setData);
};

/// 시간이 지남에 따라 자동으로 출력할 도움말 큐
class XPopupHelpAutoQueue
{
private:
			list<XHelp*>		m_Queue;
			float				m_fQueueElapsedTime;
			float				m_fAutoCycle;
			bool				m_bAutoPopupHelp;
public:
								XPopupHelpAutoQueue();
								~XPopupHelpAutoQueue();

			void				InitAutoQueue( float fCycleTime );
			void				ClearAll();

			void				Push_Back(XHelp* pHelp);
			void				Push_Front(XHelp* pHelp);
			void				ResetTime();
			int					Update( float fDelta );
			size_t GetSize()	{ return m_Queue.size(); }
};

// HelpID
typedef map< int, XHelp* >						mapHelpData;
typedef mapHelpData::iterator					itHelpData;
// Event
typedef map< wstring, XHelp* >					mapStringTriggerHelpData;
typedef mapStringTriggerHelpData::iterator		itStringTriggerHelpData;
// StartChecker
typedef list< XTutorialStartChecker* >			listStartChecker;
typedef listStartChecker::iterator				itStartChecker;

typedef list< XHelp* >							listReserveHelp;

// Helper( PopUpHelper, Tutorial ) 관리 메니저 
class XHelpMgr
{
public:
										XHelpMgr();
	virtual								~XHelpMgr();

			bool						LoadHelpMgr();
			bool						InitHelpMgr();
			void						FinishHelpMgr();
			void						DestroyHelpMgr();
			void						DestroyStartChecker();

			void						UpdateHelpMgr( float fDeleta );
			
			bool						IsEnabled();

			SPECIAL_ACTION_COMMAND		GetSpecialActionCmd()							{ return m_eSpecialActionCmd; }
			wstring						GetHelpImg( int nHelpID, int nImgIndex );
			wstring						GetHelpMsg( int nHelpID );
			wstring						GetEventTrigger( int nHelpID );
			float						GetHelpTimer( int nHelpID );

			bool						OnEvent( MEvent& e );
			void						SpecialActionCmd( const SPECIAL_ACTION_COMMAND& eCmd );
			void						MyControlEvent( const XEvent& msg );		// 중간에 메세지 확인만 해서 동작

			bool						OnHelpTrigger( int nHelpID );
			bool						OnHelpTriggerCmd( int nHelpID );
			void						OnHelpTriggerClose( XHelp* pHelp = NULL );
			void						OnHelpTriggerClose( const string& strTutorialType );

			bool						OnHelpTriggerString( const wstring& strEventName );

			void						FinishedHelp( int nHelpID );
		
			void						ResetForTest();

protected:
			bool						LoadHelpData();
			void						ParseHelpData( MXmlElement* pElement, MXml* pXml );

			void						UpdateReserveHelp();
			void						UpdateStartChecker();
			void						UpdateFinishHelp();
			void						UpdatePopupHelpAuto( float fDeleta );

			bool						CheckHelpID( XHelp* pHelp );
			bool						CheckHelpType( wstring strVal, XHelp* pHelp );
			bool						InsertHelpData( XHelp* pHelp );
			void						InitEventTrigger();
			void						InitPopupHelpAuto();
			bool						SetEventChecker( XHelp* pHelp );
			void						DeleteHelpData( XHelp* pHelp );
			void						DeleteStartChecker( XTutorialStartChecker* pChecker );

			

			XHelp*						FindHelpData( int nHelpID );
			XHelp*						FindEventTriggerHelpData( const wstring& strEventName );
			bool						OnGlueEventHelpTrigger( XHelp* pHelp );
			bool						OnGlueEventPopupHelpTrigger( XHelp* pHelp );

			void						GetHelpTypeString( XHelpType eType, string& strType );

			void						SetReserveHelp( XHelp* pHelp );
			void						SetFinishHelp();

protected:
			mapHelpData					m_mapHelpData;						// Helper( PopUpHelper, Tutorial ) 관리		_!!해제
			mapStringTriggerHelpData	m_mapEventTriggerHelpData;			// 해제 X
			listStartChecker			m_listStartChecker;					// 해제
			listReserveHelp				m_listReserveHelp;
			XHelp*						m_pCurrHelp;
			XHelp*						m_pCurrPopHelp;
			XHelp*						m_pFinishDelayHelp;
			XHelpHistory				m_History;
			XPopupHelpAutoQueue			m_PopupHelpAuto;

			XTutorialFinishChecker		m_TutorialFinishChecker;

			bool						m_bEnabled;							// HelpMgr 활성화 여부

			SPECIAL_ACTION_COMMAND		m_eSpecialActionCmd;				// 내 캐릭터 특정 동작 저장

			float						m_fFinishDelayTime;
};
