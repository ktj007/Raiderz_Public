#include "stdafx.h"
#include "MListBox.h"
#include "MColorTable.h"
#include "Mint.h"

namespace mint3 {

#define MLISTBOX_MARGIN_X	2
#define MLISTBOX_MARGIN_Y	2

#define MLISTBOX_DEFAULT_WIDTH	100
#define MLISTBOX_DEFAULT_HEIGHT	100
#define MLISTBOX_ITEM_MARGIN_Y	2
#define MLISTBOX_ITEM_MARGIN_X	2

#define COLORTEXT_SUPPORT


IMPLEMENT_LOOK(MListBox, MListBoxLook)
IMPLEMENT_NULL_LOOK(MListBox, MNullListBoxLook)

int MListBox::GetItemHeight(void)
{
	if(m_nItemHeight>0) return m_nItemHeight;
	return GetFont()->GetHeight()+MLISTBOX_ITEM_MARGIN_Y;
}

void MListBox::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;
}

int MListBox::FindItem(MPOINT& p)
{
	int nItemHeight = GetItemHeight();

	int nHeaderHeight = 0;
	if(IsVisibleHeader()==true) nHeaderHeight = nItemHeight;

	MRECT r = GetClientRect();
	if (m_ViewStyle == MVS_LIST)
	{
		for(int i=0; i<GetCount()-m_nStartItemPos; i++){
			if(!IsShowItem(i+m_nStartItemPos)) break;
			if(MRECT(r.x, r.y+nHeaderHeight+nItemHeight*i, r.w, nItemHeight).InPoint(p)==true){
				return (i+m_nStartItemPos);
			}
		}
	}
	else if (m_ViewStyle == MVS_ICON)
	{
		int nColCount = r.w / GetTabSize();
		int nColIndex, nRowIndex;
		nColIndex = p.x / GetTabSize();
		nRowIndex = p.y / GetItemHeight();
		int nItemIndex = m_nStartItemPos + (nRowIndex * nColCount) + nColIndex;
		if (nItemIndex < GetCount()) return nItemIndex;
	}
	return -1;
}

bool MListBox::GetItemPos(MPOINT* p, int i)
{
	int nFontHeight = GetItemHeight();

	int nHeaderHeight = 0;
	if(IsVisibleHeader()==true) nHeaderHeight = nFontHeight;

	MRECT r = GetClientRect();
	p->x = r.x;
	p->y = r.y + nHeaderHeight + (i-m_nStartItemPos) * nFontHeight;
	/*
	p->x = m_Rect.x + MLISTBOX_MARGIN_X;
	p->y = m_Rect.y + (i-m_nStartItemPos) * nFontHeight;
	*/

	if(i>=m_nStartItemPos && i<m_nStartItemPos+m_nShowItemCount) return true;
	return false;
}

