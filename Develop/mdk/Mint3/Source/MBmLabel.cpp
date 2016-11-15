#include "stdafx.h"
#include "MBmLabel.h"

namespace mint3 {

MBmLabel::MBmLabel(const char* szName, MWidget* pParent, MListener* pListener)
			: MWidget(szName, pParent, pListener)
{
	m_pLabelBitmap = NULL;
}


void MBmLabel::SetLabelBitmap(MBitmap* pLabelBitmap)
{
	m_pLabelBitmap = pLabelBitmap;

	if(m_pLabelBitmap==NULL) return;
	SetSize(m_pLabelBitmap->GetWidth(), m_pLabelBitmap->GetHeight());
}

void MBmLabel::SetCharSize(MSIZE &size)
{
	m_CharSize = size;
}

void MBmLabel::OnDraw(MDrawContext* pDC)
{
	if (m_pLabelBitmap == NULL) return;

	pDC->SetBitmap(m_pLabelBitmap);
	MRECT r = GetClientRect();

	int sx = 0, sy = 0;
	int tx = m_Rect.x, ty = m_Rect.y;
	
	int nLen = strlen( GetText());
	int nTexCol = m_pLabelBitmap->GetWidth() / m_CharSize.w;

	for (int i = 0; i < nLen; i++)
	{
		if ((isdigit(m_strText.at(i))) || (isalpha(m_strText.at(i))))
		{
			int n=0;
			if (isdigit(m_strText.at(i)))
			{
				n = m_strText.at(i) - '0';
			}
			else if (islower(m_strText.at(i)))
			{
				n = m_strText.at(i) - 'A' + 10;
			}
			else if (isupper(m_strText.at(i)))
			{
				n = m_strText.at(i) - 'a' + 36;
			}
			sx = (n % nTexCol) * m_CharSize.w;
			sy = (n / nTexCol) * m_CharSize.h;
			MRECT SrcRect = MRECT(sx, sy, m_CharSize.w, m_CharSize.h);
			pDC->Draw(tx, ty, SrcRect);
		}

		tx += m_CharSize.w;
	}
}

} // namespace mint3