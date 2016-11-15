#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>
using namespace std;

namespace mint3
{


class MListCtrl;
class MScrollBar;
class MLCITEM;
class MLCCOLUMN;



enum MLISTCTRL_VIEWSTYLE
{
	MLCVS_ICON		= 0,
	MLCVS_LIST,
	MLCVS_REPORT
};






// struct : MLCSUBITEM
struct MLCSUBITEM
{
	string				m_strText;
	MCOLOR*				m_pColor;

	MLCSUBITEM() : m_pColor( NULL)	{}
	virtual ~MLCSUBITEM()			{ if ( m_pColor != NULL) delete m_pColor;	}
};





/// class : MLCITEM
class MLCITEM
{
	friend MListCtrl;


protected:
	bool					m_bEnable;
	vector< MLCSUBITEM*>	m_SubItemList;
	string					m_strText;
	MBitmap*				m_pImage;
	MCOLOR*					m_pColor;
	DWORD					m_dwItemData;


public:
	MLCITEM( size_t nCol, const char* szText, MBitmap* pImage);
	virtual ~MLCITEM();

protected:
	void _set_item_text( size_t nSubItem, const char* szText);
	const char* _get_item_text( size_t nSubItem);

	void _set_item_color( size_t nSubItem, MCOLOR& nColor);
	bool _get_item_color( size_t nSubItem, MCOLOR& nColor);
};









// class : MLCCOLUMN
class MLCCOLUMN
{
protected:
	string		m_strText;
	int			m_nAlign;
	int			m_nWidth;


public :
	MLCCOLUMN() : m_nAlign( MAM_LEFT | MAM_VCENTER), m_nWidth( 10)		{}
	MLCCOLUMN( const string& strText, int nAlign, int nWidth) : m_strText( strText), m_nAlign( nAlign), m_nWidth( nWidth)		{}

	void SetText( const char* szText)			{ m_strText = szText;				}
	const char* GetText()						{ return m_strText.c_str();			}

	void SetAlign( int nAlign)					{ m_nAlign = nAlign;				}
	int GetAlign()								{ return m_nAlign;					}

	void SetWidth( int nWidth)					{ m_nWidth = nWidth;				}
	int GetWidth()								{ return m_nWidth;					}
};









/// class : MListCtrlLook
class MListCtrlLook : public MLookBase
{
/// Member variables
protected:
	int		m_nItemHeight;
	int		m_nColumnHeaderHeight;
	MSIZE	m_LargeImageSize;
	MSIZE	m_SmallImageSize;



/// Member functions
public:
	MListCtrlLook();

	void SetItemHeight( int height)				{ m_nItemHeight = height;			}
	int GetItemHeight()							{ return m_nItemHeight;				}

	void SetColumnHeaderHeight( int height)		{ m_nColumnHeaderHeight = height;	}
	int GetColumnHeaderHeight() const			{ return m_nColumnHeaderHeight;		}

	void SetLargeImageSize( int w, int h)		{ m_LargeImageSize = MSIZE( w, h);	}
	MSIZE GetLargeImageSize() const				{ return m_LargeImageSize;			}

