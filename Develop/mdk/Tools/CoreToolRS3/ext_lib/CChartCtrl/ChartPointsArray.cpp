/*
 *
 *	ChartPointsArray.cpp
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
 */

#include "stdafx.h"
#include "ChartPointsArray.h"

CChartPointsArray::CChartPointsArray(unsigned iResize) 
  : m_pPoints(NULL), m_iMaxPoints(iResize), m_iCurrentPoints(0),
    m_iResize(iResize), m_Ordering(poXOrdering)
{
	m_pPoints = new SChartPoint[iResize];
}

CChartPointsArray::~CChartPointsArray()
{
	if (m_pPoints)
	{
		delete[] m_pPoints;
		m_pPoints = NULL;
	}
}

void CChartPointsArray::AddPoint(double X, double Y)
{
	if (m_iCurrentPoints == m_iMaxPoints)
	{
		m_iMaxPoints += m_iResize;
		SChartPoint* pOldPoints = m_pPoints;
		m_pPoints =	new SChartPoint[m_iMaxPoints];
		memcpy(m_pPoints,pOldPoints,m_iCurrentPoints*sizeof(SChartPoint));
		delete[] pOldPoints;
	}

	if (m_iCurrentPoints == 0)
	{
		m_dXMinVal = m_dXMaxVal = X;
		m_dYMinVal = m_dYMaxVal = Y;
	}
	else
	{
		if (X > m_dXMaxVal)  m_dXMaxVal = X;
		if (X < m_dXMinVal)  m_dXMinVal = X;
		if (Y > m_dYMaxVal)  m_dYMaxVal = Y;
		if (Y < m_dYMinVal)  m_dYMinVal = Y;
	}

	if (m_Ordering==poNoOrdering)
	{
		m_pPoints[m_iCurrentPoints].X = X;
		m_pPoints[m_iCurrentPoints].Y = Y;
		m_iCurrentPoints++;
	}
	else
	{
		SChartPoint newPoint;
		newPoint.X = X;
		newPoint.Y = Y;
		InsertNewPoint(newPoint);
	}
}

void CChartPointsArray::AddPoints(double* pX, double* pY, unsigned uCount)
{
	if (m_iCurrentPoints+uCount > m_iMaxPoints)
	{
		m_iMaxPoints = m_iCurrentPoints+uCount;
		SChartPoint* pOldPoints = m_pPoints;
		m_pPoints =	new SChartPoint[m_iMaxPoints];
		memcpy(m_pPoints,pOldPoints,m_iCurrentPoints*sizeof(SChartPoint));
		delete[] pOldPoints;
	}
	for (unsigned i=0; i<uCount; i++)
		AddPoint(pX[i], pY[i]);
}

void CChartPointsArray::SetPoints(double *X, double *Y, unsigned uCount)
{
	if (uCount > m_iMaxPoints)
	{
		if (m_pPoints)
			delete[] m_pPoints;
		m_pPoints = new SChartPoint[uCount];
		m_iMaxPoints = uCount;
	}
	m_iCurrentPoints = uCount;

	for (unsigned i=0;i<uCount;i++)
	{
		m_pPoints[i].X = X[i];
		m_pPoints[i].Y = Y[i];
	}
	ReorderPoints();
	RefreshMinMax();
}

void CChartPointsArray::Clear()
{
	if (m_pPoints)
		delete[] m_pPoints;
	m_pPoints = new SChartPoint[m_iResize];
	m_iMaxPoints = m_iResize;
	m_iCurrentPoints = 0;
}

void CChartPointsArray::RemovePointsFromBegin(unsigned Count)
{
	ASSERT (Count < m_iCurrentPoints);
	SChartPoint* pSource = m_pPoints + Count;
	memmove(m_pPoints, pSource, sizeof(SChartPoint) * (m_iCurrentPoints-Count));
	m_iCurrentPoints -= Count;
	RefreshMinMax();
}

void CChartPointsArray::RemovePointsFromEnd(unsigned Count)
{
	ASSERT (Count < m_iCurrentPoints);
	m_iCurrentPoints -= Count;
	RefreshMinMax();
}

void CChartPointsArray::SetXPointValue(unsigned PointIndex, double NewVal)
{
	ASSERT (PointIndex < m_iCurrentPoints);
	m_pPoints[PointIndex].X = NewVal;

	m_dXMinVal = m_dXMaxVal = m_pPoints[0].X;
	for (unsigned uIndex=0; uIndex<m_iCurrentPoints; uIndex++)
	{
		if (m_pPoints[uIndex].X > m_dXMaxVal)  m_dXMaxVal = m_pPoints[uIndex].X;
		if (m_pPoints[uIndex].X < m_dXMinVal)  m_dXMinVal = m_pPoints[uIndex].X;
	}

	if (m_Ordering == poXOrdering)
		ReorderPoints();
}

