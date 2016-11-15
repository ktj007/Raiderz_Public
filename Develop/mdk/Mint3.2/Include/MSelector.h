#pragma once

#include "MWidget.h"
#include "MLookBase.h"
#include <vector>


namespace mint3
{

class MSelector;


// Selector style
#define MSES_HORZ					0x0000L
#define MSES_VERT					0x0001L





/// class : MSelectorLook
class MSelectorLook : public MLookBase
{
/// Member variables
protected:


/// Member functions
public:
	MSelectorLook();

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;

protected:
	virtual void OnDrawItem( MSelector* pSelector, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawItemText( MSelector* pSelector, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawTrack( MSelector* pSelector, MRECT& r, MDrawContext* pDC);
};






/// struct : MSelectorItem
struct MSelectorItem
{
	string		strString;
	double		fValue;
};







/// class : MSelector
class MSelector : public MWidget
{
/// Member variables
protected:
	int							m_nStyle;
	vector< MSelectorItem*>		m_ItemList;
	int							m_nCurSel;
	int							m_nMouseOver;



// Member functions
public:
	MSelector( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MSelector();

	int AddItem( const char* szString, double fVal);
	int InsertItem( size_t nIndex, const char* szString, double fVal);
	int DeleteItem( size_t nIndex);
	void ResetContent();
	size_t GetItemCount() const							{ return m_ItemList.size();							}

	void SetStyle( int nStyle)							{ m_nStyle = (nStyle  ?  MSES_VERT : MSES_HORZ);	}
	int GetStyle() const								{ return m_nStyle;									}

	int SetCurSel( int nIndex =-1);
	int GetCurSel() const								{ return m_nCurSel;									}

	int GetMouseOver() const							{ return m_nMouseOver;								}

	int GetItemFromValue( double fVal);

	void SetItemString( size_t nIndex, const char* szString);
	const char* GetItemString( size_t nIndex);

	void SetItemValue( size_t nIndex, double fVal);
	double GetItemValue( size_t nIndex) const;



protected:
	int ItemFromPoint( const MPOINT& pt);


// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnLeave() override;
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor) override;


// Lua glue functions
public:
	int glueAddItem( const char* szString, double fVal)						{ return AddItem( szString, fVal);				}
	int glueInsertItem( size_t nIndex, const char* szString, double fVal)	{ return InsertItem( nIndex, szString, fVal);	}
	int glueDeleteItem( size_t nIndex)										{ return DeleteItem( nIndex);					}
	void glueResetContent()													{ ResetContent();								}
	int glueGetItemCount() const											{ return (int)GetItemCount();					}
	void glueSetStyle( int nStyle)											{ SetStyle( nStyle);							}
	int glueGetStyle() const												{ return GetStyle();							}
	int glueSetCurSel( int nIndex)											{ return SetCurSel( nIndex);					}
	int glueGetCurSel() const												{ return GetCurSel();							}
	int glueGetItemFromValue( double fVal)									{ return GetItemFromValue( fVal);				}
	void glueSetItemString( size_t nIndex, const char* szString)			{ SetItemString( nIndex, szString);				}
	const char* glueGetItemString( size_t nIndex)							{ return GetItemString( nIndex);				}
	void glueSetItemValue( size_t nIndex, double fVal)						{ SetItemValue( nIndex, fVal);					}
	double glueGetItemValue( size_t nIndex) const							{ return GetItemValue( nIndex);					}



#define MINT_SELECTOR	"Selector"
	virtual const char* GetClassName() override								{ return MINT_SELECTOR;							}
};



} // namespace mint3
