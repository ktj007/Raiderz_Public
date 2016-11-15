#include "stdafx.h"
#include "C_MintManager.h"
#include "C_ControlCursor.h"

CMintManager::CMintManager() :
	m_pQuadPopup_(NULL),
	m_bQuadPopupActive_(false),
	m_pCurrentTrigger_(NULL),
	m_pGulimMFont_(NULL), m_pArialMFont_(NULL), m_pMintDC_(NULL),
	m_pCaptionWidget(NULL), m_bCaptionActive(false),
	m_pLetterBoxTop(NULL), m_pLetterBoxBottom(NULL), m_bLetterBoxActive(false),
	m_pNoticeLabel(NULL), m_bNoticeActive(false)
	, m_pFadeWidget(NULL)
{
}

CMintManager::~CMintManager()
{
}

void CMintManager::DestroyMintManager()
{
	m_mint_.Finalize();

	// mint 가 지워주는데
	m_pQuadPopup_ = NULL;

	SAFE_DELETE(m_pMintDC_);
	SAFE_DELETE(m_pGulimMFont_);
	SAFE_DELETE(m_pArialMFont_);

	// caption
	m_pCaptionWidget = NULL;

	// 레터박스
	m_pLetterBoxTop = NULL;
	m_pLetterBoxBottom = NULL;

	// 알림
	m_pNoticeLabel = NULL;

	// fade
	m_pFadeWidget = NULL;

	MFontManager::Destroy();
	MBitmapManager::Destroy();
}

void CMintManager::InitMintManager(CControlMediator* _pControlMediator)
{
	m_pControlMediator_ = _pControlMediator;

	rs3::RDeviceD3D* pDevice = m_pControlMediator_->GetCurrentDevice();
	int width, height; m_pControlMediator_->GetWBSize( (LONG&)width, (LONG&)height );

	m_pGulimMFont_ = new MFontR3;
	if( !m_pGulimMFont_->Create(pDevice, "Default", "굴림체", 12, 1.0f) )
	{
		m_pGulimMFont_->Destroy();
		SAFE_DELETE( m_pGulimMFont_ );
		AfxMessageBox("민트 굴림폰트 초기화 실패");
	}

	m_pArialMFont_ = new MFontR3;
	if( !m_pArialMFont_->Create(pDevice, "Arial", "Arial", 13, 1.0f) )
	{
		m_pArialMFont_->Destroy();
		SAFE_DELETE( m_pArialMFont_ );
		AfxMessageBox("민트 Arial 폰트 초기화 실패");
	}

	m_pMintDC_ = new MDrawContextR3(pDevice);
	m_mint_.Initialize(
		width, height, STANDARD_SCREEN_WIDTH, STANDARD_SCREEN_HEIGHT,
		m_pMintDC_, m_pGulimMFont_, NULL );

	MEvent::SetIMESupport(true);	// ime 설정을 따로 해줘야 한글이 써진다.
	m_mint_.SetHWND(m_pControlMediator_->GetWBWindowHandle());

	// 생성된 민트 위젯은 민트단에서 처리해준다.
	MWidget* pMainFrameWidget = (MWidget*)(m_mint_.GetMainFrame());
	MRECT rtScreen = pMainFrameWidget->GetScreenRect();	
	
	if( m_pQuadPopup_ == NULL)
	{
		m_pQuadPopup_ = new QuadPopup_("QuadPopup", pMainFrameWidget, pMainFrameWidget, this);
		m_pQuadPopup_->Show(false);
	}

	// 레터 박스 위젯
	if(m_pLetterBoxTop == NULL)
	{
		m_pLetterBoxTop = (MPanel*)m_mint_.NewWidget(MINT_PANEL, "letterbox_top", pMainFrameWidget, NULL);
		m_pLetterBoxTop->Show(false);
	}

	if(m_pLetterBoxBottom == NULL)
	{
		m_pLetterBoxBottom = (MPanel*)m_mint_.NewWidget(MINT_PANEL, "letterbox_bottom", pMainFrameWidget, NULL);
		m_pLetterBoxBottom->Show(false);
	}

	// 자막 위젯
	if(m_pCaptionWidget == NULL)
	{
		m_pCaptionWidget = (MTextView*)m_mint_.NewWidget(MINT_TEXTVIEW, "caption", pMainFrameWidget, NULL);
		m_pCaptionWidget->Show(false);
		m_pCaptionWidget->SetPosition(0, rtScreen.h - 100);
		m_pCaptionWidget->SetSize(rtScreen.w, 50);
	}

	if(m_pNoticeLabel == NULL)
	{
		m_pNoticeLabel = (MLabel*)m_mint_.NewWidget(MINT_LABEL, "noticeLabel", pMainFrameWidget, NULL);
		m_pNoticeLabel->Show(false);
		m_pNoticeLabel->SetSize(rtScreen.w, 100);
		MFont* pFont = CheckFont("fnt_DefUI", "다음_Regular", "../../data/interface/font/Daum_Regular.ttf", 20, true, false);
		m_pNoticeLabel->SetFont(pFont);
		m_pNoticeLabel->SetText("본 화면은 실제 게임과 다를 수 있습니다.");
		m_pNoticeLabel->SetTextColor(MCOLOR(255, 229, 0, 255));
	}

	if (m_pFadeWidget == NULL)
	{
		m_pFadeWidget = (MPanel*)m_mint_.NewWidget(MINT_PANEL, "fade", pMainFrameWidget, NULL);
		m_pFadeWidget->Show(false);
		m_pFadeWidget->SetPosition(0, 0);
		m_pFadeWidget->SetSize(rtScreen.w, rtScreen.h);
	}
}

