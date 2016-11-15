#pragma once

#include "VL_ListenableView.h"
#include "C_ControlMediatorHelperAction.h"

#include "afxwin.h"
#include "resource.h"

#include "RSceneNode.h"
#include "RSceneNodeID.h"

#include "V_PropertyGridCtrl.h"
#include "ext_lib/TreeCtrlExm.h"

#include "ReflectiveUI.h"
#include "ReflectivePropertyGridCtrl.h"


////////////////////////////////////////////////////////////
// CObjectPropertyGridCtrl
//
class CObjectPropertyGridCtrl : public CCoreToolPropertyGridCtrl
{
public:
	CObjectPropertyGridCtrl();
	virtual ~CObjectPropertyGridCtrl();

	// set scenenode for property
	void SetSelectedSceneNode( rs3::RSceneNode* _pSelectedNode );
	bool IsFocused() { return m_bFocused; }

	virtual void OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const;
	virtual void OnChangeSelection( CMFCPropertyGridProperty* pNewSel, CMFCPropertyGridProperty* pOldSel );

private:
	rs3::RSceneNode* m_pSelectedSceneNode_;
	bool m_bFocused;
};

/////////////////////////////////////////////////////////////////////
// treeview control / treeview checkbox observer
//
// custom treeview control : mutil selection and check observer & custom draw for color text
class CSceneNodeTreeCtrl : public CTreeCtrlExM 
{
	DECLARE_DYNAMIC(CSceneNodeTreeCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
} ;

////////////////////////////////////////////////////////////
// Special SceneNode Editor
//
class CDlgWaterEditor;

// CEditObjectList 대화 상자입니다.
/** @brief RSceneNode Viewer Dialog */
class CORETOOL_API CDlgSceneNodeList : public CDialog, public CListenableViewImpl_Dialog<CDlgSceneNodeList>
{
	DECLARE_DYNAMIC(CDlgSceneNodeList)

private:
	bool m_bShowList_;
	void RecheckAllSelection_(SCENEOBJECTMAP *pSelectionList);
	void UpdateSelectCount_();

public:
							CDlgSceneNodeList(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual					~CDlgSceneNodeList();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);			///< Listen
	virtual const char*		GetViewName(){ return "SceneNode List"; }	///< View Name

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VL_OBJECT_LIST };

	// 오브제트 선택 및 UI 업데이트
	void UpdateTreeControl();
	void ExpandTreeControlToggle();

	bool IsListableNode( const rs3::RSceneNode *pSceneNode );

	void RemoveSelection(HTREEITEM hItem);
	void UpdatePropertyUI();
	void ObserveSelection(rs3::RSceneNode *pNewSelectionSceneNode, bool _bUpdateUI, bool _bSelectionClear, bool _bReCheckAllSelected = false);
	void SetListLock(bool _bLock);

	//object visible setter / getter
	void ViewStateToggle(const char* _pSzObjectClassName);
	bool GetViewState(const char* _pSzObjectClassName);

	// show toggle
	void ShowToggle();

	// 특수한 씬노드 편집
	CDlgWaterEditor*			GetWaterEditor(){ return m_pEditWater_; }

	void						ShowWaterEditor();
	
	// activation check for hot key
	bool HasActiveSceneEditor();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	// property dialog
	CObjectPropertyGridCtrl m_ctlObjProperty_;

	CSceneNodeTreeCtrl	m_ctlObjectTree;
	class CTreeCheckBoxObserver : public CTreeCtrlExMCheckBoxObserver
	{
	public:
		CTreeCheckBoxObserver() : m_pEditObjectList(NULL) {}
		virtual ~CTreeCheckBoxObserver(){}

		virtual void ObserveCheckBox( HTREEITEM hClickedItem );
		void UpdateCheckBox(bool _bWillCheck, HTREEITEM hTreeItem );

		void Process(rs3::RSceneNode* pSceneNode, bool _bWillCheck );

		CDlgSceneNodeList* m_pEditObjectList;
	} m_treeCheckBoxObserver;
	friend class CTreeCheckBoxObserver;

	typedef map< std::string, const rs3::RSceneNode* > MAP_NAME_SCENENODE;
	typedef map<const char*, MAP_NAME_SCENENODE > NODEMAPBYCLASS;
	NODEMAPBYCLASS	m_nodesByClass;
	
// 	typedef set< rs3::RSceneNode* >	SET_SCENENODE;
// 	SET_SCENENODE	m_setSelectedNodes;

	// 클래스 이름, 트리아이템, 열림여부
	typedef map< const char*,  std::pair<HTREEITEM, bool> > SCENE_CLASS_HITEM;
	SCENE_CLASS_HITEM m_sceneClassHItem;

	void UpdateTreeViewByClass(const rs3::RSceneNode *pNode);
	void UpdateTreeView(HTREEITEM hTreeItem, const rs3::RSceneNode *pNode);
	HTREEITEM SetSelectNode_(HTREEITEM hTreeItem, rs3::RSceneNode *pSceneNode, bool bSelect = true );

	int GetImageIndex( const rs3::RSceneNode* pNode );

	// 계층구조 펼쳐보기 / 모아보기
	BOOL m_bHierachyView;

	////////////////////////////////////////////////////////////
	// Special SceneNode Editor
	//
	CDlgWaterEditor*				m_pEditWater_;				// water editor

	// message map method
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckHierachyView();
	afx_msg void OnTvnItemexpandedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);


	CReflectiveUI					m_UI;
	CReflectivePropertyGridCtrl*	m_pPropCtrl;

public:
	afx_msg void OnBnClickedButtonExpandToggle();
	int m_nSelectedObjectCount;
	afx_msg void OnEnChangeEditObjectSelectCount();
	afx_msg void OnBnClickedButtonPlayEffect();
};
