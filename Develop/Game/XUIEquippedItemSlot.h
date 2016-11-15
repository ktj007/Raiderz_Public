#pragma once

#include "MWidget.h"
#include "MBitmap.h"


using namespace std;
using namespace mint3;



/// class : XEquippedItemSlotLook
class XEquippedItemSlotLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pSmallFont;



/// Member functions
public:
	XEquippedItemSlotLook();

	void SetSmallFont( const char* szFont)			{ m_pSmallFont = MFontManager::Get( szFont);			}
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}


	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};






/// class : XUIEquippedItemSlot
class XUIEquippedItemSlot : public MWidget
{
protected:
	SH_ITEM_SLOT	m_nSlotType;
	XItemData*		m_pItem;
	MBitmap*		m_pDefaultImage;
	MBitmap*		m_pIconImage;



public:
	XUIEquippedItemSlot( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual const char* GetState() override;
	virtual int GetStateNum() override;


	void SetSlotType( const wchar_t* szType);
	SH_ITEM_SLOT GetSlotType() const						{ return m_nSlotType;								}
	const wchar_t* GetSlotTypeStr() const					{ return CSItemSlotStr[ m_nSlotType];				}

	XItemData* GetItem() const								{ return m_pItem;									}
	void SetDefaultImage( const wchar_t* szImage);
	MBitmap* GetIconImage() const							{ return m_pIconImage;								}
	int GetItemID();
	int GetDurability();
	int GetRepairPrice();
	bool IsEmpty() const									{ return (m_pItem == NULL ? true : false);			}

	bool UpdateInfo();

	bool DoUseItem();
	bool DoUnequipItem();

	virtual bool IsEnableDragAndDrop() const override		{ return true;										}



protected:
	virtual void OnDblClick( MEvent* pEvent) override;
 	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;
// 	virtual bool OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped) override;
// 	virtual bool OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt) override;



public:
	int glueGetSlotType() const								{ return (int)GetSlotType();						}
	int glueGetItemID()										{ return GetItemID();								}
	int glueGetDurability()									{ return GetDurability();							}
	int glueGetRepairPrice()								{ return GetRepairPrice();							}
	bool glueIsEmpty()										{ return IsEmpty();									}
	void glueUpdateInfo()									{ UpdateInfo();										}
	void glueDoUseItem()									{ DoUseItem();										}
	void glueDoUnequipItem()								{ DoUnequipItem();									}



#define MINT_EQUIPPEDITEMSLOT		"EquippedItemSlot"
	virtual const char* GetClassName() override				{ return MINT_EQUIPPEDITEMSLOT;						}
};