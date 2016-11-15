/// @file WS_Shared.h
#pragma once

#include <map>
#include <vector>
#include <deque>
#include <string>

#include "C_ControlDoAction.h"

class CDialog;
class CDialogBar;
class CBHTabedReBar;
class CRollupCtrl;

class CListenableView;
class CCoreToolToolBar;
class CControlMediator;
class CWorkSpaceManager;
class CWorkSpaceScript;

struct SListenableViewInfo
{
	SListenableViewInfo(CDialog* _pDlg, HINSTANCE hResourceHandle, int _nIDD, CListenableView* _pListenableView, CORE_VIEW_TYPE eType)
	: m_pDialog(_pDlg)
	, m_nIDD( _nIDD)
	, m_pListenableView( _pListenableView )
	, m_hResourceHandle(hResourceHandle)
	, m_eType(eType)
	{
	}

	CDialog*					m_pDialog;
	int							m_nIDD;
	HINSTANCE					m_hResourceHandle;
	bool						m_bExclusiveOpen;	///< 이게 뭘까요
	CORE_VIEW_TYPE				m_eType;			///< 뷰 타입
	WORKBENCH_STATE::STATE		m_eWorkBenchState;

	CListenableView*			m_pListenableView;
};
typedef							std::map<int, SListenableViewInfo*> MAP_LISTENABLEVIEW;

struct CORETOOL_API SWorkSpaceUIInfo
{
	std::string					m_strSidePanel;
	int							m_nTabIndex;
	WORKBENCH_STATE::STATE		m_eTabWorkBenchState;

	void						AddRollup(int _nId, bool bExpandLayout, WORKBENCH_STATE::STATE _eRollupWB, bool bExclusiveOpen);
	typedef						std::pair<bool, WORKBENCH_STATE::STATE> ROLLUI_EXPAND;
	typedef						std::vector< std::pair<int, ROLLUI_EXPAND > > ROLLUPS;
	ROLLUPS						m_vecRollups;
	std::vector<int>			m_vecExpandLayoutRollups;
};

struct SToolBarButtonInfo
{
	SToolBarButtonInfo()
		: m_bAxis(true), m_bLock(true), m_bOnTerrain(true), m_bSceneList(true), m_bLayerManager(true)
		, m_bWeather(true), m_bLightMap(true), m_bClip(true)
	{
	}

	bool						m_bAxis;
	bool						m_bLock;
	bool						m_bOnTerrain;
	bool						m_bSceneList;
	bool						m_bLayerManager;
	bool						m_bWeather;
	bool						m_bLightMap;
	bool						m_bClip;
};

typedef							std::vector<SWorkSpaceUIInfo> VEC_WS_UI;

typedef							std::pair<int, int>			BIND_CACHE;		// pair ( tabid, rollupid )
typedef							std::vector< BIND_CACHE >	VEC_BIND_CACHE;
typedef							std::pair<CRollupCtrl*, int> ROLLUP_BIND;	// CRollupCtrl*, rollup_control_Index

struct WB_BIND
{
	int nTabID;
	CListenableView* pListenableView;
	ROLLUP_BIND rollupBind;	
};

typedef							std::vector< WB_BIND > VEC_WB_BIND;
typedef							std::map< ROLLUP_BIND, SListenableViewInfo*> MAP_CURLISTENABLEVIEW;

class ViewIDGenerator
{
public:
	ViewIDGenerator() : m_nLastGenerated(0) {}

	unsigned int GetNewID() { return m_nLastGenerated++; }

protected:
	unsigned int m_nLastGenerated;
};

class CControlMediatorHelperEntityAction;
class CControlMediatorHelperNewMapResource;
class CControlMediatorHelperObjectRenderer;
class CKeyEventHandlerHelper;

class CORETOOL_API CWorkSpaceHelpersAndActions
{
public:
	CWorkSpaceHelpersAndActions()
	{
		m_pEntityActionHelper = NULL;
		m_pNewMapResourceHelper = NULL;
		m_pObjectRenderHelper = NULL;
		m_pKeyEventHandlerHelper = NULL;

		m_pDoActionObjectNew = NULL;
		m_pDoActionObjectSelect = NULL;
		m_pDoActionObjectMove = NULL;
		m_pDoActionObjectRotate = NULL;
		m_pDoActionObjectScale = NULL;

		m_pDoActionEtcPathSelect = NULL;

		m_pPopupActionRepository = NULL;
	}

	~CWorkSpaceHelpersAndActions()
	{
		_ASSERT( m_pEntityActionHelper == NULL );
		_ASSERT( m_pNewMapResourceHelper == NULL );
		_ASSERT( m_pObjectRenderHelper == NULL );
		_ASSERT( m_pKeyEventHandlerHelper == NULL );

		_ASSERT( m_pDoActionObjectNew == NULL );
		_ASSERT( m_pDoActionObjectSelect == NULL );
		_ASSERT( m_pDoActionObjectMove == NULL );
		_ASSERT( m_pDoActionObjectRotate == NULL );
		_ASSERT( m_pDoActionObjectScale == NULL );

		_ASSERT( m_pDoActionEtcPathSelect == NULL );

		_ASSERT( m_pPopupActionRepository == NULL );
	}

	// Helper
	CControlMediatorHelperEntityAction*		m_pEntityActionHelper;
	CControlMediatorHelperNewMapResource*	m_pNewMapResourceHelper;
	CControlMediatorHelperObjectRenderer*	m_pObjectRenderHelper;
	CKeyEventHandlerHelper*					m_pKeyEventHandlerHelper;

