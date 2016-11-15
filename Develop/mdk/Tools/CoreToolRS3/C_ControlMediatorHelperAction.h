#pragma once

#include "RSceneNode.h"

#include <list>
#include <string>
#include <map>

#include "CoreTypes.h"

class CLogicObject;
class CEntityObject;
class CControlMediator;

class CORETOOL_API CControlMediatorHelperEntityAction
{
public:
	CControlMediatorHelperEntityAction(){}
	virtual ~CControlMediatorHelperEntityAction(){}

	virtual bool OnCheckAddSelection(CEntityObject* _pEntityObject, bool _bUpdateUI) = 0;

	virtual void OnRemoveSelection(CEntityObject* _pEntityObject, bool _bUpdateUI) = 0;
	virtual void OnRemoveSelectionAll(bool _bUpdateUI) = 0;

	virtual void OnDeleteSelectedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList) = 0;
	virtual void OnPasteCopiedObject(ENTITY_OBJECTLIST& _list, ENTITY_OBJECTLIST& _copyList) = 0;

	// TODO: 결국 logic object 로 일반화 되어야 하지 않을까 
	// CControlMediatorHelperEntityAction -> CControlMediatorHelperLogicObjectAction
	virtual void OnDeleteSelectedObject(LOGIC_OBJECTLIST& _list, LOGIC_OBJECTLIST& _copyList) {}

	virtual void OnAddToLogicObjectManager(CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx) = 0;
	virtual void OnRemoveFromLogicObjectManager(CEntityObject* _pEntityObject, int& _nCnt, int& _nIdx) = 0;

	virtual void OnGetNavigationDirection(CEntityObject* _pEntityObject, const rs3::RVector& _vPos, rs3::RVector& _outDir) = 0;
	virtual void OnChagnedEnvWeather(const char* _pSzName) = 0;
	virtual void OnToolBarNewEntityClicked() = 0;
	virtual void OnToolBarEntityOnTerrainClicked() = 0;
	virtual void OnToolBarSelectionLocked(bool _bLock) = 0;
};

class CORETOOL_API CControlMediatorHelperAction
{
	friend class CControlMediator;
	friend class CToolLogicObjectManager;

	CControlMediatorHelperAction(void);
	~CControlMediatorHelperAction(void);

	void InitHelperAction(CControlMediator* _pMediator);
	void ResetHelperAction();

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 피킹관련
	//
	//////////////////////////////////////////////////////////////////////////

	/** @brief 마우스와 카메라를 연결하는 직선상의 두 벡터 */
	bool GetMouseLine(LONG sx, LONG sy, rs3::RVector& origin, rs3::RVector& to) const;

	/** @brief 마우스와 카메라를 연결하는 직선상의 두 벡터 */
	void GetCurrentMouseLine( const rs3::RVector** ppOrigin, const rs3::RVector** ppTo) const
	{
		if(m_bValidMouseLine_)
		{
			*ppOrigin = &m_vecMouseLineOrg_;
			*ppTo = &m_vecMouseLineTo_;
		}
		else
		{
			*ppOrigin = NULL;
			*ppTo = NULL;
		}
	}

	/** @brief 마우스 라인과 지형 충돌점 설정
		마우스가 움직일때 마다 불리는 함수
		일단 마우스 라인을 구하고 지형 충돌점이 정해져야만 참을 리턴한다.
	*/
	bool SetMouseLineAndPickingPosition();
	void PickingCameraPlane( rs3::RVector& _vTerrainContactPos, rs3::RVector& _vMapPickPos, bool& _bRet );

	/** @brief 마우스 왼쪽버튼 눌릴때 */
	void OnLButtonDown();

	/** @brief 마우스 왼쪽버튼 떨어질때 */
	void OnLButtonUp();

	/** @brief 마우스 좌표와 지형의 충돌점 얻기 */
	inline const rs3::RVector* GetTerrainContactPos() const
	{
		return m_pVecTerrainContactPos_;
	}
	/** @brief 마우스 좌표의 충돌점 얻기 */
	inline const rs3::RVector* GetCurrentPickingPosition() const
	{
		return m_pVecCurrentPickingPosition_;
	}

	/** @brief Action Plane과 최초의 Action Plane과의 충돌점 설정
		Mouse Command Entry 일때만 불려진다.
		MouseCmdEntry 를 확인해서 참이라면, 이 함수가 수행되었다고 볼 수 있다.
		Action Plane이란, 액션 컨트롤에서 액션을 수행할때 필요한 평면이다.
	*/
	void					SetActionPlaneAndContactPos();
	const rs3::RPlane&		GetActionPlane() const { return m_actionPlane_; }
	void					SetActionPlanePosAs( const rs3::RVector& vecPosition );
	const rs3::RVector&		GetActionPlanePos() const { return m_vecContactActionPlanePos_; }

	/// 선택된 오브젝트들 업데이트
	void					UpdateSelectedObjects();

	//////////////////////////////////////////////////////////////////////////
	//
	// Cursor / Navigation / Pause
	//
	//////////////////////////////////////////////////////////////////////////
	
	void					SetCursorToScreenCenter();						///< Cusror를 중앙으로 옮긴다. FPS모드에서 필요
	void					GetMovedCursorPosFromCenter(int& _x, int& _y);	///< 중앙으로부터 움직인 커서 위치
	void					SetNavigationModeToggle(bool _bXYNavi = false);	///< Navigation Mode Toggle
	inline bool				IsPause(){ return m_bPause_; }					///< Pause 여부
	void					SetPauseToggle();								///< Pause Toggle


	//////////////////////////////////////////////////////////////////////////
	//
	// 물체 선택과 관련된 액션
	//
	//////////////////////////////////////////////////////////////////////////

