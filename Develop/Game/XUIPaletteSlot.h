#pragma once

#include "MWidget.h"
#include "MBitmap.h"


using namespace std;
using namespace mint3;


/// class : XPaletteSlotLook
class XPaletteSlotLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pLargeFont;
	MFont*		m_pSmallFont;
	MBitmap*	m_pCooltimeImage;
	int			m_nCooltimeImgCellCount;
	int			m_nCooltimeImgCellCountRow;
	int			m_nCooltimeImgCellCountCol;
	MSIZE		m_CooltimeImgSize;


/// Member functions
public:
	XPaletteSlotLook();

	void SetLargeFont( const char* szFont);
	MFont* GetLargeFont()							{ return ( m_pLargeFont ? m_pLargeFont : GetFont());	}

	void SetSmallFont( const char* szFont);
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetCooltimeImage( const char* szImage);
	MBitmap* GetCooltimeImage() const				{ return m_pCooltimeImage;								}

	void SliceCellCooltimeImg( int row, int col);

	void SetCooltimeImgCellCount( int nCount)		{ m_nCooltimeImgCellCount = min( nCount, m_nCooltimeImgCellCount);	}
	int GetCooltimeImgCellCount() const				{ return m_nCooltimeImgCellCount;						}



	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};






/// class : XUIPaletteSlot
class XUIPaletteSlot : public MWidget
{
protected:
	XPaletteItem*	m_pPaletteItem;
	int				m_nIndex;
	int				m_nType;
	int				m_nID;
	MBitmap*		m_pIconImage;
	MTimer			m_tActivate;



public:
	XUIPaletteSlot( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual const char* GetState() override;
	virtual int GetStateNum() override;

	XPaletteItem* GetPaletteItem() const					{ return m_pPaletteItem;							}

	void SetSlotIndex( int nIndex)							{ m_nIndex = nIndex;								}
	int GetSlotIndex() const								{ return m_nIndex;									}

	int GetType() const										{ return m_nType;									}
	int GetID() const										{ return m_nID;										}
	bool IsEmpty() const									{ return (m_nType == PIT_NONE);						}
	bool IsTalent() const									{ return (m_nType == PIT_TALENT);					}
	bool IsItem() const										{ return (m_nType == PIT_ITEM);						}
	MBitmap* GetIconImage() const							{ return m_pIconImage;								}
	int GetTalentID();
	bool IsUsableItem();
	bool GetActivate() const								{ return m_tActivate.IsReversed();					}
	float GetActivateRatio() const							{ return m_tActivate.GetRatio();					}

	bool UpdateInfo();

	bool DoUsePaletteSlot();

	virtual bool IsEnableDragAndDrop() const override;



protected:
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnClick( MEvent* pEvent) override;
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;


public:
	int glueGetSlotIndex() const							{ return GetSlotIndex();							}	   
	int glueGetID()											{ return GetID();									}
	int glueGetType()										{ return GetType();									}
	bool glueIsEmpty()										{ return IsEmpty();									}
	bool glueIsTalent()										{ return IsTalent();								}
	bool glueIsItem()										{ return IsItem();									}
	void glueUpdateInfo()									{ UpdateInfo();										}
	void glueDoUsePaletteSlot()								{ DoUsePaletteSlot();								}



#define MINT_PALETTESLOT		"PaletteSlot"
	virtual const char* GetClassName() override				{ return MINT_PALETTESLOT;							}
};