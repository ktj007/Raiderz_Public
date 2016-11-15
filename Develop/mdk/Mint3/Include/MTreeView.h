#pragma once

#include "MWidget.h"
#include "MScrollBar.h"
#include "MLookNFeel.h"
#include <vector>
using namespace std;


namespace mint3
{

#define MTREEINDENT		15
	

class MTreeView;
class MTreeItem;
class MTreeItemEx;



// MTreeHitRgn
enum MTreeHitRgn
{
	MHTNOWHERE		= 0,		// 히트 영역 없음
	MHTEXPAND,					// 확장 버튼 영역
	MHTSTRING,					// 스트링 영역
	MHTICON						// 아이콘 영역
};


// struct : MTREEHITINFO
struct MTREEHITINFO
{
	MTreeItem*		m_pItem;
	int				m_nHitRegion;
	MPOINT			m_ptHitPos;

	MTREEHITINFO() : m_pItem( NULL), m_nHitRegion( MHTNOWHERE), m_ptHitPos( MPOINT( 0, 0))	{ }
};





// class : MTreeItem
class MTreeItem
{
// Member variables
protected:
	vector< MTreeItem*>		m_ChildList;		// Child list
	string					m_strString;		// String of item
	int						m_nIconIndex;		// Index of icon
	int						m_nData;			// Data of item
	bool					m_bExpand;			// Expand


// Member functions
public:
	MTreeItem();
	MTreeItem( const char* szString, int nIconIndex);
	virtual ~MTreeItem();

	size_t			GetRootItemCount();
	MTreeItem*		GetRootItem( size_t index);
	
	const char*		GetString();
	int				GetIcon() const;
	int				GetUserData() const;
	bool			IsExpanded() const;
	bool			HasChild() const;
	int				GetCount() const;
	virtual MFont*	GetFont()														{ return NULL;			}
	virtual MCOLOR	GetFontColor( MCOLOR def_color =MCOLOR( 255, 255, 255)) const	{ return def_color;		}
	virtual int		GetHeight() const												{ return 0;				}


protected:
	bool			InsertItem( MTreeItem* pItem);
	bool			DeleteItem( MTreeItem* pItem);

	void			SetString( const char* szString);
	void			SetIcon( int index);
	void			SetUserData( int data);
	void			SetExpand( bool bExpand);
	virtual void	SetFont( MFont* pFont)			{ }
	virtual void	SetFontColor( MCOLOR& color)	{ }
	virtual void	SetHeight( int height)			{ }

	int				GetVisibledItemHeight( int nDefaultHeight);
	MTreeItem* 		GetItemPos( int nDefaultHeight, MPOINT& pos, int x, int& y);					// 해당 위치에 있는 아이템 포인터를 구한다
	bool			GetItemPos( MTreeItem* pItem, int nDefaultHeight, MPOINT& pos, int x, int& y);	// 해당 아이템이 위치 좌표를 구한다
	MTREEHITINFO	GetSelItem( int nDefaultHeight, MPOINT& pos, int dx, int& dy);					// 해당 좌표에 있는 아이템 정보를 구한다


public:
	friend MTreeView;
};



// class : MTreeItemEx (MTreeItem 확장형)
class MTreeItemEx : public MTreeItem
{
// Member variables
protected:
	MFont*					m_pFont;			// Font of item string
	MCOLOR					m_nFontColor;		// Color of font
	unsigned char			m_nHeight;			// Height of item column


// Member functions
public:
	MTreeItemEx();
	MTreeItemEx( const char* szString, int nIconIndex =0, const char* szFont =NULL, MCOLOR nFontColor =MCOLOR( 255, 255, 255), unsigned char nHeight =0);
	virtual ~MTreeItemEx();


public:
	virtual MFont*	GetFont();
	virtual MCOLOR	GetFontColor( MCOLOR def_color =MCOLOR( 255, 255, 255)) const;
	virtual int		GetHeight() const;


protected:
	virtual void	SetFont( MFont* pFont);
	virtual void	SetFontColor( MCOLOR& color);
	virtual void	SetHeight( int height);


public:
	friend MTreeView;
};



// class : MTreeViewLook
class MTreeViewLook
{
public:
	MTreeViewLook();

	virtual void OnDraw( MTreeView* pTreeView, MDrawContext* pDC);
	virtual void OnDrawFrame( MTreeView* pTreeView, MDrawContext* pDC, MRECT& client);
	virtual void OnDrawItem( MTreeView* pTreeView, MTreeItem* pTreeItem, MDrawContext* pDC, MRECT& client, int x, int& y);
	virtual void OnDrawSelectbar( MTreeView* pTreeView, MDrawContext* pDC, MRECT& r);
	virtual MRECT GetClientRect( MTreeView* pTreeView, MRECT& client);

protected:
	int DrawColumnRatioBmp( MDrawContext* pDC, MBitmap* bmp, int x, int y, int column, int align);
};


// MNullTreeViewLook
class MNullTreeViewLook : public MTreeViewLook
{
protected:
	bool		m_bCustomLook;

public:
	virtual void OnDrawFrame( MTreeView* pTreeView, MDrawContext* pDC, MRECT& client)		{ }
};





// class : MTreeVeiw
class MTreeView : public MWidget
{
// Member variables
protected:
	MScrollBar*				m_pScrollbar;			// Scrollbar
	int						m_nScrollbarVisible;

