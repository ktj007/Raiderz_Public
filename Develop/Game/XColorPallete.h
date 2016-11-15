#pragma once

#include "MWidget.h"

using namespace mint3;

class XColorPallete : public MWidget
{
protected:
	struct ColorCell
	{
		MCOLOR Color;
	};

	struct CellShape
	{
		int nWidth;
		int nHeight;
		int nMargin;
		MCOLOR clrNormalBorder;
		MCOLOR clrSelBorder;
		MCOLOR clrFocusBorder;

		CellShape() : nWidth(8), nHeight(8), nMargin(4), clrNormalBorder(0xFF909090), clrSelBorder(0xFF00FF00), clrFocusBorder(0xFF00FF00) {}
	};

	CellShape				m_CellShape;
	vector<ColorCell>		m_Nodes;
	int						m_nSelected;
	int						m_nFocusCell;

	MRECT GetCellRect(int nIndex);
	void DrawCell(MDrawContext* pDC, int nIndex, const ColorCell& node);
	void DrawCellFrame(MDrawContext* pDC, MRECT rt, bool bSelected, bool bFocus);
	void AddCell(MCOLOR color);
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const wchar_t* szArgs=NULL);

	void OnSelectedItemChanged();
public:
	XColorPallete( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XColorPallete();
	void SetCellShape(int nCellWidth, int nCellHeight, int nCellMargin);
	void SelCell(int nIndex);
	void Assign(const wchar_t* szColorTableGroupName);
	void SetNormalBorderColor(MCOLOR clr) { m_CellShape.clrNormalBorder = clr; }
	void SetSelBorderColor(MCOLOR clr) { m_CellShape.clrSelBorder = clr; }
	void SetFocusBorderColor(MCOLOR clr) { m_CellShape.clrFocusBorder = clr; }
protected:
	virtual void OnDraw( MDrawContext* pDC);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);



#define MINT_COLOR_PALLETE		"ColorPallete"
	virtual const char* GetClassName(void)		{ return MINT_COLOR_PALLETE; }
	
public:
	int glueGetSelectedIndex();

};
