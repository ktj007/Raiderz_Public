#include "StdAfx.h"
#include "V_MFCAnchorSupport.h"

CMFCAnchorSupport::CMFCAnchorSupport()
: m_pParent( NULL )
{
}

void CMFCAnchorSupport::Initialize(CWnd* pParent, int cx, int cy)
{
	m_pParent = pParent;

	RECT rt;
	rt.left = rt.top = 0;
	rt.right = cx; rt.bottom = cy;
	MapDialogRect( m_pParent->m_hWnd, &rt );

	m_nInitialCX = rt.right;
	m_nInitialCY = rt.bottom;
}


void CMFCAnchorSupport::RegisterAnchor( int nResourceID, CAnchorStyle anchorStyle )
{
	AnchorData anchorData;
	anchorData.style = anchorStyle;

	// 초기 위치 구함
	CWnd* pWnd = m_pParent->GetDlgItem( nResourceID );
	pWnd->GetWindowRect( &anchorData.initialRect );
	m_pParent->ScreenToClient( &anchorData.initialRect );

	m_mapAnchorWindows.insert( make_pair( nResourceID, anchorData ) );
}

void CMFCAnchorSupport::OnSize( int cx, int cy )
{
	for each (pair< int, AnchorData > item in m_mapAnchorWindows)
	{

		CAnchorStyle anchor = item.second.style;
		RECT rt = item.second.initialRect;

		// 수평방향 처리
		if( anchor & ANCHOR_LEFT )
		{
			if( anchor & ANCHOR_RIGHT )
			{
				// left&right 인 경우 size 가 바뀐만큼 같이 적용
				rt.right += cx - m_nInitialCX;
			}

			// left only 인 경우 아무것도 안함
		}else
		{
			if( anchor & ANCHOR_RIGHT )
			{
				// right only 이면 size 바뀐만큼 위치가 바뀜
				rt.left += cx - m_nInitialCX;
				rt.right += cx - m_nInitialCX;
			}else
			{
				// left, right 둘다 없으면 위치만 바뀜
				int nHalfMove = (cx - m_nInitialCX) / 2;
				rt.left += nHalfMove;
				rt.right += nHalfMove;
			}
		}

		// 수직방향 처리
		if( anchor & ANCHOR_TOP )
		{
			if( anchor & ANCHOR_BOTTOM )
			{
				// top & bottom 인 경우 size 가 바뀐만큼 같이 적용
				rt.bottom += cy - m_nInitialCY;
			}

			// top only 인 경우 아무것도 안함
		}else
		{
			if( anchor & ANCHOR_BOTTOM )
			{
				// bottom only 이면 size 바뀐만큼 위치가 바뀜
				rt.top += cy - m_nInitialCY;
				rt.bottom += cy - m_nInitialCY;
			}else
			{
				// top, bottom 둘다 없으면 위치만 바뀜
				int nHalfMove = (cy - m_nInitialCY) / 2;
				rt.top += nHalfMove;
				rt.bottom += nHalfMove;
			}
		}

		CWnd* pWnd = m_pParent->GetDlgItem( item.first );
		pWnd->MoveWindow( &rt );

	}
}