	vector< MTreeItem*>		m_vItemList;			// Item list
	MTreeItem*				m_pSelItem;				// Selected item
	int						m_nItemHeight;			// Height of item
	MBitmap*				m_pImage[ 2];			// Collapsed/Expanded Image
	vector< MBitmap*>		m_vIcon;				// Icons
	MAlignmentMode			m_nAlign;				// Align

	bool					m_bScrollUpdate;



// Member functions
public:
	MTreeView( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MTreeView();

	// 아이템 추가 및 삭제
	MTreeItem* InsertItem( const char* szText, int nIconIndex =0, MTreeItem* pParentItem =NULL);
	MTreeItem* InsertItem( MTreeItem* pItem, MTreeItem* pParentItem =NULL);
	MTreeItem* InsertItemEx( const char* szText, int nIconIndex =0, const char* szFont =NULL, MCOLOR nColor =MCOLOR( 255, 255, 255), unsigned char nHeight =0, MTreeItem* pParentItem =NULL);
	bool DeleteItem( MTreeItem* pItem);

	// 모든 아이템 삭제
	void ResetContents();

	// 루트에 등록된 아이템 개수를 구함
	size_t GetRootItemCount();

	// 루트에 등록된 아이템을 구함
	MTreeItem* GetRootItem( size_t index);


	// 아이템 확장
	void Expand( MTreeItem* pItem, bool bExpand);

	// 아이템 스트링 설정
	void SetItemString( MTreeItem* pItem, const char* szString);

	// 아이템 아이콘 설정
	void SetItemIcon( MTreeItem* pItem, int index);

	// 아이템 데이터 설정
	void SetUserData( MTreeItem* pItem, int data);

	// 아이템 폰트 설정
	void SetItemFont( MTreeItem* pItem, MFont* pFont);

	// 아이템 폰트 색상 설정
	void SetItemFontColor( MTreeItem* pItem, MCOLOR& color);

	// 아이템 높이 설정 및 구함
	void SetItemHeight( MTreeItem* pItem, int height);
	void SetItemHeight( int height);
	int GetItemHeight();

	// Collapsed, Expand 이미지 설정
	void SetCollapsedImage( const char* szAliasName);
	void SetCollapsedImage( MBitmap* pBitmap);
	MBitmap* GetCollapsedImage();
	void SetExpandedImage( const char* szAliasName);
	void SetExpandedImage( MBitmap* pBitmap);
	MBitmap* GetExpandedImage();

	// 아이콘 설정
	int AddIcon( const char* szAliasName);
	int AddIcon( MBitmap* pIcon);
	MBitmap* GetIcon( size_t index);
	size_t GetIconCount();
	void ClearIcon();

	// 아이템을 선택하거나 선택된 아이템을 구함
	void SelectItem( MTreeItem* pItem);
	MTreeItem* GetSelectedItem();

	// 정렬을 설정 및 구함
	void SetAlign( int align);
	MAlignmentMode GetAlign() const;

	// 아이템 개수를 구함
	int GetCount();

	// 현재 보이는 아이템들의 높이를 구함
	int GetVisibledItemHeight();


	// Scrollbar
	void ShowScrollbar( int mode);
	void SetScrollbarLook( MScrollBarLook* pLook);
	void SetScrollbarWidth( int nWidth);
	MScrollBar* GetScrollbar();
	void ScrollPage( int delta);

	// 작업 영역의 크기를 구함
	MRECT GetWorkRect();


	// 아이템 선택
	void SelPrevItem( bool bPostEvent =true);
	void SelNextItem( bool bPostEvent =true);
	void SelPrevPageItem( bool bPostEvent =true);
	void SelNextPageItem( bool bPostEvent =true);
	void SelFirstItem( bool bPostEvent =true);
	void SelLastItem( bool bPostEvent =true);


	// 선택한 아이템이 화면 안에 위치하도록 함
	void EnsureSelItemVisible();


protected:
	void DrawItem( MDrawContext* pDC, MRECT& rect, int x, int& y, MTreeItem* pItem);

	// 스크롤바를 업데이트 하는 함수
	void UpdateScrollbar();

	// 선택한 위치에 해당하는 아이템의 포인터를 구함
	MTREEHITINFO FindSelItemPos( MPOINT& pPos);



// Message handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL);
	virtual void OnRun( void);
//	virtual void OnDraw( MDrawContext* pDC);


// Declare look
protected:
	DECLARE_LOOK( MTreeViewLook)
	DECLARE_NULL_LOOK( MNullTreeViewLook)
	DECLARE_LOOK_CLIENT()



#define MINT_TREEVIEW	"TreeView"
	virtual const char* GetClassName( void)		{ return MINT_TREEVIEW;		}
};


} // namespace mint3
