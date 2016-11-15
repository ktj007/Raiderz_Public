#pragma once

#include "VL_ListenableView.h"
#include <string>
#include <vector>
#include "afxcmn.h"

#include "resource.h"

// CRollupZoneObjectNew 대화 상자입니다.
/** @brief Zone Object New Rollup */
class CORETOOL_API CRollupZoneObjectNew : public CDialog, public CListenableViewImpl_Rollup<CRollupZoneObjectNew>
{
	DECLARE_DYNAMIC(CRollupZoneObjectNew)

public:
							CRollupZoneObjectNew(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual					~CRollupZoneObjectNew();						///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);				///< Listen
	virtual const char*		GetViewName(){ return "새로운 오브젝트"; }		///< View Name

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ZONE_OBJECT_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTcnSelchangeTabFile(NMHDR *pNMHDR, LRESULT *pResult);
	// control
	CTabCtrl m_ctlFileTab;

	class CTreeCtrlNewObject : public CTreeCtrl
	{
	public:
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		CRollupZoneObjectNew* m_pObjectNewWindow;
		CPoint m_margin;
	} m_ctrlTreeNewList_;

	// 현재 선택된 파일/타입
	std::string m_strFullPathName;
	std::string m_strCurrentFile;
	std::string m_strCurrentType;
	MXmlElement *m_pCurrentNodeInfo;

private:
	enum E_CREATE_TYPE
	{
		AS_INSTANCE = 0,
		AS_COPY = 1
	};

	enum E_NODE_TYPE
	{
		SCENE = 0,
		EFFECT,
		TREE,
		ETC,

		TOTAL_NODETYPE
	};
	static const int E_TOTAL_FILE_NODE = ETC;

	// 파일 목록
	std::vector<std::string> m_arrayFile[E_TOTAL_FILE_NODE];
	std::string m_initial[E_TOTAL_FILE_NODE];

	// 워터 파일이름을 다루기위한 멤버
	// 워터 오브젝트는 존 폴더에 존재하며, 파일이름 규칙이 있다.
	std::string m_strPreConcWaterFileName_;
	int m_nSizePreConcForWater_;

	// 현재 선택된 생성타입
	int m_nCreateSceneType;

	HTREEITEM AddToTreeItem(std::map<std::string, HTREEITEM >& mapFile, string& strFullName);
	void UpdateFileList_();
	void UpdateFileListCtrl_(int nIndex);
	afx_msg void OnTvnSelchangedTreeNewList(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void NoneSelect();
	void SelectNewObjectListTab( int nIndex );
	bool IsCreatingAsInstance(){ return m_nCreateSceneType == AS_INSTANCE; }
	void NotifyToNewAction();	///< 선택이 변경되었음을 알린다

public:
	afx_msg void OnBnClickedButtonEtcWaterPlaneCreate();
	afx_msg void OnBnClickedRadioAsInstance();
	afx_msg void OnBnClickedRadioAsCopy();
};
