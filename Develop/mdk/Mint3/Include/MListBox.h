#pragma once

#include "MWidget.h"
#include "CMList.h"
#include "MScrollBar.h"
#include "MColorTable.h"
#include <vector>
using namespace std;

namespace mint3 {

enum MListViewStyle
{
	MVS_LIST = 0,
	MVS_ICON
};

enum MListScrollBarBase
{
	MSB_RIGHT = 0,
	MSB_LEFT,
	MSB_BOTTOM,
	MSB_TOP
};

enum MListFieldType
{
	MLFT_STRING = 0,
	MLFT_BITMAP
};

class MListItem
{
public:
	bool					m_bSelected;
	int						m_nItemType;					// 아이템 타입
	bool					m_bEnabled;

	MListItem(void) 
	{ 
		m_bSelected = false;
		m_nItemType = -1;
		m_bEnabled	= true;
	}
	virtual ~MListItem(void) {}

	virtual const char*		GetString(void) = 0;
	virtual const char*		GetString(int i)
	{
		if(i==0) 
			return	GetString();
		return NULL;
	}
	virtual const char*		GetUserData() { return NULL; }

	virtual void			SetString(int i, const char *szText) {}
	virtual void			SetUserData(const char* szUserData) {}
	virtual void			SetBitmap(int i, MBitmap* pBitmap) {}
	MBitmap*				GetBitmap(void) { return GetBitmap(0); }
	virtual MBitmap*		GetBitmap(int i) { return NULL; }
	virtual void			SetColor(int i, MCOLOR color)	{}
	virtual const MCOLOR	GetColor(void) { return GetColor(0); }
	virtual const MCOLOR	GetColor(int i) { return MColorTable::MLIST_TEXT; }
	virtual void			RemoveItem(int i) {}
	virtual void			InsertItem(int i) {}
	virtual void			SetEnable( bool bEnable =true)		{ m_bEnabled = bEnable;		}
	virtual bool			IsEnabled()							{ return m_bEnabled;		}
	// 드래그 & 드롭을 했을때 전송될 스트링
	virtual bool			GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString) { return false; }
};

class MListFieldItem{
protected:
	MCOLOR		m_Color;
	string		m_strString;
	MBitmap*	m_pBitmap;
public:
	MListFieldItem()
	{
		m_Color = MColorTable::MLIST_TEXT;
		m_pBitmap = NULL;
	}
	MListFieldItem(const char* szString, MCOLOR color) {
		m_Color = color;
		if ( szString != NULL)	m_strString = szString;
		m_pBitmap = NULL;
	}
	MListFieldItem(const char* szString){
		m_Color = MColorTable::MLIST_TEXT;
		if ( szString != NULL)	m_strString = szString;
		m_pBitmap = NULL;
	}
	MListFieldItem(MBitmap* pBitmap){
		m_Color = MColorTable::MLIST_TEXT;
		m_pBitmap = pBitmap;
	}
	virtual ~MListFieldItem(void){
		m_strString.clear();
	}

	virtual const char* GetString(void){
		return m_strString.c_str();
	}
	virtual void SetString(const char* szString){
		if ( szString != NULL)	m_strString = szString;
	}

	void SetColor(MCOLOR color) { m_Color = color; }
	virtual const MCOLOR GetColor() { return m_Color; }

	MBitmap* GetBitmap(void){ return m_pBitmap; }
	void SetBitmap(MBitmap* pBitmap){ m_pBitmap = pBitmap; }
};

