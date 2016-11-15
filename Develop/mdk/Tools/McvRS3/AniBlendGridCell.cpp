// AniBlendGridCell.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AniBlendGridCell.h"

IMPLEMENT_DYNCREATE(CAniBlendGridCell, CGridCell)

// CAniBlendGridCell
CAniBlendGridCell::CAniBlendGridCell()
{

}

CAniBlendGridCell::~CAniBlendGridCell()
{
}

void CAniBlendGridCell::OnDblClick( CPoint PointCellRelative )
{
	CGridCell::OnDblClick( PointCellRelative );

	return;

	if( !IsFixed() )
	{
		CCellID clickedCellID = GetGrid()->GetFocusCell();
		if( clickedCellID.row != clickedCellID.col )
		{
			RECT cellRect;
			if( GetGrid()->GetCellRect( clickedCellID.row, clickedCellID.col, &cellRect ) )
			{
				int centerX = ( cellRect.right - cellRect.left ) / 2;
				CString strTargetAniName;

				if( PointCellRelative.x < centerX )
				{
					strTargetAniName = GetGrid()->GetItemText( clickedCellID.row, 0 );
				}
				else
				{
					strTargetAniName = GetGrid()->GetItemText( 0, clickedCellID.col );
				}

//				int sel = GetAnimationSidebar()->m_cAnimationListBox.FindStringExact( 0, strTargetAniName );
//				GetAnimationSidebar()->m_cAnimationListBox.SetCurSel( sel );
//				GetMainView()->ChangeAnimationList( strTargetAniName );
			}
		}
	}
}

void CAniBlendGridCell::OnRClick( CPoint PointCellRelative )
{
	CGridCell::OnRClick( PointCellRelative );

	OnSetCursor();

	if( !IsFixed() )
	{
		CCellID clickedCellID = GetGrid()->GetFocusCell();
		if( clickedCellID.row != clickedCellID.col )
		{
			RECT cellRect;
			if( GetGrid()->GetCellRect( clickedCellID.row, clickedCellID.col, &cellRect ) )
			{
				RECT gridRect;
				GetGrid()->GetWindowRect( &gridRect );

				CPoint popupPos( gridRect.left + cellRect.left + PointCellRelative.x,
					gridRect.top + cellRect.top + PointCellRelative.y );

				CMenu menu;
				menu.LoadMenu( IDR_MENU_ANIBLEND );
				CMenu* subMenu = menu.GetSubMenu( 0 );
				subMenu->TrackPopupMenu( 0, popupPos.x, popupPos.y, GetMainView() );
			}
		}
	}
	// Fixed Column 이라면
	else
	if( GetData() )
	{
		CCellID clickedCellID = GetGrid()->GetFocusCell();

		RECT cellRect;
		if( GetGrid()->GetCellRect( 0, clickedCellID.col, &cellRect ) )
		{
			RECT gridRect;
			GetGrid()->GetWindowRect( &gridRect );

			CPoint popupPos( gridRect.left + cellRect.left + PointCellRelative.x,
				gridRect.top + cellRect.top + PointCellRelative.y );

			CMenu menu;
			menu.LoadMenu( IDR_MENU_ANIBLEND_TARGET_ALL );
			CMenu* subMenu = menu.GetSubMenu( 0 );
			subMenu->TrackPopupMenu( 0, popupPos.x, popupPos.y, GetMainView() );
		}
	}
}

BOOL CAniBlendGridCell::OnSetCursor()
{
	BOOL bReturn = CGridCell::OnSetCursor();

	static const COLORREF fixedSelectColor = RGB( 150, 150, 250 );
	static const COLORREF fixedUnselectColor = RGB( 220, 220, 220 );

	int count = GetGrid()->GetColumnCount();
	if( count > 1 )
	{
		for( int i = 1; i < count; ++i )
		{
			GetGrid()->SetItemBkColour( 0, i, fixedUnselectColor );
			GetGrid()->SetItemBkColour( i, 0, fixedUnselectColor );
		}
	}

	CCellID id = GetGrid()->GetFocusCell();
	if( id.IsValid() )
	{
		GetGrid()->SetItemBkColour( 0, id.col, fixedSelectColor );
		GetGrid()->SetItemBkColour( id.row, 0, fixedSelectColor );
	}

	GetGrid()->RedrawWindow();

	return bReturn;
}