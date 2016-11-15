#pragma once

#include "MWidget.h"

using namespace std;
using namespace mint3;



/// class : XBuffViewerLook
class XBuffViewerLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pSmallFont;
	MSIZE		m_IconSize;
	MBitmap*	m_pCooltimeImage;
	int			m_nCooltimeImgCellCount;
	int			m_nCooltimeImgCellCountRow;
	int			m_nCooltimeImgCellCountCol;
	MSIZE		m_CooltimeImgSize;


/// Member functions
public:
	XBuffViewerLook();

	void SetSmallFont( const char* szFont);
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetIconSize( int w, int h)					{ m_IconSize = MSIZE( w, h);							}
	MSIZE GetIconSize() const						{ return m_IconSize;									}

	void SetCooltimeImage( const char* szImage);
	MBitmap* GetCooltimeImage() const				{ return m_pCooltimeImage;								}

	void SliceCellCooltimeImg( int row, int col);

	void SetCooltimeImgCellCount( int nCount)		{ m_nCooltimeImgCellCount = min( nCount, m_nCooltimeImgCellCount);	}
	int GetCooltimeImgCellCount() const				{ return m_nCooltimeImgCellCount;						}


	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};





/// class : XBuffViewerItem
class XBuffViewerItem : public MWidget
{
/// Member variables
protected:
	int			m_nID;				// buff id
	bool		m_bDelete;
	MTimer		m_tFade;			// Fade on/off
	MPOINT		m_StartPos;			// StartPosition
	MPOINT		m_EndPos;			// End position
	MTimer		m_tMove;			// Move timer


/// Member functions
public:
	XBuffViewerItem( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XBuffViewerItem();

	void Create( int nID);
	void Delete();

	bool IsDelete() const				{ return m_bDelete;			}

	void MoveTo( MPOINT& pos);

	int GetID() const					{ return m_nID;				}
	MTimer GetFadeTimer() const			{ return m_tFade;			}



/// Event handler
protected:
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnRClick( MEvent* pEvent) override;
};





/// class : XUIBuffViewer
class XUIBuffViewer : public MWidget
{
/// Member variables
protected:
	list< XBuffViewerItem*>		m_BuffList;
	bool						m_bRearrange;



/// Member functions
public:
	XUIBuffViewer( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XUIBuffViewer();

	bool Add( int nID);
	bool Remove( int nID);
	void Clear();


/// Event handler
protected:
	virtual void OnRun( DWORD dwCurrTime) override;



/// LUA script
public:
	bool	glueAdd( int nID)							{ return Add( nID);					}
	bool	glueRemove( int nID)						{ return Remove( nID);				}
	void	glueClear()									{ Clear();							}



#define MINT_BUFFVIEWER		"BuffViewer"
	virtual const char* GetClassName() override			{ return MINT_BUFFVIEWER;			}
};