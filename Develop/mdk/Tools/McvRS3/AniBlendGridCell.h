#pragma once

#include "GridCtrl_src/GridCell.h"

// CAniBlendGridCell

class CAniBlendGridCell : public CGridCell
{
	DECLARE_DYNCREATE(CAniBlendGridCell)

public:
	CAniBlendGridCell();
	virtual ~CAniBlendGridCell();

	virtual void OnDblClick( CPoint PointCellRelative );
    virtual void OnRClick( CPoint PointCellRelative );
	virtual BOOL OnSetCursor();
};