class MDefaultListItem : public MListItem
{
protected:
	vector<MListFieldItem*>		m_Items;
	string						m_strUserData;
public:
	MDefaultListItem(void){ }
	MDefaultListItem(const char* szText, const MCOLOR color) 
	{
		MListFieldItem* pNew = new MListFieldItem(szText, color);
		m_Items.push_back(pNew);
	}
	MDefaultListItem(const char* szText)
	{
		MListFieldItem* pNew = new MListFieldItem(szText);
		m_Items.push_back(pNew);
	}
	MDefaultListItem(MBitmap* pBitmap, const char* szText)
	{
		MListFieldItem* pNew = new MListFieldItem(pBitmap);
		m_Items.push_back(pNew);
		pNew = new MListFieldItem(szText);
		m_Items.push_back(pNew);
	}
	virtual ~MDefaultListItem(void)
	{
		if ( m_Items.empty() == false)
		{
			size_t _size = m_Items.size();
			for ( size_t i = 0;  i < _size;  i++)	delete m_Items[i];
			m_Items.clear();
		}
		m_strUserData.clear();
	}
	virtual const char* GetString(void)
	{
		if(!m_Items.empty()) return (m_Items[0])->GetString();
		return NULL;
	}
	virtual const char* GetString(int i)
	{
		if(i < (int)m_Items.size()) return m_Items[i]->GetString();
		return NULL;
	}
	virtual void SetString(int i, const char *szText)
	{
		if((int)m_Items.size() <= i)
		{
			MListFieldItem* pNew = new MListFieldItem(szText);
			m_Items.push_back(pNew);
		}
		else
		{
			m_Items[i]->SetString(szText);
		}
	}

	virtual MBitmap* GetBitmap(int i)
	{
		if(i < (int)m_Items.size()) return m_Items[i]->GetBitmap();
		return NULL;
	}

	virtual void SetColor(int i, MCOLOR color)
	{
		if((int)m_Items.size() > i)
		{
			m_Items[i]->SetColor(color);
		}
	}

	virtual const MCOLOR GetColor() 
	{
		if (!m_Items.empty()) return m_Items[0]->GetColor();
		return MColorTable::MLIST_TEXT;
	}

	virtual const MCOLOR GetColor(int i) 
	{
		if((int)m_Items.size() > i)
		{
			return m_Items[i]->GetColor();
		}

		return MColorTable::MLIST_TEXT;
	}

	virtual void SetUserData(const char* szUserData)
	{
		m_strUserData = szUserData;
	}
	virtual const char* GetUserData()
	{
		return m_strUserData.c_str();
	}
	/*
	virtual int GetBitmapMaxHeight(void){
		int nHeight = 0;
		for(int i=0; i<m_Items.GetCount(); i++){
			MListFieldItem* pItem = m_Items.Get(i);
			MBitmap* pBitmap = pItem->GetBitmap();
			if(pBitmap!=NULL) nHeight = max(nHeight, pBitmap->GetHeight());
		}
		return nHeight;
	}
	*/

	virtual void SetBitmap(int i, MBitmap* pBitmap)
	{
		if((int)m_Items.size() <= i)
		{
			MListFieldItem* pNew = new MListFieldItem(pBitmap);
			m_Items.push_back(pNew);
		}
		else
		{
			m_Items[i]->SetBitmap(pBitmap);
		}
	}

	virtual void RemoveItem(int i)
	{
		if((int)m_Items.size() > i)
		{
			int nCount = 0;
			vector<MListFieldItem *>::iterator it = m_Items.begin();
			while(it != m_Items.end())
			{
				if(i == nCount)
				{
					MListFieldItem* pDelItem = *it;
					delete pDelItem;
					
					m_Items.erase(it);
					return;
				}

				nCount++;
				it++;
			}
		}	
	}

	virtual void InsertItem(int i)
	{
		MListFieldItem* pNew = new MListFieldItem();
		m_Items.insert(m_Items.begin() + i, pNew);
	}
	virtual bool GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString)
	{
		// TODO : 보완 필요
		//m_Items.size();

		*ppDragBitmap = GetBitmap(0);
		if( *ppDragBitmap == NULL)
		{
			for( int i = 0; i < (int)m_Items.size(); i++)
			{
				if(m_Items[i]->GetBitmap())
				{
					*ppDragBitmap = GetBitmap(i);
					break;
				}
			}
		}

		if( m_Items[1]->GetString() == NULL)
		{
			for( int i = 0; i < (int)m_Items.size(); i++)
			{
				if(m_Items[i]->GetString())
				{
					strcpy_s(szDragString, 256, m_Items[i]->GetString());
					strcpy_s(szDragItemString, 256, m_Items[i]->GetString());
					break;
				}
			}
		}
		else
		{
			strcpy_s(szDragString, 256, m_Items[1]->GetString());
			strcpy_s(szDragItemString, 256, m_Items[1]->GetString());
		}

		return true;
	}
};

struct MLISTFIELD{
	string			strFieldName;
	int				nTabSize;
	MListFieldType	nFieldType;
};