//MListener 건즈1때 이벤트 핸들러로 사용되던것으로써 현재 루아로 대체. 지워주세요...
bool MListBox::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if (MWidget::OnEvent(pEvent, pListener) == true) return true;

 	MRECT r = GetInitialClientRect();

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			if ( r.InPoint( pEvent->Pos) == false)
			{
				if (pListener!=NULL) pListener->OnCommand(this,MEVENT_LB_ITEM_CLICKOUT);
				return false;
			}

			int nSelItem = FindItem(pEvent->Pos);
			if(nSelItem==-1)	return true;

			if ( Get( nSelItem)->IsEnabled() == false)	return true;

			SetSelIndex(nSelItem);
			OnItemSelected(pEvent);

			if(pListener!=NULL) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL);

			// 드래그 & 드롭
			if ( m_bDragAndDrop)
			{
				MListItem* pItem = GetSelItem();
				if( (pItem != NULL) && pItem->IsEnabled())
				{
					MBitmap* pDragBitmap = NULL;
					char szDragString[256] = "";
					char szDragItemString[256] = "";
					if(pItem->GetDragItem(&pDragBitmap, szDragString, szDragItemString)==true){
						Mint::GetInstance()->SetDragObject(this, pDragBitmap, szDragString, szDragItemString);
					}
				}
			}

			return true;
		}

	case MWM_LBUTTONDBLCLK:
		{
			if(r.InPoint(pEvent->Pos)==false) return false;
			int nSelItem = FindItem(pEvent->Pos);
			if(nSelItem==-1) return true;
			if ( Get( nSelItem)->IsEnabled() == false)
				return true;

			m_nSelItem = nSelItem;
			if(pListener!=NULL)
			{
				if(m_nSelItem!=-1) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL);
				else pListener->OnCommand(this, MEVENT_LB_ITEM_SELLOST);
				pListener->OnCommand(this, MEVENT_LB_ITEM_DBLCLK);
			}
			return true;
		}

	case MWM_RBUTTONDOWN:
		{
			if(r.InPoint(pEvent->Pos)==false)
			{
				if(pListener!=NULL) pListener->OnCommand(this,MEVENT_LB_ITEM_CLICKOUT);
				return false;
			}
			int nSelItem = FindItem(pEvent->Pos);
			if(nSelItem==-1) return true;
			if ( Get( nSelItem)->IsEnabled() == false)	return true;

			SetSelIndex(nSelItem);
			OnItemSelected(pEvent);

			if(m_nSelItem!=-1)
			{
				if(pListener!=NULL) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL2);
				return true;
			}

			return true;
		}

	case MWM_MOUSEMOVE:
		{
			if ( r.InPoint( pEvent->Pos) == false)	NULL;

			int nOverItem = FindItem( pEvent->Pos);
			if ( nOverItem != m_nOverItem)	OnChangeOvered( nOverItem);
			m_nOverItem = nOverItem;

			return true;
		}

	case MWM_MOUSEWHEEL:
		{
#define MAX_WHEEL_RANGE	4

			if(r.InPoint(pEvent->Pos)==false) return false;

			if (m_ViewStyle == MVS_LIST)
				SetStartItem(m_nStartItemPos+min(max(-pEvent->nDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE));
			else if (m_ViewStyle == MVS_ICON)
			{
				int nTabSize = GetTabSize();
				int nColCount = r.w / nTabSize;
				int t = (m_nStartItemPos+min(max(-pEvent->nDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE)) * nColCount;
				SetStartItem((m_nStartItemPos+min(max(-pEvent->nDelta, -MAX_WHEEL_RANGE), MAX_WHEEL_RANGE)) * nColCount);
			}

			//뭔가 더체크를 해야되나...
			OnMouseWheel(pEvent);

			return true;
		}

	case MWM_LBUTTONUP:
	case MWM_RBUTTONUP:
	case MWM_RBUTTONDBLCLK:
		{
			if ( r.InPoint(pEvent->Pos) == true)	return true;
			break;
		}

	case MWM_KEYDOWN:
		{
			if(pEvent->nKey==VK_DELETE)
			{
				if(pListener!=NULL) 
					pListener->OnCommand(this, MEVENT_LB_ITEM_DEL);
			}
			else if(pEvent->nKey==VK_UP)
			{
				int nSelIndex = GetSelIndex();
				while ( 1)
				{
					nSelIndex--;
					if ( nSelIndex < 0)
						return true;

					if ( Get( nSelIndex)->IsEnabled())
					{
						SetSelIndex( nSelIndex);
						OnItemSelected(pEvent);
						break;
					}
				}

				ShowItem(GetSelIndex());
				if(pListener!=NULL){
					if(m_nSelItem!=-1) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL);
					else pListener->OnCommand(this, MEVENT_LB_ITEM_SELLOST);
				}
			}
			else if(pEvent->nKey==VK_DOWN)
			{
				int nSelIndex = GetSelIndex();
				while ( 1)
				{
					nSelIndex++;
					if ( nSelIndex >= GetCount())
					{
						SetSelIndex( nSelIndex-1);
						OnItemSelected(pEvent);
						return true;
					}

					if ( Get( nSelIndex)->IsEnabled())
					{
						SetSelIndex( nSelIndex);
						OnItemSelected(pEvent);
						break;
					}
				}

				ShowItem(GetSelIndex());
				if(pListener!=NULL){
					if(m_nSelItem!=-1) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL);
					else pListener->OnCommand(this, MEVENT_LB_ITEM_SELLOST);
				}
			}
			break;
		}

	case MWM_CHAR:
		{
			int nIndex = FindNextItem(GetSelIndex(), pEvent->nKey);
			if(nIndex>=0)
			{
				SetSelIndex(nIndex);
				OnItemSelected(pEvent);
				ShowItem(nIndex);
				if(pListener!=NULL)
				{
					if(m_nSelItem!=-1) pListener->OnCommand(this, MEVENT_LB_ITEM_SEL);
					else pListener->OnCommand(this, MEVENT_LB_ITEM_SELLOST);
				}
			}
			break;
		}
	}

	return false;
}

void MListBox::OnChangeOvered( int nSelItem)
{
/*	MWidget* pToolTip = GetToolTip();

	if ( pToolTip == NULL)
		return;

	if ( nSelItem < 0  ||  nSelItem >= GetCount())
	{
		pToolTip->Show( false);
		return;
	}


	// Get item string
	MListItem* pItem = Get( nSelItem);
	string str;

	for ( int i = 0;  i < GetFieldCount();  i++)
	{
		if ( pItem->GetString( i) == NULL)
			continue;

		str = pItem->GetString( i);
		break;
	}

	if ( str.empty())
		return;


	pToolTip->SetText( str.c_str());
	pToolTip->Show( true);
*/
}

void MListBox::RecalcList(void)
{
	int nItemHeight = GetItemHeight();
	MRECT r = GetClientRect();

	if (m_ViewStyle == MVS_LIST)
	{
		int nHeaderHeight = 0;
		if(IsVisibleHeader()==true) nHeaderHeight = nItemHeight;

		m_nShowItemCount = (r.h-nHeaderHeight) / nItemHeight;
	}
	else if (m_ViewStyle == MVS_ICON)
	{
		int nTabSize = GetTabSize();
		m_nShowItemCount = (r.w / nTabSize) * (r.h / nItemHeight);
	}

	RecalcScrollBar();
}

void MListBox::RecalcScrollBar(void)
{
	if(m_nShowItemCount<GetCount())
	{
		m_pScrollBar->SetRange(0, GetCount()-m_nShowItemCount);
		
		if( !m_bHideScrollBar )
			m_pScrollBar->Show(true);
		
		if(m_bAlwaysVisibleScrollbar)
			m_pScrollBar->Enable(true);
	}
	else{
		m_pScrollBar->SetRange(0, 0);

		if(m_bAlwaysVisibleScrollbar)
			m_pScrollBar->Enable(false);
		else
			m_pScrollBar->Show(false);
	}

}

