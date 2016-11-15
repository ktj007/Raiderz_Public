#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>
using namespace std;


namespace mint3
{

#define MTREEINDENT		15
	

class MTreeCtrl;
class MTreeCtrlLook;
class MScrollBar;







/// class : MTreeItem
class MTreeItem
{
	friend MTreeCtrl;
	friend MTreeCtrlLook;


protected:
	vector< MTreeItem*>		m_Items;
	MTreeItem*				m_pParent;
	MTreeItem*				m_pPrevSibling;
	MTreeItem*				m_pNextSibling;
	string					m_strText;
	MBitmap*				m_pImage;
	DWORD					m_dwItemData;
	bool					m_bExpand;
	unsigned int			m_nID;


public:
	MTreeItem();
	virtual ~MTreeItem();


protected:
	bool _add( MTreeItem* pItem);
	bool _insert( size_t nIndex, MTreeItem* pItem);
	bool _delete( MTreeItem* pItem);
	size_t _get_count();
	size_t _get_visible_count();

	MTreeItem* _get_prev_sibling_item()					{ return m_pPrevSibling;	}
	MTreeItem* _get_next_sibling_item()					{ return m_pNextSibling;	}
	MTreeItem* _get_first_item();
	MTreeItem* _get_last_item();
	MTreeItem* _get_prev_item();
	MTreeItem* _get_next_item();
	MTreeItem* _get_first_visible_item();
	MTreeItem* _get_last_visible_item();
	MTreeItem* _get_prev_visible_item();
	MTreeItem* _get_next_visible_item();

	int _get_indent( int indent);

	virtual void _set_height( int height)				{}
	virtual int _get_height() const						{ return 0;					}

	virtual void _set_font( MFont* pFont)				{}
	virtual MFont* _get_font() const					{ return NULL;				}

	virtual void _set_color( MCOLOR& color)				{}
	virtual bool _get_color( MCOLOR& color) const		{ return false;				}
};





/// class : MTreeItemEx
class MTreeItemEx : public MTreeItem
{
	friend MTreeCtrl;
	friend MTreeCtrlLook;


protected:
	int						m_nHeight;
	MFont*					m_pFont;
	MCOLOR					m_nColor;


public:
	MTreeItemEx();


protected:
	virtual void _set_height( int height) override			{ m_nHeight = height;					}
	virtual int _get_height() const override				{ return m_nHeight;						}

	virtual void _set_font( MFont* pFont) override			{ m_pFont = pFont;						}
	virtual MFont* _get_font() const override				{ return m_pFont;						}

	virtual void _set_color( MCOLOR& color) override		{ m_nColor = color;						}
	virtual bool _get_color( MCOLOR& color) const override	{ color = m_nColor;	return true;		}
};






/// class : MTreeCtrlLook
class MTreeCtrlLook : public MLookBase
{
/// Member variables
protected:
	int					m_nIndent;
	int					m_nItemHeight;
	MSIZE				m_ImageSize;



/// Member functions
public:
	MTreeCtrlLook();

	void SetIndent( int indent)							{ m_nIndent = indent;				}
	int GetIndent() const								{ return m_nIndent;					}

	void SetItemHeight( int height)						{ m_nItemHeight = height;			}
	int GetItemHeight()									{ return m_nItemHeight;				}

