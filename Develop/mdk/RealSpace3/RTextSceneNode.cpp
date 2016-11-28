#include "StdAfx.h"

#include "RTextSceneNode.h"
#include "RFont.h"

namespace rs3 {

RTextSceneNode::RTextSceneNode() : RSceneNode(RRP_2D), m_nBufferSize(0), m_fScale(1.f), m_pFont(NULL), m_szText(NULL), m_dwColor(0xffffffff)
{
#if (_MSC_VER >= 1900)
	SetAABB(RBoundingBox::_INFINITY);
#else
	SetAABB( RBoundingBox::INFINITY ); // 무조건 렌더링 되도록
#endif
}

RTextSceneNode::~RTextSceneNode(void)
{
	ClearText();
}

void RTextSceneNode::ClearText()
{
	if(m_szText) 
	{
		delete[] m_szText;
		m_szText = NULL;
	}
}

void RTextSceneNode::SetFont(RFont *pFont)
{
	m_pFont = pFont;
}

void RTextSceneNode::SetBufferSize(int nSize)
{
	ClearText();
	m_szText = new TCHAR[nSize];
	m_nBufferSize = nSize;
}

void RTextSceneNode::SetText(const TCHAR *szText)
{
	int nLength = (int)_tcslen(szText) +1;
	if(nLength>m_nBufferSize)	// 버퍼 사이즈가 작으면 늘린다.
		SetBufferSize(nLength);

	_tcscpy(m_szText,szText);
}

void RTextSceneNode::SetPosition(int x,int y)
{
	m_nX = x;
	m_nY = y;
}

void RTextSceneNode::SetColor(DWORD dwColor)
{
	m_dwColor = dwColor;
}


void RTextSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	RFont::BeginFont();
	_ASSERT(m_pFont);

	TCHAR *ptString = m_szText;

	float fx = m_nX;
	float fy = m_nY;

	int nHeight = m_pFont->GetHeight();

	while ( *ptString )
	{
		int nStatement = 0;
		while (ptString[nStatement] != '\n'
			&& ptString[nStatement] != '\0')
			++nStatement;

		m_pFont->DrawText_UNICODE(fx, fy, MLocale::ConvAnsiToUTF16(ptString).c_str(), nStatement, m_dwColor);
		fy += nHeight;

		ptString += nStatement;
		if ( *ptString )
			++ptString;
	}

	RFont::EndFont();
}

}
