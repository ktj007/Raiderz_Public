#include "stdafx.h"
#include "XUITalentSlot.h"
#include "XTalentInfoMgr.h"
#include "XPost_Training.h"




// XUITalentSlot
XUITalentSlot::XUITalentSlot( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XTalentSlotLook);

	m_pTalentInfo = NULL;
	m_bLearned = false;
	m_bLearnable = false;
	m_bFocusTalent = false;
	m_nCurrRank = 0;
	m_pIconImage = NULL;

	m_tLearning.SetReverse( true);
	m_tLearning.SetArrivalTime( 500);

	SetEnableFocus( true);
}


// GetState
const char* XUITalentSlot::GetState()
{
	if ( m_pTalentInfo == NULL)									return MWS_DISABLE;
	else if ( m_bLearned == false)								return MWS_DISABLE;
	else if ( m_bFocusTalent == true)							return MWS_NORMAL;
	else if ( m_bLearnable == true)								return MWS_FOCUS;
	else														return MWS_DOWN;

	return MWS_DISABLE;
}


// GetStateNum
int XUITalentSlot::GetStateNum()
{
	if ( m_pTalentInfo == NULL)									return MWSN_DISABLE;
	else if ( m_bLearned == false)								return MWSN_DISABLE;
	else if ( m_bFocusTalent == true)							return MWSN_NORMAL;
	else if ( m_bLearnable == true)								return MWSN_FOCUS;
	else														return MWSN_DOWN;

	return MWSN_DISABLE;
}


// SetTalentID
void XUITalentSlot::SetTalent( int nRank, int nID)
{
	if ( nRank < 1  ||  nID <= 0)		return;

	while ( (int)m_TalentList.size() < nRank)		m_TalentList.push_back( 0);

	m_TalentList[ nRank - 1] = nID;
}


// GetTalentName
const wchar_t* XUITalentSlot::GetTalentName()
{
	if ( m_pTalentInfo != NULL)		return m_pTalentInfo->GetName();
	return L"";
}


// UpdateInfo
bool XUITalentSlot::UpdateInfo()
{
	m_pTalentInfo = NULL;
	m_bLearned = false;
	m_bLearnable = false;
	m_bFocusTalent = false;
	m_nCurrRank = 0;
	m_pIconImage = NULL;

	if ( m_TalentList.empty() == true)		return false;

	m_pTalentInfo = info.talent->Get( m_TalentList[ 0]);


	size_t _size = m_TalentList.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if ( gvar.MyInfo.Talent.IsLearnedTalent( m_TalentList[ i]) == true)
		{
			m_bLearned = true;

			m_pTalentInfo = info.talent->Get( m_TalentList[ i]);
			m_nCurrRank = i + 1;
		}
	}

	if ( m_pTalentInfo == NULL  ||  m_pTalentInfo->m_nID == 0 )
		return false;


	// Set icon image
	wstring strIconName = m_pTalentInfo->m_szIcon;
	if ( strIconName.empty() == true  &&  m_pTalentInfo->m_nTalentLine > 0)
	{
		XTalentInfo* pInfo = info.talent->Get( m_pTalentInfo->m_nTalentLine);
		if ( pInfo != NULL)		strIconName = pInfo->m_szIcon;
	}

	if ( strIconName.empty())	m_pIconImage = MBitmapManager::Get( "iconUnknown");
	else
	{
		m_pIconImage = global.ui->GetIcon(strIconName);
		if ( m_pIconImage == NULL)			m_pIconImage = MBitmapManager::Get( "iconUnknown");
	}


	// Check is focus talent
	if ( m_pTalentInfo->m_nExtraPassive == TEPT_FOCUS_USE_BERSERK  ||  m_pTalentInfo->m_nExtraPassive == TEPT_FOCUS_USE_COUNTER  ||
		m_pTalentInfo->m_nExtraActive == TEAT_ADVENT)
	{
		m_bFocusTalent = true;
		m_bLearnable = false;
	}


	// Set learnable
	if ( m_bFocusTalent == false  &&  gvar.MyInfo.Talent.GetTP() > 0)
	{
		// 이미 배운 탤런트가 최대 rank 이하 일 때
		if ( m_bLearned == true)
		{
			if ( m_nCurrRank < (int)m_TalentList.size())
				m_bLearnable = true;
		}

		// 아직 배우지 않은 탤런트 일 때
		else
		{
			// 요구 TP를 채웠는지 검사함
			if ( gvar.MyInfo.Talent.GetStyleUsedTP( m_pTalentInfo->m_nStyle) >= m_pTalentInfo->m_nTNeedStyleTP)
			{
				// 선행 탤런트를 배웠는지 확인함
				if ( m_pTalentInfo->m_nTNeedTalent == 0  ||  gvar.MyInfo.Talent.IsLearnedTalent( m_pTalentInfo->m_nTNeedTalent) == true)
					m_bLearnable = true;
			}
		}
	}

	return true;
}


