

#include "stdafx.h"
#include "MBPopupBoxLook.h"


// namespace : mint3
namespace mint3
{


// MBPopupBoxLook
MBPopupBoxLook::MBPopupBoxLook()
{
	m_bCustomLook = false;
	m_BitmapStyle = DRAWBITMAP_STRETCH;
	m_bTiled = false;
}

MBPopupBoxLook::~MBPopupBoxLook()
{
}


// SetBitmap
void MBPopupBoxLook::SetBitmap( const string strFileName)
{
	m_Bitmaps.strBitmapName = strFileName;
}


// SetAssignRect
void MBPopupBoxLook::SetAssignRect( int index, float x1, float y1, float x2, float y2)
{
	if ( index >= 9)	return;

	m_Bitmaps.m_rCoord[ index].x = (int)x1;
	m_Bitmaps.m_rCoord[ index].y = (int)y1;
	m_Bitmaps.m_rCoord[ index].w = (int)x2;
	m_Bitmaps.m_rCoord[ index].h = (int)y2;
}


// OnDraw
void MBPopupBoxLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	// 프레임을 그립니다
	MRECT client = pWidget->GetClientRect();
	DrawBitmapByStyle( pDC, client, MBitmapManager::Get( m_Bitmaps.strBitmapName.c_str()), m_Bitmaps.m_rCoord, m_BitmapStyle, m_bTiled);
}


// SetStretchBlt
void MBPopupBoxLook::SetStretchBlt( bool bStretch /*=true*/)
{
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}




} // namespace mint3