void CChartPointsArray::SetYPointValue(unsigned PointIndex, double NewVal)
{
	ASSERT (PointIndex < m_iCurrentPoints);
	m_pPoints[PointIndex].Y = NewVal;

	m_dYMinVal = m_dYMaxVal = m_pPoints[0].Y;
	for (unsigned uIndex=0; uIndex<m_iCurrentPoints; uIndex++)
	{
		if (m_pPoints[uIndex].Y > m_dYMaxVal)  m_dYMaxVal = m_pPoints[uIndex].Y;
		if (m_pPoints[uIndex].Y < m_dYMinVal)  m_dYMinVal = m_pPoints[uIndex].Y;
	}

	if (m_Ordering == poYOrdering)
		ReorderPoints();
}

bool CChartPointsArray::GetSerieXMinMax(double& Min, double& Max)  const
{
	if (m_iCurrentPoints==0)
		return false;

	Min = m_dXMinVal;
	Max = m_dXMaxVal;
	return true;
}

bool CChartPointsArray::GetSerieYMinMax(double& Min, double& Max)  const
{
	if (m_iCurrentPoints==0)
		return false;

	Min = m_dYMinVal;
	Max = m_dYMaxVal;
	return true;
}

void CChartPointsArray::SetOrdering(PointsOrdering newOrdering)
{
	m_Ordering = newOrdering;
	ReorderPoints();
}

bool CChartPointsArray::GetVisiblePoints(double dAxisMin, 
										 double dAxisMax,
										 unsigned& uFirstPt, 
										 unsigned& uLastPt) const
{
	if (m_iCurrentPoints == 0)
		return false;

	if (m_Ordering == poNoOrdering)
	{
		uFirstPt = 0;
		uLastPt = m_iCurrentPoints - 1;
		return true;
	}

	uFirstPt = BinarySearch(0,m_iCurrentPoints-1,dAxisMin);		     
	uLastPt = BinarySearch(uFirstPt,m_iCurrentPoints-1,dAxisMax);                
	return true;
}

void CChartPointsArray::ReorderPoints()
{
	switch (m_Ordering)
	{
	case poNoOrdering:
		break;
	case poXOrdering:
	    qsort(m_pPoints, m_iCurrentPoints, sizeof(SChartPoint), 
			  CChartPointsArray::ComparePointsOnX);
		break;
	case poYOrdering:
	    qsort(m_pPoints, m_iCurrentPoints, sizeof(SChartPoint), 
			  CChartPointsArray::ComparePointsOnY);
		break;
	}
}

void CChartPointsArray::InsertNewPoint(const SChartPoint& newPoint)
{
	if (m_iCurrentPoints == 0)
	{
		m_pPoints[0] = newPoint;
		m_iCurrentPoints++;
		return;
	}

	if (m_Ordering == poXOrdering)
	{
		if (newPoint.X >= m_pPoints[m_iCurrentPoints-1].X)
			m_pPoints[m_iCurrentPoints] = newPoint;
		else
		{
			for (unsigned i=0; i<m_iCurrentPoints; i++)
			{
				if (newPoint.X < m_pPoints[i].X)
				{
					InsertPointAtPos(newPoint,i);
					break;
				}
			}
		}
	}
	else if (m_Ordering == poYOrdering)
	{
		if (newPoint.Y >= m_pPoints[m_iCurrentPoints-1].Y)
			m_pPoints[m_iCurrentPoints] = newPoint;
		else
		{
			for (unsigned i=0; i<m_iCurrentPoints; i++)
			{
				if (newPoint.Y < m_pPoints[i].Y)
				{
					InsertPointAtPos(newPoint,i);
					break;
				}
			}
		}
	}

	m_iCurrentPoints++;
}

void CChartPointsArray::InsertPointAtPos(const SChartPoint& newPoint, int iPos)
{
	// Find the address of the insert point
	SChartPoint* pPointPos = m_pPoints + iPos;
	// Move all remaining points one place to the right.
	memmove(pPointPos+1,pPointPos,(m_iCurrentPoints-iPos)*sizeof(SChartPoint));
	// Store the new point
	*pPointPos = newPoint;
}

int CChartPointsArray::ComparePointsOnX(void const* pA, void const* pB)
{
    SChartPoint* pPointA = (SChartPoint *) pA;
    SChartPoint* pPointB = (SChartPoint *) pB;

    if (pPointA->X < pPointB->X) return -1;
    if (pPointA->X > pPointB->X) return 1;
    return 0;
}

int CChartPointsArray::ComparePointsOnY(void const* pA, void const* pB)
{
    SChartPoint* pPointA = (SChartPoint *) pA;
    SChartPoint* pPointB = (SChartPoint *) pB;

    if (pPointA->Y < pPointB->Y) return -1;
    if (pPointA->Y > pPointB->Y) return 1;
    return 0;
}

void CChartPointsArray::RefreshMinMax()
{
	m_dXMinVal = m_dXMaxVal = m_pPoints[0].X;
	m_dYMinVal = m_dYMaxVal = m_pPoints[0].Y;
	for (unsigned uIndex=0; uIndex<m_iCurrentPoints; uIndex++)
	{
		if (m_pPoints[uIndex].X > m_dXMaxVal)  m_dXMaxVal = m_pPoints[uIndex].X;
		if (m_pPoints[uIndex].X < m_dXMinVal)  m_dXMinVal = m_pPoints[uIndex].X;
		if (m_pPoints[uIndex].Y > m_dYMaxVal)  m_dYMaxVal = m_pPoints[uIndex].Y;
		if (m_pPoints[uIndex].Y < m_dYMinVal)  m_dYMinVal = m_pPoints[uIndex].Y;
	}
}

