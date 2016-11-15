#include "stdafx.h"
#include "MScriptHelper.h"
#include "Mint.h"
#include "MDrawContext.h"
#include "MColorTable.h"

namespace mint3 {

void MScriptHelper::glueDrawLine()// int sx, int sy, int tx)//, int ty, unsigned int color ) 
{
	/*
	MDrawContext* pDC = MGetMint()->GetDC();
	if (pDC == NULL) return;

	pDC->SetColor(MCOLOR(color));
	pDC->Line(sx, sy, tx, ty);
	*/
}

void mint3::MScriptHelper::glueDrawBox( int sx, int sy, int tx, int ty ) 
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if (pDC == NULL) return;

	MCOLOR stColor(m_r, m_g, m_b, m_a);
	pDC->SetColor(stColor);
	pDC->Rectangle(MRECT(sx, sy, tx-sx+1, ty-sy+1));
}
void mint3::MScriptHelper::glueFillBox( int sx, int sy, int tx, int ty ) 
{
	MDrawContext* pDC = MGetMint()->GetDC();
	if (pDC == NULL) return;

	MCOLOR stColor(m_r, m_g, m_b, m_a);
	pDC->SetColor(stColor);
	pDC->FillRectangle(MRECT(sx, sy, tx-sx+1, ty-sy+1));
}
} // namespace mint3
