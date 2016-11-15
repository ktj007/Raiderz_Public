#include "stdafx.h"
#include "XUIEquippedItemSlot.h"
#include "XMyPlayer.h"
#include "XPost_Item.h"
#include "XItemManager.h"
#include "XUICustomLook.h"
#include "CSNPCShopCalculator.h"



// XUIEquippedItemSlot
XUIEquippedItemSlot::XUIEquippedItemSlot( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XEquippedItemSlotLook);

	SetEnableFocus( true);

	m_nSlotType = ITEMSLOT_NONE;
	m_pItem = NULL;
	m_pDefaultImage = NULL;
	m_pIconImage = NULL;
}


// GetState
const char* XUIEquippedItemSlot::GetState()
{
	return MWS_NORMAL;
}


// GetStateNum
int XUIEquippedItemSlot::GetStateNum()
{
	return MWSN_NORMAL;
}


// SetSlotType
void XUIEquippedItemSlot::SetSlotType( const wchar_t* szType)
{
	m_nSlotType = ITEMSLOT_NONE;

	for ( int i = 0;  i < ITEMSLOT_MAX;  i++)
	{
		if ( wcscmp( szType, CSItemSlotStr[ i]) == 0)
		{
			m_nSlotType = (SH_ITEM_SLOT)i;
			break;
		}
	}
}


// GetItemID
int XUIEquippedItemSlot::GetItemID()
{
	if ( IsEmpty() == true)		return 0;

	return m_pItem->m_nID;
}


// GetDurability
int XUIEquippedItemSlot::GetDurability()
{
	if ( IsEmpty() == true)		return 0;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( m_nSlotType);
	if ( pItem == NULL)			return 0;

	return pItem->m_nDurability;
}


// GetRepairPrice
int XUIEquippedItemSlot::GetRepairPrice()
{
	if ( IsEmpty() == true)		return 0;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( m_nSlotType);
	if ( pItem == NULL)			return 0;

	int nMaxDurability = pItem->m_pItemData->m_nMaxDurability;
	int nDurability = pItem->m_nDurability;

	return CSNPCShopCalculator::CalcRepairPrice( nMaxDurability-nDurability, pItem->m_pItemData->m_nRepairGrade, 0);
}


// UpdateInfo
bool XUIEquippedItemSlot::UpdateInfo()
{
	m_pItem = NULL;
	m_pIconImage = m_pDefaultImage;

	if ( m_nSlotType == ITEMSLOT_NONE)	return false;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( m_nSlotType);
	if ( pItem == NULL)					return false;

	m_pItem = info.item->GetItemData( pItem->m_pItemData->m_nID);
	if ( m_pItem == NULL)				return false;

	if ( m_pItem->m_strIconName.empty() == false)
	{
		m_pIconImage = global.ui->GetIcon(m_pItem->m_strIconName);
		if ( m_pIconImage == NULL)		m_pIconImage = MBitmapManager::Get( "iconUnknown");
	}

	return true;
}


// DoUseItem
bool XUIEquippedItemSlot::DoUseItem()
{
	if ( m_nSlotType == ITEMSLOT_NONE  ||  m_pItem == NULL)		return false;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( m_pItem->m_nItemSlot);
	if ( pItem == NULL)				return false;

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl == NULL)	return false;

	pModuleMyControl->DoActionUseItem( pItem->m_nSlotID);
	return true;
}


// DoUnequipItem
bool XUIEquippedItemSlot::DoUnequipItem()
{
	if ( m_nSlotType == ITEMSLOT_NONE  ||  m_pItem == NULL)		return false;

	if(XGetMyPlayer()->IsUsableBreakablePartsWeapon())
		return false;

	XPostItem_Unequip( m_nSlotType);
	return true;
}


// OnDblClick
void XUIEquippedItemSlot::OnDblClick( MEvent* pEvent)
{
	MWidget::OnDblClick( pEvent);

	DoUnequipItem();
}