int MListBox::FindNextItem(int i, char c)
{
	for(int s=0; s<GetCount()-1; s++){
		int idx = (i+s+1)%GetCount();
		const char* szItem = GetString(idx);

		if (szItem != NULL)
		{
			char a = (char)towupper(szItem[0]);
			char b = (char)towupper(c);
			if(a==b) return idx;
		}
	}
	return -1;
}

void MListBox::OnSize(int w, int h)
{
	// 스크롤 위치 설정
/*	MRECT cr = GetInitialClientRect();
	if(m_pScrollBar->IsVisible()==true)
	{
		if(m_nScrollBarBase == MSB_RIGHT)
		{
			// 오른쪽(기본)
//			m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth()-5, cr.y+3, m_pScrollBar->GetDefaultBreadth(), cr.h-6));
			m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
		}
		else if(m_nScrollBarBase == MSB_LEFT)
		{
			// 왼쪽
			m_pScrollBar->SetBounds(MRECT(cr.x+1, cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
		}
	}
	else	// 안보이는 경우 클라이언트 영역이 스크롤바 영역까지 있으므로, 감안해서 계산
	{
		if(m_nScrollBarBase == MSB_RIGHT)
		{
			// 오른쪽(기본)
	//		m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth()-5, cr.y+3, m_pScrollBar->GetDefaultBreadth(), cr.h-6));
			m_pScrollBar->SetBounds(MRECT(cr.x+cr.w-m_pScrollBar->GetDefaultBreadth(), cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
		}
		else if(m_nScrollBarBase == MSB_LEFT)
		{
			// 왼쪽
			m_pScrollBar->SetBounds(MRECT(cr.x, cr.y+1, m_pScrollBar->GetDefaultBreadth(), cr.h-1));
		}
	}
*/

	RecalcList();
}

void MListBox::Initialize(void)
{
	LOOK_IN_CONSTRUCTOR()

	m_nOverItem = -1;
	m_nSelItem = -1;
	m_nStartItemPos = 0;
	m_nShowItemCount = 0;
	m_nItemHeight = -1;
	m_bSelected = true;
	m_pScrollBar = new MScrollBar(this, this);
	m_nScrollBarBase = MSB_RIGHT;
	m_ViewStyle = MVS_LIST;
	m_bDragAndDrop = false;
	m_pOnDropFunc = NULL;

	SetSize(MLISTBOX_DEFAULT_WIDTH, MLISTBOX_DEFAULT_HEIGHT);

	SetFocusEnable(true);

	m_bVisibleHeader = true;

	m_bAbsoulteTabSpacing = true;
	m_bAlwaysVisibleScrollbar = false;
	m_bHideScrollBar = false;
	m_FontColor = MColorTable::MLIST_TEXT;
	m_TextAlign = MAM_NOTALIGN;

	m_bNullFrame = false;
	m_bMultiSelect = false;
}


MListBox::MListBox(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	Initialize();
}

MListBox::MListBox(MWidget* pParent, MListener* pListener)
: MWidget("MListBox", pParent, pListener)
{
	Initialize();
}

MListBox::~MListBox(void)
{
	if ( m_Items.empty() == false)
	{
		size_t _size = m_Items.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_Items[ i];
		m_Items.clear();
	}

	if ( m_Fields.empty() == false)
	{
		size_t _size = m_Fields.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_Fields[ i];
		m_Fields.clear();
	}
	
	if(m_pScrollBar!=NULL)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}

void MListBox::Add(const char* szItem)
{
	MDefaultListItem* pItem = new MDefaultListItem(szItem);
	m_Items.push_back( pItem);

	if(m_Items.size()) {
		if(m_nSelItem == -1)
			m_nSelItem = 0;
	}

	RecalcList();
	RecalcScrollBar();
}

void MListBox::Add(const char* szItem, MCOLOR color)
{
	MDefaultListItem* pItem = new MDefaultListItem(szItem, color);
	m_Items.push_back( pItem);

	RecalcList();
	RecalcScrollBar();
}

void MListBox::Add(MListItem* pItem)
{
	m_Items.push_back( pItem);

	RecalcList();
	RecalcScrollBar();
}

int MListBox::AddItem()
{
	// 필드가 만들어져 있어야 아이템 추가가 가능하다.
	if(m_Fields.size() > 0)
	{
		MDefaultListItem* pItem = new MDefaultListItem();

		// 필드 순서대로 생성한다.
		size_t _size = m_Fields.size();
		for(size_t i = 0; i < _size; i++)
		{
			MListFieldType nFieldType = m_Fields[ i]->nFieldType;

			if(nFieldType == MLFT_STRING)
			{
				pItem->SetString(i, "");
			}
			else if(nFieldType == MLFT_BITMAP)
			{
				pItem->SetBitmap(i, NULL);
			}
		}

		Add(pItem);

		return (int)(m_Items.size() - 1);	//마지막 인덱스
	}

	return -1;
}

void MListBox::InsertItem(int nFieldIndex)
{
	size_t _size = m_Items.size();
	for(size_t i = 0; i < _size; i++) 
	{
		MListItem* pItem = m_Items[ i];
		if(pItem)
			pItem->InsertItem(nFieldIndex);
	}
}

const char* MListBox::GetString(int i)
{
	return m_Items[ i]->GetString();
}