void CMintManager::ActivateQuadPopup(bool _bActive, const CPoint* _pPoint)
{
	m_bQuadPopupActive_ = _bActive;
	if( m_bQuadPopupActive_ && _pPoint)
	{
		// 쿼드 메뉴가 활성화 되었을때 커서 초기화
		L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);

		int width, height; m_pControlMediator_->GetWBSize( (LONG&)width, (LONG&)height );
		m_pQuadPopup_->SetBounds(0, 0, width, height);
		m_pQuadPopup_->SetPosition(0, 0);
		if( m_pQuadPopup_->m_bMapLoaded )
		{
			// 4개의 메뉴에 대해
			for(int i = 0; i < 4; ++i)
			{
				// 타이틀이 비어있으면 가능한 쿼드 메뉴가 아니다.
				if(m_quadDecoration_[i].first.second.empty()) continue;

				int item_num = (int)m_quadPopupTable_[i].size();
				int item_w = m_quadDecoration_[i].second.second;
				int start_x, start_y;

				if( i == 0 )
				{
					start_x = _pPoint->x - item_w + 1;
					start_y = _pPoint->y - ( item_num * QUAD_ITEM_H + QUAD_TITLE_H );

					// item total rect
					m_quadDecoration_[i].second.first.x = start_x;
					m_quadDecoration_[i].second.first.y = start_y;
					m_quadDecoration_[i].second.first.w = item_w;
					m_quadDecoration_[i].second.first.h = _pPoint->y - start_y;
					
					// title rect
					m_quadDecoration_[i].first.first.x = start_x;
					m_quadDecoration_[i].first.first.y = _pPoint->y - QUAD_TITLE_H;
					m_quadDecoration_[i].first.first.w = item_w;
					m_quadDecoration_[i].first.first.h = QUAD_TITLE_H;
				}
				else if( i == 1 )
				{
					start_x = _pPoint->x;
					start_y = _pPoint->y - ( item_num * QUAD_ITEM_H + QUAD_TITLE_H );

					// item total rect
					m_quadDecoration_[i].second.first.x = start_x;
					m_quadDecoration_[i].second.first.y = start_y;
					m_quadDecoration_[i].second.first.w = item_w;
					m_quadDecoration_[i].second.first.h = _pPoint->y - start_y;
					
					// title rect
					m_quadDecoration_[i].first.first.x = start_x;
					m_quadDecoration_[i].first.first.y = _pPoint->y - QUAD_TITLE_H;
					m_quadDecoration_[i].first.first.w = item_w;
					m_quadDecoration_[i].first.first.h = QUAD_TITLE_H;
				}
				else if( i == 2 )
				{
					start_x = _pPoint->x - item_w + 1;
					start_y = _pPoint->y + QUAD_TITLE_H;

					// item total rect
					m_quadDecoration_[i].second.first.x = start_x;
					m_quadDecoration_[i].second.first.y = _pPoint->y;
					m_quadDecoration_[i].second.first.w = item_w;
					m_quadDecoration_[i].second.first.h = item_num * QUAD_ITEM_H + QUAD_TITLE_H;
					
					// title rect
					m_quadDecoration_[i].first.first.x = start_x;
					m_quadDecoration_[i].first.first.y = _pPoint->y;
					m_quadDecoration_[i].first.first.w = item_w;
					m_quadDecoration_[i].first.first.h = QUAD_TITLE_H;
				}
				else if( i == 3 )
				{
					start_x = _pPoint->x;
					start_y = _pPoint->y + QUAD_TITLE_H;

					// item total rect
					m_quadDecoration_[i].second.first.x = start_x;
					m_quadDecoration_[i].second.first.y = _pPoint->y;
					m_quadDecoration_[i].second.first.w = item_w;
					m_quadDecoration_[i].second.first.h = item_num * QUAD_ITEM_H + QUAD_TITLE_H;
					
					// title rect
					m_quadDecoration_[i].first.first.x = start_x;
					m_quadDecoration_[i].first.first.y = _pPoint->y;
					m_quadDecoration_[i].first.first.w = item_w;
					m_quadDecoration_[i].first.first.h = QUAD_TITLE_H;
				}

				// Quad Trigger Rect
				int nOffsetIndex = 0;
				for(RECT_TRIGGER::iterator itr = m_quadPopupTable_[i].begin();
					itr != m_quadPopupTable_[i].end(); ++itr, ++nOffsetIndex)
				{
					itr->first.x = start_x;
					itr->first.y = start_y + nOffsetIndex * QUAD_ITEM_H;
					itr->first.h = QUAD_ITEM_H;
					itr->first.w = item_w;
				}
			}
		}
		else
		{
			// title
			m_quadDecoration_[0].first.first.h = QUAD_TITLE_H;
			m_quadDecoration_[0].first.first.w = 120;
			m_quadDecoration_[0].first.first.x = _pPoint->x;
			m_quadDecoration_[0].first.first.y = _pPoint->y;

			// item
			m_quadDecoration_[0].second.first.h = QUAD_ITEM_H;
			m_quadDecoration_[0].second.first.w = 120;
			m_quadDecoration_[0].second.first.x = _pPoint->x;
			m_quadDecoration_[0].second.first.y = _pPoint->y + QUAD_TITLE_H;
		}
	}
	// 비활성화 되었을때
	else
	{
		m_pCurrentTrigger_ = NULL;

		// mouse state 초기화
		m_pControlMediator_->m_mouseInfo.mouseState.bDrag    = false;
		m_pControlMediator_->m_mouseInfo.mouseState.bEvented = false;
		m_pControlMediator_->m_mouseInfo.mouseState.bLButton = false;
		m_pControlMediator_->m_mouseInfo.mouseState.bMButton = false;
		m_pControlMediator_->m_mouseInfo.mouseState.bMDrag   = false;
		m_pControlMediator_->m_mouseInfo.mouseState.bRButton = false;
	}
	m_pQuadPopup_->Show(m_bQuadPopupActive_);
}

