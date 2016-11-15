#include "stdafx.h"
#include "MBitmap.h"


namespace mint3
{


MBitmap::MBitmap()
{
	m_DrawMode	= MBM_NORMAL;
	m_nTime		= INT_MAX;
	m_bCaching	= true;
}

MBitmap::~MBitmap()
{
	Destroy();
}


bool MBitmap::Create( const char* szName, const char* szFileName)
{
	m_strName = szName;
	m_strFileName = szFileName;

	return true;
}


}