int CChartPointsArray::BinarySearch(unsigned uLeft, 
									unsigned uRight, 
									double dFind) const
{
	unsigned middle = uLeft + ((uRight - uLeft) /2);	
	double midVal = 0;
	if (m_Ordering == poXOrdering)
		midVal = m_pPoints[middle].X;
	if (m_Ordering == poYOrdering)
		midVal = m_pPoints[middle].Y;

	if(midVal > dFind)
	{
		if(uLeft < middle)				
			return BinarySearch(uLeft,middle-1,dFind);			
		else				
			return uLeft;	
	}
	else if(middle < uRight)					
		return BinarySearch(middle+1,uRight,dFind);				
	else					
		return uRight;
}

void CChartPointsArray::GetBezierControlPoints(unsigned uFirst, 
											   unsigned uLast,
											   SChartPoint* &pKnots, 
											   SChartPoint* &pFirstControlPoints, 
											   SChartPoint* &pSecondControlPoints) const
{
	int Count = uLast - uFirst - 1;
	if (Count < 1)
	{
		pFirstControlPoints = new SChartPoint[0];
		pSecondControlPoints = new SChartPoint[0];
		pKnots = new SChartPoint[0];
		return;
	}

	pKnots = new SChartPoint[uLast-uFirst];
	memcpy(pKnots,m_pPoints+uFirst,(uLast-uFirst)*sizeof(SChartPoint));

	// Calculate first Bezier control points
	// Right hand side vector
	double* rhs = new double[Count];

	// Set right hand side X values
	int i=0;
	for (i = 1; i < Count - 1; ++i)
		rhs[i] = 4 * pKnots[i].X + 2 * pKnots[i + 1].X;
	rhs[0] = pKnots[0].X + 2 * pKnots[1].X;
	rhs[Count - 1] = 3 * pKnots[Count - 1].X;
	// Get first control points X-values
	double* pFirstX = GetFirstControlPoints(rhs, Count);

	// Set right hand side Y values
	for (i = 1; i < Count - 1; ++i)
		rhs[i] = 4 * pKnots[i].Y + 2 * pKnots[i + 1].Y;
	rhs[0] = pKnots[0].Y + 2 * pKnots[1].Y;
	rhs[Count - 1] = 3 * pKnots[Count - 1].Y;
	// Get first control points Y-values
	double* pFirstY = GetFirstControlPoints(rhs, Count);

	// Fill output arrays.
	pFirstControlPoints = new SChartPoint[Count];
	pSecondControlPoints = new SChartPoint[Count];
	for (i = 0; i < Count; ++i)
	{
		// First control point
		pFirstControlPoints[i].X = pFirstX[i];
		pFirstControlPoints[i].Y = pFirstY[i];
		// Second control point
		if (i < Count - 1)
		{
			pSecondControlPoints[i].X =  2 * pKnots[i + 1].X - pFirstX[i + 1];
			pSecondControlPoints[i].Y =  2 * pKnots[i + 1].Y - pFirstY[i + 1];
		}
		else
		{
			pSecondControlPoints[i].X =  (pKnots[Count].X + pFirstX[Count - 1]) / 2;
			pSecondControlPoints[i].Y =  (pKnots[Count].Y + pFirstY[Count - 1]) / 2;
		}
	}

	delete[] rhs;
	delete[] pFirstX;
	delete[] pFirstY;
}

double* CChartPointsArray::GetFirstControlPoints(double* rhs, int Count) const
{
	double* pPoints = new double[Count]; // Solution vector.
	double* pTemp = new double[Count]; // Temp workspace.

	double b = 2.0;
	pPoints[0] = rhs[0] / b;
	int i =0;
	for (i = 1; i < Count; i++) // Decomposition and forward substitution.
	{
		pTemp[i] = 1 / b;
		b = (i < Count - 1 ? 4.0 : 2.0) - pTemp[i];
		pPoints[i] = (rhs[i] - pPoints[i - 1]) / b;
	}
	for (i = 1; i < Count; i++)
		pPoints[Count - i - 1] -= pTemp[Count - i] * pPoints[Count - i]; // Backsubstitution.
	delete[] pTemp;
	return pPoints;
}

#ifndef NO_USER_DATA
void  CChartPointsArray::SetUserData(unsigned uPointIndex, void* pData)
{
	ASSERT (uPointIndex < m_iCurrentPoints);
	m_pPoints[uPointIndex].pUserData = pData;
}

void* CChartPointsArray::GetUserData(unsigned uPointIndex)
{
	ASSERT (uPointIndex < m_iCurrentPoints);
	return m_pPoints[uPointIndex].pUserData;
}
#endif
