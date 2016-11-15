#pragma once

#include "Mint4R3.h"
#include "MWidget.h"
#include "MTextView.h"
#include "MPanel.h"
#include "MLabel.h"

class CControlMediator;
class CQuadPopupTrigger
{
public:
	CQuadPopupTrigger(){}
	virtual ~CQuadPopupTrigger(){}

	virtual bool GetCheck() = 0;
	virtual const char* GetTriggerName() = 0;
	virtual void Trigger() = 0;
};

class CORETOOL_API CMintManager
{
private:
	MFont* CheckFont( const char* chFontAliasName, const char* chFontName, const char* chFileName, int nSize, bool bBold, bool bItalic );

public:
	void InitMintManager(CControlMediator* _pControlMediator);
	void DestroyMintManager();

	inline static CMintManager& GetReference()
	{
		static CMintManager popup;
		return popup;
	}

	void ListenEvent( MEvent& _refEvent );
	void RenderMint();

	// mint workspace
	void SetWorkspaceSize(int w, int h);

	// quad popup
	void ActivateQuadPopup(bool _bActive, const CPoint* _pPoint = NULL);
	bool IsActiveQuadPopup(){ return m_bQuadPopupActive_; }

	// caption TextView
	void ActiveCaptionTextView(bool _bActive);
	void SetCaptionTextView(const char* pCaption, const char* chFontAliasName, const char* chFontName, int nSize, bool bBold = false, bool bItalic = false);

	// letterbox Paint
	void SetLetterBoxPaint(bool bShow, float fRatioW, float fRatioH);

	// Notice Label
	void ShowNoticeLabel(bool bShow);

	void FadeDraw( float fAlpha );
	void FadeEnd();

private:
	static const int STANDARD_SCREEN_WIDTH  = 1024;
	static const int STANDARD_SCREEN_HEIGHT = 768;

	static const int QUAD_TITLE_H = 15;
	static const int QUAD_ITEM_H = 18;

	CMintManager();
	~CMintManager();

	CControlMediator* m_pControlMediator_;

	// mint
	Mint4R3 m_mint_;
	MFontR3* m_pGulimMFont_;
	MFontR3* m_pArialMFont_;
	MDrawContextR3* m_pMintDC_;

	// quad popup widget
	class QuadPopup_ : public MWidget
	{
	private:
		virtual void OnDraw(MDrawContext* pDC);
		virtual void OnClick(MEvent* pEvent);
		CMintManager* m_pToolMintMgr_;

	public:
		QuadPopup_(const char* szName, MWidget* pParent, MListener* pListener, CMintManager* _pMintMgr)
			: MWidget(szName, pParent, pListener), m_pToolMintMgr_( _pMintMgr ), m_bMapLoaded(false){}
		virtual ~QuadPopup_(void){}
		bool m_bMapLoaded;
		POINT m_currentMousePoint_;

	} *m_pQuadPopup_;
	friend class QuadPopup_;
	bool m_bQuadPopupActive_;

	typedef std::vector< std::pair< MRECT, CQuadPopupTrigger* > > RECT_TRIGGER;
	RECT_TRIGGER m_quadPopupTable_[4];
	CQuadPopupTrigger* m_pCurrentTrigger_;

	typedef std::pair< std::pair< MRECT, std::string >, std::pair<MRECT, int> > RECT_DECO;
	RECT_DECO m_quadDecoration_[4];


	//////////////////////////////////////////////////////////////////////////
	// 컷신용 위젯
	// 자막
	bool		m_bCaptionActive;
	MTextView *	m_pCaptionWidget;

	// 레터박스
	bool		m_bLetterBoxActive;
	MPanel *	m_pLetterBoxTop;
	MPanel *	m_pLetterBoxBottom;

	// 알림
	bool		m_bNoticeActive;
	MLabel *	m_pNoticeLabel;

	MPanel *	m_pFadeWidget;
};
