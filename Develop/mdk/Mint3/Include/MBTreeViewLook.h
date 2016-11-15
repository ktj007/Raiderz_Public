
#pragma once

#include "Mint.h"
#include "MTreeView.h"
#include "MBitmapDrawer.h"



// namespace : mint3
namespace mint3
{



// MTREEVIEWREGION
enum MTREEVIEWREGION
{
	MTVR_NONE			= -1,		// none
	MTVR_FRAMEBOX		= 0,		// Frame box
	MTVR_SELECTBAR,					// Select bar
	MTVR_MAX
};



// MBTreeViewLook
class MBTreeViewLook : public MTreeViewLook
{
private:
	// 프레임의 비트맵을 저장하는 변수 선언
	MBitmapRep			m_Bitmaps[ MTVR_MAX][ 3];		// [Frame, Select bar] [Normal, Focused, Disabled]

	// 비트맵 그리기 스타일 변수
	MDRAWBITMAPSTYLE	m_BitmapStyle;

	// 비트맵 이미지를 타일형으로 찍는지 여부를 저장하는 변수
	bool				m_bTiled;

	// 커스텀 Look을 사용하는지 여부를 설정하는 변수
	bool				m_bCustomLook;

	// 폰트를 지정하는 변수
	MFont*				m_pFont;

	// 텍스트 색상을 저장하는 변수
	MCOLOR				m_nFontColor[ MTVR_MAX][ 3];	// [Frame, Select bar] [Normal, Focused, Disabled]




// Member functions
public:
	MBTreeViewLook();
	virtual ~MBTreeViewLook();


	// 커스텀 Look을 사용하는지 여부를 설정하는 함수
	void SetCustomLook( bool bCustom =true)			{ m_bCustomLook = bCustom;			}

	// 커스텀 Look을 사용하는지 여부를 구하는 함수
	bool IsCustomLook() const						{ return m_bCustomLook;				}



	/* Interface functions */

	// 프레임에 사용될 이미지를 설정하는 함수
	void SetBitmap( const string strFileName);

	// 프레임에 사용될 비트맵의 영역을 설정하는 함수
	void SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2);

	// 비트맵을 늘려 찍을 수 있게 설정하는 함수
	void SetStretchBlt( bool bStretch =true);

	// 타일 비트맵을 사용하는지 여부를 설정하는 함수
	void SetTiled( bool bTiled)									{ m_bTiled = bTiled;						}

	// 텍스트 폰트를 설정하는 함수
	void SetFont( MFont* pFont)									{ m_pFont = pFont;							}

	// 텍스트 색상을 설정하는 함수
	void SetFontColor( int type, int state, MCOLOR& color)		{ m_nFontColor[ type][ state] = color;		}



protected:
	virtual void OnDrawFrame( MTreeView* pTreeView, MDrawContext* pDC, MRECT& client);
	virtual void OnDrawItem( MTreeView* pTreeView, MTreeItem* pTreeItem, MDrawContext* pDC, MRECT& client, int x, int& y);
	virtual void OnDrawSelectbar( MTreeView* pTreeView, MDrawContext* pDC, MRECT& r);
	virtual MRECT GetClientRect( MTreeView* pTreeView, MRECT& client);
};


} // namespace mint3


