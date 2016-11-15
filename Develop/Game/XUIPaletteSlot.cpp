#include "stdafx.h"
#include "XUIPaletteSlot.h"
#include "XUITalentSlot.h"
#include "XUICustomLook.h"
#include "MBitmap.h"
#include "XGame.h"
#include "XTalentInfoMgr.h"
#include "XMyPlayer.h"
#include "XItemManager.h"
#include "XTalentAvailable.h"
#include "XStrings.h"



// XUIPaletteSlot
XUIPaletteSlot::XUIPaletteSlot( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XPaletteSlotLook);

	m_pPaletteItem = NULL;
	m_nIndex = -1;
	m_nType = PIT_NONE;
	m_nID = 0;
	m_pIconImage = NULL;

	m_tActivate.SetArrivalTime( 300);

	SetEnableFocus( true);
}


// GetState
const char* XUIPaletteSlot::GetState()
{
	if ( m_nType == PIT_NONE)				return MWS_DISABLE;
	else if ( IsMouseOver() == true)		return MWS_FOCUS;
	return MWS_NORMAL;
}


// GetStateNum
int XUIPaletteSlot::GetStateNum()
{
	if ( m_nType == PIT_NONE)				return MWSN_NORMAL;
	else if ( IsMouseOver() == true)		return MWSN_FOCUS;
	return MWSN_NORMAL;
}


// UpdateInfo
bool XUIPaletteSlot::UpdateInfo()
{
	m_pPaletteItem = NULL;
	m_nType = PIT_NONE;
	m_nID = 0;
	m_pIconImage = NULL;

	if ( m_nIndex < 0  ||  m_nIndex > 9)	return false;

	int nIndex = (int)gvar.MyInfo.PaletteList.GetCurPaletteListIndex() * 10 + m_nIndex;
	if (nIndex < 0 || nIndex > (gvar.MyInfo.PaletteList.GetContainerSize()-1))	return false;

	// Set palette type
	m_pPaletteItem = &gvar.MyInfo.PaletteList.m_vContains[nIndex];
	if ( m_pPaletteItem == NULL)			return false;

	if ( m_pPaletteItem->talentID > 0)
	{
		m_nType = PIT_TALENT;
		m_nID = m_pPaletteItem->talentID;
	}
	else if ( m_pPaletteItem->itemID > 0)
	{
		m_nType = PIT_ITEM;
		m_nID = m_pPaletteItem->itemID;
	}


	// Set icon image
	if ( m_nType == PIT_TALENT)
	{
		XTalentInfo* pTalentInfo = info.talent->Get( m_pPaletteItem->talentID);
		if ( pTalentInfo != NULL)
		{
			wstring strIconName = pTalentInfo->m_szIcon;
			if ( strIconName.empty())
			{
				XTalentInfo* pInfo = info.talent->Get( pTalentInfo->m_nTalentLine);
				if ( pInfo != NULL)		strIconName = pInfo->m_szIcon;
			}
			if ( strIconName.empty())	m_pIconImage = MBitmapManager::Get( "iconUnknown");
			else
			{
				m_pIconImage = global.ui->GetIcon(strIconName);
				if ( m_pIconImage == NULL)	m_pIconImage = MBitmapManager::Get( "iconUnknown");
			}
		}
	}
	else if ( m_nType == PIT_ITEM)
	{
		XItemData* pItemData = info.item->GetItemData( m_pPaletteItem->itemID);
		if ( pItemData != NULL)
		{
			m_pIconImage = global.ui->GetIcon(pItemData->m_strIconName);
			if ( m_pIconImage == NULL)	m_pIconImage = MBitmapManager::Get( "iconUnknown");
		}
	}

	return ( (m_nType == PIT_NONE) ? false : true);
}


// DoUsePaletteSlot
bool XUIPaletteSlot::DoUsePaletteSlot()
{
	if ( m_nType == PIT_TALENT)
	{
		XTalentInfo* pTalentInfo = info.talent->Get( m_nID);
		if ( pTalentInfo != NULL)
		{
			gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseTalent( m_nID);
			return true;
		}
	}
	else if ( m_nType == PIT_ITEM)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItemByID( m_nID);
		if ( pItem != NULL)
		{
			gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseItem(pItem->m_nSlotID);
			return true;
		}
	}

	return false;
}


