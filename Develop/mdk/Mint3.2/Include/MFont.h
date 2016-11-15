#pragma once


#include "MintTypes.h"
#include <string>
using namespace std;


namespace mint3
{


///< class : MFont
class MFont
{
protected:
	string		m_strName;
	int			m_nHeight;
	bool		m_bAntiAlias;
	bool		m_bBold;
	bool		m_bItalic;
	float		m_fOutline;
	float		m_fScale;


public:
	MFont();
	virtual ~MFont();

	virtual bool Create( const char* szName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f);
	virtual bool Destroy();

	const char* GetName() const									{ return m_strName.c_str();		}
	int GetHeight() const										{ return m_nHeight;				}
	bool GetAntialias() const									{ return m_bAntiAlias;			}
	bool GetBold() const										{ return m_bBold;				}
	bool GetItalic() const										{ return m_bItalic;				}
	float GetOutline() const									{ return m_fOutline;			}
	float GetScale() const										{ return m_fScale;				}

	virtual int GetWidth( const char* szText, int nLen= -1)		{ return 0;						}
	virtual int GetHeight()										{ return 0;						}
};


}