void CMintManager::RenderMint()
{
	// 쿼드 팝업 패인
	if(m_bQuadPopupActive_ ||
		m_bCaptionActive ||
		m_bLetterBoxActive ||
		m_bNoticeActive)
		m_mint_.Draw();
}

void CMintManager::ListenEvent( MEvent& _refEvent )
{
	m_mint_.ProcessEvent( _refEvent );
}

void CMintManager::QuadPopup_::OnDraw(MDrawContext* pDC)
{

#define FILL_1_PIX_SMALL( _rect, _type ) { \
	if( (_type) == 0 || (_type) == 1 ) { \
		pDC->FillRectangle( (_rect).x + 1, (_rect).y + 1, (_rect).w - 2, (_rect).h - 1 ); \
	} \
	else { \
		pDC->FillRectangle( (_rect).x + 1, (_rect).y, (_rect).w - 2, (_rect).h - 1 ); \
	}\
}
	::GetCursorPos( &m_currentMousePoint_ );
	::ScreenToClient(m_pToolMintMgr_->m_pControlMediator_->GetWBWindowHandle(), &m_currentMousePoint_);
	
	if( m_bMapLoaded )
	{
		/* 쿼드 팝업 인덱스
			0 1
			2 3
		*/
		bool bChecked = false;
		for( int i = 0; i < 4; ++i)
		{
			if( m_pToolMintMgr_->m_quadDecoration_[i].first.second.empty() ) continue;

			int nAlignRL;
			if( i == 0 || i == 2 )
				nAlignRL = MAM_LEFT;
			else
				nAlignRL = MAM_RIGHT;

			// decoration total rect
			pDC->SetColor(64, 64, 64);
			pDC->FillRectangle( m_pToolMintMgr_->m_quadDecoration_[i].second.first );
			//pDC->FillRectangle( m_pPaneMgr_->m_quadDecoration_[i].first.first );

			pDC->SetFont( m_pToolMintMgr_->m_pArialMFont_ );
			pDC->SetColor( 255, 255, 255 );
			{
				pDC->Text( m_pToolMintMgr_->m_quadDecoration_[i].first.first,
					m_pToolMintMgr_->m_quadDecoration_[i].first.second.c_str(), nAlignRL | MAM_VCENTER );
			}

			// Quad Popup Trigger
			pDC->SetFont( m_pToolMintMgr_->m_pGulimMFont_ );
			for(CMintManager::RECT_TRIGGER::iterator itr = m_pToolMintMgr_->m_quadPopupTable_[i].begin();
				itr != m_pToolMintMgr_->m_quadPopupTable_[i].end(); ++itr)
			{
				if(itr->second == 0 )
					continue;

				if(!bChecked && itr->first.InPoint( MPOINT(m_currentMousePoint_.x, m_currentMousePoint_.y) ) )
				{
					bChecked = true;
					m_pToolMintMgr_->m_pCurrentTrigger_ = itr->second;
					pDC->SetColor(128, 128, 128);
					FILL_1_PIX_SMALL( itr->first, i);
					pDC->SetColor(255, 255, 255);
				}
				else
				{
					if( itr->second->GetCheck() )
						pDC->SetColor(225, 225, 225);
					else
						pDC->SetColor(196, 196, 196);

					FILL_1_PIX_SMALL( itr->first, i);
					pDC->SetColor(0, 0, 0);
				}

				{
					pDC->Text( itr->first, itr->second->GetTriggerName() );
				}
			}
		}

		if(!bChecked)
			m_pToolMintMgr_->m_pCurrentTrigger_ = NULL;
	}
	else
	{
		// title and decoration
		pDC->SetColor(64, 64, 64);
		pDC->FillRectangle( m_pToolMintMgr_->m_quadDecoration_[0].first.first );
		pDC->FillRectangle( m_pToolMintMgr_->m_quadDecoration_[0].second.first );

		pDC->SetFont( m_pToolMintMgr_->m_pArialMFont_ );
		pDC->SetColor( 255, 255, 255 );

		{
			pDC->Text( m_pToolMintMgr_->m_quadDecoration_[0].first.first, "Not Loaded", MAM_RIGHT | MAM_VCENTER );
		}

		// item
		pDC->SetFont( m_pToolMintMgr_->m_pGulimMFont_ );
		if( m_pToolMintMgr_->m_quadDecoration_[0].second.first.InPoint(MPOINT(m_currentMousePoint_.x, m_currentMousePoint_.y)) )
		{
			pDC->SetColor(128, 128, 128);
			FILL_1_PIX_SMALL( m_pToolMintMgr_->m_quadDecoration_[0].second.first, 3);
			pDC->SetColor(255, 255, 255);
		}
		else
		{
			pDC->SetColor(196, 196, 196);
			FILL_1_PIX_SMALL( m_pToolMintMgr_->m_quadDecoration_[0].second.first, 3);
			pDC->SetColor(0, 0, 0);
		}

		{
			pDC->Text( m_pToolMintMgr_->m_quadDecoration_[0].second.first, "로딩된 쿼드팝업 없음" );
		}
	}
}