class MListBox;
class MListBoxLook{
public:
	bool		m_bItemTextMultiLine;
	MBitmap*	m_pItemSlotBitmap;
protected:
	virtual void OnHeaderDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText);
	virtual void OnDrawSelectBar( MListBox* pListBox, MDrawContext* pDC, MRECT& r, bool bSelected, bool bEnabled, bool bFocus);
	virtual int OnItemDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText, MCOLOR color, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth = 0);
	virtual int OnItemDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, MBitmap* pBitmap, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth = 0);
	virtual void OnFrameDraw(MListBox* pListBox, MDrawContext* pDC);
public:
	MListBoxLook(void);

	virtual void OnDraw(MListBox* pListBox, MDrawContext* pDC);
	virtual MRECT GetClientRect(MListBox* pListBox, MRECT& r);
};

class MNullListBoxLook : public MListBoxLook
{
protected:
	bool		m_bCustomLook;
public:
	virtual void OnFrameDraw(MListBox* pListBox, MDrawContext* pDC) {}
};

typedef void (*ZCB_ONDROP)(void* pSelf, MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

/// 리스트 박스
class MListBox : public MWidget{
protected:
	vector<MListItem*>	m_Items;
	int				m_nOverItem;			// 커서에 의해 가리켜진 아이템
	int				m_nSelItem;				// 선택된 아이템
	int				m_nShowItemCount;		// 현재 리스트에 보여질 수 있는 아이템 개수
	int				m_nStartItemPos;		// 현재 리스트에서 맨 처음 보여지는 아이템
	int				m_nItemHeight;			// Item 높이
	MScrollBar*		m_pScrollBar;
	MListScrollBarBase	m_nScrollBarBase;	// 스크롤바 위치(좌/우, 상/하)

	vector<MLISTFIELD*>	m_Fields;

	bool			m_bVisibleHeader;
	bool			m_bSelected;
	MListViewStyle	m_ViewStyle;
	bool			m_bAlwaysVisibleScrollbar;
	bool			m_bDragAndDrop;
	ZCB_ONDROP		m_pOnDropFunc;

public:
	bool			m_bAbsoulteTabSpacing;	// Field간격이 절대 Pixel값인지, 아님 %값인지.
	bool			m_bHideScrollBar;
	bool			m_bNullFrame;
	bool			m_bMultiSelect;

protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
	virtual bool IsDropable(MWidget* pSender)		{ return m_bDragAndDrop; }

	virtual void RecalcList(void);			// 리스트 박스 레이 아웃 전체 정보 갱신
	virtual void RecalcScrollBar(void);		// 현재 아이템 개수에 따른 스크롤 바 세팅

	int FindNextItem(int i, char c);

	virtual void OnSize(int w, int h);

	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);

	virtual void OnChangeOvered( int nSelItem);

	void Initialize(void);

public:
	MCOLOR m_FontColor;
protected:
	MAlignmentMode m_TextAlign;

public:
	void SetTextAlign( MAlignmentMode align)	{ m_TextAlign = align;		}
	MAlignmentMode GetTextAlign()				{ return m_TextAlign;		}


public:
	MListBox(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
	MListBox(MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MListBox(void);

	void Add(const char* szItem);
	void Add(const char* szItem, MCOLOR color);
	void Add(MListItem* pItem);

	const char* GetString(int i);
	MListItem* Get(int i);

	bool Set(int i, const char* szItem);
	bool Set(int i, MListItem* pItem);

	void Remove(int i);
	void RemoveAll(void);
	bool Swap(int i, int j);

	int GetCount(void);
	int GetSelIndex(void);
	bool SetSelIndex(int i);
	const char* GetSelItemString(void);
	MListItem* GetSelItem(void);

	// multiselect 관련
	int GetSelCount(void);

	int FindItem(MPOINT& p);
	bool GetItemPos(MPOINT* p, int i);

	int GetItemHeight(void);
	void SetItemHeight(int nHeight);

	bool IsShowItem(int i);
	void ShowItem(int nIndex);
	virtual void SetStartItem(int i);
	int GetStartItem(void);
	int GetShowItemCount(void);

	MScrollBar* GetScrollBar(void);

	void Sort(bool bAscend=true);

	// Field Support
	void AddField(const char* szFieldName, int nTabSize, MListFieldType nFieldType = MLFT_STRING);
	void InsertField(int nFieldIndex, const char* szFieldName, int nTabSize, MListFieldType nFieldType);
	void RemoveField(const char* szFieldName);
	MLISTFIELD* GetField(int i);
	int GetFieldCount(void);
	void RemoveAllField(void);

	int AddItem();
	void InsertItem(int nFieldIndex);

	bool SetUserData(int nListIndex, const char* szUserData);
	void SetItemColor(int nListIndex, int nFieldIndex, MCOLOR color);
	bool SetItemString(int nListIndex, int nFieldIndex, const char* szString);
	bool SetItemBitmapName(int nListIndex, int nFieldIndex, const char* dzBitmapName);
	bool SetItemBitmapIndex(int nListIndex, int nFieldIndex, int nBitmapIndex);
	bool SetItemEnable(int nListIndex, bool bEnable=true);
	void RemoveFieldItem(int nFieldIndex);

	bool IsVisibleHeader(void);
	void SetVisibleHeader(bool bVisible);

	bool IsAlwaysVisibleScrollbar(void);
	void SetAlwaysVisibleScrollbar(bool bVisible);

	bool IsSelected() { return m_bSelected; }
	void SetSelected(bool bSelected) { m_bSelected = bSelected; }
	MListViewStyle GetViewStyle() { return m_ViewStyle; }
	void SetViewStyle(MListViewStyle ViewStyle);
	int GetTabSize();
	void EnableDragAndDrop( bool bEnable);

	void SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }

	// 스크롤 위치 설정
	void SetScrollBarBase(MListScrollBarBase nScrollBarBase);
	MListScrollBarBase GetScrollBarBase() { return m_nScrollBarBase; }

	// 스크롤 타입 설정
	void SetScrollBarType(MScrollBarTypes nScrollBartype)	{ m_pScrollBar->SetType(nScrollBartype);	}

	// 스크롤바 Look 설정
	void SetScrollBarLook( MScrollBarLook* pLook)			{ m_pScrollBar->ChangeCustomLook( pLook);	}

	// 스크롤바 두께 설정
	void SetScrollBarWidth( int nWidth)					{ m_pScrollBar->SetWidth( nWidth);			}

	// glue 함수
	void glueAdd(const char* szItem);
	void glueAddField(const char* szFieldName, int nTabSize, MListFieldType nFieldType);
	int glueAddItem();

	void glueSetUserData( int nListIndex, const char* szUserData );
	void glueSetItemColor( int nListIndex, int nFieldIndex, unsigned int color );
	void glueSetItemString(int nListIndex, int nFieldIndex, const char* szString);
	void glueSetItemBitmapName(int nListIndex, int nFieldIndex, const char* szBitmapName);
	void glueSetItemBitmapIndex(int nListIndex, int nFieldIndex, int nBitmapIndex);
	void glueSetItemEnable(int nListIndex, bool bEnable=true);
	int glueGetCount();
	
	void glueRemove(int i);
	void glueRemoveField(const char* szFieldName);
	void glueRemoveAll();
	
	void glueMoveEndLine();

	int				glueGetSelectedIndex();
	void			glueSetSelIndex(int index);

	const char* glueGetSelectedText();
	const char*	glueGetSelUserData();
	const char*	glueGetIndexUserData(int index);

	void glueNoneSelected();
	void glueEnableDragAndDrop(bool bEnable);
	int	glueFindIndex();

	void		glueShowItem(int Index);
	int			glueGetStartItem();

	int			glueGetItemHeight()	{ return GetItemHeight(); }

	/// 기준 스크린 사이즈와의 비율 사용(상대값, 절대값) 유/무
// 	virtual void SetUseRatioToStandardScreenSize(bool bUse);
	// 현재 스크린 사이즈와 기준 스크린 사이즈와의 비율
// 	virtual void SetRatioToStandardScreenWidth(float fRatio);
// 	virtual void SetRatioToStandardScreenHeight(float fRatio);

	DECLARE_LOOK(MListBoxLook)
	DECLARE_NULL_LOOK(MNullListBoxLook)
	DECLARE_LOOK_CLIENT()

#define MINT_LISTBOX	"ListBox"
	virtual const char* GetClassName(void){ return MINT_LISTBOX; }
};

} // namespace mint3
