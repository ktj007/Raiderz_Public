/*
 *
 *	ChartSerie.h
 *
 *	Written by C?ric Moonen (cedric_moonen@hotmail.com)
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

#if !defined(AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_)
#define AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartAxis.h"
#include "ChartPointsArray.h"
#include "ChartLabel.h"
#include "ChartBalloonLabel.h"

#include <map>
#include "ChartString.h"

class CChartLineSerie;
class CChartPointsSerie;
class CChartLegend;

#define INVALID_POINT	UINT_MAX

//! Base class for all series of the control
/**
	This class is much more than a simple base class. It already store
	all the data points and provide utility functions to manipulate them.
	The drawing of the series is made through pure virtual functions which
	should be implemented by derived classes.
	Each series is identified by an Id.
**/
class CORETOOL_API CChartSerie
{
	friend CChartCtrl;
	friend CChartLegend;

public:
	//! Adds a single data point to the series.
	void AddPoint(double X, double Y);
	//! Adds an array of points to the series.
	/**
		Points which were already present in the series are kept.
		@param pX
			Array of X values for the points
		@param pY
			Array of Y values for the points
		@param Count
			Size of each of both arrays (number of points to add)
	**/
	void AddPoints(double* pX, double* pY, unsigned Count);
	//! Sets an array of points to the series.
	/**
		Points which were already present in the series are discarded.
		@param pX
			Array of X values for the points
		@param pY
			Array of Y values for the points
		@param Count
			Size of each of both arrays (number of points to add)
	**/
	void SetPoints(double* pX, double* pY, unsigned Count);
	//! Removes a certain amount of points from the begining of the series.
	void RemovePointsFromBegin(unsigned Count);
	//! Removes a certain amount of points from the end of the series.
	void RemovePointsFromEnd(unsigned Count);
	//! Removes all points from the series.
	void ClearSerie();

	//! Returns the number of points in the series.
	unsigned GetPointsCount() const  { return m_vPoints.GetPointsCount(); }
	//! Returns the Y value of a specific point in the series.
	double GetYPointValue(unsigned PointIndex) const;
	//! Returns the X value of a specific point in the series.
	double GetXPointValue(unsigned PointIndex) const;
	//! Sets the Y value of a specific point in the series.
	/**
		The control is refreshed to display the change.
		@param PointIndex
			The index of the points to change the Y value
		@param NewVal 
			The new Y value of the point
	**/
	void   SetYPointValue(unsigned PointIndex, double NewVal);
	//! Sets the X value of a specific point in the series.
	/**
		The control is refreshed to display the change.
		@param PointIndex
			The index of the points to change the Y value
		@param NewVal 
			The new X value of the point
	**/
	void   SetXPointValue(unsigned PointIndex, double NewVal);	
	
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

	//! Retrieves the minimum and maxium Y values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieYMinMax(double& Min, double& Max)  const;
	//! Retrieves the minimum and maxium X values of the series.
	/**
		@param Min
			Minimum value will be stored in this parameter
		@param Max
			Maximum value will be stored in this parameter
		@return
			false if the series doesn't contain data or is invisible
	**/
	bool GetSerieXMinMax(double& Min, double& Max)  const;

	//! Sets the name of the series, which is displayed in the legend.
	void		 SetName(const TChartString& NewName);
	//! Returns the name of the series.
	TChartString GetName() const              { return m_strSerieName; }

	//! Converts any data point into its relative screen point.
	/**
		@param XValue
			The X value of the data point
		@param YValue
			The Y value of the data point
		@param ScreenPoint
			The screen point will be stored in the parameter
	**/
	void ValueToScreen(double XValue, double YValue, CPoint& ScreenPoint)  const;
	//! Converts an Y screen value into its relative Y data value.
	double YScreenToValue(long YScreenCoord)  const;
	//! Converts an Xscreen value into its relative X data value.
	double XScreenToValue(long XScreenCoord)  const;

	//! Constructor
	CChartSerie(CChartCtrl* pParent);
	//! Destructor
	virtual ~CChartSerie();

	//! Specifies how the points should be ordered in the series.
	/**
		This specifies if the points should be ordered on their X values,
		on their Y values or not ordered (kept in order they are added to 
		the control). Ordering can improve performances a lot but makes it
		impossible to draw some specific curves (for instance, drawing an 
		ellipse is only possible if no ordering is set).
	**/
	virtual void SetSeriesOrdering(CChartPointsArray::PointsOrdering newOrdering);

	//! Specifies if the series is visible or not.
	/**
		An invisible series won't affect automatic axis: it is considered
		as if it was not in the control.
	**/
	void SetVisible(bool bVisible);
	//! Returns true if the series is visible.
	bool IsVisible()  const         { return m_bIsVisible; }

	//! Returns the color of the series.
	COLORREF GetColor() const			   { return m_SerieColor; }
	//! Sets the color of the series.
	void SetColor(COLORREF NewColor);
	//! Returns the color of the shadow.
	COLORREF GetShadowColor() const		   { return m_ShadowColor; }
	//! Sets the color of the shadow.
	void SetShadowColor(COLORREF NewColor);
	//! Enables or disables the shadow for the series.
	void EnableShadow(bool bEnable);
	//! Sepcifies the depth (in pixels) of the shadow.
	void SetShadowDepth(int Depth);