void CMintManager::QuadPopup_::OnClick(MEvent* pEvent)
{
	if( m_pToolMintMgr_->m_pCurrentTrigger_)
		m_pToolMintMgr_->m_pCurrentTrigger_->Trigger();

	m_pToolMintMgr_->ActivateQuadPopup(false);
}

void CMintManager::SetWorkspaceSize(int w, int h)
{
	m_mint_.SetWorkspaceSize(w, h);
	MWidget* pWidget = (MWidget*)(m_mint_.GetMainFrame());
	if ( pWidget == NULL ) return;

	pWidget->SetSize(w, h);
	if (m_pQuadPopup_)
		m_pQuadPopup_->SetSize(w, h);
}

void CMintManager::ActiveCaptionTextView( bool _bActive )
{
	if(m_pCaptionWidget)
	{
		m_bCaptionActive = _bActive;
		m_pCaptionWidget->Show(_bActive);
	}
}

void CMintManager::SetCaptionTextView( const char* pCaption, const char* chFontAliasName, const char* chFontName, int nSize, bool bBold /*= false*/, bool bItalic /*= false*/ )
{
	if(m_bCaptionActive &&
		m_pCaptionWidget)
	{
		// 자막 체크
		MFont* pFont = CheckFont(chFontAliasName, chFontName, "", nSize, bBold, bItalic);

		// 위치 계산
		MWidget* pWidget = (MWidget*)(m_mint_.GetMainFrame());
		MRECT rtScreen = pWidget->GetScreenRect();
		MRECT rtLetterBox = m_pLetterBoxBottom->GetRect();
		int nH = rtScreen.h - (rtLetterBox.h / 2) - (pFont->GetHeight() / 2);

		m_pCaptionWidget->SetPosition(0, nH);
		m_pCaptionWidget->SetText(pCaption);
	}
}

