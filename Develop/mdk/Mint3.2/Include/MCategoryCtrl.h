#pragma once

#include "MListCtrl.h"
#include "MLookBase.h"
#include <vector>
using namespace std;

namespace mint3
{


class MCategoryCtrl;
class MScrollBar;
class MCCITEM;
class MCCCOLUMN;
class MCCCATEGORY;




// struct : MCCINDEX
struct MCCINDEX
{
	int			nCategory;
	int			nItem;

	MCCINDEX() : nCategory( -1), nItem( -1)										{}
	MCCINDEX( int _category, int _item) : nCategory( _category), nItem( _item)	{}
	void clear()									{ nCategory = -1;		nItem = -1;														}
	bool empty() const								{ return ( (nCategory < 0  &&  nItem < 0) ? true : false);								}
	MCCINDEX& operator = (const MCCINDEX& other)	{ nCategory = other.nCategory;		nItem = other.nItem;	return *this;				}
	bool operator == (const MCCINDEX& other) const	{ return ( (nCategory == other.nCategory  &&  nItem == other.nItem) ? true : false);	}
	bool operator != (const MCCINDEX& other) const	{ return ( (nCategory != other.nCategory  ||  nItem != other.nItem) ? true : false);	}
};





// struct : MCCSUBITEM
struct MCCSUBITEM
{
	string				m_strText;
	MCOLOR*				m_pColor;

	MCCSUBITEM() : m_pColor( NULL)	{}
	virtual ~MCCSUBITEM()			{ if ( m_pColor != NULL) delete m_pColor;	}
};





/// class : MCCITEM
class MCCITEM
{
	friend MCCCATEGORY;
	friend MCategoryCtrl;


protected:
	bool					m_bEnable;
	vector< MCCSUBITEM*>	m_SubItemList;
	string					m_strText;
	MBitmap*				m_pImage;
	MCOLOR*					m_pColor;
	DWORD					m_dwItemData;


public:
	MCCITEM( size_t nCol, const char* szText, MBitmap* pImage);
	virtual ~MCCITEM();

protected:
	void _set_item_text( size_t nSubItem, const char* szText);
	const char* _get_item_text( size_t nSubItem);

	void _set_item_color( size_t nSubItem, MCOLOR& nColor);
	bool _get_item_color( size_t nSubItem, MCOLOR& nColor);
};





/// class : MCCCATEGORY
class MCCCATEGORY
{
	friend MCategoryCtrl;


protected:
	vector< MCCITEM*>		m_Items;
	string					m_strText;
	bool					m_bExpand;


public:
	MCCCATEGORY( const char* szText);
	virtual ~MCCCATEGORY();

protected:
	void _set_text( const char* szText)			{ m_strText = szText;			}
	const char* _get_text() const				{ return m_strText.c_str();		}

	int _add_item( MCCITEM* pItem);
	int _insert_item( size_t nIndex, MCCITEM* pItem);
	bool _delete_item( size_t nIndex);

	size_t _get_count() const					{ return m_Items.size();		}
	bool _is_empty() const						{ return m_Items.empty();		}

	void _set_expand( bool bExpand)				{ m_bExpand = bExpand;			}
	bool _get_expand() const					{ return m_bExpand;				}
};










// class : MLCCOLUMN
class MCCCOLUMN
{
protected:
	string		m_strText;
	int			m_nAlign;
	int			m_nWidth;


public :
	MCCCOLUMN() : m_nAlign( MAM_LEFT | MAM_VCENTER), m_nWidth( 10)		{}
	MCCCOLUMN( const string& strText, int nAlign, int nWidth) : m_strText( strText), m_nAlign( nAlign), m_nWidth( nWidth)		{}

	void SetText( const char* szText)			{ m_strText = szText;				}
	const char* GetText()						{ return m_strText.c_str();			}

	void SetAlign( int nAlign)					{ m_nAlign = nAlign;				}
	int GetAlign()								{ return m_nAlign;					}

	void SetWidth( int nWidth)					{ m_nWidth = nWidth;				}
	int GetWidth()								{ return m_nWidth;					}
};









/// class : MCategoryCtrlLook
class MCategoryCtrlLook : public MLookBase
{
/// Member variables
protected:
	int			m_nIndent;
	int			m_nItemHeight;
	int			m_nCategoryHeight;
	MSIZE		m_ExpandIconSize;
	MSIZE		m_ImageSize;



/// Member functions
public:
	MCategoryCtrlLook();

	void SetIndent( int indent)					{ m_nIndent = indent;				}
	int GetIndent()								{ return m_nIndent;					}

	void SetItemHeight( int height)				{ m_nItemHeight = height;			}
	int GetItemHeight()							{ return m_nItemHeight;				}

	void SetCategoryHeight( int height)			{ m_nCategoryHeight = height;		}
	int GetCategoryHeight()						{ return m_nCategoryHeight;			}