MListItem* MListBox::Get(int i)
{
	if(i<0 || i>=(int)m_Items.size()) return NULL;

	return m_Items[ i];
}

bool MListBox::Set(int i, const char* szItem)
{
	if((int)m_Items.size() <= i) 
	{
		return false;
	}

	MListItem* pItem = m_Items[ i];
	if(pItem)
	{
		pItem->SetString(i, szItem);
	}

	return true;
}

bool MListBox::Set(int i, MListItem* pItem)
{
	if((int)m_Items.size()<=i) return false;

	delete m_Items[ i];
	m_Items[ i] = pItem;

	return true;
}

bool MListBox::SetUserData(int nListIndex, const char* szUserData)
{
	if(szUserData == NULL) return false;
	if(nListIndex < 0 || nListIndex >= (int)m_Items.size()) return false;

	MListItem* pItem = m_Items[ nListIndex];
	if(pItem)
	{
		pItem->SetUserData(szUserData);
	}

	return true;
}

void MListBox::SetItemColor( int nListIndex, int nFieldIndex, MCOLOR color )
{
	if((int)m_Items.size() > nListIndex && -1 < nListIndex) 
	{
		MListItem* pItem = m_Items[ nListIndex];
		if(pItem)
		{
			pItem->SetColor(nFieldIndex, color);
		}
	}
}

bool MListBox::SetItemString(int nListIndex, int nFieldIndex, const char* szString)
{
	if((int)m_Items.size() > nListIndex && -1 < nListIndex) 
	{
		MListItem* pItem = m_Items[ nListIndex];
		if(pItem)
		{
			pItem->SetString(nFieldIndex, szString);
		}
	}

	return true;
}

bool MListBox::SetItemBitmapName(int nListIndex, int nFieldIndex, const char* dzBitmapName)
{
	if((int)m_Items.size() > nListIndex && -1 < nListIndex) 
	{
		MListItem* pItem = m_Items[ nListIndex];
		if(pItem)
		{
			MBitmap * pBitmap = MBitmapManager::Get(dzBitmapName);
			if(pBitmap == NULL)
			{
				// [11/21/2007 isnara] 나중에 여기서 디폴이미지를 넣는다.
			}

			pItem->SetBitmap(nFieldIndex, pBitmap);

			return true;
		}
	}

	return false;
}

bool MListBox::SetItemBitmapIndex(int nListIndex, int nFieldIndex, int nBitmapIndex)
{
	if((int)m_Items.size() > nListIndex && -1 < nListIndex) 
	{
		MListItem* pItem = m_Items[ nListIndex];
		if(pItem)
		{
			MBitmap * pBitmap = MBitmapManager::Get(nBitmapIndex);
			if(pBitmap == NULL)
			{
				// [11/21/2007 isnara] 나중에 여기서 디폴이미지를 넣는다.
			}

			pItem->SetBitmap(nFieldIndex, pBitmap);
			return true;
		}
	}

	return false;
}

bool MListBox::SetItemEnable( int nListIndex, bool bEnable/*=true*/ )
{
	if((int)m_Items.size() > nListIndex && -1 < nListIndex) 
	{
		MListItem* pItem = m_Items[ nListIndex];
		if(pItem)
		{
			pItem->SetEnable(bEnable);
			return true;
		}
	}

	return false;
}

void MListBox::Remove(int i)
{
	if(i<0 || i>=(int)m_Items.size()) return;

	vector<MListItem*>::iterator itr = m_Items.begin();
	for ( int index = 0;  index < i;  index++)	itr++;
	delete m_Items[ i];
	m_Items.erase( itr);

	if(i==m_nSelItem) m_nSelItem = -1;

	RecalcScrollBar();
}

void MListBox::RemoveAll(void)
{
	if ( m_Items.empty() == false)
	{
		size_t _size = m_Items.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_Items[ i];
		m_Items.clear();
	}


	m_nSelItem = -1;
	m_nStartItemPos = 0;
	m_pScrollBar->SetValue(0);
	RecalcScrollBar();


	if ( m_nOverItem > -1)
	{
		m_nOverItem = -1;
		OnChangeOvered( m_nOverItem);
	}
}

void MListBox::RemoveFieldItem(int nFieldIndex)
{
	size_t _size = m_Items.size();
	for(size_t i = 0; i < _size; i++) 
	{
		MListItem* pItem = m_Items[ i];
		if(pItem)
		{
			pItem->RemoveItem(nFieldIndex);
		}
	}


	if ( nFieldIndex == m_nOverItem)
	{
		if ( m_nOverItem > -1)
		{
			m_nOverItem = -1;
			OnChangeOvered( m_nOverItem);
		}
	}
	else if ( nFieldIndex < m_nOverItem)
		OnChangeOvered( m_nOverItem);
}

bool MListBox::Swap(int i, int j)
{
	int _size = m_Items.size();
	if(i<0 || j<0 || i>=_size || j>=_size) return false;

	MListItem* tmp = m_Items[ i];
	m_Items[ i] = m_Items[ j];
	m_Items[ j] = tmp;

	return true;
}

int MListBox::GetCount(void)
{
	return (int)m_Items.size();
}

int MListBox::GetSelIndex(void)
{
	return m_nSelItem;
}

const char* MListBox::GetSelItemString(void)
{
	if(m_nSelItem==-1) return NULL;
	if(m_nSelItem>=(int)m_Items.size()) return NULL;
	return m_Items[ m_nSelItem]->GetString();
}