	void					AddSelectection( CLogicObject* _pLogicObject, bool _bUpdateUI);
	void					RemoveSelection( CLogicObject* _pLogicObject, bool _bUpdateUI);
	void					RemoveAllSelection(bool _bUpdateUI);

	const SCENEOBJECTMAP*	GetSelectedSceneObject()	{ return &m_listSelectedSceneObject_; }
	SCENEOBJECTMAP*			GetSelectedSceneObjectPtr()	{ return &m_listSelectedSceneObject_; }
	ENTITY_OBJECTLIST*		GetSelectedEntityObject()	{ return &m_listSelectedEntityObject_; }
	LOGIC_OBJECTLIST*		GetSelectedLogicObject()	{ return &m_listSelectedLogicObject_; }
	LOGIC_OBJECTLIST*		GetSelectedLogicObjectToCopy()	{ return &m_listSelectedLogicObjectToCopy_; }
	void					GetWBObjectAtCurrentMouse(rs3::RSceneNode** _ppSceneNode, CLogicObject** _ppLogicObject) const;

	void					SetSelectedObjectCenterAndActionFrame();
	const rs3::RMatrix&		GetSelectedObjectActionFrame(){ return m_matSelectedObjectActionFrame_; }
	rs3::RVector*			GetSelectNodeCenter();		///< 선택된 노드들의 center ( scenenode / logic object 포함 )
	rs3::RVector*			GetSelectSceneNodeCenter();
	bool					IsInSelectObjectList( const CLogicObject* _pLogicObject);
	bool					GetSelectedObjectAABBCenter( rs3::RVector& _refOut, rs3::RBoundingBox* _pGetAABB, bool _bPickedPriority = false, const rs3::RVector* _p = NULL, const rs3::RVector* _q = NULL );

	void					SetSelectedLogicObjectCntForDoAction(int nCnt){ m_nSelectedLogicObjCntForDoAction = nCnt; }
	int						GetSelectedLogicObjectCntForDoAction(){ return m_nSelectedLogicObjCntForDoAction; }

	//////////////////////////////////////////////////////////////////////////
	//
	// 물체 Hide / Freeze
	//
	//////////////////////////////////////////////////////////////////////////
	void					SetVisible( CLogicObject* pLogicObject, bool bVisible );
	void					SetEditable( CLogicObject* pLogicObject, bool bEditable );

	//////////////////////////////////////////////////////////////////////////
	//
	// 물체 삭제 / 복사 / 붙여넣기
	//
	//////////////////////////////////////////////////////////////////////////

	void					DeleteSelectedObject();
	void					CopyShallowSelectedEntityObject();
	void					PasteCopiedEntityObject();
//	bool					CopyDeepSelectedZoneObject();
	bool					CopyDeepSelectedObject();


	//////////////////////////////////////////////////////////////////////////
	//
	// WorkBench Mouse Action Observer
	//
	//////////////////////////////////////////////////////////////////////////

	enum					WB_MOUSE_ACTION { WB_MOUSE_L_DOWN = 0, WB_MOUSE_L_UP, WB_MOUSE_ACTION_NUM };
	void					AddToWBMouseObserver( WB_MOUSE_ACTION _eAction, int _nId ) { m_vViewIDToObserveMouseAction_[_eAction].push_back( _nId ); }
	void					NotifyWBMouseAction( WB_MOUSE_ACTION _eAction );


	//////////////////////////////////////////////////////////////////////////
	//
	// Entity Action 알리기
	//
	//////////////////////////////////////////////////////////////////////////

	void					NotifyNewEntityToolBarButtonClicked();
	void					NotifyEntityOnTerrainToolBarButtonClicked();
	void					NotifySelectionLockToolBarButtonClicked(bool _bLock);
	
private:
	CControlMediator*		m_pControlMediator_;
	bool					m_bPause_;

	//select node 선택된 Object List
	//for SceneNode
	SCENEOBJECTMAP			m_listSelectedSceneObject_;
	rs3::RVector			m_vSelectNodeSceneCenter_;

	// for EntityObject, LogicObject
	// TODO : 엔티티 오브젝트가 로직 오브젝트로 합쳐져야한다.
	ENTITY_OBJECTLIST		m_listSelectedEntityObject_;
	ENTITY_OBJECTLIST		m_listSelectedEntityToCopy_;
	rs3::RVector			m_vSelectNodeCenter_;
	rs3::RMatrix			m_matSelectedObjectActionFrame_;
	LOGIC_OBJECTLIST		m_listSelectedLogicObject_;
	LOGIC_OBJECTLIST		m_listSelectedLogicObjectToCopy_;

	// mouse up observer
	std::vector< int >		m_vViewIDToObserveMouseAction_[ WB_MOUSE_ACTION_NUM ];

	rs3::RVector*			m_pVecTerrainContactPos_;
	rs3::RVector*			m_pVecCurrentPickingPosition_;

	rs3::RVector			m_vecContactActionPlanePos_;
	rs3::RPlane				m_actionPlane_;

	//Current MouseRay
	rs3::RVector			m_vecMouseLineOrg_;
	rs3::RVector			m_vecMouseLineTo_;
	bool					m_bValidMouseLine_;

	UINT					m_nOldAxis;	/// gizmo 선택 축 해제시 복구할 툴바의 축 선택
	int						m_nSelectedLogicObjCntForDoAction;
};
//----------------------------------------------------------------------------------------------------
inline rs3::RVector* CControlMediatorHelperAction::GetSelectSceneNodeCenter()
{
	if(m_listSelectedSceneObject_.empty())
		return NULL;
	else
		return &m_vSelectNodeSceneCenter_;
}
//----------------------------------------------------------------------------------------------------