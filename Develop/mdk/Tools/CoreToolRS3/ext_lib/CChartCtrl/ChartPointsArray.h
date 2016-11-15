/*
 *
 *	ChartPointsArray.h
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

#pragma once

//! Manages an array of points which supports fast resizing.
/**
	This class is used internally to store the data for all the points. The data
	is stored in a C-style array. The internal buffer can grow dynamically depending
	on the needs.
**/
class CChartPointsArray
{
public:
	//! Constructor
	/**
		@param iResize
			The size by which the internal buffer is increased when reallocation occurs
	**/
	CChartPointsArray(unsigned iResize = 1000);
	//! Destructor
	~CChartPointsArray();

	//! Returns the number of points currently stored.
	unsigned GetPointsCount() const   { return m_iCurrentPoints; }
	//! Sets the size by which the internal buffer is increased when reallocation occurs
	void SetResize(int iResize)  { m_iResize = iResize; }

	//! Adds a new point in the array.
	/**
		@param X
			The X value of the point
		@param Y
			The Y value of the point
	**/
	void AddPoint(double X, double Y);
	//! Adds multiple points in the array.
	/**
		The points are added to the ones currently stored in the array.
		@param pX
			Array containing the X value of the points
		@param pY
			Array containing the Y value of the points
		@param uCount
			The number of points to add
	**/
	void AddPoints(double* pX, double* pY, unsigned uCount);
	//! Sets multiple points in the array.
	/**
		The points currently stored in the array are first removed
		before adding the new points.
		@param X
			Array containing the X value of the points
		@param X
			Array containing the Y value of the points
		@param uCount
			The number of points to add
	**/
	void SetPoints(double *X, double *Y, unsigned uCount);
	//! Removes all the points from the array.
	void Clear();
	//! Removes a certain amount of points from the begining of the series.
	void RemovePointsFromBegin(unsigned Count);
	//! Removes a certain amount of points from the end of the series.
	void RemovePointsFromEnd(unsigned Count);

	//! Returns the X value of the point specified by its index in the array.
	inline double GetXPointValue(unsigned PointIndex) const
	{
		ASSERT (PointIndex < m_iCurrentPoints);
		return m_pPoints[PointIndex].X;
	}
	//! Returns the Y value of the point specified by its index in the array.
	inline double GetYPointValue(unsigned PointIndex) const
	{
		ASSERT (PointIndex < m_iCurrentPoints);
		return m_pPoints[PointIndex].Y;
	}
	//! Sets the X value of the point specified by its index in the array.
	void   SetXPointValue(unsigned PointIndex, double NewVal);	
	//! Returns the Y value of the point specified by its index in the array.
	void   SetYPointValue(unsigned PointIndex, double NewVal);
	
	//! Retrieves the minimum and maximum X values of the points stored in the array.
	bool GetSerieXMinMax(double& Min, double& Max)  const;
	//! Retrieves the minimum and maximum Y values of the points stored in the array.
	bool GetSerieYMinMax(double& Min, double& Max)  const;

	//! Enumeration listing the types of ordering.
	enum PointsOrdering
	{
		poNoOrdering,
		poXOrdering,
		poYOrdering
	};
	//! Specifies how the points should be ordered in the array.
	/**
		This specifies if the points should be ordered on their X values,
		on their Y values or not ordered (kept in order they are added to 
		the control). Ordering can improve performances a lot but makes it
		impossible to draw some specific curves (for instance, drawing an 
		ellipse is only possible if no ordering is set).
	**/
	void SetOrdering(PointsOrdering newOrdering);
	//! Retrieves the ordering of the points in the array.
	PointsOrdering GetOrdering() const  { return m_Ordering; } 

	//! Retrieves the index of the points which are between two given values.
	/**
		If the points are not ordered, uFirstPt will contain 0 and uLastPt
		will contain the index of the last point in the array.
		@param dAxisMin
			The minimum value to retrieve the first visible point
		@param dAxisMax
			The maximum value to retrieve the last visible point
		@param uFirstPt
			This parameter will store the index of the first visible point
		@param uLastPt
			This parameter will store the index of the last visible point
		@return false if no points are in the array. 
	**/
	bool GetVisiblePoints(double dAxisMin, double dAxisMax, 
						  unsigned& uFirstPt, unsigned& uLastPt) const;
	
	//! Structure containing a point data
	struct SChartPoint
	{
		SChartPoint() : X(0.0), Y(0.0)  
		{ 
			#ifndef NO_USER_DATA
			pUserData = NULL;
			#endif
		}

		//! The point X value.
		double X;
		//! The point Y value.
		double Y;
		#ifndef NO_USER_DATA
		//! Optional user data.
		void *pUserData;
		#endif
	};
	//! Retrieves the control points of a bezier curve fitting the points stored in the array.
	/**
		@param uFirst
			The index of the first point of the bezier curve
		@param uLast
			The index of the last point of the bezier curve
		@param pKnots
			This parameter will store the points data
		@param pFirstControlPoints
			This parameter will store the primary control points of the bezier curve
		@param pSecondControlPoints
			This parameter will store the secondary control points of the bezier curve
	**/
	void GetBezierControlPoints(unsigned uFirst, unsigned uLast, SChartPoint* &pKnots,
				SChartPoint* &pFirstControlPoints, SChartPoint* &pSecondControlPoints) const;

#ifndef NO_USER_DATA
	//! Sets user data for a specific point. 
	/**
		User data can be disabled by adding the flag NO_USER_DATA in the preprocessor
		definitions. This is usefull when you don't want to have an additional pointer
		stored for each points in your series.
	**/
	void  SetUserData(unsigned uPointIndex, void* pData);
	//! Retrieves user data for a specific point. 
	/**
		User data can be disabled by adding the flag NO_USER_DATA in the preprocessor
		definitions. This is usefull when you don't want to have an additional pointer
		stored for each points in your series.
	**/
	void* GetUserData(unsigned uPointIndex);
#endif

private:
	//! Caches the minimum X value.
	double m_dXMinVal;
	//! Caches the maximum X value.
	double m_dXMaxVal;
	//! Caches the minimum Y value.
	double m_dYMinVal;
	//! Caches the maximum Y value.
	double m_dYMaxVal;

	//! Refreshes the point ordering.
	void ReorderPoints();
	//! Recalculates the min and max values.
	void RefreshMinMax();
	//! Inserts a new point in the array.
	void InsertNewPoint(const SChartPoint& newPoint);
	//! Inserts a new point at a specific position in the array.
	void InsertPointAtPos(const SChartPoint& newPoint, int iPos);
	//! Comparison function which compares two points based on their X values.
	static int ComparePointsOnX(void const* pA, void const* pB);
	//! Comparison function which compares two points based on their Y values.
	static int ComparePointsOnY(void const* pA, void const* pB);
	//! Implements a binary search used to find the index of a points give the X or Y value.
	int BinarySearch(unsigned uLeft, unsigned uRight, double dFind) const;

	double* GetFirstControlPoints(double* rhs, int Count) const;


	//! The array of points
	SChartPoint* m_pPoints;
	//! The number of allocated points 
	unsigned m_iMaxPoints;
	//! The number of points currently used 
	unsigned m_iCurrentPoints;
	//! The size by which the array is incremented once it is full
	unsigned m_iResize;
	//! The ordering of the points
	PointsOrdering m_Ordering;
};