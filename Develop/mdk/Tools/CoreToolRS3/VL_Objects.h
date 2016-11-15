#pragma once

#include "VL_ListenableView.h"
#include "afxwin.h"
#include "Resource.h"
#include <vector>
#include "ReflectiveUI.h"
#include "V_PropertyGridCtrl.h"
#include "ext_lib/GridTree/GridCtrl_src/GridCtrl.h"
#include "ext_lib/GridTree/TreeColumn_src/TreeColumn.h"
#include "ext_lib/GridTree/GridBtnCell_src/BtnDataBase.h"

enum GUNZ_ENTITY;
class CBaseObject;
class CLogicObject;
class GEntity;
class GunzLogicObject;

////////////////////////////////////////////////////////////
// CObjectPropertyGridCtrl
//

class LayerGridCtrl : public CGridCtrl
{
public:
	LayerGridCtrl();
	virtual ~LayerGridCtrl();

	bool	Initialize(CControlMediator* pController);

	//////////////////////////////////////////////////////////////////////////
	// overrides for custom drag & drop support
	void CutSelectedText() override;
	COleDataSource* CopyTextFromGrid() override;
	BOOL PasteTextToGrid(CCellID cell, COleDataObject* pDataObject) override;

	CLogicObject*	GetLogicObject( int nRow );
	const char*		GetName( int nRow );

	void SetVerticalScroll( int nRow );

	using CGridCtrl::GetVisibleNonFixedCellRange;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) override;

	UINT							m_cfData;			///< clipboard format
	CGridDropTarget*				m_gridDropTarget;	///< drag&drop support
	CControlMediator*				m_pController;
};

// CRollupObjects 대화 상자입니다.
class CORETOOL_API CDlgObjects : public CDialog, public CListenableViewImpl_Dialog<CDlgObjects>
{
	DECLARE_DYNAMIC(CDlgObjects)

public:
	CDlgObjects(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgObjects();

	// Listenable View
	virtual void		Listen(CoreMessage& _message);				///< Listen
	virtual const char*	GetViewName(){ return "Logic Objects"; }	///< View Name

	void				ResetLayers();
	void				RefreshObjectList();
	void				RefreshProperty();

	const char*			GetDefaultLayerName();

	void				SetSelectNone();
	bool				SetSelectObject(CLogicObject* _pLogicObject, BOOL _bFlag);

	void				ShowToggle();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VL_LAYER_OBJECTS };

protected:
	typedef map< string, LOGIC_OBJECTLIST > LAYER_OBJECTS_MAP;

	CReflectiveUI					m_UI;			///< reflective ui
	CReflectivePropertyGridCtrl*	m_pPropCtrl;	///< property grid ctrl

	LAYER_OBJECTS_MAP				m_mapLayerObjects;	///< layer-object container
	string							m_strDefaultLayer;	///< default layer
	int								m_nContextMenuRow;	///< row index for context menu

	LayerGridCtrl					m_gridCtrl;
	CTreeColumn						m_treeColumn;
	CBtnDataBase					m_BtnDataBase;	///< grid with some cells with buttons / controls

	bool							m_bShowList_;
	string							m_strBeforeEdit; ///< string before edit

	void				OnPropertyChanged();

	// grid helper
	void				SetUpGridHeader();
	void				SetUpGridRowAsObject( int nRow, CLogicObject* pObject );
	void				SetUpGridRowAsLayer( int nRow, const char* szName);
	void				SetUpGridRow( int nRow, const char* szName, bool bLayer, CLogicObject* pLogicObject );

	bool				GetButtonCheckState(int nRow, int nCol );
	void				SetButtonCheckState(int nRow, int nCol, bool bCheck );

	// manage layer
	const char*			GetUniqueLayerName();

	void				OnLogicObjectChanged( CLogicObject* pLogicObject );
	void				SetVisible( CLogicObject* pLogicObject, bool bHide );
	void				HideLayer( const char* szLayerName, bool bHide );
	void				FreezeObject( CLogicObject* pLogicObject, bool bHide );
	void				FreezeLayer( const char* szLayerName, bool bHide );

	void				SelectedMoveLayerTo( const char* szLayerName );
	void				RedrawLayerRows();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnUpdateCmdUI(CCmdUI* pCmdUI); ///< 툴바의 컨트롤을 활성화 하기위한 함수
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClk();
	afx_msg void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedButtonNewLayer();
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLayerSetAsDefaultLayer();
};
