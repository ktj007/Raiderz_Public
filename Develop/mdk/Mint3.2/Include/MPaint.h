#pragma once

#include "MWidget.h"


namespace mint3
{


/// class : MPaint
class MPaint : public MWidget
{
/// Member variables
protected:
	MCOLOR		m_Color;



/// Member functions
public:
	MPaint( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);


	// Set color
	void SetColor( int r, int g, int b, int a =255)					{ m_Color = MCOLOR( r, g, b, a);		}
	MCOLOR GetColor()												{ return m_Color;						}



/// Event handler
protected:
	virtual void OnDraw( MDrawContext* pDC) override;



/// LUA script
public:
	void			glueSetColor( int r, int g, int b)				{ SetColor( r, g, b);					}
	void			glueSetColorEx( int r, int g, int b, int a)		{ SetColor( r, g, b, a);				}
	MWLua::ret_int4	glueGetColor()									{ return MWLua::ret_int4( m_Color.r, m_Color.g, m_Color.b, m_Color.a);	}




#define MINT_PAINT	"Paint"
	virtual const char* GetClassName() override						{ return MINT_PAINT;					}
};

} // namespace mint3