	void SetImageSize( int w, int h)					{ m_ImageSize = MSIZE( w, h);		}
	MSIZE GetImageSize() const							{ return m_ImageSize;				}


	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;

protected:
	virtual void OnDrawItems( MTreeCtrl* pTreeCtrl, MDrawContext* pDC);
	virtual void OnDrawItemText( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawItemBkgrnd( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCaret( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawExpandIcon( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCollapsedIcon( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawImage( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC);
};









/// class : MTreeVeiw
class MTreeCtrl : public MWidget
{
/// Member variables
protected:
	MTreeItem				m_RootItem;
	set< MTreeItem*>		m_SelectedItems;
	MTreeItem*				m_pCaret;
	MTreeItem*				m_pStartSel;
	MTreeItem*				m_pStartPos;
	MTreeItem*				m_pMouseOver;
	MScrollBar*				m_pScrollBar;
	bool					m_bShowRootItem;
	bool					m_bDragAndDrop;
	bool					m_bMultipleSel;
	bool					m_bEnsureCaretVisible;
	bool					m_bUpdateScrollBar;
	bool					m_bUpdateStartPos;
	bool					m_bReleaseSelOnLBtnUp;
	unsigned int			m_nItemIDCounter;



/// Member functions
public:
	MTreeCtrl( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MTreeCtrl();

	MTreeItem* InsertItem( const char* szText, const char* szImage =NULL, MTreeItem* pParentItem =NULL);
	MTreeItem* InsertItemEx( const char* szText, const char* szImage =NULL, int nHeight =0, const char* szFont =0, MCOLOR& nFontColor =MCOLOR( 255, 255, 255), MTreeItem* pParentItem =NULL);
 	bool DeleteItem( MTreeItem* pItem);
 	void DeleteAllItems();
	size_t GetItemCount();
	size_t GetVisibleCount();

	void SetItemText( MTreeItem* pItem, const char* szText)		{ pItem->m_strText = szText;						}
	const char* GetItemText( MTreeItem* pItem) const			{ return pItem->m_strText.c_str();					}

	void SetItemImage( MTreeItem* pItem, const char* szImage)	{ pItem->m_pImage = MBitmapManager::Get( szImage);	}
	MBitmap* GetItemImage( MTreeItem* pItem) const				{ return pItem->m_pImage;							}

	void SetItemData( MTreeItem* pItem, DWORD dwData) const		{ pItem->m_dwItemData = dwData;						}
	DWORD GetItemData( MTreeItem* pItem) const					{ return pItem->m_dwItemData;						}

	void SetItemHeight( MTreeItem* pItem, int nHeight)			{ pItem->_set_height( nHeight);						}
	int GetItemHeight( MTreeItem* pItem);

	void SetItemFont( MTreeItem* pItem, MFont* pFont)			{ pItem->_set_font( pFont);							}
	MFont* GetItemFont( MTreeItem* pItem);

	void SetItemColor( MTreeItem* pItem, MCOLOR& nColor)		{ pItem->_set_color( nColor);						}
	bool GetItemColor( MTreeItem* pItem, MCOLOR& nColor);

	void Expand( MTreeItem* pItem, bool bExpand);

	void EnsureVisible( MTreeItem* pItem);

	MTreeItem* GetRootItem()									{ return &m_RootItem;								}
	MTreeItem* GetParentItem( MTreeItem* pItem)					{ return pItem->m_pParent;							}

	bool ItemHasChildren( MTreeItem* pItem) const				{ return (pItem->m_Items.empty() ? false : true);	}

	void ShowRootItem( bool bShow);
	bool IsShowRootItem() const									{ return m_bShowRootItem;							}

	MTreeItem* GetPrevSiblingItem( MTreeItem* pItem);
	MTreeItem* GetNextSiblingItem( MTreeItem* pItem);
	MTreeItem* GetFirstVisibleItem();
	MTreeItem* GetLastVisibleItem()								{ return m_RootItem._get_last_visible_item();		}
	MTreeItem* GetPrevVisibleItem( MTreeItem* pItem);
	MTreeItem* GetNextVisibleItem( MTreeItem* pItem);

	// For single-selection
	void SetCurSel( MTreeItem* pItem);
	MTreeItem* GetCurSel() const								{ return ( m_SelectedItems.empty() ? NULL : (*m_SelectedItems.begin()));			}

	// for multiple-selection
	void SetSel( MTreeItem* pItem, bool bSelect =true);
	bool GetSel( MTreeItem* pItem) const						{ return (m_SelectedItems.find( pItem) != m_SelectedItems.end()) ? true : false;	}
	size_t GetSelCount() const									{ return m_SelectedItems.size();					}
	MTreeItem* GetSelItem( size_t sel_index);
	void ReleaseSelect()										{ m_SelectedItems.clear();							}

	void SetCaretPos( MTreeItem* pItem)							{ m_pCaret = pItem;									}
	MTreeItem* GetCaretPos() const								{ return m_pCaret;									}

	MTreeItem* GetStartPos()									{ return m_pStartPos;								}

	virtual void EnableDragAndDrop( bool bEnable) override		{ m_bDragAndDrop = bEnable;							}
	virtual bool IsEnableDragAndDrop() const override			{ return m_bDragAndDrop;							}

	MTreeItem* GetItemByID( int nID);

	void EnableMultipleSel( bool bEnable);
	bool IsEnableMultipleSel()									{ return m_bMultipleSel;							}

	int GetIndent( MTreeItem* pItem);


	// Scrollbar
	virtual MScrollBar* GetScrollBar() override					{ return m_pScrollBar;								}
	void ScrollPage( int delta);


protected:
	MTreeItem* ItemFromPoint( const MPOINT& pt, int* pHitRegion =NULL);
	void EnsureCaretVisible()									{ m_bEnsureCaretVisible = true;						}
	void UpdateScrollBar()										{ m_bUpdateScrollBar = true;						}
	void UpdateStartPos()										{ m_bUpdateStartPos = true;							}



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;
	virtual void OnScrollBarChanged( int nPos) override;
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;
	virtual MWidget* OnToolTip() override;
	virtual void OnLeave() override;



/// LUA script
public:
	int			glueInsertItem( const char* szText, const char* szImage, int nParentItem);
	int			glueInsertItemEx( const char* szText, const char* szImage, int nParentItem);
	bool		glueDeleteItem( int nItem);
	void		glueDeleteAllItems()							{ DeleteAllItems();									}
	int			glueGetItemCount()								{ return (int)GetItemCount();						}
	int			glueGetVisibleCount()							{ return (int)GetVisibleCount();					}
	void		glueSetItemText( int nItem, const char* szText);
	const char*	glueGetItemText( int nItem);
	void		glueSetItemImage( int nItem, const char* szImage);
	const char* glueGetItemImage( int nItem);
	void		glueSetItemData( int nItem, int nData);
	int			glueGetItemData( int nItem);
	void		glueSetItemHeight( int nItem, int nHeight);
	int			glueGetItemHeight( int nItem);
	void		glueSetItemFont( int nItem, const char* szFont);
	void		glueSetItemColor( int nItem, int r, int g, int b);
	void		glueExpand( int nItem, bool bExpand);
	int			glueGetRootItem();
	int			glueGetParentItem( int nItem);
	bool		glueItemHasChildren( int nItem);
	void		glueShowRootItem( bool bShow)					{ ShowRootItem( bShow);								}
	bool		glueIsShowRootItem() const						{ return IsShowRootItem();							}
	int			glueGetPrevSiblingItem( int nItem);
	int			glueGetNextSiblingItem( int nItem);
	int			glueGetFirstVisibleItem();
	int			glueGetLastVisibleItem();
	int			glueGetPrevVisibleItem( int nItem);
	int			glueGetNextVisibleItem( int nItem);
	void		glueSetCurSel( int nItem);
	int			glueGetCurSel();
	void		glueSetSel( int nItem, bool bSelect);
	bool		glueGetSel( int nItem);
	int			glueGetSelCount() const							{ return (int)GetSelCount();						}
	int			glueGetSelItem( int sel_index);
	void		glueReleaseSelect()								{ ReleaseSelect();									}
	void		glueSetCaretPos( int nItem);
	int			glueGetCaretPos();
	int			glueGetMouseOver();
	void		glueEnableMultipleSel( bool bEnable)			{ EnableMultipleSel( bEnable);						}


#define MINT_TREECTRL	"TreeCtrl"
	virtual const char* GetClassName() override					{ return MINT_TREECTRL;								}
};


} // namespace mint3