MFont* CMintManager::CheckFont( const char* chFontAliasName, const char* chFontName, const char* chFileName, int nSize, bool bBold, bool bItalic )
{
	MFont* pCheckFont = m_pCaptionWidget->GetFont();
	bool bFontCreate = false;
	if(pCheckFont == NULL ||
		pCheckFont->m_szName != string(chFontAliasName))
	{
		bFontCreate = true;
	}

	if(bFontCreate)
	{
		bool bNewFont = false;
		pCheckFont = (MFontR3*)MFontManager::Get(chFontAliasName);
		if(pCheckFont == NULL ||
			(pCheckFont && (MFontR3*)MFontManager::Get(NULL) == pCheckFont))
		{
			bNewFont = true;
		}

		if(bNewFont)
		{
			MFontR3* pNewFont = new MFontR3;
			if(!pNewFont->Create(m_pControlMediator_->GetCurrentDevice(), chFontAliasName, chFontName, nSize, 1.0f, bBold, bItalic))
			{
				pNewFont->Destroy();
				SAFE_DELETE(pNewFont);
			}
			else
			{
				MFontManager::Add(pNewFont);
				pCheckFont = pNewFont;
			}
		}
	}

	return pCheckFont;
}

void CMintManager::SetLetterBoxPaint( bool bShow, float fRatioW, float fRatioH )
{
	if(m_pLetterBoxBottom == NULL)
		return;

	m_pLetterBoxTop->Show(bShow);
	m_pLetterBoxBottom->Show(bShow);

	m_bLetterBoxActive = bShow;

	if(bShow)
	{
		MWidget* pWidget = (MWidget*)(m_mint_.GetMainFrame());
		MRECT rtScreen = pWidget->GetScreenRect();

		float fH = (float)(fRatioH * rtScreen.w) / fRatioW;

		int nLetterBoxH = (rtScreen.h - fH) / 2.0f;

		fH += 0.5f;
		m_pLetterBoxTop->SetSize(rtScreen.w, nLetterBoxH);
		m_pLetterBoxBottom->SetSize(rtScreen.w, nLetterBoxH);
		m_pLetterBoxBottom->SetPosition(0, nLetterBoxH + fH);

		MCOLOR stColor(0, 0, 0);

		m_pLetterBoxTop->SetBackgroundColor(stColor);
		m_pLetterBoxTop->SetBorderColor(stColor);
		m_pLetterBoxBottom->SetBackgroundColor(stColor);
		m_pLetterBoxBottom->SetBorderColor(stColor);
	}
}

void CMintManager::ShowNoticeLabel( bool bShow )
{
	m_bNoticeActive = bShow;

	if(m_pNoticeLabel)
		m_pNoticeLabel->Show(bShow);
}

void CMintManager::FadeDraw( float fAlpha )
{
	m_pFadeWidget->Show(true);

	unsigned char alpha = (unsigned char)(255 * fAlpha);
	m_pFadeWidget->SetBackgroundColor(MCOLOR(0, 0, 0, alpha));
}

void CMintManager::FadeEnd()
{
	m_pFadeWidget->Show(false);
}
