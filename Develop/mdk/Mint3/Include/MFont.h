//
//		Font
//
#pragma once

#include "MintTypes.h"

namespace mint3 {

#define MFONT_NAME_LENGTH		32

// Font abstract class
class MFont{
public:
#ifdef _DEBUG
	int		m_nTypeID;
#endif

public:
	char	m_szName[MFONT_NAME_LENGTH];
protected:
	bool	m_bOutline;
public:
	MFont(void);
	virtual ~MFont(void);
	virtual bool Create(const char* szName);
	virtual void Destroy(void);
	virtual int DrawText(int x, int y, const char* szText, int nLength = -1, DWORD dwColor = 0xffffffff) = 0;
	virtual int GetHeight(void) = 0;
	virtual int GetWidth(const char* szText, int nSize=-1) = 0;
	virtual int GetWidthWithoutAmpersand(const char* szText, int nSize=-1);
//	virtual int GetLineCount(int nRectWidth, const char* szText);		// dc 의 TextMultiLine펑션과 맞춰야함
	virtual int GetVisibleLength(int nRectWidth,const char* szText);	// nRectWidth 안에 보이는 글자수
	bool IsOutline() { return m_bOutline; }
};

} // namespace mint3 {