// DoLearnTalent
bool XUITalentSlot::DoLearnTalent()
{
	if ( m_bLearnable == false)		return false;

	XPostTraining_Train( m_TalentList[ m_nCurrRank]);

	m_tLearning.SetReverse( false);
	return true;
}


// OnDragBegin
bool XUITalentSlot::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( m_bLearned == true  &&  m_pTalentInfo->IsPassiveSkill() == false)
	{
		MDragObject* pObject = new MDragObject( string(MLocale::ConvUTF16ToAnsi(m_pTalentInfo->GetName())), m_pIconImage, m_pTalentInfo->m_nID);
		pDragData->AddObject( pObject);

		if ( HasLookAndFill())
		{
			pDragData->SetFont( m_pLookAndFill->GetFont());
			pDragData->SetImageSize( MSIZE( m_Rect.w, m_Rect.h));
			pDragData->SetHotSpot( MPOINT( m_Rect.w >> 1, m_Rect.h >> 1));
		}
	}

	return MWidget::OnDragBegin( pDragData, pt);
}











// XTalentSlotLook
XTalentSlotLook::XTalentSlotLook()
{
	m_pSmallFont = NULL;
	m_pGlowImage = NULL;
}


void XTalentSlotLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUITalentSlot* pTalentSlot = (XUITalentSlot*)pWidget;
	XTalentInfo* pTalentInfo = pTalentSlot->GetTalent();
	if ( pTalentInfo == NULL)
	{
		// Draw border
		MLookBase::OnDrawBorder( pWidget, pDC);

		return;
	}

	MRECT r = pTalentSlot->GetClientRect();

	// Draw glow
	if ( m_pGlowImage != NULL  &&  pTalentSlot->IsFocusTalent() == false  &&  pTalentSlot->IsLearnable())
	{
		float _old_opacity = pDC->SetOpacity( 0.4 * sin( (double)timeGetTime() * 0.004) + 0.6);
		pDC->SetBitmap( m_pGlowImage);
		pDC->Draw( (r.w - m_pGlowImage->GetWidth()) >> 1, (r.h - m_pGlowImage->GetHeight()) >> 1, m_pGlowImage->GetWidth(), m_pGlowImage->GetHeight());
		pDC->SetOpacity( _old_opacity);
	}


	// Draw icon
	MBitmap* pImage = pTalentSlot->GetIconImage();
	if ( pImage != NULL)
	{
		pDC->SetBitmap( pImage);
		pDC->Draw( r);
	}


	// Draw learning
	if ( pTalentSlot->IsFocusTalent() == false)
	{
		MTimer& tLearnTimer = pTalentSlot->GetLearningTimer();
		if ( tLearnTimer.IsReversed() == false  ||  tLearnTimer.IsArrival() == false)
		{
			pDC->SetColor( 255, 255, 255, min( 180, 200 * tLearnTimer.GetRatio()) );
			pDC->FillRectangle( r);

			if ( tLearnTimer.IsReversed() == false  &&  tLearnTimer.IsArrival())
				tLearnTimer.SetReverse( true);
		}
	}


	// Draw border
	MLookBase::OnDrawBorder( pWidget, pDC);


	// Draw selectbar
	if ( pTalentSlot->IsMouseOver() == true)	Draw( pDC, r, MWP_SELECTBAR, MWS_NORMAL);


	// Draw rank
	if ( pTalentSlot->IsFocusTalent() == false)
	{
		r.x += r.w + 2;
		if ( pTalentSlot->GetCurrentRank() == pTalentSlot->GetMaxRank())	pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DOWN, MCOLOR( 255, 255, 0)));
		else if ( pTalentSlot->GetCurrentRank() > 0)						pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 255, 0)));
		else																pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255)));
		pDC->SetFont( GetSmallFont());

		// Max rank
		char buff[ 64];
		sprintf_s( buff, "/%d", pTalentSlot->GetMaxRank());
		pDC->Text( r, buff, MAM_LEFT | MAM_BOTTOM);

		// Current rank
		r.y -= pDC->GetFont()->GetHeight();
		sprintf_s( buff, "%d", pTalentSlot->GetCurrentRank());
		pDC->Text( r, buff, MAM_LEFT | MAM_BOTTOM);
	}
}


void XTalentSlotLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	XUITalentSlot* pTalentSlot = (XUITalentSlot*)pWidget;
	XTalentInfo* pTalentInfo = pTalentSlot->GetTalent();
	if ( pTalentInfo == NULL)		return;


	MRECT r = pTalentSlot->GetClientRect();

	if ( pTalentSlot->IsLearnable() == true)
	{
	}


	if ( pTalentSlot->IsLearned() == false  &&  pTalentSlot->IsLearnable() == false)
	{
	}
}

void XTalentSlotLook::SetSmallFont(const char* szFont)
{
	m_pSmallFont = MFontManager::Get( szFont);
}

void XTalentSlotLook::SetGlowImage(const char* szImage)
{
	m_pGlowImage = MBitmapManager::Get( szImage);
}