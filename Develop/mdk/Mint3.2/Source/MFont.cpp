#include "stdafx.h"
#include "MFont.h"


namespace mint3
{


MFont::MFont()
{
	m_nHeight = 0;
	m_bBold = false;
	m_bItalic = false;
	m_bAntiAlias = false;
	m_fOutline = 0.0f;
	m_fScale = 1.0f;
}


MFont::~MFont()
{
	Destroy();
}


bool MFont::Create( const char* szName, const char* szFileName, int nHeight, bool bBold, bool bItalic, bool bAntiAlias, float fOutline, float fScale)
{
	m_strName = szName;
	m_nHeight = nHeight;
	m_bBold = bBold;
	m_bItalic = bItalic;
	m_bAntiAlias = false;
	m_fOutline = fOutline;
	m_fScale = fScale;

	return true;
}


bool MFont::Destroy()
{
	return true;
}


}