	void SetSmallImageSize( int w, int h)		{ m_SmallImageSize = MSIZE( w, h);	}
	MSIZE GetSmallImageSize() const				{ return m_SmallImageSize;			}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawItems( MListCtrl* pListCtrl, MDrawContext* pDC);
	virtual void OnDrawItemText( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, int align, MDrawContext* pDC);
	virtual void OnDrawColumnHeader( MListCtrl* pListCtrl, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawItemBkgrnd( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCaret( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawImage( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC);
};








/// class : MListCtrl
class MListCtrl : public MWidget
{
/// Member variables
protected:
	vector< MLCITEM*>		m_ItemList;
	vector< MLCCOLUMN*>		m_ColumnList;
	set< size_t>			m_SelectedItems;
	int						m_nViewStyle;
	int						m_nHorzMargin, m_nVertMargin;
	bool					m_bShowColumnHeader;
	int						m_nCaret;
	int						m_nStartSel;
	int						m_nMouseOver;
	MScrollBar*				m_pScrollBar;
	bool					m_bDragAndDrop;
	bool					m_bMultipleSel;
	bool					m_bAlwaysSel;
	bool					m_bEnsureCaretVisible;
	bool					m_bUpdateScrollBar;
	bool					m_bReleaseSelOnLBtnUp;



/// Member functions
public:
	MListCtrl( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MListCtrl();

	int AddItem( const char* szText, const char* szImage =NULL);
	int InsertItem( size_t nIndex, const char* szText, const char* szImage =NULL);
	bool DeleteItem( size_t nIndex);
	void DeleteAllItems();
	size_t GetItemCount() const								{ return m_ItemList.size();							}

	int AddColumn( const char* szText, int nAlign = MAM_LEFT|MAM_VCENTER, int nWidth =10);
	MLCCOLUMN* GetColumn( size_t nCol)						{ return m_ColumnList[ nCol];						}
	bool DeleteColumn( size_t nCol);
	size_t GetColumnCount() const							{ return m_ColumnList.size();						}

	void SetItemEnable( size_t nIndex, bool bEnable);
	bool GetItemEnable( size_t nIndex) const				{ return m_ItemList[ nIndex]->m_bEnable;			}

	void SetItemText( size_t nIndex, size_t nSubItem, const char* szText)		{ m_ItemList[ nIndex]->_set_item_text( nSubItem, szText);			}
	const char* GetItemText( size_t nIndex, size_t nSubItem) const				{ return m_ItemList[ nIndex]->_get_item_text( nSubItem);			}

	void SetItemImage( size_t nIndex, const char* szImage)						{ m_ItemList[ nIndex]->m_pImage = MBitmapManager::Get( szImage);	}
	MBitmap* GetItemImage( size_t nIndex) const									{ return m_ItemList[ nIndex]->m_pImage;								}

	void SetItemColor( size_t nIndex, size_t nSubItem, MCOLOR& nColor)			{ m_ItemList[ nIndex]->_set_item_color( nSubItem, nColor);			}
	bool GetItemColor( size_t nIndex, size_t nSubItem, MCOLOR& nColor) const	{ return m_ItemList[ nIndex]->_get_item_color( nSubItem, nColor);	}

	void SetItemData( size_t nIndex, DWORD dwItemData)		{ m_ItemList[ nIndex]->m_dwItemData = dwItemData;	}
	DWORD GetItemData( size_t nIndex) const					{ return m_ItemList[ nIndex]->m_dwItemData;			}

	void SetColumnText( size_t nIndex, const char* szText)	{ m_ColumnList[ nIndex]->SetText( szText);			}
	const char* GetColumnText( size_t nIndex) const			{ return m_ColumnList[ nIndex]->GetText();			}

	void SetColumnAlign( size_t nIndex, int nAlign)			{ m_ColumnList[ nIndex]->SetAlign( nAlign);			}
	int GetColumnAlign( size_t nIndex) const				{ return m_ColumnList[ nIndex]->GetAlign();			}

	void SetWidth( size_t nIndex, int nWidth)				{ m_ColumnList[ nIndex]->SetWidth( nWidth);			}
	int GetWidth( size_t nIndex) const						{ return m_ColumnList[ nIndex]->GetWidth();			}

	int GetItemHeight();
	int GetColumnHeaderHeight();

	void SetViewStyle( int nViewStyle);
	int GetViewStyle() const								{ return m_nViewStyle;								}

	void ShowColumnHeader( bool bShow);
	bool IsShowColumnHeader() const							{ return m_bShowColumnHeader;						}

	void SetAlwaysSel( bool bSel);
	bool GetAlwaysSel() const;

	void SetItemMargin( int nHorzMargin, int nVertMargin);
	void GetItemMargin( int* pnHorzMargin, int* pnVertMargin);

	// For single-selection
	int SetCurSel( int nIndex =-1);
	int GetCurSel() const									{ return ( m_SelectedItems.empty() ? -1 : (*m_SelectedItems.begin()));				}

	// for multiple-selection
	int SetSel( size_t nIndex, bool bSelect =true);
	bool GetSel( size_t nIndex) const						{ return (m_SelectedItems.find( nIndex) != m_SelectedItems.end()) ? true : false;	}
	size_t GetSelCount() const								{ return m_SelectedItems.size();					}
	int GetSelItem( size_t nSelIndex) const;
	void ReleaseSelect();

	// Selection helpers
	int SelItemRange( size_t first, size_t last, bool bSelect =true);

	void SetCaretPos( int nPos);
	int GetCaretPos() const									{ return m_nCaret;									}

	virtual void EnableDragAndDrop( bool bEnable) override	{ m_bDragAndDrop = bEnable;							}
	virtual bool IsEnableDragAndDrop() const override		{ return m_bDragAndDrop;							}

	void EnableMultipleSel( bool bEnable);
	bool IsEnableMultipleSel()								{ return m_bMultipleSel;							}

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override				{ return m_pScrollBar;								}
	bool ScrollPage( int delta);



protected:
	int ItemFromPoint( const MPOINT& pt);
	void EnsureCaretVisible()								{ m_bEnsureCaretVisible = true;						}
	void UpdateScrollBar()									{ m_bUpdateScrollBar = true;						}



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;
	virtual MWidget* OnToolTip() override;
	virtual void OnLeave() override;



/// LUA script
public:
	int				glueAddItem( const char* szText, const char* szImage)	{ return AddItem( szText, szImage);				}
	int				glueInsertItem( int nIndex, const char* szText, const char* szImage)	{ return InsertItem( nIndex, szText, szImage);			}
	bool			glueDeleteItem( int nIndex)								{ return DeleteItem( nIndex);					}
	void			glueDeleteAllItems()									{ DeleteAllItems();								}
	int				glueGetItemCount() const								{ return (int)GetItemCount();					}
	void			glueSetItemEnable( int nIndex, bool bEnable)			{ SetItemEnable( nIndex, bEnable);				}
	bool			glueGetItemEnable( int nIndex) const					{ return GetItemEnable( nIndex);				}
	void			glueSetItemText( int nIndex, int nSubItem, const char* szText)			{ SetItemText( nIndex, nSubItem, szText);				}
	const char*		glueGetItemText( int nIndex, int nSubItem) const		{ return GetItemText( nIndex, nSubItem);		}
	void			glueSetItemImage( int nIndex, const char* szImage)		{ SetItemImage( nIndex, szImage);				}
	void			glueSetItemColor( int nIndex, int nSubItem, int r, int g, int b)		{ SetItemColor( nIndex, nSubItem, MCOLOR( r, g, b));	}
	void			glueSetItemData( int nIndex, int nItemData)				{ SetItemData( nIndex, nItemData);				}
	int				glueGetItemData( int nIndex) const						{ return GetItemData( nIndex);					}
	void			glueSetColumnText( int nIndex, const char* szText)		{ SetColumnText( nIndex, szText);				}
	const char*		glueGetColumnText( int nIndex) const					{ return GetColumnText( nIndex);				}
	void 			glueSetColumnAlign( int nIndex, const char* szHor, const char* szVer);
	int 			glueGetColumnAlign( int nIndex) const					{ return GetColumnAlign( nIndex);				}
	void 			glueSetColumnWidth( int nIndex, int nWidth)				{ SetWidth( nIndex, nWidth);					}
	int 			glueGetColumnWidth( int nIndex) const					{ return GetWidth( nIndex);						}
	void			glueSetViewStyle( int nViewStyle)						{ SetViewStyle( nViewStyle);					}
	int				glueSetCurSel( int nIndex)								{ return SetCurSel( nIndex);					}
	int				glueGetCurSel() const									{ return GetCurSel();							}
	int				glueSetSel( int nIndex, bool bSelect)					{ return SetSel( nIndex, bSelect);				}
	bool			glueGetSel( int nIndex) const							{ return GetSel( nIndex);						}
	int				glueGetSelCount() const									{ return (int)GetSelCount();					}
	int				glueGetSelItem( int nSelIndex) const					{ return GetSelItem( nSelIndex);				}
	void			glueReleaseSelect()										{ ReleaseSelect();								}
	int				glueSelItemRange( int first, int last, bool bSelect)	{ return SelItemRange( first, last, bSelect);	}
	void			glueSetCaretPos( int nPos)								{ SetCaretPos( nPos);							}
	int				glueGetCaretPos() const									{ return GetCaretPos();							}
	int				glueGetMouseOver();
	void			glueSetAlwaysSel( bool bSel)							{ SetAlwaysSel( bSel);							}
	bool			glueGetAlwaysSel()										{ return GetAlwaysSel();						}
	void			glueSetItemMargin( int horz, int vert)					{ SetItemMargin( horz, vert);					}
	MWLua::ret_int2	glueGetItemMargin() const								{ return MWLua::ret_int2( m_nHorzMargin, m_nVertMargin);				}
	void			glueShowColumnHeader( bool bShow)						{ ShowColumnHeader( bShow);						}
	void			glueEnableMultipleSel( bool bEnable)					{ EnableMultipleSel( bEnable);					}





#define MINT_LISTCTRL	"ListCtrl"
	virtual const char* GetClassName() override				{ return MINT_LISTCTRL;								}
};

} // namespace mint3