	//! Retrieves the screen point of a specific data point.
	CPoint GetPointScreenCoord(unsigned uPointIndex);
	//! Creates and attaches a balloon label on a point of the series.
	/**
		This functions specifies a static text to display in the label.
		@param uPointIndex
			The index of the point on which the label should be attached
		@param strLabelText
			The text of the label
		@return the created CChartBalloonLabel
	**/
	CChartBalloonLabel* CreateBalloonLabel(unsigned uPointIndex, const TChartString& strLabelText);
	//! Creates and attaches a balloon label on a point of the series.
	/**
		This functions specifies a CChartLabelProvider object which is used to
		supply the text of the label. This is useful if you want more flexibility
		for the text of the label (display information about the point value for
		instance).
		@param uPointIndex
			The index of the point on which the label should be attached
		@param pLabelProvider
			Object providing the text displayed on the label
		@return the created CChartBalloonLabel
	**/
	CChartBalloonLabel* CreateBalloonLabel(unsigned uPointIndex, CChartLabelProvider* pLabelProvider);
	//! Attaches a custom label on a point of the series.
	/**
		@param uPointIndex
			The index of the point on which the label should be attached
		@param pLabel
			The label to attach to the point
	**/
	void AttachCustomLabel(unsigned uPointIndex, CChartLabel* pLabel);

	//! Tests if a certain screen point is on the series.
	/**
		This function should be overidden by all child classes.
		@param screenPoint
			The screen point to test
		@param uIndex
			If the point is close to a specific point of the series, its index is stored here.
		@return true if the point is on the series
	**/
	virtual bool IsPointOnSerie(const CPoint& screenPoint, unsigned& uIndex) const = 0;

	//! Returns the series Id.
	unsigned GetSerieId() const  { return m_uSerieId; }
	//! Enables or disables certain mouse notifications on the series.
	/**
		Checking if a point is on the series could degrade performances if 
		it has to be done for each mouse event. This function allows to disable
		certain notifications, in which case the test won't be done. By default
		the series reacts on mouse clicks but not on mouse moves.
		@param bClickEnabled
			Specifies if the series reacts on mouse clicks.
		@param bMoveEnabled
			Specifies if the series reacts on mouse moves.
	**/
	void EnableMouseNotifications(bool bClickEnabled, bool bMoveEnabled);

protected:
	//! Returns the first and last visible points of the series.
	/**
		This function only works if ordering is enabled.
		@param uFirst
			The index of the first visible point is stored in this argument
		@param uLast
			The index of the last visible point is stored in this argument
		@return false if the series has no ordering or no data points.
	**/
	bool GetVisiblePoints(unsigned& uFirst, unsigned& uLast) const;

	//! Draws the legend icon for the series.
	/**
		This pure virtual function should be overriden by child classes.
		@param pDC
			The device context used to draw
		@param rectBitmap
			The rectangle in which to draw the legend icon
	**/
    virtual void DrawLegend(CDC* pDC, const CRect& rectBitmap) const =0;

	//! Draws the most recent points of the series.
	/**
		This pure virtual function should be overriden by child classes.
		This function should only draw the points that were not previously 
		drawn.
		@param pDC
			The device context used to draw
	**/
	virtual void Draw(CDC* pDC) =0;
	//! Redraws the full series.
	/**
		This pure virtual function should be overriden by child classes.
		@param pDC
			The device context used to draw
	**/
	virtual void DrawAll(CDC *pDC) =0;

	//! The helper class containing all the data points.
	CChartPointsArray m_vPoints;
	//! Index of the last point drawn
	unsigned m_uLastDrawnPoint;		

	//! The parent charting control.
	CChartCtrl* m_pParentCtrl;
	//! The related vertical axis.
	CChartAxis* m_pVerticalAxis;    
	//! The related horizontal axis.
	CChartAxis* m_pHorizontalAxis;  

	//! The series name displayed in the legend.
	TChartString m_strSerieName;	

	//! Specifies if the series is visible.
	bool        m_bIsVisible;
	//! Specifies if the series has shadow enabled.
	bool		m_bShadow;	
	//! Color of the series
	COLORREF	m_SerieColor;
	//! Color of the shadow
	COLORREF	m_ShadowColor;
	//! Depth (in pixels) of the shadow
	int			m_iShadowDepth;
	//! The rectangle in which the series should be drawn.
	CRect		m_PlottingRect;

private:
	//! Sets the plotting rectangle.
	void SetPlottingRect(const CRect& plottingRect)  { m_PlottingRect = plottingRect; }
	//! Draws the labels of the series.
	void DrawLabels(CDC* pDC);

	//! Returns true if the series reacts on mouse moves.
	bool NotifyMouseMoveEnabled()  { return m_bMouseMoveNotifications;  }
	//! Returns true if the series reacts on mouse clicks.
	bool NotifyMouseClickEnabled() { return m_bMouseClickNotifications; }

	//! The next available series Id
	static unsigned m_uNextFreeId;
	//! The series Id
	unsigned m_uSerieId;

	typedef std::map<unsigned, CChartLabel*> TLabelMap;
	//! Map containing the labels of the series.
	TLabelMap m_mapLabels;

	//! Specifies if the series reacts on mouse clicks.
	bool m_bMouseClickNotifications;
	//! Specifies if the series reacts on mouse moves.
	bool m_bMouseMoveNotifications;
};

#endif // !defined(AFX_CHARTSERIE_H__FFCF0E32_10E7_4A4D_9FF3_3C6177EDE4B1__INCLUDED_)
