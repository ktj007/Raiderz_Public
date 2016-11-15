#pragma once

#include "MLookBase.h"
#include "MListCtrl.h"
#include "MCategoryCtrl.h"
using namespace std;

namespace mint3
{

class MScrollBar;




/// class : XListCtrlCustomLook
class XListCtrlCustomLook : public MListCtrlLook
{
protected:
	MFont*		m_pSmallFont;
	int			m_nItemBkgrndRectExpand;


public:
	XListCtrlCustomLook();

	void SetSmallFont( const char* szFont)			{ m_pSmallFont = MFontManager::Get( szFont);			}
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetItemBkgrndRectExpand( int nExpand)		{ m_nItemBkgrndRectExpand = nExpand;					}
	int GetItemBkgrndRectExpand()					{ return m_nItemBkgrndRectExpand;						}


protected:
	virtual void OnDrawItems( MListCtrl* pListCtrl, MDrawContext* pDC) override;
	virtual void OnDrawItemText( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, int align, MDrawContext* pDC) override;
	virtual void OnDrawItemBkgrnd( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, MDrawContext* pDC) override;
};





/// class : XCategoryCtrlCustomLook
class XCategoryCtrlCustomLook : public MCategoryCtrlLook
{
protected:
	MFont*		m_pSmallFont;
	int			m_nItemBkgrndRectExpand;


public:
	XCategoryCtrlCustomLook();

	void SetSmallFont( const char* szFont)			{ m_pSmallFont = MFontManager::Get( szFont);			}
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetItemBkgrndRectExpand( int nExpand)		{ m_nItemBkgrndRectExpand = nExpand;					}
	int GetItemBkgrndRectExpand()					{ return m_nItemBkgrndRectExpand;						}


protected:
	virtual void OnDrawItems( MCategoryCtrl* pCategory, MDrawContext* pDC) override;
	virtual void OnDrawItemText( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, int align, MDrawContext* pDC) override;
	virtual void OnDrawCategoryBkgrnd( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawCategory( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawItemBkgrnd( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawCaret( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawSelectbar( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawExpandIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawCollapsedIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC) override;
	virtual void OnDrawImage( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC) override;
};





/// class : XResizableFrameCustomLook
class XResizableFrameCustomLook : public MFrameLook
{
public:
	XResizableFrameCustomLook();


protected:
	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
};


} // namespace mint3
