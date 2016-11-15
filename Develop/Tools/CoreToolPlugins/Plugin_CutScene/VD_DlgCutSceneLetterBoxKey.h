#pragma once


// CDlgCutSceneLetterBoxKey 대화 상자입니다.

class CDlgCutSceneLetterBoxKey : public CDialog
{
public:
	CDlgCutSceneLetterBoxKey(UINT nIDTemplate, CWnd* pParent = NULL) : CDialog(nIDTemplate, pParent), m_fTime(0.0f) {}   // 표준 생성자입니다.
	virtual ~CDlgCutSceneLetterBoxKey() {}

public:
	float m_fTime;
};

//class CDlgCutSceneLetterBoxStartKey : public CDlgCutSceneLetterBoxKey
//{
//	DECLARE_DYNAMIC(CDlgCutSceneLetterBoxStartKey)
//
//public:
//	CDlgCutSceneLetterBoxStartKey(CWnd* pParent = NULL);   // 표준 생성자입니다.
//	virtual ~CDlgCutSceneLetterBoxStartKey();
//
//// 대화 상자 데이터입니다.
//	enum { IDD = IDD_DIALOG_CUTSCENE_LETTERBOX_START };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//
//	DECLARE_MESSAGE_MAP()
//};
//
//class CDlgCutSceneLetterBoxEndKey : public CDlgCutSceneLetterBoxKey
//{
//	DECLARE_DYNAMIC(CDlgCutSceneLetterBoxEndKey)
//
//public:
//	CDlgCutSceneLetterBoxEndKey(CWnd* pParent = NULL);   // 표준 생성자입니다.
//	virtual ~CDlgCutSceneLetterBoxEndKey();
//
//	// 대화 상자 데이터입니다.
//	enum { IDD = IDD_DIALOG_CUTSCENE_LETTERBOX_END };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//
//	DECLARE_MESSAGE_MAP()
//};
//
