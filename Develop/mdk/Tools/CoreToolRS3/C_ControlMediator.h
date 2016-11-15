/// @file C_ControlMediator.h
#pragma once

#include <string>
#include <vector>

#include "VL_ListenableView.h"

#include "M_ToolSceneManager.h"
#include "M_ToolCamera.h"
#include "ext_lib/FileWatcher.h"

#include "RDebuggingInfo.h"

//Tool Const
namespace Tool3Const
{
	/** @brief Keyboard factor */
	const float CONST_KEY_MOVE = 0.001f;

	/** @brief Mouse factor */
	const float CONST_MOUSE_RATATE_SAFE_Z = 0.99f;
	const float CONST_MOUSE_PANNING = 0.000017f;
	const float CONST_MOUSE_ROTATE = 0.0001f;

	/** @brief Workbench factor */
	const int   CONST_WB_STATIC_OBJ_TO_CENTER = 2000;
	const int   CONST_WB_ENTITY_OBJ_TO_CENTER = 3000;
	const float CONST_WB_NAVI_HEIGHT = 170.f;
};

//manager class or helper class
class CToolCmdMgr;
class CToolLogicObjectManager;
class CWorkSpaceManager;
class CRubyTestSet;

// Mediator Helper
class CControlMediatorHelperAction;
class CControlMediatorHelperRender;
class CControlMediatorHelperResource;

/**
	@brief 컨트롤 중계자

	뷰들의 컨트롤을 중계하는 컨트롤 중계자입니다. \n
	전통적인 컨트롤러보다 Mediator 패턴의 중계자적인 성격이 강합니다.
*/
class CORETOOL_API CControlMediator
{

friend class CControlMediatorHelperAction;
friend class CControlMediatorHelperRender;
friend class CControlMediatorHelperResource;

friend class CWorkSpaceManager;
friend class CRubyTestSet;

public:
	enum EDITMODE
	{
		EDITMODE_NONE = 0,
		EDITMODE_ZONE = 1,
		EDITMODE_FIELD = 2,
	};

public:
	CControlMediator();
	~CControlMediator();


	//////////////////////////////////////////////////////////////////////////
	//
	// 뷰 관련
	//
	//////////////////////////////////////////////////////////////////////////

	/** @brief Message Broad Casting */
	inline void BroadCastMessage(VIEW_MESSAGE::ID _message)
	{
		for (std::vector<CListenableView*>::iterator itr = m_vecListenableViews_.begin(); itr != m_vecListenableViews_.end(); ++itr)
			(*itr)->Listen( CoreMessage(_message) );
	}

	/** @brief Message Broad Casting for CoreMessage object*/
	inline void BroadCastMessage(CoreMessage& _message)
	{
		for (std::vector<CListenableView*>::iterator itr = m_vecListenableViews_.begin(); itr != m_vecListenableViews_.end(); ++itr)
			(*itr)->Listen( _message );
	}

	/** @brief Message Casting for specific view */
	inline void CastMessageToView(int nViewID, VIEW_MESSAGE::ID _message)
	{
		m_vecListenableViews_[nViewID]->Listen( CoreMessage(_message) );
	}

	/** @brief View Getter */
	template <typename T>
	inline T* GetViewPtr()
	{
		return static_cast<T*>(GetViewPtr(T::GetID()));
	}

	/** @brief View Getter using ID */
	CListenableView* GetViewPtr(int _nViewId)
	{
		if (_nViewId < 0)
			return NULL;

		CListenableView* pView = m_vecListenableViews_[_nViewId];
		_ASSERT( pView->GetViewID() == _nViewId );
		return pView;
	}

