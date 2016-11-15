#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{


class MLinePrinter;
class MScrollBar;




/// class : MLinePrinterLook
class MLinePrinterLook : public MLookBase
{
	/// Member variables
protected:
	int		m_nItemHeight;



	/// Member functions
public:
	MLinePrinterLook();

	void SetItemHeight( int height)						{ m_nItemHeight = height;			}
	int GetItemHeight()									{ return m_nItemHeight;				}

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawItems( MLinePrinter* pLinePrinter, MDrawContext* pDC);
	virtual void OnDrawItemText( MLinePrinter* pLinePrinter, size_t index, MRECT& r, MDrawContext* pDC);
};





/// class : MLinePrinter
class MLinePrinter : public MWidget
{
/// Member variables
protected:
	string*				m_StringList;				// Ring buffer
	size_t				m_nHead, m_nTail;
	size_t				m_nCapacity;
	MScrollBar*			m_pScrollBar;
	bool				m_bUpdateScrollBar;



/// Member functions
public:
	MLinePrinter( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MLinePrinter();

	void AddString( const char* szString);
	void ResetContent();

	void SetCapacity( size_t nCapacity);
	size_t GetCapacity() const							{ return m_nCapacity;					}

	size_t GetHead() const								{ return m_nHead;						}
	size_t GetTail() const								{ return m_nTail;						}

	const char* GetItemText( size_t nIndex) const		{ return m_StringList[ nIndex].c_str();	}

	int GetItemHeight();

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override			{ return m_pScrollBar;					}
	void ScrollPage( int delta);



protected:
	void UpdateScrollBar()								{ m_bUpdateScrollBar = true;			}



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;



/// LUA script
public:
	void		glueAddString( const char* szString)	{ AddString( szString);					}
	void		glueResetContent()						{ ResetContent();						}



#define MINT_LINEPRINTER	"LinePrinter"
	virtual const char* GetClassName() override			{ return MINT_LINEPRINTER;				}
};


} // namespace mint3
