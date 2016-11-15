#pragma once

#include "MintTypes.h"
#include "MBitmapDrawer.h"
#include "MResourceManager.h"
#include <string>
using namespace std;


namespace mint3
{


/// class : MLookStateInfo
class MLookStateInfo
{
/// Member variables
protected:
	MRECT*				m_pAssign;			///< 비트맵 assignment를 저장하는 변수 선언
	bool*				m_pTiled;			///< 비트맵 출력을 타일 형식으로 할지 여부
	MCOLOR*				m_pFontColor;		///< 텍스트 색상을 저장하는 변수


/// Member functions
public:
	MLookStateInfo();
	virtual ~MLookStateInfo();

	void SetAssignRect( int index, MRECT& r, bool bTile);
	MRECT* GetAssignRect();

	void SetFontColor( MCOLOR& color);
	MCOLOR GetFontColor() const;

	bool IsValidAssign() const						{ return ( m_pAssign ? true : false);		}
	bool IsValidFontColor() const					{ return ( m_pFontColor ? true : false);	}
};






/// class : MLookPartsInfo
class MLookPartsInfo
{
/// Member variables
protected:
	map< string, MLookStateInfo*>		m_StateInfo;


/// Member functions
public:
	MLookPartsInfo();
	virtual ~MLookPartsInfo();

	MLookStateInfo* FindState( const string& strState);

	void SetAssignRect( const string& strState, int index, MRECT& r, bool bTile);
	MRECT* GetAssignRect( const string& strState);
	bool IsTiledBmp( const string& strState);

	void SetFontColor( const string& strState, MCOLOR& color);
	MCOLOR GetFontColor( const string& strState);

	bool IsValidAssign( const string& strState);
	bool IsValidFontColor( const string& strState);
};






/// class : MLookBase
class MLookBase
{
/// Member variables
protected:
	string								m_strName;				///< 이름
	map< string, MLookPartsInfo*>		m_PartsInfo;			///< 파츠 정보
	MBitmap*							m_pBitmap;				///< 비트맵
	MDRAWBITMAPSTYLE					m_BitmapStyle;			///< 비트맵 그리기 스타일 변수
	MFont*								m_pFont;				///< Look에서 사용하는 폰트
	int									m_nTextAlign;			///< 텍스트 정렬
	MRECT								m_WorkRectOffset;		///< 작업 영역 오프셋
	MRECT								m_BkgrndRectOffset;		///< 배경 Look과 위젯의 영역이 맞지 않을 경우 조절 가능한 오프셋 값
	MEventHandler						m_EventHandler;			///< 스크립트 이벤트 핸들러
	static MWidget*						m_pWindow;				///< 호출 윈도우 포인터




/// Member functions
public:
	MLookBase();
	virtual ~MLookBase();

	void SetName( const char* szName)							{ m_strName = szName;										}
	const char* GetName() const									{ return m_strName.c_str();									}

	MLookPartsInfo* FindPart( const string& strPart);

	void SetBitmap( const string& strName)						{ m_pBitmap = MBitmapManager::Get( strName.c_str());		}
	MBitmap* GetBitmap()										{ return m_pBitmap;											}

	void SetAssignRect( const string& strPart, const string& strState, int index, MRECT& r, bool bTile =false);
	MRECT* GetAssignRect( const string& strPart, const string& strState);
	bool IsTiledBmp( const string& strPart, const string& strState);

	void SetStretchBlt( bool bStretch =true)					{ m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;	}
	bool IsStretchBlt() const									{ return ((m_BitmapStyle == DRAWBITMAP_STRETCH) ? true : false);		}

	void SetBitmapStyle( MDRAWBITMAPSTYLE style)				{ m_BitmapStyle = style;									}
	MDRAWBITMAPSTYLE GetBitmapStyle() const						{ return m_BitmapStyle;										}

	void SetFont( MFont* pFont)									{ m_pFont = pFont;											}
	MFont* GetFont()											{ return (m_pFont ? m_pFont : MFontManager::Get( NULL));	}

	void SetFontColor( const string& strPart, const string& strState, MCOLOR& color);
	MCOLOR GetFontColor( const string& strPart, const string& strState, MCOLOR& _default =MCOLOR(255,255,255));

	void SetTextAlign( int align)								{ m_nTextAlign = align;										}
	int GetTextAlign() const									{ return m_nTextAlign;										}

	void SetWorkRectOffset( MRECT& offset)						{ m_WorkRectOffset = offset;								}
	MRECT GetWorkRectOffset() const								{ return m_WorkRectOffset;									}
	MRECT GetWorkRect( MWidget* pWidget) const;

	void SetBkgrndRectOffset( MRECT& offset)					{ m_BkgrndRectOffset = offset;								}
	MRECT GetBkgrndRectOffset() const							{ return m_BkgrndRectOffset;								}
	MRECT GetBkgrndRect( MWidget* pWidget) const;

	void BindScriptEvent( string& strEventName, string& strScript);


	// 해당 파트에 비트맵 assign 정보가 유효한지 확인한다
	bool IsValidAssign( const string& strPart, const string& strState);

	// 해당 파트에 폰트 색상 정보가 유효한지 확인한다
	bool IsValidFontColor( const string& strPart, const string& strState);

	// 해당 파트를 그린다
	bool Draw( MDrawContext* pDC, MRECT& rect, const string& strPart, const string& strState);


/// Event handler
public:
	virtual bool OnEvent( MWidget* pWidget, MEventMsg nEvent);
	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC);
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC);
};




} // namespace mint3