MListItem* MListBox::GetSelItem(void)
{
	if(m_nSelItem==-1) return NULL;
	return m_Items[ m_nSelItem];
}

bool MListBox::SetSelIndex(int i)
{
	if ((i >= (int)m_Items.size()) || (i<0))
		return false;

	if(!m_bMultiSelect) {
		if(0<=m_nSelItem && m_nSelItem<(int)m_Items.size())
			Get(m_nSelItem)->m_bSelected=false;
	}

	m_nSelItem = i;

	if(!m_bMultiSelect) {
		Get(m_nSelItem)->m_bSelected=true;
	}else
		Get(m_nSelItem)->m_bSelected=!Get(m_nSelItem)->m_bSelected;

	//OnItemSelected(NULL);

	return true;
}

bool MListBox::IsShowItem(int i)
{
	if(i>=m_nStartItemPos && i<m_nStartItemPos+m_nShowItemCount) return true;
	return false;
}

void MListBox::ShowItem(int i)
{
	if(i<m_nStartItemPos){
		m_nStartItemPos = i;
		m_pScrollBar->SetValue(m_nStartItemPos);
	}
	else if(i>=m_nStartItemPos+m_nShowItemCount){
		m_nStartItemPos = i - m_nShowItemCount + 1;
		if(m_nStartItemPos+m_nShowItemCount>GetCount())
			m_nStartItemPos = GetCount()-m_nShowItemCount;
		m_pScrollBar->SetValue(m_nStartItemPos);
	}
}

void MListBox::SetStartItem(int i)
{
	if(GetCount()<=m_nShowItemCount) return;	// 이미 다 보이면 리턴

	if(i<0) i = 0;
	else if(i+m_nShowItemCount>GetCount()) i = GetCount()-m_nShowItemCount;
	m_nStartItemPos = i;
	m_pScrollBar->SetValue(m_nStartItemPos);

	MListener* pListner = GetListener();
	if ( pListner)
		GetListener()->OnCommand(this, MEVENT_LB_ITEM_START);	
}

int MListBox::GetStartItem(void)
{
	return m_nStartItemPos;
}

int MListBox::GetShowItemCount(void)
{
	return m_nShowItemCount;
}

MScrollBar* MListBox::GetScrollBar(void)
{
	return m_pScrollBar;
}

void MListBox::Sort(bool bAscend)
{
//	m_Items.m_bAscend = bAscend;
//	m_Items.Sort();
}

bool MListBox::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if(pWidget==m_pScrollBar && nMsg == MEVENT_LIST_VALUE_CHANGED)
	{
		if (m_ViewStyle == MVS_LIST)
		{
			m_nStartItemPos = m_pScrollBar->GetValue();
		}
		else if (m_ViewStyle == MVS_ICON)
		{
			MRECT r = GetClientRect();
			int nColCount = r.w / GetTabSize();
			int nValue = m_pScrollBar->GetValue() - (m_pScrollBar->GetValue() % nColCount);
			m_nStartItemPos = nValue;
		}
		return true;
	}

	return false;
}

void MListBox::AddField(const char* szFieldName, int nTabSize, MListFieldType nFieldType /* = MLFT_STRING */)
{
	bool bVisibleHeader = IsVisibleHeader();
	MLISTFIELD* pNew = new MLISTFIELD;
	if ( szFieldName != NULL)	pNew->strFieldName = szFieldName;
	pNew->nTabSize = nTabSize;
	pNew->nFieldType = nFieldType;
	m_Fields.push_back( pNew);
	if(bVisibleHeader!=IsVisibleHeader()) RecalcList();	// 헤더가 보이게 되는 경우
}

void MListBox::InsertField(int nFieldIndex, const char* szFieldName, int nTabSize, MListFieldType nFieldType)
{
	bool bVisibleHeader = IsVisibleHeader();
	MLISTFIELD* pNew = new MLISTFIELD;
	if ( szFieldName != NULL)	pNew->strFieldName = szFieldName;
	pNew->nTabSize = nTabSize;
	pNew->nFieldType = nFieldType;

	vector<MLISTFIELD*>::iterator itr = m_Fields.begin();
	for ( int i = 0;  i < nFieldIndex;  i++)	itr++;
	m_Fields.insert( itr, pNew);


	InsertItem(nFieldIndex);

	if(bVisibleHeader!=IsVisibleHeader()) RecalcList();	// 헤더가 보이게 되는 경우
}

void MListBox::RemoveField(const char* szFieldName)
{
	bool bVisibleHeader = IsVisibleHeader();
	for(int i=0; i<(int)m_Fields.size(); i++){
		MLISTFIELD* pField = m_Fields[ i];
		if( pField->strFieldName == string( szFieldName))
		{
			vector<MLISTFIELD*>::iterator itr = m_Fields.begin();
			for ( int index = 0;  index < i;  index++)	itr++;
			delete m_Fields[ i];
			m_Fields.erase( itr);

			if(bVisibleHeader!=IsVisibleHeader()) RecalcList();	// 헤더가 사라지게 되는 경우
			return;
		}
	}
}

void MListBox::RemoveAllField(void)
{
	if ( m_Fields.empty() == false)
	{
		size_t _size = m_Fields.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_Fields[ i];
		m_Fields.clear();
	}
}

