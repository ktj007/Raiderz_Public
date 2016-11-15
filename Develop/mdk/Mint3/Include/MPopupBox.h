

#pragma once

#include "MWidget.h"
#include "MBitmapDrawer.h"


// namespace : mint3
namespace mint3
{



// class : MPopupBoxLook
class MPopupBoxLook
{
protected:
	MPOINT		m_ptOffset;


public:
	MPopupBoxLook();

	void SetOffset( int x, int y)		{ m_ptOffset.x = x;		m_ptOffset.y = y;	}
	MPOINT GetOffset() const			{ return m_ptOffset;						}

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC);
	virtual MRECT GetClientRect( MWidget* pWidget, MRECT& client);
};






// class : MPopupBox
class MPopupBox
{
public:
	MPopupBox()					{}
	virtual ~MPopupBox()		{}
};



} // namespace mint3

