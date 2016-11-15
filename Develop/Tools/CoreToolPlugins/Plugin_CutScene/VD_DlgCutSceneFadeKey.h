#pragma once

#include "MTypes.h"
// CDlgCutSceneFadeKey 대화 상자입니다.
class CControlMediator;
class XEffectInfoMgr;

class CDlgCutSceneFadeKey : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneFadeKey)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CDlgCutSceneFadeKey(CControlMediator* _pControlMediator, float fTime, float fAlpha, CWnd* pParent = NULL);
	virtual ~CDlgCutSceneFadeKey();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUTSCENE_FADEKEY };

	void GetFadeData( float& fTime, float& fAlpha);
public:
	float m_fTime;
	float m_fAlpha;
};
