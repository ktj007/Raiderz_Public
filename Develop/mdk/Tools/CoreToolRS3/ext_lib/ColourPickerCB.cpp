// ColourPickerCB.cpp
//
// Copyright (C)2000 Mark Jackson
//   e-mail: mark@mjsoft.co.uk
// web-site: http://www.mjsoft.co.uk
//
// based on the control by James R. Twine.

#include "stdafx.h"
#include "ColourPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct ColourCB
{
	COLORREF m_crColour;
	CString m_strName;

	ColourCB( COLORREF crColour, CString strName )
		: m_crColour( crColour )
		, m_strName( strName )
	{
	}
} g_arrColourCB[] =
{
	ColourCB( RGB( 000, 000, 000 ), "Black" ),
	ColourCB( RGB( 255, 255, 255 ), "White" ),
	ColourCB( RGB( 127, 127, 127 ), "Grey" ),
	ColourCB( RGB( 255, 000, 000 ), "Red" ),
	ColourCB( RGB( 255, 127, 000 ), "Orange" ),
	ColourCB( RGB( 255, 255, 000 ), "Yellow" ),
	ColourCB( RGB( 000, 255, 000 ), "Green" ),
	ColourCB( RGB( 000, 255, 255 ), "Cyan" ),
	ColourCB( RGB( 000, 000, 255 ), "Blue" ),
	ColourCB( RGB( 255, 000, 255 ), "Fuchsia" )
};

struct ColourTable
{
	std::map<COLORREF, std::string> m_mapColorStringTable;
	ColourTable()
	{
		m_mapColorStringTable[RGB( 000, 000, 000 )] = "Black";
		m_mapColorStringTable[RGB( 255, 255, 255 )] = "White";
		m_mapColorStringTable[RGB( 127, 127, 127 )] = "Grey";
		m_mapColorStringTable[RGB( 255, 000, 000 )] = "Red";
		m_mapColorStringTable[RGB( 255, 127, 000 )] = "Orange";
		m_mapColorStringTable[RGB( 255, 255, 000 )] = "Yellow";
		m_mapColorStringTable[RGB( 000, 255, 000 )] = "Green";
		m_mapColorStringTable[RGB( 000, 255, 255 )] = "Cyan";
		m_mapColorStringTable[RGB( 000, 000, 255 )] = "Blue";
		m_mapColorStringTable[RGB( 255, 000, 255 )] = "Fuchsia";
	}
} g_mapColourTable;

CColourPickerCB::CColourPickerCB()
{
}

CColourPickerCB::~CColourPickerCB()
{
}

void DDX_ColourPickerCB( CDataExchange *pDX, int nIDC, COLORREF& rgbColor )
{
	HWND hWndCtrl = pDX->PrepareCtrl( nIDC );
	ASSERT( hWndCtrl );

	CColourPickerCB *pPicker = (CColourPickerCB*)CWnd::FromHandle( hWndCtrl );
	ASSERT( pPicker );

	// only support getting of colour.
	if( pDX->m_bSaveAndValidate )
	{
		rgbColor = pPicker->GetSelectedColourValue();
	}
}


BEGIN_MESSAGE_MAP(CColourPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColourPickerCB)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColourPickerCB message handlers

void CColourPickerCB::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

	Initialise();
}

void CColourPickerCB::Initialise()
{
	// add colours.
	int iNumColours = sizeof( g_arrColourCB ) / sizeof( g_arrColourCB[0] );

	for( int iNum = 0; iNum < iNumColours; iNum++ )
	{
		ColourCB& colour = g_arrColourCB[iNum];
		AddColour( colour.m_strName, colour.m_crColour );
	}

	// add a custom item on the end.
	//AddColour( "Custom...", RGB( 255, 255, 255 ) );

	SetCurSel( 0 );
}

void CColourPickerCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	CString strColour;
	CDC dcContext;
	CRect rItemRect( pDIStruct->rcItem );
	CRect rBlockRect( rItemRect );
	CRect rTextRect( rBlockRect );
	CBrush brFrameBrush;
	int iFourthWidth = 0;
	int iItem = pDIStruct->itemID;
	int iAction = pDIStruct->itemAction;
	int iState = pDIStruct->itemState;
	COLORREF crColour = NULL;
	COLORREF crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( pDIStruct->hDC ) )
	{
		return;
	}

	iFourthWidth = ( rBlockRect.Width() / 4 );
	brFrameBrush.CreateStockObject( BLACK_BRUSH );

	if( iState & ODS_SELECTED )
	{
		dcContext.SetTextColor(	( 0x00FFFFFF & ~( crText ) ) );
		dcContext.SetBkColor( crSelected );
		dcContext.FillSolidRect( &rBlockRect, crSelected );
	}
	else
	{
		dcContext.SetTextColor( crText );
		dcContext.SetBkColor( crNormal );
		dcContext.FillSolidRect( &rBlockRect, crNormal );
	}

	if( iState & ODS_FOCUS )
	{
		dcContext.DrawFocusRect( &rItemRect );
	}

	// calculate text area.
	rTextRect.left += ( iFourthWidth + 2 );
	rTextRect.top += 2;

	// calculate colour block area.
	rBlockRect.DeflateRect( CSize( 2, 2 ) );
	rBlockRect.right = iFourthWidth;

	// draw colour text and block.
	if( iItem != -1 )
	{
		GetLBText( iItem, strColour );

		if( iState & ODS_DISABLED )
		{
			crColour = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColour );
		}
		else
		{
			crColour = GetItemData( iItem );
		}

		dcContext.SetBkMode( TRANSPARENT );
		dcContext.TextOut( rTextRect.left, rTextRect.top,	strColour );

		dcContext.FillSolidRect( &rBlockRect, crColour );
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );
	}

	dcContext.Detach();
}

COLORREF CColourPickerCB::GetSelectedColourValue()
{
	return GetItemData( GetCurSel() );
}

void CColourPickerCB::AddColour( CString strName, COLORREF crColour )
{
	int iIndex = AddString( strName );

	if( iIndex >= 0 )
	{
		SetItemData( iIndex, crColour );
	}
}

void CColourPickerCB::OnSelchange() 
{
	// the last item is "Custom...".
	CString strText;
	GetLBText( GetCurSel(), strText );

	if( strText == "Custom..." )
	{
		CColorDialog dlg;

		if( dlg.DoModal() == IDOK )
		{
			SetItemData( GetCurSel(), dlg.GetColor() );
		}
	}
}

const char* CColourPickerCB::GetColorName(COLORREF _color)
{
	std::map<COLORREF, std::string>::iterator itr = g_mapColourTable.m_mapColorStringTable.find( _color );
	if( itr != g_mapColourTable.m_mapColorStringTable.end() )
		return itr->second.c_str();
	else
		return NULL;
}

DWORD CColourPickerCB::GetColorValue(const std::string& _refName)
{
	for(std::map<COLORREF, std::string>::iterator itr = g_mapColourTable.m_mapColorStringTable.begin();
		itr != g_mapColourTable.m_mapColorStringTable.end(); ++itr)
	{
		if(itr->second == _refName)
			return D3DCOLOR_XRGB( GetRValue(itr->first), GetGValue(itr->first), GetBValue(itr->first) );
	}
	return 0;
}
