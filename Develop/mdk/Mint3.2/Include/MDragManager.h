#pragma once

#include <string>
#include <vector>
using namespace std;


namespace mint3
{

class MWidget;
class MFont;
struct MSIZE;
struct MPOINT;
class MBitmap;
class MDrawContext;




/// struct : MDragObject
struct MDragObject
{
	string		m_strText;
	MBitmap*	m_pImage;
	DWORD		m_dwItemData;


	MDragObject( string& strText, MBitmap* pImage =NULL, DWORD dwUserData =0)
		: m_strText( strText), m_pImage( pImage), m_dwItemData( dwUserData)		{}
};





/// class : MDragData
class MDragData
{
protected:
	vector< MDragObject*>	m_DragObjects;
	MSIZE					m_ImageSize;
	MFont*					m_pFont;
	MCOLOR					m_nColor;
	MPOINT					m_ptHotSpot;
	float					m_fOpacity;
	MDrawEffect				m_DrawEffect;


public:
	MDragData();
	virtual ~MDragData();

	bool AddObject( MDragObject* pObject);

	size_t GetCount() const					{ return m_DragObjects.size();							}
	bool IsEmpty() const					{ return m_DragObjects.empty();							}

	void SetItemText( size_t index, const char* szText)	{ m_DragObjects[ index]->m_strText = szText;		}
	const char* GetItemText( size_t index)	{ return m_DragObjects[ index]->m_strText.c_str();		}
	void SetItemImage( size_t index, MBitmap* pImage)	{ m_DragObjects[ index]->m_pImage = pImage;			}
	MBitmap* GetItemImage( size_t index)	{ return m_DragObjects[ index]->m_pImage;				}
	DWORD GetItemData( size_t index)		{ return m_DragObjects[ index]->m_dwItemData;			}

	void SetImageSize( MSIZE& size)			{ m_ImageSize = size;									}
	MSIZE GetImageSize() const				{ return m_ImageSize;									}

	void SetFont( MFont* font)				{ m_pFont = font;										}
	MFont* GetFont() const					{ return m_pFont;										}

	void SetColor( MCOLOR& color)			{ m_nColor = color;										}
	MCOLOR GetColor() const					{ return m_nColor;										}

	void SetHotSpot( MPOINT& pt)			{ m_ptHotSpot = pt;										}
	MPOINT GetHotSpot() const				{ return m_ptHotSpot;									}

	void SetOpacity( float fOpacity)		{ m_fOpacity = fOpacity;								}
	float GetOpacity() const				{ return m_fOpacity;									}

	void SetEffect( MDrawEffect _effect)	{ m_DrawEffect = _effect;								}
	MDrawEffect GetEffect() const			{ return m_DrawEffect;									}

	void Clear();
};




/// class : MDragManager
class MDragManager
{
protected:
	MWidget*		m_pOwner;
	MDragData*		m_pDragData;
	MPOINT			m_ptCursor;
	MPOINT			m_ptLBtnDown;
	bool			m_bDragging;
	bool			m_bDropped;
	bool			m_bQueryDragBegin;
	bool			m_bDropWhenBtnDown;


public:
	MDragManager();
	virtual ~MDragManager();

	bool PostProcessDragEvent( MWidget* pMainFrame, MEvent* pEvent);

	bool Clear();
	bool IsDragging()						{ return m_bDragging;														}
	bool IsEmpty() const					{ return (m_pDragData ? false : true);										}

	MWidget* GetOwner()						{ return m_pOwner;															}
	MDragData* GetDragData()				{ return m_pDragData;														}
	bool IsDragging() const					{ return ( (m_pOwner != NULL  &&  m_pDragData != NULL) ? true : false);		}
	bool IsExternAdded() const				{ return m_bDropWhenBtnDown;													}

	void AddDragData( MWidget* pOwner, MDragObject* pDragObject);

	void DrawDragImage( MDrawContext* pDC, MPOINT& pt);


protected:
	MWidget* FindWindowFromPoint( MWidget* pMainFrame, MPOINT& pt);
};





} // namespace mint3
