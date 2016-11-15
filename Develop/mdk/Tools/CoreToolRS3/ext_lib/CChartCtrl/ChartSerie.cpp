/*
 *
 *	ChartSerie.cpp
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
 *	History:
 *		- 11/08/2006: Management of the auto axis now done in the axis. Series Register
 *					  Unregister themselves to their respective axes	.
 *		- 29/02/2008: Taking into account that RefreshAutoAxis doesn't refresh the control.
 *		- 01/03/2008: RemovePointsFromBegin and RemovePointsFromEnd functions added.
 *		- 08/03/2008: AddPoints function added (thanks to Bruno Lavier).
 *		- 11/03/2008: Min and max values are now cached.
 *		- 14/03/2008: Series can be ordered. Speed improvement done in that case.
 *		- 13/08/2008: Bug fix: calling AddPoint was not drawing the new points.
 *		- 27/11/2008: Points are now stored into the CChartPointsArray class instead of
 *					  std::vector for efficiency.
 *
 */

#include "stdafx.h"
#include "ChartSerie.h"
#include "ChartAxis.h"
#include "ChartCtrl.h"
#include "ChartLabel.h"

#include "Math.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

unsigned CChartSerie::m_uNextFreeId = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartSerie::CChartSerie(CChartCtrl* pParent)
{
	m_pParentCtrl = pParent;
	m_uLastDrawnPoint = 0;
	m_strSerieName = _T("");

	m_pHorizontalAxis = m_pVerticalAxis = NULL;
	m_uSerieId = m_uNextFreeId;
	m_uNextFreeId++;

	m_bIsVisible = true;
	m_bShadow = false;		
	m_SerieColor = RGB(0, 0, 0);
	m_ShadowColor = RGB(150,150,150);
	m_iShadowDepth = 2;

	m_bMouseClickNotifications = true;
	m_bMouseMoveNotifications = false;
}

CChartSerie::~CChartSerie()
{
	m_pHorizontalAxis->UnregisterSeries(this);
	m_pVerticalAxis->UnregisterSeries(this);

	TLabelMap::iterator iter = m_mapLabels.begin();
	for (iter; iter!=m_mapLabels.end(); iter++)
	{
		delete iter->second;
	}
}

void CChartSerie::SetSeriesOrdering(CChartPointsArray::PointsOrdering newOrdering)
{
	m_vPoints.SetOrdering(newOrdering);
}

void CChartSerie::SetName(const TChartString& NewName) 
{ 
	m_strSerieName = NewName; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::AddPoint(double X, double Y)
{
	m_vPoints.AddPoint(X, Y);

	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->EnableRefresh(true);

	CDC* pDC = m_pParentCtrl->GetDC();
	Draw(pDC);
	m_pParentCtrl->Invalidate();
}

void CChartSerie::AddPoints(double* pX, double* pY, unsigned Count)
{
	m_vPoints.AddPoints(pX, pY, Count);

	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->EnableRefresh(true);

	CDC* pDC = m_pParentCtrl->GetDC();
	Draw(pDC);
	m_pParentCtrl->Invalidate();
}

void CChartSerie::SetPoints(double* pX, double* pY, unsigned Count)
{
	m_vPoints.SetPoints(pX, pY, Count);
	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);
}

void CChartSerie::RemovePointsFromBegin(unsigned Count)
{
	m_vPoints.RemovePointsFromBegin(Count);
	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);

	// Remove all the labels associated with thos points
	for (unsigned i=0; i<=Count; i++)
	{
		TLabelMap::iterator iter = m_mapLabels.find(i);
		if (iter != m_mapLabels.end())
			m_mapLabels.erase(iter);
	}
}

void CChartSerie::RemovePointsFromEnd(unsigned Count)
{
	unsigned uPtsCount = m_vPoints.GetPointsCount();

	m_vPoints.RemovePointsFromEnd(Count);
	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);

	// Remove all the labels associated with thos points
	unsigned uStart = uPtsCount - Count;
	for (unsigned i=0; i<=Count; i++)
	{
		TLabelMap::iterator iter = m_mapLabels.find(uStart + i);
		if (iter != m_mapLabels.end())
			m_mapLabels.erase(iter);
	}
}

void CChartSerie::ClearSerie()
{
	// We don't care about the return of RefreshAutoAxis: 
	// we will always refresh the control afterwards.
	m_vPoints.Clear();
	m_uLastDrawnPoint = 0;
	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);
}

double CChartSerie::GetXPointValue(unsigned PointIndex) const
{
	return m_vPoints.GetXPointValue(PointIndex);
}

double CChartSerie::GetYPointValue(unsigned PointIndex) const
{
	return m_vPoints.GetYPointValue(PointIndex);
}

void CChartSerie::SetXPointValue(unsigned PointIndex, double NewVal)
{
	m_vPoints.SetXPointValue(PointIndex, NewVal);

	// We don't care about the return of RefreshAutoAxis: 
	// we will always refresh the control afterwards.
	m_pParentCtrl->EnableRefresh(false);
	m_pHorizontalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);
}

void CChartSerie::SetYPointValue(unsigned PointIndex, double NewVal)
{
	m_vPoints.SetYPointValue(PointIndex, NewVal);

	// We don't care about the return of RefreshAutoAxis: 
	// we will always refresh the control afterwards.
	m_pParentCtrl->EnableRefresh(false);
	m_pVerticalAxis->RefreshAutoAxis();
	m_pParentCtrl->RefreshCtrl();
	m_pParentCtrl->EnableRefresh(true);
}