MLISTFIELD* MListBox::GetField(int i)
{
	return m_Fields[ i];
}

int MListBox::GetFieldCount(void)
{
	return m_Fields.size();
}

bool MListBox::IsVisibleHeader(void)
{
	if(GetFieldCount()==0) return false;

	return m_bVisibleHeader;
}

void MListBox::SetVisibleHeader(bool bVisible)
{
	m_bVisibleHeader = bVisible;
}

void MListBox::SetViewStyle(MListViewStyle ViewStyle)
{
	m_ViewStyle = ViewStyle;
	if (ViewStyle == MVS_ICON) 
	{
		SetVisibleHeader(false);
	}
	RecalcList();
	RecalcScrollBar();
}

int MListBox::GetTabSize()
{
	if (GetFieldCount() > 0)
	{
		return GetField(0)->nTabSize;
	}

	return GetItemHeight();
}

//////////////////////////////////////////////////////////////////////////
// MListBoxLook
//////////////////////////////////////////////////////////////////////////

void MListBoxLook::OnHeaderDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText)
{
	if(szText==NULL) return;
	pDC->SetColor(MColorTable::MLIST_SELECTEDPLANE);
	pDC->FillRectangle(r);

	pDC->SetColor(MColorTable::MLIST_SELECTEDTEXT);
	pDC->Text( r, szText );
}


void MListBoxLook::OnDrawSelectBar( MListBox* pListBox, MDrawContext* pDC, MRECT& r, bool bSelected, bool bEnabled, bool bFocus)
{
	if(bSelected==true){
		if(bFocus==true) pDC->SetColor(MColorTable::MLIST_SELECTEDPLANE);
		else pDC->SetColor(MColorTable::MLIST_UNFOCUSED_SELECTED_PLANE);
		pDC->FillRectangle(r);
	}
}


int MListBoxLook::OnItemDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText, MCOLOR color, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth )
{
	int nLine = 0;

	if(szText==NULL) return nLine;

	if(bSelected==true) pDC->SetColor(MColorTable::MLIST_SELECTEDTEXT);
	else pDC->SetColor(color);

	if( m_pItemSlotBitmap != NULL )
	{
		pDC->SetBitmap( m_pItemSlotBitmap );
		pDC->Draw(r.x, r.y, r.w, r.h );
	}


#ifdef COLORTEXT_SUPPORT
	nLine = pDC->TextMultiLine2( r, szText, 0, m_bItemTextMultiLine, MAM_LEFT | MAM_VCENTER);
#else
	pDC->Text(r.x, r.y+(r.h-pDC->GetFont()->GetHeight())/2, szText);
	nLine	= 1;
#endif

	return nLine;
}

int MListBoxLook::OnItemDraw(MListBox* pListBox, MDrawContext* pDC, MRECT& r, MBitmap* pBitmap, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth)
{
	int nLine = 0;

	if(pBitmap==NULL) return nLine;

	pDC->SetBitmap(pBitmap);
	pDC->Draw(r.x, r.y, nAdjustWidth, nAdjustWidth);

	nLine = 1;
	return nLine;
}

void MListBoxLook::OnFrameDraw(MListBox* pListBox, MDrawContext* pDC)
{
	MRECT r = pListBox->GetInitialClientRect();
	pDC->SetColor(MColorTable::MLIST_PLANE);
	pDC->FillRectangle(r);
	pDC->SetColor(MColorTable::MLIST_OUTLINE);
	pDC->Rectangle(r);
}

