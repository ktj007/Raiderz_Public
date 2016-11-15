/*
 *
 *	ChartBarSerie.cpp
 *
 *	Written by Cédric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#include "stdafx.h"
#include "ChartBarSerie.h"
#include "ChartCtrl.h"

int CChartBarSerie::m_iInterSpace = 0;
std::list<CChartBarSerie*> CChartBarSerie::m_lstBarSeries;

CChartBarSerie::CChartBarSerie(CChartCtrl* pParent) 
 : CChartSerie(pParent), m_iBarWidth(20), m_iBorderWidth(1),
   m_BorderColor(RGB(0,0,0)), m_uGroupId(0), m_bGradient(true), 
   m_GradientColor(RGB(255,255,255)), m_GradientType(gtHorizontalDouble),
   m_bHorizontal(false), m_dBaseLine(0), m_bAutoBaseLine(true)
{
	m_lstBarSeries.push_back(this);
	m_iShadowDepth = 3;
	m_bShadow = true;
}

CChartBarSerie::~CChartBarSerie()
{
	TBarSeriesList::iterator iter = m_lstBarSeries.begin();
	while (iter!=m_lstBarSeries.end())
	{
		if ((*iter) == this)
			iter = m_lstBarSeries.erase(iter);
		else
			iter++;
	}
}

void CChartBarSerie::SetHorizontal(bool bHorizontal)  
{ 
	m_bHorizontal = bHorizontal; 
	if (m_bHorizontal)
		SetSeriesOrdering(CChartPointsArray::poYOrdering);
	else
		SetSeriesOrdering(CChartPointsArray::poXOrdering);
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::SetBorderColor(COLORREF BorderColor)  
{ 
	m_BorderColor = BorderColor; 
	m_pParentCtrl->RefreshCtrl();
}
void CChartBarSerie::SetBorderWidth(int Width)  
{ 
	m_iBorderWidth = Width; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::SetBarWidth(int Width)		
{ 
	m_iBarWidth = Width;
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::SetGroupId(unsigned GroupId)	
{ 
	m_uGroupId = GroupId; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::ShowGradient(bool bShow)		
{ 
	m_bGradient = bShow; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::SetGradient(COLORREF GradientColor, EGradientType GradientType)
{
	m_GradientColor = GradientColor;
	m_GradientType = GradientType;
	m_pParentCtrl->RefreshCtrl();
}

void CChartBarSerie::DrawBar(CDC* pDC, CBrush* pFillBrush, CBrush* pBorderBrush, 
							 int XPos, int YPos)
{
	CRect BorderRect = GetBarRectangle(XPos, YPos);
	if (m_bShadow)
	{
		CBrush ShadowBrush(m_ShadowColor);
		CRect ShadowRect(BorderRect);
		ShadowRect.OffsetRect(m_iShadowDepth,m_iShadowDepth);
		pDC->FillRect(ShadowRect,&ShadowBrush);
	}
	pDC->FillRect(BorderRect,pBorderBrush);

	CRect FillRect(BorderRect);
	FillRect.DeflateRect(m_iBorderWidth,m_iBorderWidth);
	if (m_bGradient)
	{
		CChartGradient::DrawGradient(pDC,FillRect,m_SerieColor,m_GradientColor,m_GradientType);
	}
	else
	{
		pDC->FillRect(FillRect,pFillBrush);
	}

}

void CChartBarSerie::Draw(CDC* pDC)
{
	if (!m_bIsVisible)
		return;
	if (!pDC->GetSafeHdc())
		return;

	int iPosition = GetSerieStartPos();

	CRect TempClipRect(m_PlottingRect);
	TempClipRect.DeflateRect(1,1);
	pDC->SetBkMode(TRANSPARENT);
	pDC->IntersectClipRect(TempClipRect);

	CBrush BorderBrush(m_BorderColor);
	CBrush FillBrush(m_SerieColor);
	//Draw all points that haven't been drawn yet
	for (m_uLastDrawnPoint;m_uLastDrawnPoint<(int)GetPointsCount();m_uLastDrawnPoint++)
	{
		CPoint ScreenPoint;
		ValueToScreen(m_vPoints.GetXPointValue(m_uLastDrawnPoint),m_vPoints.GetYPointValue(m_uLastDrawnPoint),ScreenPoint);

		if (m_bHorizontal)
			DrawBar(pDC,&FillBrush,&BorderBrush,ScreenPoint.x,ScreenPoint.y+iPosition);
		else
			DrawBar(pDC,&FillBrush,&BorderBrush,ScreenPoint.x+iPosition,ScreenPoint.y);
	}

	pDC->SelectClipRgn(NULL);
	DeleteObject(BorderBrush);
	DeleteObject(FillBrush);
}

void CChartBarSerie::DrawAll(CDC *pDC)
{
	if (!m_bIsVisible)
		return;
	if (!pDC->GetSafeHdc())
		return;

	int iPosition = GetSerieStartPos();

	unsigned uFirst=0, uLast=0;
	if (!GetVisiblePoints(uFirst,uLast))
		return;

	CRect TempClipRect(m_PlottingRect);
	TempClipRect.DeflateRect(1,1);
	if (uFirst>0)
		uFirst--;
	if (uLast<GetPointsCount()-1)
		uLast++;

	pDC->SetBkMode(TRANSPARENT);
	pDC->IntersectClipRect(TempClipRect);

	CBrush BorderBrush(m_BorderColor);
	CBrush FillBrush(m_SerieColor);
	for (m_uLastDrawnPoint=uFirst;m_uLastDrawnPoint<=uLast;m_uLastDrawnPoint++)
	{
		CPoint ScreenPoint;
		ValueToScreen(m_vPoints.GetXPointValue(m_uLastDrawnPoint),m_vPoints.GetYPointValue(m_uLastDrawnPoint),ScreenPoint);

		if (m_bHorizontal)
			DrawBar(pDC,&FillBrush,&BorderBrush,ScreenPoint.x,ScreenPoint.y+iPosition);
		else
			DrawBar(pDC,&FillBrush,&BorderBrush,ScreenPoint.x+iPosition,ScreenPoint.y);
	}

	pDC->SelectClipRgn(NULL);
	DeleteObject(BorderBrush);
	DeleteObject(FillBrush);
}

int CChartBarSerie::GetSerieStartPos() const
{
	int iOffset = 0;
	int iTotalWidth = 0;
	bool bFound = false;
	TBarSeriesList::const_iterator iter = m_lstBarSeries.begin();
	for (iter; iter!=m_lstBarSeries.end(); iter++)
	{
		if (!bFound)
		{
			if ((*iter)==this)
				bFound = true;
			else if ( (*iter)->GetGroupId() == m_uGroupId)
				iOffset += (*iter)->GetBarWidth() + m_iInterSpace;
		}

		if ( (*iter)->GetGroupId() == m_uGroupId)
			iTotalWidth += (*iter)->GetBarWidth() + m_iInterSpace;
	}
	// Remove the interspace because it has been counted once too much.
	iTotalWidth -= m_iInterSpace;
	return (iOffset - iTotalWidth/2);  
}

void CChartBarSerie::DrawLegend(CDC* pDC, const CRect& rectBitmap) const
{
	if (m_strSerieName== _T(""))
		return;

	//Draw bar:
	CBrush BorderBrush(m_BorderColor);
	pDC->FillRect(rectBitmap,&BorderBrush);

	CRect FillRect(rectBitmap);
	CBrush FillBrush(m_SerieColor);
	FillRect.DeflateRect(m_iBorderWidth,m_iBorderWidth);
	if (m_bGradient)
	{
		CChartGradient::DrawGradient(pDC,FillRect,m_SerieColor,m_GradientColor,m_GradientType);
	}
	else
	{
		pDC->FillRect(FillRect,&FillBrush);
	}
}

CRect CChartBarSerie::GetBarRectangle(int XPos, int YPos) const
{
	CRect BorderRect;
	if (m_bHorizontal)
	{
		int AxisPos = 0;
		if (!m_bAutoBaseLine)
			AxisPos = m_pHorizontalAxis->ValueToScreen(m_dBaseLine);
		else
		{
			double Position = m_pVerticalAxis->GetPosition()/100.00;
			AxisPos = m_PlottingRect.left + (int)(Position * (m_PlottingRect.right-m_PlottingRect.left));
		}
		if (AxisPos<XPos)
			BorderRect.SetRect(AxisPos,YPos,XPos,YPos+m_iBarWidth);
		else
			BorderRect.SetRect(XPos,YPos,AxisPos,YPos+m_iBarWidth);
	}
	else
	{
		int AxisPos = 0;
		if (!m_bAutoBaseLine)
			AxisPos = m_pVerticalAxis->ValueToScreen(m_dBaseLine);
		else
		{
			double Position = m_pHorizontalAxis->GetPosition()/100.00;
			AxisPos = m_PlottingRect.top + (int)(Position * (m_PlottingRect.bottom-m_PlottingRect.top));
		}
		if (YPos<AxisPos)
			BorderRect.SetRect(XPos,YPos,XPos+m_iBarWidth,AxisPos);
		else
			BorderRect.SetRect(XPos,AxisPos,XPos+m_iBarWidth,YPos);
	}
	return BorderRect;
}

bool CChartBarSerie::IsPointOnSerie(const CPoint& screenPoint, unsigned& uIndex) const 
{ 
	uIndex = INVALID_POINT;
	if (!m_bIsVisible)
		return false;

	int iPosition = GetSerieStartPos();
	unsigned uFirst=0, uLast=0;
	if (!GetVisiblePoints(uFirst,uLast))
		return false;
	if (uFirst>0)
		uFirst--;
	if (uLast<GetPointsCount())
		uLast++;

	bool bResult = false;
	for (unsigned i=uFirst ; i < uLast ; i++)
	{
		CPoint BarPoint;
		ValueToScreen(m_vPoints.GetXPointValue(i),m_vPoints.GetYPointValue(i),BarPoint);

		CRect BarRect; 
		if (m_bHorizontal)
			BarRect = GetBarRectangle(BarPoint.x,BarPoint.y+iPosition);
		else
			BarRect = GetBarRectangle(BarPoint.x+iPosition,BarPoint.y);

		if (BarRect.PtInRect(screenPoint))
		{
			bResult = true;
			uIndex = i;
			break;
		}
	}
	return bResult; 
}