// OnRun
void XUIPaletteSlot::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	if ( m_nType == PIT_NONE)
		m_tActivate.SetReverse( false);

	else
	{
		bool bActivate;

		// Check activate state
		if ( IsUsableItem() == true)
			bActivate = true;

		else
		{
			XTalentAvailable TalentAvailable;
			bActivate = TalentAvailable.IsAvailable( GetTalentID());
		}

		m_tActivate.SetReverse( bActivate);
	}
}


// OnClick
void XUIPaletteSlot::OnClick( MEvent* pEvent)
{
	MWidget::OnClick( pEvent);

	DoUsePaletteSlot();
}


// OnDragBegin
bool XUIPaletteSlot::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( m_nType == PIT_TALENT)
	{
		XTalentInfo* pTalentInfo = info.talent->Get( m_nID);
		if ( pTalentInfo != NULL)
		{
			MDragObject* pObject = new MDragObject( string(MLocale::ConvUTF16ToAnsi(pTalentInfo->GetName())), m_pIconImage);
			pObject->m_dwItemData = (DWORD)( gvar.MyInfo.PaletteList.GetCurPaletteListIndex() * PALETTESLOT_MAX + m_nIndex);
			pDragData->AddObject( pObject);
		}
	}
	else if ( m_nType == PIT_ITEM)
	{
		XItemData* pItemData = info.item->GetItemData( m_nID);
		if ( pItemData != NULL)
		{
			MDragObject* pObject = new MDragObject( MLocale::ConvTCHARToAnsi(pItemData->GetName()), m_pIconImage);
			pObject->m_dwItemData = (DWORD)( gvar.MyInfo.PaletteList.GetCurPaletteListIndex() * PALETTESLOT_MAX + m_nIndex);
			pDragData->AddObject( pObject);
		}
	}


	if ( HasLookAndFill())
	{
		pDragData->SetFont( m_pLookAndFill->GetFont());
		pDragData->SetImageSize( MSIZE( m_Rect.w, m_Rect.h));
		pDragData->SetHotSpot( MPOINT( m_Rect.w >> 1, m_Rect.h >> 1));
	}

	return MWidget::OnDragBegin( pDragData, pt);
}


int XUIPaletteSlot::GetTalentID()
{
	// 팔래트는 탤런트 데이터로 작동한다.
	if ( m_nType == PIT_ITEM)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItemByID( m_nID);
		if( pItem == NULL ) return  0;

		return pItem->GetTalentID();
	}

	return m_nID;
}


bool XUIPaletteSlot::IsUsableItem()
{
	if ( m_nType == PIT_ITEM)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItemByID( m_nID);
		if( pItem == NULL ) return false;

		if(pItem->m_pItemData->m_nUsableType >= USABLE_TALENT_TRAIN)
			return true;
	}

	return false;
}


bool XUIPaletteSlot::IsEnableDragAndDrop() const 
{
	return !gvar.MyInfo.PaletteList.IsLock_PaletteSlot();
}










// XPaletteSlotLook
XPaletteSlotLook::XPaletteSlotLook()
{
	m_pLargeFont = NULL;
	m_pSmallFont = NULL;
	m_pCooltimeImage = NULL;
	m_nCooltimeImgCellCount = 1;
	m_nCooltimeImgCellCountRow = 0;
	m_nCooltimeImgCellCountCol = 0;
	m_CooltimeImgSize = MSIZE( 0, 0);
}


void XPaletteSlotLook::SliceCellCooltimeImg( int row, int col)
{
	if ( m_pCooltimeImage == NULL)		return;
	row = max( 0, row);
	col = max( 0, col);

	m_nCooltimeImgCellCountRow = row;
	m_nCooltimeImgCellCountCol = col;
	m_nCooltimeImgCellCount = row * col;

	m_CooltimeImgSize.w = m_pCooltimeImage->GetWidth() / row;
	m_CooltimeImgSize.h = m_pCooltimeImage->GetHeight() / col;
}


void XPaletteSlotLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUIPaletteSlot* pPaletteSlot = (XUIPaletteSlot*)pWidget;

	MRECT r = pPaletteSlot->GetClientRect();
	bool bActivate = pPaletteSlot->GetActivate();
	float fRemaindCoolTimeRatio = gvar.MyInfo.Talent.CoolTime.GetRemainedTimeRatio( pPaletteSlot->GetTalentID());


	if ( pPaletteSlot->IsEmpty() == false)
	{
		// Get activate ratio
		float fRatio = 1.0f - pPaletteSlot->GetActivateRatio();
		

		// Draw icon
		MBitmap* pImage = pPaletteSlot->GetIconImage();
		if ( pImage != NULL)
		{
			pDC->SetBitmap( pImage);

// 			if ( fRemaindCoolTimeRatio > 0.0f)
// 				pDC->Draw( r);
// 
// 			else if ( fRatio < 1.0f)
			if ( fRatio < 1.0f)
			{
				int _col = 64 + (int)( 191.0f * fRatio);
				MCOLOR nOldColor = pDC->SetBitmapColor( _col, _col, _col);
				pDC->Draw( r);
				pDC->SetBitmapColor( nOldColor);
			}

			else
				pDC->Draw( r);
		}


		// Draw cooltime
//		if ( fRemainCoolTimeRatio > 0.0f)
		{
			int nIndex = (int)( (1.0f - fRemaindCoolTimeRatio) * (float)m_nCooltimeImgCellCount);
			MRECT sr( (nIndex % m_nCooltimeImgCellCountRow) * m_CooltimeImgSize.w, (nIndex / m_nCooltimeImgCellCountRow) * m_CooltimeImgSize.h, m_CooltimeImgSize.w, m_CooltimeImgSize.h);

			float _opacity = pDC->SetOpacity( 0.8f);
			pDC->SetBitmap( GetCooltimeImage());
			pDC->Draw( r.x, r.y, r.w, r.h, sr.x, sr.y, sr.w, sr.h);
			pDC->SetOpacity( _opacity);
		}
	}


	// Draw border
	MLookBase::OnDrawBorder( pWidget, pDC);


	// Draw number
	if ( pPaletteSlot->GetSlotIndex() >= 0)
	{
		int nIndex = pPaletteSlot->GetSlotIndex() + 1;
		if ( nIndex == 10)		nIndex = 0;

		char buff[ 4];
		sprintf_s( buff, "%d", nIndex);

		pDC->SetColor( GetFontColor( MWP_BORDER, bActivate ? MWS_NORMAL : MWS_DISABLE, MCOLOR( 255, 255, 255)));
		pDC->SetFont( GetLargeFont());
		pDC->Text( r, buff, MAM_TOP | MAM_LEFT);
	}


	// Draw remained cooltime
	if ( fRemaindCoolTimeRatio > 0.0f)
	{
		int nRemainedTime = gvar.MyInfo.Talent.CoolTime.GetRemainedTime( pPaletteSlot->GetTalentID()) + 1;
		char szRemained[ 64] = { 0, };

		int nHour = nRemainedTime / 3600;
		if ( nHour > 0)
		{
			string _unit = MLocale::ConvUTF16ToAnsi( XGetStr( L"UNIT_HOUR"));
			sprintf( szRemained, "%d%s", nHour, _unit.c_str());
		}
		else
		{
			int nMin = nRemainedTime / 60;
			if ( nMin > 0)
			{
				string _unit = MLocale::ConvUTF16ToAnsi( XGetStr( L"UNIT_MINUTE"));
				sprintf( szRemained, "%d%s", nMin, _unit.c_str());
			}
			else
			{
				int nSec = nRemainedTime % 60;
				string _unit = MLocale::ConvUTF16ToAnsi( XGetStr( L"UNIT_SECOND"));
				sprintf( szRemained, "%d%s", nSec, _unit.c_str());
			}
		}

		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 255, 0)));
		pDC->SetFont( GetSmallFont());
		pDC->Text( r, szRemained, MAM_VCENTER | MAM_HCENTER);
	}


	// Draw quantity
	if ( pPaletteSlot->IsItem() == true)
	{
		char buff[ 4];
		sprintf_s( buff, "%d", gvar.MyInfo.Inventory.GetItemAmountByID( pPaletteSlot->GetID()));

		pDC->SetColor( GetFontColor( MWP_BORDER, bActivate ? MWS_DOWN : MWS_DISABLE, MCOLOR( 230, 230, 230)));
		pDC->SetFont( GetFont());
		pDC->Text( r, buff, MAM_RIGHT | MAM_BOTTOM);
	}
}


void XPaletteSlotLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
}


void XPaletteSlotLook::SetLargeFont(const char* szFont)
{
	m_pLargeFont = MFontManager::Get( szFont);
}


void XPaletteSlotLook::SetSmallFont(const char* szFont)
{
	m_pSmallFont = MFontManager::Get( szFont);
}


void XPaletteSlotLook::SetCooltimeImage(const char* szImage)
{
	m_pCooltimeImage = MBitmapManager::Get( szImage);
}