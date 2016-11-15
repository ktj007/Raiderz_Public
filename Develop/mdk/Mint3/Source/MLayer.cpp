/************************************************************************
MLayer

Prog. : Lim Dong Hwan
Date  : 29.JUN.2009
Desc. : Layer 위젯 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MLayer.h"



// namespace : mint3
namespace mint3
{

/*	MLayer
	생성자
*/
MLayer::MLayer( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MWidget( szName, pParent, pListener)
{
	Enable( true);
	Show( true);
	SetAnchor( MAnchorStyle( true, true, true, true));
}


/*	OnEvent
	OnEvent 이벤트 핸들러
*/
bool MLayer::OnEvent( MEvent* pEvent, MListener* pListener)
{
	return false;
}


/*	OnDraw
	OnDraw 이벤트 핸들러
*/
void MLayer::OnDraw( MDrawContext* pDC)
{
}


/*	OnSize
	OnSize 이벤트 핸들러
*/
void MLayer::OnSize(int w, int h)
{
	MWidget::OnSize( w, h);
}


} // namespace mint3