void MListBoxLook::OnDraw(MListBox* pListBox, MDrawContext* pDC)
{
	int nLine = 0;

	// client edge
#ifdef _DEBUG
	if ( string( pListBox->GetText()) == string( "show rect"))
	{
		pDC->SetColor( 255, 255, 255, (int)(25.0 * sin( (double)timeGetTime() * 0.005) + 25.0));
		pDC->Rectangle( 0, 0, pListBox->GetRect().w, pListBox->GetRect().h);
	}
#endif

	if(!pListBox->m_bNullFrame)
		OnFrameDraw(pListBox, pDC);

	int nItemHeight = pListBox->GetItemHeight();
	int nShowCount = 0;

	MRECT r = pListBox->GetClientRect();
	if ( pListBox->GetScrollBar()->IsVisible())
	{
		if ( pListBox->GetScrollBar()->GetType() == MSBT_VERTICAL)
			r.w -= pListBox->GetScrollBar()->IsVisible() ? pListBox->GetScrollBar()->GetWidth() : 0;
		else
			r.h -= pListBox->GetScrollBar()->IsVisible() ? pListBox->GetScrollBar()->GetWidth() : 0;
	}

	int nHeaderHeight = 0;

	pDC->SetFont( pListBox->GetFont() );	
//	pDC->BeginFont();

	if(pListBox->IsVisibleHeader()==true){
		int nFieldStartX = 0;
		for(int i=0; i<pListBox->GetFieldCount(); i++){
			MLISTFIELD* pField = pListBox->GetField(i);
			int nWidth = min(pField->nTabSize, r.w-nFieldStartX);
			if(pListBox->m_bAbsoulteTabSpacing==false) nWidth = r.w*pField->nTabSize/100;
			MRECT ir(r.x+nFieldStartX, r.y, nWidth, nItemHeight);
			OnHeaderDraw(pListBox, pDC, ir, pField->strFieldName.c_str());
			nFieldStartX += (nWidth+1);
			if(nFieldStartX>=r.w) break;
		}
		nHeaderHeight = nItemHeight;
	}

	if (pListBox->GetViewStyle() == MVS_LIST)
	{
		for(int i=pListBox->GetStartItem(); i<pListBox->GetCount(); i++)
		{
			MPOINT p;
			p.x = r.x;
			p.y = r.y+nHeaderHeight+nItemHeight*nShowCount;

			MListItem* pItem = pListBox->Get(i);
			if ( pItem == NULL)
				continue;
			bool bSelected = pItem->m_bSelected;
			bool bFocused = (pListBox->IsFocus());

			// 시작 x, y 위치값 셋팅
			int nFieldStartX = 0;
			if(pListBox->GetScrollBarBase() == MSB_LEFT)
			{
				// 스크롤이 왼쪽이라면
				nFieldStartX += pListBox->GetScrollBar()->GetDefaultBreadth() + 1;
			}

			OnDrawSelectBar( pListBox, pDC, MRECT(p.x, p.y, r.w, nItemHeight), bSelected, pItem->IsEnabled(), bFocused);

			for(int j=0; j<max(pListBox->GetFieldCount(), 1); j++)
			{
				int nTabSize = r.w;
				if(j<pListBox->GetFieldCount()) nTabSize = pListBox->GetField(j)->nTabSize;

				// Width값 구하기
				int	nWidth = min(nTabSize, r.w);
				if(pListBox->m_bAbsoulteTabSpacing==false) nWidth = r.w*nTabSize/100;

				int nAdjustWidth = 0;
				if(pListBox->GetScrollBar()->IsVisible())
				{
					if(pListBox->GetScrollBarBase() == MSB_RIGHT)
						nAdjustWidth = pListBox->GetScrollBar()->GetRect().w + pListBox->GetScrollBar()->GetRect().w/2;
				}
				MRECT ir(p.x+nFieldStartX, p.y, nWidth, nItemHeight);
				const char* szText = pItem->GetString(j);
				MBitmap* pBitmap = pItem->GetBitmap(j);
							
				if(pBitmap!=NULL)//이미지 그리고
					OnItemDraw(pListBox, pDC, ir, pBitmap,  bSelected, pItem->IsEnabled(), bFocused, nWidth);

 				MCOLOR color;
				if( (MCOLOR(pListBox->m_FontColor)) == MColorTable::MLIST_TEXT )
					color = pItem->GetColor();
				else
					color = pListBox->m_FontColor;

				if(szText!=NULL && szText[0]!=0 )// 텍스트 그리고
				{
					nLine = OnItemDraw(pListBox, pDC, ir, szText, color, bSelected, pItem->IsEnabled(), bFocused, nAdjustWidth);
				}

				nFieldStartX += nWidth;
				if(nFieldStartX>=r.w) break;
			}

			nShowCount += nLine;

			if(nShowCount>=pListBox->GetShowItemCount()) break;
		}
	}
	else if (pListBox->GetViewStyle() == MVS_ICON)
	{
		MPOINT p;
		p.x = r.x;
		p.y = r.y + nHeaderHeight;

		int nStartX = 0, nStartY = 0;
		MSIZE TabSize(r.w, r.h);

		if(pListBox->GetFieldCount() > 0) 
		{
			TabSize.w = pListBox->GetField(0)->nTabSize;
			TabSize.h = pListBox->GetItemHeight();
		}

		for(int i=pListBox->GetStartItem(); i<pListBox->GetCount(); i++)
		{
			MListItem* pItem = pListBox->Get(i);
			bool bSelected = pItem->m_bSelected;
			bool bFocused = (pListBox->IsFocus());

			int nWidth = min(TabSize.w, r.w - nStartX);

			int nAdjustWidth = 0;
			if(pListBox->GetScrollBar()->IsVisible())
			{
				nAdjustWidth = pListBox->GetScrollBar()->GetRect().w + pListBox->GetScrollBar()->GetRect().w/2;
			}

			MRECT ir(p.x+nStartX, p.y+nStartY, nWidth, nItemHeight);
			const char* szText = pItem->GetString(0);
			MBitmap* pBitmap = pItem->GetBitmap(0);
			const MCOLOR color = pItem->GetColor(0);
			if(szText!=NULL)
				OnItemDraw(pListBox, pDC, ir, szText, color, bSelected, pItem->IsEnabled(), bFocused, nAdjustWidth);
			else if(pBitmap!=NULL)
				OnItemDraw(pListBox, pDC, ir, pBitmap,  bSelected, pItem->IsEnabled(), bFocused, nAdjustWidth);

			nStartX += TabSize.w;
			if(nStartX >= (r.w - TabSize.w )) 
			{
				nStartX = 0;
				nStartY += nItemHeight + 5;
			}

			nShowCount++;
			if(nShowCount>=pListBox->GetShowItemCount()) break;
		}
	}

//	pDC->EndFont();
}

MRECT MListBoxLook::GetClientRect(MListBox* pListBox, MRECT& r)
{
	return r;
//	return MRECT(r.x+1, r.y+1, (pListBox->GetScrollBar()->IsVisible()==true)?(r.w - pListBox->GetScrollBar()->GetClientRect().w-2):(r.w-2), r.h-2);
}

