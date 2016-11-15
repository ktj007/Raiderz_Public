#include "stdafx.h"
#include "XColorPallete.h"
#include "XColorTable.h"

XColorPallete::XColorPallete( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: MWidget(szName, pParent, pListener)
, m_nSelected(0)
, m_nFocusCell(-1)
{

}

XColorPallete::~XColorPallete()
{

}

void XColorPallete::OnDraw( MDrawContext* pDC )
{
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		DrawCell(pDC, i, m_Nodes[i]);
	}
}

bool XColorPallete::OnEvent( MEvent* pEvent, MListener* pListener )
{
	if (MWidget::OnEvent(pEvent, pListener) == true) return true;

	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			for (size_t nCellIndex = 0; nCellIndex < m_Nodes.size(); nCellIndex++)
			{
				MRECT rtCell = GetCellRect(nCellIndex);
				if (rtCell.InPoint(pEvent->Pos) == true)
				{
					SelCell(nCellIndex);
					break;
				}
			}
		}
		break;
	case MWM_MOUSEMOVE:
		{
			m_nFocusCell = -1;
			for (size_t nCellIndex = 0; nCellIndex < m_Nodes.size(); nCellIndex++)
			{
				MRECT rtCell = GetCellRect(nCellIndex);
				if (rtCell.InPoint(pEvent->Pos) == true)
				{
					m_nFocusCell = nCellIndex;
					break;
				}
			}
		}
		break;
	};

	return false;
}

MRECT XColorPallete::GetCellRect( int nIndex )
{
	int x = 0;
	int y = 0;

	int width = max(1, m_Rect.w - m_CellShape.nMargin);
	int nx = max(1, (width+m_CellShape.nMargin) / max(1, (m_CellShape.nWidth + m_CellShape.nMargin)));

	x = ((nIndex % nx) * (m_CellShape.nWidth + m_CellShape.nMargin));
	y = ((nIndex / nx) * (m_CellShape.nHeight + m_CellShape.nMargin));

	return MRECT(x, y, m_CellShape.nWidth, m_CellShape.nHeight);
}

void XColorPallete::DrawCell( MDrawContext* pDC, int nIndex, const ColorCell& node )
{
	MRECT rt = GetCellRect(nIndex);

	MCOLOR color = node.Color;
	pDC->SetColor(color);

	pDC->FillRectangle(rt);

	DrawCellFrame(pDC, rt, (nIndex == m_nSelected), (nIndex == m_nFocusCell));
}

void XColorPallete::DrawCellFrame( MDrawContext* pDC, MRECT rt, bool bSelected, bool bFocus )
{
	MCOLOR color;
	if (bSelected)
	{
		color = MCOLOR(m_CellShape.clrSelBorder);
	}
	else if (bFocus)
	{
		color = MCOLOR(m_CellShape.clrFocusBorder);
	}
	else
	{
		color = MCOLOR(m_CellShape.clrNormalBorder);
	}
	pDC->SetColor(color);
	pDC->Rectangle(rt);

	if (bSelected || bFocus)
	{
		rt.x++;
		rt.y++;
		rt.w -= 2;
		rt.h -= 2;
		pDC->Rectangle(rt);
	}
}

void XColorPallete::AddCell( MCOLOR color )
{
	ColorCell node;
	node.Color = color;

	m_Nodes.push_back(node);
}

void XColorPallete::SetCellShape( int nCellWidth, int nCellHeight, int nCellMargin )
{
	m_CellShape.nWidth = nCellWidth;
	m_CellShape.nHeight = nCellHeight;
	m_CellShape.nMargin = nCellMargin;
}

void XColorPallete::SelCell( int nIndex )
{
	if (nIndex == m_nSelected) return;

	if (nIndex >= 0 && nIndex < (int)m_Nodes.size())
	{
		m_nSelected = nIndex;
	}
	else
	{
		m_nSelected = -1;
	}
	OnSelectedItemChanged();
}

void XColorPallete::Assign( const wchar_t* szColorTableGroupName )
{
	if (!info.colorTable) return;

	XColorGroup* pColorGroup = info.colorTable->GetGroup(szColorTableGroupName);
	if (pColorGroup == NULL) return;

	for (size_t i = 0; i < pColorGroup->GetSize(); i++)
	{
		MCOLOR color = pColorGroup->GetColor(i);
		AddCell(color);
	}
}

void XColorPallete::OnSelectedItemChanged()
{
//	m_EventHandler.OnCommand(this, MEVENT_CB_SELECTED_INDEX_CHANGED);
}

bool XColorPallete::OnCommand( MWidget* pWidget, MEventMsg nMsg, const wchar_t* szArgs/*=NULL*/ )
{
	if ( pWidget == this)
	{
		if (nMsg == MEVENT_SELCHANGE)
		{
			//int index = m_pListBox->GetSelIndex();
			//gvar.MyInfo.PaletteList.Binding(index);
			return false;
		}
	}

	return false;
}

int XColorPallete::glueGetSelectedIndex()
{
	return m_nSelected;
}