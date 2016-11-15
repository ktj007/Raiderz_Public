#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{

class MLabelEx;




/// class : Label
class MLabelEx : public MWidget
{
/// Member variables
	MFont*		m_pFont;
	MCOLOR		m_nColor;
	int			m_nAlign;



/// Member functions
public:
	MLabelEx( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	void SetFont( MFont* pFont)					{ m_pFont = pFont;							}
	void SetColor( MCOLOR& nColor)				{ m_nColor = nColor;						}
	void SetAlign( int nAlign)					{ m_nAlign = nAlign;						}



/// Event handler
protected:
	virtual void OnChangedLookAndFill() override;
	virtual void OnDraw( MDrawContext* pDC) override;



/// LUA script
public:
	void glueSetFont( const char* szFont)		{ SetFont( MFontManager::Get( szFont));		}
	void glueSetColor( int r, int g, int b)		{ SetColor( MCOLOR( r, g, b));				}
	void glueSetAlign( const char* szAlign);



#define MINT_LABELEX	"LabelEx"
	virtual const char* GetClassName()			{ return MINT_LABELEX;						}
};


} // namespace mint3