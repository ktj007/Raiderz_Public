#pragma once
#include "VL_ListenableView.h"
#include <vector>
#include <string>
#include "afxwin.h"

// CDlgMapNew 대화 상자입니다.
/** @brief "Map New" Dialog */
class CDlgMapNew : public CDialog, public CListenableViewImpl_Dialog<CDlgMapNew>
{
	DECLARE_DYNAMIC(CDlgMapNew)

public:
							CDlgMapNew(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual					~CDlgMapNew();							///< 소멸자

	// Listenable View
	virtual void			Listen(CoreMessage& _message);		///< Listen
	virtual const char*		GetViewName(){ return "Map New"; }		///< View Name

	// Dialog
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	// Map Type
	enum MAP_NEW_TYPE { ZONE = 0, FIELD, CUTSCENE };
	void SetNewType(MAP_NEW_TYPE _type){ m_enumNewType_ = _type; }

	const bool IsActive() const
	{
		return m_bActive_;
	}

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VL_MAP_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	bool m_bActive_;
	MAP_NEW_TYPE m_enumNewType_;

	void EnableTemplateCtrl_(UINT _bool);
	void EnableImportSceneCtrl_(UINT _bool);
	void EnableZone_(UINT _bool);
	void NewZoneType_(BOOL _bool);

	void UpdateListBox_(int nIndex);
	CListBox m_ctlListBox_;
	std::vector<std::string> m_listBoxArray_[2];
	int m_nZoneType_;
	int m_nSizeType_;
	int m_nDetailType_;
	int m_nFieldType_;

	CString m_strNewX_;
	CString m_strNewY_;
	CString m_strNewXQ_;
	CString m_strNewYQ_;
	CString m_strNewBaseTile_;
	CString m_strImportSceneFile_;
	CTabCtrl m_ctlFieldTab_;

	afx_msg void OnBnClickedRadioTemplate();
	afx_msg void OnBnClickedRadioImportScene();
	afx_msg void OnBnClickedRadio600();
	afx_msg void OnBnClickedRadio800();
	afx_msg void OnBnClickedRadioCustom();
	afx_msg void OnBnClickedRadio256();
	afx_msg void OnBnClickedRadio512();
	afx_msg void OnBnClickedRadioCustom2();
	afx_msg void OnBnClickedButtonImportSceneFind();
	afx_msg void OnBnClickedButtonBaseTileFind();
	afx_msg void OnBnClickedZoneOk();
	afx_msg void OnBnClickedZoneCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedNewZone();
	afx_msg void OnBnClickedFromZone();
	afx_msg void OnBnClickedFromField();
	//afx_msg void OnBnClickedButtonZoneManager();
	afx_msg void OnBnClickedButtonFieldListEditor();
	afx_msg void OnCbnDropdownComboZoneId();
	afx_msg void OnCbnDropdownComboFieldId();

private:
	CComboBox m_zoneComboBox_;
	CComboBox m_fieldComboBox_;
	void PutListStringFromComboBox_(std::vector<std::string>& _refVec, CComboBox& _refCombo);
	void PutNewZoneInfoForCreation_();
	bool IsCreateAtLoaded_();
	bool IsValidNewMapInfo_();
	bool IsValidNewZone_();
	void SetToolAfterCreationMap_(MAP_NEW_TYPE _type);

	float m_mapHeight_, m_mapWidth_;
	int   m_colFace_, m_rowFace_;
	int   m_nZoneID_, m_nFromFieldID_;
	std::string m_strFieldName_;
	std::string m_strZonePath_;
	std::string m_strZoneFile_;
	std::string m_strMapBaseTile_;
	//자료들의 data 폴더
	std::string m_strDataPath_;
};
