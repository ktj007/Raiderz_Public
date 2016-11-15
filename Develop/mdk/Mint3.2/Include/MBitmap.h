#pragma once

#include "MintTypes.h"
#include <string>
using namespace std;


namespace mint3
{

#define MBM_NORMAL		(0)
#define MBM_FlipLR		(1)
#define MBM_FlipUD		(1<<1)
#define MBM_RotL90		(1<<2)
#define MBM_RotR90		(1<<3)




/// class : MBitmap
class MBitmap
{
protected:
	string			m_strName;
	string			m_strFileName;
	unsigned int	m_DrawMode;
	unsigned int	m_nTime;
	bool			m_bCaching;

public:
	MBitmap();
	virtual ~MBitmap();

	virtual bool Create( const char* szName, const char* szFileName);
	virtual bool Destroy()							{ return true;					}

	const char* GetName() const						{ return m_strName.c_str();		}

	virtual void SetDrawMode( unsigned int dm)		{ m_DrawMode = dm;				}
	virtual unsigned int GetDrawMode()				{ return m_DrawMode;			}

	void SetCaching(bool bCaching)					{ m_bCaching = bCaching;	}
	bool IsCaching()								{ return m_bCaching;		}

	virtual void SetTexCoord( float x1, float y1, float x2, float y2)		{}
	virtual void GetTexCoord( float* pTexCoord)								{}
	
	virtual int GetWidth()							{ return 0;						}
	virtual int GetHeight()							{ return 0;						}

	virtual bool CreateTexture()					{ return false;		}
	unsigned int GetTime()							{ return m_nTime;	}
};


}
