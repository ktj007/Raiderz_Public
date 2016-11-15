#include "stdafx.h"
#include "MFont.h"
#include "MDrawContext.h"

namespace mint3 {

// MFont Implementation
///////////////////////
MFont::MFont(void) : m_bOutline(false)
{
#ifdef _DEBUG
	m_nTypeID = MINT_BASE_CLASS_TYPE;
#endif
	m_szName[0] = 0;
}

MFont::~MFont(void)
{
}

bool MFont::Create(const char* szName)
{
	_ASSERT(strlen(szName)<MFONT_NAME_LENGTH);
	strcpy(m_szName, szName);

	return true;
}

void MFont::Destroy(void)
{
}

int MFont::GetWidthWithoutAmpersand(const char* szText, int nSize)
{
	int nAmpWidth = GetWidth("&");
	int nTextLen = strlen(szText);
	int nAmpCount = 0;
	for(int i=0; i<((nSize>0)?nSize:nTextLen); i++){
		if(szText[i]=='&') nAmpCount++;
	}
	int nTextWidth = GetWidth(szText, nSize);
	return nTextWidth-nAmpWidth*nAmpCount;
}

int MFont::GetVisibleLength(int nRectWidth, const char* szText)
{
	int nLength=strlen(szText);
	char text[3];

	int nCurrentWidth = 0;
	for(int i=0;i<nLength;i++)
	{
		text[1]=0;
		text[2]=0;
		text[0]=szText[i];
		if((unsigned char)szText[i]>127) {
			text[1]=szText[i+1];
			i++;
		}
		nCurrentWidth += GetWidth(text);

		if(nRectWidth<nCurrentWidth)
			return i;
	}

	return nLength;
}



} // namespace mint3