MListBoxLook::MListBoxLook(void)
{
	m_bItemTextMultiLine			= false;
	m_pItemSlotBitmap				= NULL;
}

bool MListBox::IsAlwaysVisibleScrollbar(void)
{
	return m_bAlwaysVisibleScrollbar;
}

void MListBox::SetAlwaysVisibleScrollbar(bool bVisible)
{
	m_bAlwaysVisibleScrollbar=bVisible;
	if(m_bAlwaysVisibleScrollbar)
		m_pScrollBar->Show(true);
}

void MListBox::EnableDragAndDrop( bool bEnable)
{
	m_bDragAndDrop = bEnable;
}

bool MListBox::OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	MWidget::OnDrop(pSender, pBitmap, szString, szItemString);

	if ( m_pOnDropFunc != NULL)
	{
		m_pOnDropFunc(this, pSender, pBitmap, szString, szItemString);
	}

	return true;
}

void MListBox::glueAdd(const char* szItem)
{
	if (szItem==NULL) return;
	Add(szItem);
}

int MListBox::glueGetCount()
{
	return GetCount();
}

void MListBox::glueRemove(int i)
{
	Remove(i);
}

void MListBox::glueRemoveAll()
{
	RemoveAll();
}

void MListBox::glueMoveEndLine()
{
	m_pScrollBar->MoveEndLine();
}

int MListBox::glueGetSelectedIndex()
{
	return GetSelIndex();
}

const char* MListBox::glueGetSelectedText()
{
	return GetSelItemString();
}

void MListBox::SetScrollBarBase(MListScrollBarBase nScrollBarBase) 
{
	m_nScrollBarBase = nScrollBarBase;

	OnSize(m_Rect.w, m_Rect.h);
}

// void MListBox::SetUseRatioToStandardScreenSize(bool bUse) 
// {
// 	MWidget::SetUseRatioToStandardScreenSize(bUse);
// 
// 	m_pScrollBar->SetUseRatioToStandardScreenSize(bUse);
// }

// void MListBox::SetRatioToStandardScreenWidth(float fRatio) 
// { 
// 	MWidget::SetRatioToStandardScreenWidth(fRatio);
// 
// 	m_pScrollBar->SetRatioToStandardScreenWidth(fRatio);
// }

// void MListBox::SetRatioToStandardScreenHeight(float fRatio) 
// { 
// 	MWidget::SetRatioToStandardScreenHeight(fRatio);
// 
// 	m_pScrollBar->SetRatioToStandardScreenHeight(fRatio);
// }

const char* MListBox::glueGetSelUserData()
{
	return glueGetIndexUserData(GetSelIndex());
}

const char* MListBox::glueGetIndexUserData(int index)
{
	MListItem* pItem = Get(index);
	if( pItem==NULL) return NULL;

	return pItem->GetUserData();
}

void MListBox::glueAddField( const char* szFieldName, int nTabSize, MListFieldType nFieldType )
{
	AddField(szFieldName, nTabSize, nFieldType);
}

void MListBox::glueRemoveField( const char* szFieldName )
{
	RemoveField(szFieldName);
}

int MListBox::glueAddItem()
{
	return AddItem();
}

void MListBox::glueSetUserData( int nListIndex, const char* szUserData )
{
	SetUserData(nListIndex, szUserData);
}

void MListBox::glueSetItemColor( int nListIndex, int nFieldIndex, unsigned int color )
{
	unsigned long int t = ((((unsigned long int)255)&0xFF)<<24) | color;

	SetItemColor(nListIndex, nFieldIndex, t);
}

void MListBox::glueSetItemString( int nListIndex, int nFieldIndex, const char* szString )
{
	if( szString == NULL)	szString = "NULL";

	SetItemString(nListIndex, nFieldIndex, szString);
}

void MListBox::glueSetItemBitmapName( int nListIndex, int nFieldIndex, const char* szBitmapName )
{
	SetItemBitmapName(nListIndex, nFieldIndex, szBitmapName);
}

void MListBox::glueSetItemBitmapIndex( int nListIndex, int nFieldIndex, int nBitmapIndex )
{
	SetItemBitmapIndex(nListIndex, nFieldIndex, nBitmapIndex);
}

void MListBox::glueNoneSelected()
{
	if(!m_bMultiSelect) {
		if(0<=m_nSelItem && m_nSelItem<(int)m_Items.size())
			Get(m_nSelItem)->m_bSelected=false;
	}

	m_nSelItem = -1;
}

void MListBox::glueSetItemEnable( int nListIndex, bool bEnable/*=true*/ )
{
	SetItemEnable(nListIndex, bEnable);
}

void MListBox::glueEnableDragAndDrop( bool bEnable )
{
	EnableDragAndDrop(bEnable);
}

void MListBox::glueSetSelIndex(int index)
{
	SetSelIndex(index);
}

int MListBox::glueFindIndex()
{
	MEvent LocalEvent;
	MEvent Event;
	Event.Pos.x = m_DragEventArgs.m_nX;
	Event.Pos.y = m_DragEventArgs.m_nY;

	MakeLocalPos(&LocalEvent, &Event);

	return FindItem(LocalEvent.Pos);
}

//왜 이렇게 계산해서 넣어야 하나....
void MListBox::glueShowItem( int Index )
{
	ShowItem(Index+m_nShowItemCount-1);
}

int MListBox::glueGetStartItem()
{
	return GetStartItem();
}

}// namespace mint3