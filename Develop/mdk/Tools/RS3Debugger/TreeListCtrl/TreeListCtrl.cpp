/*
 * Implementation of the TreeListCtrl
 *
 * Copyright (c)1999 David Carballo, All Right Reserved
 *
 * Created June 1999
 * Revised: August 1999
 * Written by David Carballo
 * Internet: fibeto@redestb.es
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 */

#include "stdafx.h"
#include "TreeListCtrl.h"

#include "..\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTreeListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CTreeListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClick)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl

CTreeListCtrl::CTreeListCtrl()
:m_HScrollPos(0)
{
}

CTreeListCtrl::~CTreeListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl overrides

void CTreeListCtrl::PreSubclassWindow() 
{
	// LVS_EX_GRIDLINES // 5.0: LVS_EX_LABELTIP 
	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	if (NULL == m_ilState.GetSafeHandle())
	{
		m_ilState.Create(IDB_STATE, 16, 1, RGB(255, 0, 0));
	}
	SetImageList(&m_ilState, LVSIL_STATE);
	
	CListCtrl::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl operations

BOOL CTreeListCtrl::Create
	(
	DWORD			dwStyle,
	const RECT&		rect,
	CWnd*			pParentWnd,
	UINT			nID
	)
{
	if (!CListCtrl::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	// LVS_EX_GRIDLINES // 5.0: LVS_EX_LABELTIP 
	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	if (NULL == m_ilState.GetSafeHandle())
	{
		m_ilState.Create(IDB_STATE, 16, 1, RGB(255, 0, 0));
	}
	SetImageList(&m_ilState, LVSIL_STATE);

	return TRUE;
}

ITEM_INDEX CTreeListCtrl::AddItem
	(
	LPCTSTR		lpszItem,
	int			nImage,
	int			nLevel
	)
{
	tree_list_info	info;
	int				nItem;

	ITEM_INDEX Itemindex;
	
	Itemindex.nTreeIndex = nItem = m_tree.size();
	info.m_nLevel	= nLevel;
	info.m_nImage	= nImage;
	info.m_lstCols.resize(GetHeaderCtrl()->GetItemCount());
	info.m_lstCols[0] = lpszItem;
	info.m_bCollapsed = true;
	m_tree.push_back(info);

	if (info.m_nLevel == 0)
	{
		LVITEM			item;

		item.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT | LVIF_STATE | LVIF_PARAM;
		item.iItem		= nItem;
		item.iSubItem	= 0;
		item.pszText	= (LPTSTR)(LPCTSTR)info.m_lstCols[0];
		item.iImage		= info.m_nImage;
		item.iIndent	= info.m_nLevel;
		item.state		= INDEXTOSTATEIMAGEMASK(info.m_bCollapsed ? 1 : 2);
		item.stateMask	= LVIS_STATEIMAGEMASK;
		item.lParam		= nItem;

		Itemindex.nListIndex = CListCtrl::InsertItem(&item);
	}

	return Itemindex;
}

BOOL  CTreeListCtrl::SetItemText
	(
	//int			nItem,
	ITEM_INDEX	nItem,
	int			nSubItem,
	LPCTSTR		lpszText
	)
{
	m_tree.at(nItem.nTreeIndex).m_lstCols[nSubItem] = lpszText;
	return CListCtrl::SetItemText(nItem.nListIndex, nSubItem, lpszText);
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl protected functions

int CTreeListCtrl::InsertItem
	(
	int				nItem,
	int				nPosArray,
	tree_list_info&	info
	)
{
	LVITEM			item;

	item.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT | LVIF_STATE | LVIF_PARAM;
	item.iItem		= nItem;
	item.iSubItem	= 0;
	item.pszText	= (LPTSTR)(LPCTSTR)info.m_lstCols[0];
	item.iImage		= info.m_nImage;
	item.iIndent	= info.m_nLevel;
	item.state		= INDEXTOSTATEIMAGEMASK(info.m_bCollapsed ? 1 : 2);
	item.stateMask	= LVIS_STATEIMAGEMASK;
	item.lParam		= nPosArray;

	int	nItemRes = CListCtrl::InsertItem(&item);

	for (int i = 1; i < (int)info.m_lstCols.size(); i++)
	{
		CListCtrl::SetItemText(nItemRes, i, info.m_lstCols[i]);
	}
	return nItemRes;
}

BOOL CTreeListCtrl::DeleteAllItems()
{
	m_tree.clear();
	return CListCtrl::DeleteAllItems();
}

void CTreeListCtrl::Collapse
	(
	int		nPos,
	int		nItem
	)
{
	int				nLevel;
	int				nLast;
	LVITEM			item;
	
	tree_list_info&	info = m_tree[nPos];
	info.m_bCollapsed = true;

	SetRedraw(0);
	nLevel	= info.m_nLevel;
	nLast	= GetItemCount();
	item.mask		= LVIF_PARAM;
	item.iSubItem	= 0;

	int nCounter = nItem + 1;
	if (nCounter < nLast)
	{
		do
		{
			item.iItem		= nCounter;
			GetItem(&item);
			if (m_tree[item.lParam].m_nLevel > nLevel)
				DeleteItem(nItem + 1);
		}
		while (m_tree[item.lParam].m_nLevel > nLevel && GetItemCount() > nCounter);
	}

	SetRedraw(1);
}

int CTreeListCtrl::Expand
	(
	int		nItem,
	bool	bRecursive,
	int&	nCounterInsert
	)
{
	tree_list_info&	info	= m_tree[nItem];
	info.m_bCollapsed		= false;

	int				nCounter;
	int				nLevel;
	
	nCounter		= nItem + 1;
	nCounterInsert	= nCounterInsert + 1;
	nLevel			= info.m_nLevel;
	while (nCounter < (int)m_tree.size() && m_tree[nCounter].m_nLevel > nLevel)
	{
		if (m_tree[nCounter].m_nLevel > nLevel)
		{
			if (bRecursive)
			{
				m_tree[nCounter].m_bCollapsed = false;
			}
			InsertItem(nCounterInsert, nCounter, m_tree[nCounter]);
		}

		if (bRecursive || !m_tree[nCounter].m_bCollapsed)
		{
			nCounter = Expand(nCounter, bRecursive, nCounterInsert);
		}
		else
		{
			int	nSonLevel;

			nSonLevel = m_tree[nCounter].m_nLevel;
			while (++nCounter < (int)m_tree.size() && m_tree[nCounter].m_nLevel > nSonLevel)
			{
			}

			nCounterInsert++;
		}		
	}
	
	return nCounter;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl message handlers

void CTreeListCtrl::OnClick
	(
	NMHDR*		pNMHDR,
	LRESULT*	pResult
	)
{
	LPNMITEMACTIVATE	pItemActivate = (LPNMITEMACTIVATE)pNMHDR;

	CRect				rect;

	GetItemRect(pItemActivate->iItem, rect, LVIR_ICON);
		
	if (pItemActivate->ptAction.x < rect.left)
	{
		LVITEM	item;

		item.mask		= LVIF_INDENT | LVIF_PARAM;
		item.iItem		= pItemActivate->iItem;
		item.iSubItem	= 0;
		GetItem(&item);

		if ( (int) (pItemActivate->ptAction.x + m_HScrollPos)  > item.iIndent * 16)
		{
			if (m_tree[item.lParam].m_bCollapsed)
			{	
				int	nInsert = pItemActivate->iItem;
				SetRedraw(0);
				Expand(item.lParam, false, nInsert);
				SetRedraw(1);
				SetItemState(pItemActivate->iItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
			}
			else
			{				
				SetRedraw(0);
				Collapse(item.lParam, pItemActivate->iItem);
				SetRedraw(1);
				SetItemState(pItemActivate->iItem, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
			}
		}
	}
	
	*pResult = 0;
}

void CTreeListCtrl::OnDblClick
(
 NMHDR*		pNMHDR,
 LRESULT*	pResult
 )
{
	LPNMITEMACTIVATE	pItemActivate = (LPNMITEMACTIVATE)pNMHDR;

	CRect				rect;

	LVITEM	item;
	if( pItemActivate->iItem < 0 ) return;

	item.mask		= LVIF_INDENT | LVIF_PARAM;
	item.iItem		= pItemActivate->iItem;
	item.iSubItem	= 0;
	GetItem(&item);

	if (m_tree[item.lParam].m_bCollapsed)
	{	
		int	nInsert = pItemActivate->iItem;
		SetRedraw(0);
		Expand(item.lParam, false, nInsert);
		SetRedraw(1);
		SetItemState(pItemActivate->iItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
	}
	else
	{				
		SetRedraw(0);
		Collapse(item.lParam, pItemActivate->iItem);
		SetRedraw(1);
		SetItemState(pItemActivate->iItem, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
	}

	*pResult = 0;
}

void CTreeListCtrl::OnKeyDown
	(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags
	)
{
	int		nItem	=	GetNextItem(-1, LVNI_SELECTED);	

	if (nItem == -1)
	{
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	
	LVITEM		item;

	item.mask		= LVIF_INDENT | LVIF_PARAM;
	item.iItem		= nItem;
	item.iSubItem	= 0;
	GetItem(&item);

	switch (nChar)
	{
		case VK_ADD:
			if (m_tree[item.lParam].m_bCollapsed)
			{	
				int	nInsert = nItem;
				SetRedraw(0);
				Expand(item.lParam, false, nInsert);
				SetRedraw(1);
				SetItemState(nItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
			}
			break;
		case VK_SUBTRACT:
			if (!m_tree[item.lParam].m_bCollapsed)
			{				
				SetRedraw(0);
				Collapse(item.lParam, nItem);
				SetRedraw(1);
				SetItemState(nItem, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
			}
			break;
		case VK_RIGHT:
			if (m_tree[item.lParam].m_bCollapsed)
			{	
				int	nInsert = nItem;
				SetRedraw(0);
				Expand(item.lParam, false, nInsert);
				SetRedraw(1);
				SetItemState(nItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
			}
			else
			{
				// If any child, move to the first one
				int nNextItem	= item.lParam + 1;
				if (nNextItem < (int)m_tree.size() && m_tree[nNextItem].m_nLevel > m_tree[item.lParam].m_nLevel)
				{
					SetItemState(nItem + 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
			break;
		case VK_LEFT:
			if (!m_tree[item.lParam].m_bCollapsed)
			{				
				SetRedraw(0);
				Collapse(item.lParam, nItem);
				SetRedraw(1);
				SetItemState(nItem, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
			}
			else
			{
				// Move to our parent
				int nPrevItem	= item.lParam - 1;
				int	nLevel		= m_tree[item.lParam].m_nLevel;

				while (nPrevItem >= 0 && nLevel <= m_tree[nPrevItem].m_nLevel)
				{
					--nPrevItem;
				}

				if (nPrevItem >= 0)
				{
					LVFINDINFO		info;
					
					info.flags		= LVFI_PARAM;
					info.lParam		= nPrevItem;
			
					SetItemState(FindItem(&info), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
			break;
		default:
			CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

void CTreeListCtrl::OnChar
	(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags
	) 
{
	int		nItem	=	GetNextItem(-1, LVNI_SELECTED);	

	if (nItem != -1)
	{
		LVITEM		item;

		item.mask		= LVIF_INDENT | LVIF_PARAM;
		item.iItem		= nItem;
		item.iSubItem	= 0;
		GetItem(&item);

		if (nChar == '*')
		{
			// TODO: Recursively expands items 
			if (m_tree[item.lParam].m_bCollapsed)
			{	
				int	nInsert = nItem;
				SetRedraw(0);
				Expand(item.lParam, true, nInsert);
				SetRedraw(1);
				SetItemState(nItem, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
			}
		}	
	}

	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CTreeListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if( SB_THUMBTRACK  == nSBCode )
	{
		m_HScrollPos = nPos;
	}

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTreeListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	int nColumnsWidth = 0;
	for( int i=0; i< GetColumnsCount(); i++ )
		nColumnsWidth += GetColumnWidth(i);

	if( nColumnsWidth < cx )
	{
		int nFirstColumnWidth = GetColumnWidth(0);
		int nExtraSpace = cx - nColumnsWidth;
		SetColumnWidth( 0, nFirstColumnWidth + nExtraSpace );
	}
}

LPSTR CTreeListCtrl::GetColumnName( int nIndex )
{
	static TCHAR pcName[MAX_PATH] = {0,}; // sufficient for now
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_TEXT;
	lvColumn.pszText = (LPTSTR)pcName;
	lvColumn.cchTextMax = MAX_PATH;

	GetColumn(nIndex, &lvColumn);

	return lvColumn.pszText;
}