	void SetExpandIconSize( int w, int h)		{ m_ExpandIconSize = MSIZE( w, h);	}
	MSIZE GetExpandIconSize() const				{ return m_ExpandIconSize;			}

	void SetImageSize( int w, int h)			{ m_ImageSize = MSIZE( w, h);		}
	MSIZE GetImageSize() const					{ return m_ImageSize;				}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawItems( MCategoryCtrl* pCategory, MDrawContext* pDC);
	virtual void OnDrawItemText( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, int align, MDrawContext* pDC);
	virtual void OnDrawCategoryBkgrnd( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCategory( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawItemBkgrnd( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCaret( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawExpandIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCollapsedIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawImage( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC);
};












/// class : MCategoryCtrl
class MCategoryCtrl : public MWidget
{
/// Member variables
protected:
	vector< MCCCATEGORY*>	m_Categories;
	vector< MCCCOLUMN*>		m_ColumnList;
	vector< MCCINDEX>		m_SelectedItems;
	MCCINDEX				m_Caret;
	MCCINDEX				m_StartSel;
	MCCINDEX				m_MouseOver;
	MScrollBar*				m_pScrollBar;
	bool					m_bDragAndDrop;
	bool					m_bMultipleSel;
	bool					m_bEnsureCaretVisible;
	bool					m_bUpdateScrollBar;
	bool					m_bReleaseSelOnLBtnUp;



/// Member functions
public:
	MCategoryCtrl( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MCategoryCtrl();

	int AddCategory( const char* szText);
	int InsertCategory( size_t nCategory, const char* szText);
	bool DeleteCategory( size_t nCategory);

	int AddItem( size_t nCategory, const char* szText, const char* szImage =NULL);
	int InsertItem( size_t nCategory, size_t nItem, const char* szText, const char* szImage =NULL);
	bool DeleteItem( size_t nCategory, size_t nItem);
	void DeleteAllItems();

	MCCITEM* GetItem( size_t nCategory, size_t nItem)		{ return m_Categories[ nCategory]->m_Items[ nItem];		}

	size_t GetCategoryCount() const							{ return m_Categories.size();							}
	size_t GetItemCount( size_t nCategory) const			{ return m_Categories[ nCategory]->_get_count();		}

	int AddColumn( const char* szText, int nAlign = MAM_LEFT|MAM_VCENTER, int nWidth =10);
	MCCCOLUMN* GetColumn( size_t nCol)						{ return m_ColumnList[ nCol];							}
	bool DeleteColumn( size_t nCol);
	size_t GetColumnCount() const							{ return m_ColumnList.size();							}

	void SetCategoryText( size_t nCategory, const char* szText)									{ m_Categories[ nCategory]->_set_text( szText);											}
	const char* GetCategoryText( size_t nCategory) const										{ return m_Categories[ nCategory]->_get_text();											}

	void SetCategoryExpand( size_t nCategory, bool bExpand);
	bool GetCategoryExpand( size_t nCategory) const												{ return m_Categories[ nCategory]->_get_expand();										}

	void SetItemEnable( size_t nCategory, size_t nItem, bool bEnable);
	bool GetItemEnable( size_t nCategory, size_t nItem) const;

	void SetItemText( size_t nCategory, size_t nItem, size_t nSubItem, const char* szText)		{ m_Categories[ nCategory]->m_Items[ nItem]->_set_item_text( nSubItem, szText);			}
	const char* GetItemText( size_t nCategory, size_t nItem, size_t nSubItem) const				{ return m_Categories[ nCategory]->m_Items[ nItem]->_get_item_text( nSubItem);			}

	void SetItemImage( size_t nCategory, size_t nItem, const char* szImage)						{ m_Categories[ nCategory]->m_Items[ nItem]->m_pImage = MBitmapManager::Get( szImage);	}
	MBitmap* GetItemImage( size_t nCategory, size_t nItem) const								{ return m_Categories[ nCategory]->m_Items[ nItem]->m_pImage;							}

	void SetItemColor( size_t nCategory, size_t nItem, size_t nSubItem, MCOLOR& nColor)			{ m_Categories[ nCategory]->m_Items[ nItem]->_set_item_color( nSubItem, nColor);		}
	bool GetItemColor( size_t nCategory, size_t nItem, size_t nSubItem, MCOLOR& nColor) const	{ return m_Categories[ nCategory]->m_Items[ nItem]->_get_item_color( nSubItem, nColor);	}

	void SetItemData( size_t nCategory, size_t nItem, DWORD dwItemData)							{ m_Categories[ nCategory]->m_Items[ nItem]->m_dwItemData = dwItemData;					}
	DWORD GetItemData( size_t nCategory, size_t nItem) const									{ return m_Categories[ nCategory]->m_Items[ nItem]->m_dwItemData;						}

	int GetCategoryHeight();
	int GetItemHeight();

	// For single-selection
	MCCINDEX SetCurSel( int nCategory, int nItem);
	MCCINDEX GetCurSel() const;

	// for multiple-selection
	MCCINDEX SetSel( int nCategory, int nItem, bool bSelect =true);
	bool GetSel( int nCategory, int nItem);
	size_t GetSelCount() const								{ return m_SelectedItems.size();						}
	MCCINDEX GetSelItem( size_t nSelIndex) const;
	void ReleaseSelect()									{ m_SelectedItems.clear();								}

	void SetCaretPos( int nCategory, int nItem);
	MCCINDEX GetCaretPos() const							{ return m_Caret;										}

	virtual void EnableDragAndDrop( bool bEnable) override	{ m_bDragAndDrop = bEnable;								}
	virtual bool IsEnableDragAndDrop() const override		{ return m_bDragAndDrop;								}

	void EnableMultipleSel( bool bEnable);
	bool IsEnableMultipleSel()								{ return m_bMultipleSel;								}

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override				{ return m_pScrollBar;									}
	void ScrollPage( int delta);



protected:
	MCCINDEX ItemFromPoint( const MPOINT& pt, int* pHitRegion =NULL);
	void EnsureCaretVisible()								{ m_bEnsureCaretVisible = true;							}
	void UpdateScrollBar()									{ m_bUpdateScrollBar = true;							}



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
	int				glueAddCategory( const char* szText)												{ return AddCategory( szText);								}
	int				glueInsertCategory( int nCategory, const char* szText)								{ return InsertCategory( nCategory, szText);				}
	bool			glueDeleteCategory( int nCategory)													{ return DeleteCategory( nCategory);						}
	int				glueAddItem( int nCategory, const char* szText, const char* szImage)				{ return AddItem( nCategory, szText, szImage);				}
	int				glueInsertItem( int nCategory, int nItem, const char* szText, const char* szImage)	{ return InsertItem( nCategory, nItem, szText, szImage);	}
	bool			glueDeleteItem( int nCategory, int nItem)											{ return DeleteItem( nCategory, nItem);						}
	void			glueDeleteAllItems()																{ DeleteAllItems();											}
	int				glueGetCategoryCount() const														{ return (int)GetCategoryCount();							}
	int				glueGetItemCount( int nCategory) const												{ return (int)GetItemCount( nCategory);						}
	void			glueSetCategoryText( int nCategory, const char* szText)								{ SetCategoryText( nCategory, szText);						}
	const char*		glueGetCategoryText( int nCategory) const											{ return GetCategoryText( nCategory);						}
	void			glueSetCategoryExpand( int nCategory, bool bExpand)									{ SetCategoryExpand( nCategory, bExpand);					}
	bool			glueGetCategoryExpand( int nCategory) const											{ return GetCategoryExpand( nCategory);						}
	void			glueSetItemEnable( int nCategory, int nItem, bool bEnable)							{ SetItemEnable( nCategory, nItem, bEnable);				}
	bool			glueGetItemEnable( int nCategory, int nItem) const									{ return GetItemEnable( nCategory, nItem);					}
	void			glueSetItemText( int nCategory, int nItem, int nSubItem, const char* szText)		{ SetItemText( nCategory, nItem, nSubItem, szText);			}
	const char*		glueGetItemText( int nCategory, int nItem, int nSubItem) const						{ return GetItemText( nCategory, nItem, nSubItem);			}
	void			glueSetItemImage( int nCategory, int nItem, const char* szImage)					{ SetItemImage( nCategory, nItem, szImage);					}
	void			glueSetItemColor( int nCategory, int nItem, int nSubItem, int r, int g, int b)		{ SetItemColor( nCategory, nItem, nSubItem, MCOLOR( r, g, b));	}
	void			glueSetItemData( int nCategory, int nItem, int nItemData)							{ SetItemData( nCategory, nItem, nItemData);				}
	int				glueGetItemData( int nCategory, int nItem) const									{ return GetItemData( nCategory, nItem);					}
	MWLua::ret_int2	glueSetCurSel( int nCategory, int nItem);
	MWLua::ret_int2	glueGetCurSel();
	MWLua::ret_int2	glueSetSel( int nCategory, int nItem, bool bSelect);
	bool			glueGetSel( int nCategory, int nItem)												{ return GetSel( nCategory, nItem);							}
	int				glueGetSelCount() const																{ return (int)GetSelCount();								}
	MWLua::ret_int2	glueGetSelItem( int nSelIndex);
	void			glueReleaseSelect()																	{ ReleaseSelect();											}
	void			glueSetCaretPos( int nCategory, int nItem)											{ SetCaretPos( nCategory, nItem);							}
	MWLua::ret_int2	glueGetCaretPos();
	MWLua::ret_int2	glueGetMouseOver();
	void			glueEnableMultipleSel( bool bEnable)												{ EnableMultipleSel( bEnable);								}





#define MINT_CATEGORYCTRL	"CategoryCtrl"
	virtual const char* GetClassName() override				{ return MINT_CATEGORYCTRL;								}
};

} // namespace mint3
