#ifndef MINT4NULL_H
#define MINT4NULL_H

#include "Mint.h"
#include "MDrawContext.h"

using namespace mint3;

class MDrawContextNull : public MDrawContext
{
public:
	MDrawContextNull()						{}
	virtual ~MDrawContextNull()				{}
};



class MBitmapNull : public MBitmap
{
public:
	MBitmapNull()							{}
	virtual ~MBitmapNull()					{}
};



class MFontNull : public MFont
{
public:
	MFontNull()								{}
	virtual ~MFontNull()					{}
};



class Mint4Null : public Mint
{
public:
	Mint4Null()		{}

	virtual MBitmap* NewBitmap( const char* szName, const char* szFileName);
	virtual MFont* NewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f)				{ return NULL;			}
	virtual MFont* NewFontMemory( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f)		{ return NULL;			}
	virtual MCursor* NewCursor( const char* pszAliasName)		{ return NULL;			}
};

#endif