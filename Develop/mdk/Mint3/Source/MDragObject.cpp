#include "stdafx.h"
#include "MDragObject.h"
#include <string.h>

namespace mint3 {

MDragObject::MDragObject(int nID, const char* szString, MBitmap* pBitmap)
{
	m_nID = nID;
	strncpy(m_szString, szString, MDRAGOBJECT_STRING_LENGTH);
	m_pBitmap = pBitmap;
}

MDragObject::~MDragObject(void)
{
}

} // namespace mint3