	// DoAction
	CDoActionObjectNew*						m_pDoActionObjectNew;
	CDoActionObjectSelect*					m_pDoActionObjectSelect;
	CDoActionObjectMove*					m_pDoActionObjectMove;
	CDoActionObjectRotate*					m_pDoActionObjectRotate;
	CDoActionObjectScale*					m_pDoActionObjectScale;
	CDoActionEtcPathSelect*					m_pDoActionEtcPathSelect;

	// Popup action
	CControlPopupActionRepository*			m_pPopupActionRepository;
};

class CWorkSpaceUiSetter;

class CORETOOL_API CWorkSpace
{
	friend class				CWorkSpaceUiSetter;
	friend class				CWorkSpaceManager;
	friend class				CControlMediator;

public:
								CWorkSpace() : m_pControlMediator_(NULL), m_eDefaultWB_(WORKBENCH_STATE::ETC_MAP_VIEW) {}
	virtual						~CWorkSpace(){}

	virtual const char*			GetWorkSpaceKeyword()	= 0;

	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction) = 0;
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction) = 0;

	virtual const char*			GetFileExtension()		= 0;	///< 다룰수 있는 확장자
	virtual const char*			GetFileExtensionDesc()	= 0;	///< 확장자 설명

	void						GetOpenFileFilter( std::string& _rOutFilter );

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName)					= 0;
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs)		= 0;
	virtual void				OnCloseFile()											= 0;

protected:

	virtual void				OnBetweenCoreToolUpdateAndRS3Render()					= 0;
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy)		= 0;

	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message)	= 0;
	virtual void				OnWBDevAction()											= 0;
	virtual void				OnWBHelperRender()										= 0;

	virtual void				OnFileModified( const string& strFileName ) {}		///< 파일이 변경되었을때 처리

protected:

	virtual void				OnRollupExpandClicked(CListenableView* pRollup){}
	virtual bool				OnCheckAlwaysPickingWhenMoused(){ return true; }

	virtual void				OnCreateWorkspaceFile(){ _ASSERT(0); } // 이게 불릴때라면, 상속받는쪽이 구현해줘야한다.
	virtual void				OnSceneSaved(const char* _filename){ _ASSERT(0); } // 이게 불릴때라면, 상속받는쪽이 구현해줘야한다.

	virtual void				OnLostDevice() {}					///< rs device 가 lost 될때
	virtual void				OnRestoreDevice() {}				///< rs device 가 restore 될때

	CControlMediator*			m_pControlMediator_;
	WORKBENCH_STATE::STATE		m_eDefaultWB_;

private:

	VEC_WS_UI&					GetWorkSpaceUI(){ return m_vecWorkSpaceUI; }
	VEC_WS_UI					m_vecWorkSpaceUI;

	void						BindWorkBenchStateAndRollupFromUI(MAP_CURLISTENABLEVIEW& _rCurrRollups);
	bool						IsBindedWB( WORKBENCH_STATE::STATE _eWB){ return (m_mapWorkbenchBind.find(_eWB) != m_mapWorkbenchBind.end()); }
	typedef						std::map< WORKBENCH_STATE::STATE, VEC_WB_BIND > MAP_WB_BIND;
	MAP_WB_BIND					m_mapWorkbenchBind;
	typedef						std::map<WORKBENCH_STATE::STATE, VEC_BIND_CACHE > MAP_BIND_CACHE;
	MAP_BIND_CACHE				m_mapBindCache;

	int							AddTab(const char* _pSzPanelName, WORKBENCH_STATE::STATE _eTabWB);
	void						BindWBState(WORKBENCH_STATE::STATE _eWbState, int _nTabID, int _nRollupId);
	void						SetDefaultWB(WORKBENCH_STATE::STATE _eWbState){ m_eDefaultWB_ = _eWbState; }
	SToolBarButtonInfo			m_sToolBarButton_;

	CWorkSpaceHelpersAndActions	m_WorkspaceHelpersAndActions_;
};

template< typename T >
class CWorkSpaceImpl : public CWorkSpace
{
public:
	static std::string WS_KEYWORD;
};

template <typename T > std::string CWorkSpaceImpl<T>::WS_KEYWORD("");

class CORETOOL_API CWorkSpaceUiSetter
{
public:
	CWorkSpaceUiSetter(CWorkSpace* _pWorkSpace) : m_sToolBarButton(_pWorkSpace->m_sToolBarButton_)
	{
		m_pWorkSpace_ = _pWorkSpace;
	}

	int AddTab(const char* _pSzPanelName, WORKBENCH_STATE::STATE _eTabWB = WORKBENCH_STATE::STATE_NUM)
	{
		return m_pWorkSpace_->AddTab(_pSzPanelName, _eTabWB);
	}

	void AddRollup(int _nTabIdx, int _nRollupId, bool bExpandLayout, WORKBENCH_STATE::STATE _eRollupWB, bool bExclusiveOpen )
	{
		_ASSERT(_nTabIdx < (int)(m_pWorkSpace_->m_vecWorkSpaceUI.size()));
		_ASSERT( _nRollupId > -1 );

		m_pWorkSpace_->m_vecWorkSpaceUI[_nTabIdx].AddRollup(_nRollupId, bExpandLayout, _eRollupWB, bExclusiveOpen);
	}

	void BindWBState(WORKBENCH_STATE::STATE _eWbState, int _nTabID, int _nRollupId)
	{
		m_pWorkSpace_->BindWBState(_eWbState, _nTabID, _nRollupId);
	}

	void SetDefaultWB(WORKBENCH_STATE::STATE _eWbState)
	{
		m_pWorkSpace_->SetDefaultWB(_eWbState);
	}

	SToolBarButtonInfo& m_sToolBarButton;

private:
	CWorkSpace* m_pWorkSpace_;
};