// OnDragBegin
bool XUIEquippedItemSlot::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( m_nSlotType != ITEMSLOT_NONE  &&  m_pItem != NULL)
	{
		MDragObject* pObject = new MDragObject( string(MLocale::ConvUTF16ToAnsi(m_pItem->GetName())), m_pIconImage, m_nSlotType);
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

void XUIEquippedItemSlot::SetDefaultImage(const wchar_t* szImage)
{
	m_pDefaultImage = MBitmapManager::Get( MLocale::ConvUTF16ToAnsi(szImage).c_str());
}


/*
// OnDragEnd
bool XUIEquippedItemSlot::OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped)
{
	bool bRetVal = MWidget::OnDragEnd( pDragData, pt, bDropped);

	if ( bDropped == false)		XPostItem_Unequip( m_nSlotType);
	return bRetVal;
}


// OnDrop
bool XUIEquippedItemSlot::OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt)
{
	bool bRetVal = MWidget::OnDrop( pSender, pDragData, pt);

	if ( m_nSlotType == ITEMSLOT_NONE)	return false;
	if ( pSender == NULL)				return false;
	if ( pSender == this)				return bRetVal;

	wstring strClassName = pSender->GetClassName();

	// Sender is the inventory
	if ( strClassName == MINT_INVENLISTCTRL)
	{
		size_t _size = pDragData->GetCount();
		for ( size_t i = 0;  i < _size;  i++)
		{
			XItem* pItem = gvar.MyInfo.Inventory.GetItem( pDragData->GetItemData( i));
			if ( pItem != NULL)		XPostItem_EquipItem( pItem->m_nSlotID, m_nSlotType);
		}

		bRetVal = true;
	}

	// Sender is the item slot
	else if ( strClassName == MINT_EQUIPPEDITEMSLOT)
	{
		XUIEquippedItemSlot* pItemSlot = (XUIEquippedItemSlot*)pSender;
		SH_ITEM_SLOT nSlotType = pItemSlot->GetSlotType();

		// 아이템 슬롯에서 아이템 슬롯으로 이동이 가능한 경우는 무기와 귀걸이뿐이다
		if ( (m_nSlotType == ITEMSLOT_RWEAPON   &&  nSlotType == ITEMSLOT_RWEAPON2)	||
			 (m_nSlotType == ITEMSLOT_RWEAPON2  &&  nSlotType == ITEMSLOT_RWEAPON)	||
			 (m_nSlotType == ITEMSLOT_LWEAPON   &&  nSlotType == ITEMSLOT_LWEAPON2)	||
			 (m_nSlotType == ITEMSLOT_LWEAPON2  &&  nSlotType == ITEMSLOT_LWEAPON)	||
			 (m_nSlotType == ITEMSLOT_LFINGER  &&  nSlotType == ITEMSLOT_RFINGER)	||
			 (m_nSlotType == ITEMSLOT_RFINGER  &&  nSlotType == ITEMSLOT_LFINGER))
		{
			size_t _size = pDragData->GetCount();
			for ( size_t i = 0;  i < _size;  i++)
			{
				XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( pItemSlot->GetSlotType());
				if ( pItem == NULL)		continue;

				XPostItem_ChangeEquipSlot(pItemSlot->GetSlotType(), m_nSlotType);
			}

			bRetVal = true;
		}
	}

	return bRetVal;
}
*/









// XEquippedItemSlotLook
XEquippedItemSlotLook::XEquippedItemSlotLook()
{
	m_pSmallFont = NULL;
}


void XEquippedItemSlotLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	XUIEquippedItemSlot* pItemSlot = (XUIEquippedItemSlot*)pWidget;

	MRECT r = pItemSlot->GetClientRect();

	
	// Draw glow : dragging
	const MDragManager* pDragManager = MGetMint()->GetDragManager();
	if ( pDragManager->IsDragging() == true)
	{
	}


	// Draw border
	MLookBase::OnDrawBorder( pWidget, pDC);


	// Draw icon
	MBitmap* pImage = pItemSlot->GetIconImage();
	if ( pImage != NULL)
	{
		pDC->SetBitmap( pImage);
		pDC->Draw( r);
	}


	XItemData* pItem = pItemSlot->GetItem();
	if ( pItem == NULL)		return;

	int nMaxDurability = pItem->m_nMaxDurability;
	if ( nMaxDurability > 0)
	{
		int nAlign = GetTextAlign();
		int nDurability = pItemSlot->GetDurability();

		// Draw durability
		MFont* pFont = GetSmallFont();
		pDC->SetFont( pFont);

		char _buff[ 256];
		sprintf_s( _buff, "%d", nMaxDurability);
		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255)));
		pDC->Text( r.x + r.w + 7, r.y + ( nAlign & MAM_TOP ? pFont->GetHeight() : 0), r.w, r.h, _buff, nAlign);

		sprintf_s( _buff, "%d", nDurability);
		if ( (nDurability / nMaxDurability) < 0.2)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 255, 255)));
		pDC->Text( r.x + r.w + 7, r.y, r.w, r.h - ( nAlign & MAM_BOTTOM ? pFont->GetHeight() : 0), _buff, nAlign);
	}
}


void XEquippedItemSlotLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MLookBase::OnDraw( pWidget, pDC);
}