bool CChartSerie::GetSerieXMinMax(double &Min, double &Max) const
{
	if (!IsVisible()) 
		return false;
	return m_vPoints.GetSerieXMinMax(Min, Max); 
}

bool CChartSerie::GetSerieYMinMax(double &Min, double &Max) const
{
	if (!IsVisible())
		return false;
	return m_vPoints.GetSerieYMinMax(Min, Max); 
}

double CChartSerie::XScreenToValue(long XScreenCoord) const
{
	return m_pHorizontalAxis->ScreenToValue(XScreenCoord);
}

double CChartSerie::YScreenToValue(long YScreenCoord) const
{
	return m_pVerticalAxis->ScreenToValue(YScreenCoord);
}

void CChartSerie::ValueToScreen(double XValue, double YValue, CPoint &ScreenPoint) const
{
	ScreenPoint.x = m_pHorizontalAxis->ValueToScreen(XValue);
	ScreenPoint.y = m_pVerticalAxis->ValueToScreen(YValue);
}

bool CChartSerie::GetVisiblePoints(unsigned& uFirst, unsigned& uLast) const
{
	double Min=0, Max=0;
	bool bResult = false;
	switch (m_vPoints.GetOrdering())
	{
	case CChartPointsArray::poNoOrdering:
		uFirst = 0;
		uLast = GetPointsCount() - 1;
		bResult = true;
		break;
	case CChartPointsArray::poXOrdering:
		m_pHorizontalAxis->GetMinMax(Min, Max);
		bResult = m_vPoints.GetVisiblePoints(Min, Max, uFirst, uLast);
		break;
	case CChartPointsArray::poYOrdering:
		m_pVerticalAxis->GetMinMax(Min, Max);
		bResult = m_vPoints.GetVisiblePoints(Min, Max, uFirst, uLast);
		break;
	}

	return bResult;
}

CChartBalloonLabel* CChartSerie::CreateBalloonLabel(unsigned uPointIndex, 
													const TChartString& strLabelText)
{
	unsigned uCount = m_vPoints.GetPointsCount();
	ASSERT(uPointIndex<uCount);

	CChartBalloonLabel* pToReturn = new CChartBalloonLabel(m_pParentCtrl, this);
	pToReturn->SetLabelText(strLabelText);
	AttachCustomLabel(uPointIndex, pToReturn);
	return pToReturn;
}

CChartBalloonLabel* CChartSerie::CreateBalloonLabel(unsigned uPointIndex,
											 CChartLabelProvider* pLabelProvider)
{
	unsigned uCount = m_vPoints.GetPointsCount();
	ASSERT(uPointIndex<uCount);

	CChartBalloonLabel* pToReturn = new CChartBalloonLabel(m_pParentCtrl, this);
	pToReturn->SetLabelProvider(pLabelProvider);
	AttachCustomLabel(uPointIndex, pToReturn);
	return pToReturn;
}

void CChartSerie::AttachCustomLabel(unsigned uPointIndex, CChartLabel* pLabel)
{
	unsigned uCount = m_vPoints.GetPointsCount();
	ASSERT(uPointIndex<uCount);

	TLabelMap::iterator iter = m_mapLabels.find(uPointIndex);
	if (iter != m_mapLabels.end())
	{
		delete iter->second;
	}
	m_mapLabels[uPointIndex] = pLabel;
}

CPoint CChartSerie::GetPointScreenCoord(unsigned uPointIndex)
{
	unsigned uCount = m_vPoints.GetPointsCount();
	ASSERT(uPointIndex<uCount);

	double XVal = m_vPoints.GetXPointValue(uPointIndex);
	double YVal = m_vPoints.GetYPointValue(uPointIndex);

	CPoint ScreenPoint;
	ValueToScreen(XVal, YVal, ScreenPoint);
	return ScreenPoint;
}

void CChartSerie::DrawLabels(CDC* pDC)
{
	TLabelMap::iterator iter = m_mapLabels.begin();
	for (iter; iter!=m_mapLabels.end(); iter++)
	{
		iter->second->Draw(pDC,iter->first);
	}
}

void CChartSerie::SetVisible(bool bVisible)
{ 
	m_bIsVisible = bVisible; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::SetColor(COLORREF NewColor)	   
{ 
	m_SerieColor = NewColor; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::SetShadowColor(COLORREF NewColor) 
{ 
	m_ShadowColor = NewColor; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::EnableShadow(bool bEnable)
{
	m_bShadow = bEnable;
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::SetShadowDepth(int Depth)
{ 
	m_iShadowDepth = Depth; 
	m_pParentCtrl->RefreshCtrl();
}

void CChartSerie::EnableMouseNotifications(bool bClickEnabled, bool bMoveEnabled)
{
	m_bMouseClickNotifications = bClickEnabled;
	m_bMouseMoveNotifications = bMoveEnabled;
}

#ifndef NO_USER_DATA
void  CChartSerie::SetUserData(unsigned uPointIndex, void* pData)
{
	m_vPoints.SetUserData(uPointIndex, pData);
}

void* CChartSerie::GetUserData(unsigned uPointIndex)
{
	return m_vPoints.GetUserData(uPointIndex);
}
#endif
