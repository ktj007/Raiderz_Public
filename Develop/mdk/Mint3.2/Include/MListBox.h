#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>
using namespace std;

namespace mint3
{


class MListBox;
class MScrollBar;
class MLBITEM;





/// class : MLBITEM
class MLBITEM
{
	friend MListBox;


protected:
	string		m_strText;
	DWORD		m_dwItemData;


public:
	MLBITEM() : m_dwItemData( 0)										{}
	MLBITEM( const char* szText) : m_strText( szText), m_dwItemData( 0)	{}
};





/// class : MListBoxLook
class MListBoxLook : public MLookBase
{
/// Member variables
protected:
	int		m_nItemHeight;



/// Member functions
public:
	MListBoxLook();

	void SetItemHeight( int height)						{ m_nItemHeight = height;			}
	int GetItemHeight()									{ return m_nItemHeight;				}

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawItems( MListBox* pListbox, MDrawContext* pDC);
	virtual void OnDrawItemText( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawItemBkgrnd( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawCaret( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawSelectbar( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC);
};








/// class : MListBox
class MListBox : public MWidget
{
/// Member variables
protected:
	vector< MLBITEM*>		m_ItemList;
	set< size_t>			m_SelectedItems;
	int						m_nCaret;
	int						m_nStartSel;
	int						m_nMouseOver;
	MScrollBar*				m_pScrollBar;
	bool					m_bDragAndDrop;
	bool					m_bMultipleSel;
	bool					m_bEnsureCaretVisible;
	bool					m_bUpdateScrollBar;
	bool					m_bReleaseSelOnLBtnUp;



/// Member functions
public:
	MListBox( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MListBox();

	int AddString( const char* szString);
	int InsertString( size_t nIndex, const char* szString);
	int DeleteString( size_t nIndex);
	void ResetContent();
	size_t GetItemCount() const							{ return m_ItemList.size();							}

	void SetItemText( size_t nIndex, const char* szString)	{ m_ItemList[ nIndex]->m_strText = szString;		}
	const char* GetItemText( size_t nIndex) const			{ return m_ItemList[ nIndex]->m_strText.c_str();	}

	void SetItemData( size_t nIndex, DWORD dwItemData)	{ m_ItemList[ nIndex]->m_dwItemData = dwItemData;	}
	DWORD GetItemData( size_t nIndex) const				{ return m_ItemList[ nIndex]->m_dwItemData;			}

	int GetItemHeight();

	// For single-selection
	int SetCurSel( int nIndex =-1);
	int GetCurSel() const								{ return ( m_SelectedItems.empty() ? -1 : (*m_SelectedItems.begin()));				}

	// for multiple-selection
	int SetSel( size_t nIndex, bool bSelect =true);
	bool GetSel( size_t nIndex) const					{ return (m_SelectedItems.find( nIndex) != m_SelectedItems.end()) ? true : false;	}
	size_t GetSelCount() const							{ return m_SelectedItems.size();					}
	int GetSelItem( size_t nSelIndex) const;
	void ReleaseSelect()								{ m_SelectedItems.clear();							}

	// Selection helpers
	int FindString( size_t nStartAfter, const char* szText) const;
	int SelItemRange( size_t first, size_t last, bool bSelect =true);

	void SetCaretPos( int pos);
	int GetCaretPos() const								{ return m_nCaret;									}

	virtual void EnableDragAndDrop( bool bEnable) override	{ m_bDragAndDrop = bEnable;							}
	virtual bool IsEnableDragAndDrop() const override		{ return m_bDragAndDrop;							}

	void EnableMultipleSel( bool bEnable);
	bool IsEnableMultipleSel()							{ return m_bMultipleSel;							}

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override			{ return m_pScrollBar;								}
	void ScrollPage( int delta);



protected:
	int ItemFromPoint( const MPOINT& pt);
	void EnsureCaretVisible()							{ m_bEnsureCaretVisible = true;						}
	void UpdateScrollBar()								{ m_bUpdateScrollBar = true;						}



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
	int			glueAddString( const char* szString)					{ return AddString( szString);					}
	int			glueInsertString( int nIndex, const char* szString)		{ return InsertString( nIndex, szString);		}
	int			glueDeleteString( int nIndex)							{ return DeleteString( nIndex);					}
	void		glueResetContent()										{ ResetContent();								}
	int			glueGetItemCount() const								{ return (int)GetItemCount();					}
	void		glueSetItemText( int nIndex, const char* szString)		{ SetItemText( nIndex, szString);				}
	const char*	glueGetItemText( int nIndex) const						{ return GetItemText( nIndex);					}
	void		glueSetItemData( int nIndex, int nItemData)				{ SetItemData( nIndex, (DWORD)nItemData);		}
	int			glueGetItemData( int nIndex) const						{ return GetItemData( nIndex);					}
	int			glueSetCurSel( int nIndex)								{ return SetCurSel( nIndex);					}
	int			glueGetCurSel() const									{ return GetCurSel();							}
	void		glueSetSel( int nIndex, bool bSelect)					{ SetSel( nIndex, bSelect);						}
	bool		glueGetSel( int nIndex) const							{ return GetSel( nIndex);						}
	int			glueGetSelCount() const									{ return (int)GetSelCount();					}
	int			glueGetSelItem( int nSelIndex) const					{ return GetSelItem( nSelIndex);				}
	void		glueReleaseSelect()										{ ReleaseSelect();								}
	int			glueSelItemRange( int first, int last, bool bSelect)	{ return SelItemRange( first, last, bSelect);	}
	void		glueSetCaretPos( int nPos)								{ SetCaretPos( nPos);							}
	int			glueGetCaretPos() const									{ return GetCaretPos();							}
	int			glueGetMouseOver();
	void		glueEnableMultipleSel( bool bEnable)					{ EnableMultipleSel( bEnable);					}



#define MINT_LISTBOX	"ListBox"
	virtual const char* GetClassName() override			{ return MINT_LISTBOX;								}
};

} // namespace mint3