	/** @brief View Adder */
	inline void AddView(CListenableView* _pView)
	{
		m_vecListenableViews_.push_back(_pView);
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// 워크스페이스 관련
	//
	//////////////////////////////////////////////////////////////////////////

	MAP_EDIT_TYPE	LoadWorkSpaceFile(const char* _pStrName, const std::string& _rWsKeyword);	///< Load WorkSpace File
	std::string		SaveWorkSpaceFile(const char* _pSzFileName, bool bSaveAs);					///< Save WorkSpace File
	void			CreateWorkSpaceFile(const char* _pSzWorkSpaceName, MAP_EDIT_TYPE _mapType);	///< Create WorkSpace File

	/** @brief WorkSpace Getter */
	template<typename T>
	inline T* GetWorkSpacePtr()
	{
		return static_cast<T*>( m_pWorkSpaceMgr_->GetWorkSpace(T::WS_KEYWORD) );
	}

	/** @brief Current WorkSpace Getter */
	template<typename T>
	inline T* GetCurrentWorkSpaceAs()
	{
		return dynamic_cast<T*>( m_pWorkSpaceMgr_->GetCurrentWorkSpace() );
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// 액션관련
	//
	//////////////////////////////////////////////////////////////////////////

	inline CToolCmdMgr*						GetToolCmdMgr()			{ return m_pCmdMgr_; }				///< 커맨드 매니저
	inline CToolLogicObjectManager*			GetToolLogicObjectMgr()	{ return m_pLogicObjectMgr_; }		///< 로직 오브젝트 매니저
	inline CWorkSpaceManager*				GetWorkSpaceMgr()		{ return m_pWorkSpaceMgr_; }		///< WorkSpace Manager

	inline CControlMediatorHelperAction*	GetHelperAction()		{ return m_pHelperAction_; }		///< Action Helper
	inline CControlMediatorHelperRender*	GetHelperRender()		{ return m_pHelperRender_;}			///< Render Helper
	inline CControlMediatorHelperResource*	GetHelperResource()		{ return m_pHelperResource_; }		///< Resource Helper


	//////////////////////////////////////////////////////////////////////////
	//
	// 워크벤치 State / Mode / Handle / Timer
	//
	//////////////////////////////////////////////////////////////////////////

	/** @brief Workbench Size Getter */
	inline void GetWBSize(LONG& _refWidth, LONG& _refHeight) const
	{
		_refWidth = (LONG)m_wbWidth_;
		_refHeight = (LONG)m_wbHeight_;
	}

	/** @brief Current Workbench State Setter */
	void SetCurrentWBState(WORKBENCH_STATE::STATE _state);

	inline WORKBENCH_STATE::STATE		GetCurrentWBState()	const { return m_wbCurrentState_; }		///< Current Workbench State Getter
	inline WORKBENCH_STATE::STATE		GetPreviousWBState()const { return m_wbPreviousState_; }	///< Previous Workbench State Getter

	inline HWND							GetWBWindowHandle()	const { return m_wbhWnd_; }				///< Workbench Window Handle
	inline DWORD						GetWBTimerTic()		const { return ELAPSE_IN_TIMER; }		///< Workbench Processing Tic

	inline EDITMODE						GetEditMode() const		  { return m_nEditMode; }


	//////////////////////////////////////////////////////////////////////////
	//
	// 워크벤치 액션
	//
	//////////////////////////////////////////////////////////////////////////

	void								WB_CallBackAction();										///< WorkBench 콜백액션(카메라 액션 등)
	void								WB_OnTimeAction();											///< 워크벤치 상태에 대응하는 액션 메소드


	//////////////////////////////////////////////////////////////////////////
	//
	// 장치와 관련된 메소드 - 생성, 렌더링, 리사이즈
	//
	//////////////////////////////////////////////////////////////////////////

	/**
		@brief  장치를 생성한후에 뷰들에게 알려주는 메소드
		@param  nWidth  : 생성할 장치의 가로크기
		@param  nHeight : 생성할 장치의 세로크기
		@param  _hWnd   : 생성된 장치가 붙을 윈도우 핸들
		@param  _workTime : 장치가 불려지는 시간
		@return bool : 생성여부
	*/
	bool								CreateWBDeviceAndNotify(UINT nWidth, UINT nHeight, HWND _hWnd, UINT _workTime);
	void								DestroyDevice();

	void								RunRS3();													///< 리얼스페이스 렌더링
	inline bool							IsWBNavigation() const { return m_bNavigation_; }			///< (F2를 이용한) Navigation 중인지 여부

	void								ResizeWBDevice(UINT _w, UINT _h);							///< 랜더링 화면 재설정
	inline bool							IsWBDeviceInited() const { return m_bDeviceInited_; }		///< 워크밴치 DX Device가 생성되었는지 여부


	//////////////////////////////////////////////////////////////////////////
	//
	// RealSpace 관련 자원
	//
	//////////////////////////////////////////////////////////////////////////

	inline rs3::RDeviceD3D*				GetCurrentDevice()			{ return m_pCurrentDevice_; }		///< 장치
	inline rs3::REnvironmentSet*		GetCurrentEnvironmentSet()	{ return m_pEnvironmentSet_; }		///< 환경

	//Tool 씨리즈는 null 이면 안된다.
	inline rs3::RToolSceneManager*		GetToolSceneManager()		{ return m_pCurrentSceneManager_;}	///< 씬 메니저
	inline rs3::RToolCamera*			GetToolCamera()				{ return m_pCurrentCamera_; }		///< 카메라

	void								ResetResource();		///< 리소스 리셋
	void								DestoryResource();		///< 리소스 삭제


	//////////////////////////////////////////////////////////////////////////
	//
	// 씬/맵 관련
	//
	//////////////////////////////////////////////////////////////////////////

	MAP_EDIT_TYPE		LoadZone(const char* _pStrName, MAP_EDIT_TYPE _wantedMapType, bool _bWithRelatedConfig);	///< Load Scene
	void				SaveZone(const char* _filename, bool _bSort, bool _bSaveAs);	///< Save Scene
	void				CreateZone();													///< Create Scene

	/** @brief 맵 관련 정보 */
	struct MAP_INFO {
		MAP_INFO():
			m_bLoaded(false), m_bSceneEditableMap(false), m_bEditSceneObject(false),
			m_width(0.f), m_height(0.f), m_orgX(0.f), m_orgY(0.f)
		{}

		//////////////////////////////////////////////////////////////////////////
		//
		// 기본정보
		//
		bool				m_bLoaded;				///< 맵 로딩 여부
		bool				m_bSceneEditableMap;	///< 로딩된 씬의 편집 가능여부
		bool				m_bEditSceneObject;		///< 현재 씬을 편집할수 있는지 여부. (Zone으로 로딩되어 있어도 Entity를 편집할수 만 있을수 있음)

		MAP_EDIT_TYPE		m_eMapEditType;			///< 로딩된 맵 타입
		std::string			m_strScenePath;			///< 맵이 있는 작업 디렉토리
		std::string			m_strSceneFile;			///< 맵 파일 이름


		//////////////////////////////////////////////////////////////////////////
		//
		// 아웃도어 정보
		//
		float				m_width;				///< 맵 가로 크기
		float				m_height;				///< 맵 세로 크기

		float				m_orgX;
		float				m_orgY;

		int					m_rowFace;				///< 맵 행의 면 수
		int					m_colFace;				///< 맵 열의 면 수
		std::string			m_strTerrainBaseTile;	///< 지형 기본 타일


		//////////////////////////////////////////////////////////////////////////
		//
		// 인도어 정보
		//
		std::string			m_strImportSceneFile;
		rs3::RBoundingBox	m_aabb;

	} m_mapInfo;

	//////////////////////////////////////////////////////////////////////////
	//
	// 카메라 밎 마우스
	//
	//////////////////////////////////////////////////////////////////////////

	/** @brief 카메라 설정값 관련 */
	struct CAMERA_INFO {

		CAMERA_INFO():m_bCameraMode(true), m_bKeyW(false),
			m_bKeyA(false), m_bKeyS(false), m_bKeyD(false), FAR_Z(200000.0f)
		{}

		bool	m_bCameraMode;					///< Alt, Space 키등에 의한 카메라 모드 여부
		bool	m_bKeyW;						///< W 키에 의한 카메라 네비게이션 여부
		bool	m_bKeyA;						///< A 키에 의한 카메라 네비게이션 여부
		bool	m_bKeyS;						///< S 키에 의한 카메라 네비게이션 여부
		bool	m_bKeyD;						///< D 키에 의한 카메라 네비게이션 여부
		float	ROTATE_STEP;					///< 회전속도
		float	MOVE_STEP;						///< 패닝속도
		float	KEY_STEP;						///< 키보드 속도
		float	WHEEL_ZOOM_STEP;				///< 마우스 줌 속도
		float	FAR_Z;							///< 카메라 far_z

	} m_cameraInfo;

	/** @brief 마우스 관련 정보 */
	struct MOUSE_INFO {

		MOUSE_INFO(){}
		LONG m_orgMouseX, m_orgMouseY;			///< 드래그상태일때 처음 마우스 좌표
		LONG m_currentMouseX, m_currentMouseY;	///< 현재 마우스 좌표
		LONG m_movedMouseX  , m_movedMouseY;	///< 움직인 마우스 좌표
		LONG m_nWheelDelta;						///< 마우스 휠 값
		bool m_bPopAble;
		bool m_bCaptured;
		bool m_bIgnoreMouseMovedPickingRActor;

		void setIgnoreMouseMovedPickingRActor( bool _bSet ) { m_bIgnoreMouseMovedPickingRActor = _bSet; }
		bool getIgnoreMouseMovedPickingRActor()				{ return m_bIgnoreMouseMovedPickingRActor; }

		/** @ brief 마우스 속성 비트 플래그 */
		struct {
			bool bDrag;
			bool bMDrag;
			bool bLButton;
			bool bRButton;
			bool bMButton;
			bool bEvented;
		} mouseState;

	} m_mouseInfo;

	float m_fClipNearZ;
	float m_fClipFarZ;

	//////////////////////////////////////////////////////////////////////////
	//
	// 에러처리
	//
	//////////////////////////////////////////////////////////////////////////
	static string& GetErrorString();
	static void ClearErrorString();
	static void AddErrorString( const char* );

	//////////////////////////////////////////////////////////////////////////
	// 파일 변경 내역 통보 받을 곳
	void		Report(const FileWatcher::StringVec& changedFiles);	///< 파일 변경 통보

private:

	//View Container
	std::vector<CListenableView*>		m_vecListenableViews_;

	//자원
	rs3::RToolCamera*					m_pCurrentCamera_;
	rs3::RDeviceD3D*					m_pCurrentDevice_;
	rs3::RToolSceneManager*				m_pCurrentSceneManager_;
	rs3::REnvironmentSet*				m_pEnvironmentSet_;

	//tool manager / physx
	CToolCmdMgr*						m_pCmdMgr_;
	CWorkSpaceManager*					m_pWorkSpaceMgr_;
	CToolLogicObjectManager*			m_pLogicObjectMgr_;

	// helper
	CControlMediatorHelperResource*		m_pHelperResource_;
	CControlMediatorHelperAction*		m_pHelperAction_;
	CControlMediatorHelperRender*		m_pHelperRender_;
	FileWatcher*						m_pFileWatcher;		///< 변경 파일 감시자

	//워크벤치 정보
	UINT								m_wbWidth_;
	UINT								m_wbHeight_;
	HWND								m_wbhWnd_;
	WORKBENCH_STATE::STATE				m_wbCurrentState_;
	WORKBENCH_STATE::STATE				m_wbPreviousState_;

	bool								m_bNavigation_;
	bool								m_bDeviceInited_;
	EDITMODE							m_nEditMode;

	// WBActionTable
	void								(*WB_ActionTable_[WORKBENCH_STATE::STATE_NUM])(CControlMediator*);

	// timer : 6, 0.006초, 기본적으로 16의 ElapseTime
	DWORD								ELAPSE_IN_TIMER;

	//리소스 관련 메쏘드
	bool								LoadSceneAndEnvFromXML_(const char* _pStrZoneName, MAP_EDIT_TYPE _eMapType);

	/// 마지막 에러스트링
	static string		m_errorString;

	rs3::RDebuggingInfo	m_DebuggingInfo;
};
