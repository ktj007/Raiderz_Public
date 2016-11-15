#include "stdafx.h"
#include "XScreenDebugger.h"
#include "XConst.h"
#include "MTypes.h"

XScreenDebugger* XScreenDebugger::GetInstance()
{
	static XScreenDebugger sd;
	return &sd;
}

XScreenDebugger::XScreenDebugger() : m_nLayer(LAYER1)
{
	InitEndLine();

	for (int i = 0; i < LAYER_ALL+1; i++)
	{
		m_bReleaseTextOnDraw[i] = true;
	}

#ifdef _DEBUG
	m_bVisible = true;
#else
	m_bVisible = false;
#endif

}

XScreenDebugger::~XScreenDebugger()
{

}

void XScreenDebugger::InitEndLine()
{
	for (int i = 0; i < LAYER_ALL+1; i++)
	{
		m_nEndLine[i] = XCONST::SCREEN_DEBUG_TOP;
	}
}

void XScreenDebugger::DrawLayer(MDrawContext *pDC, _Layer layer)
{
	pDC->SetColor(MCOLOR(XCONST::SCREEN_DEBUG_FONT_COLOR));

	for (list< pair<int, wstring>> ::iterator itor = m_BufferMap[layer].begin(); itor != m_BufferMap[layer].end(); ++itor)
	{
		MPOINT pt = GetPosFromKey((*itor).first);
		pDC->Text(pt.x, pt.y, MLocale::ConvUTF16ToAnsi((*itor).second.c_str()).c_str());
	}

}


void XScreenDebugger::Draw(MDrawContext *pDC)
{
//	TIMEPROFILE_THIS_FUNCTION;
	PFC_THISFUNC;

#ifdef _PUBLISH
	return;
#endif

	if (!m_bVisible) return;

	pDC->SetFont(global.ui->GetDefFont());

//	if (pDC->BeginFont())
	{
		DrawLayer(pDC, LAYER_ALL);

		DrawLayer(pDC, (_Layer)m_nLayer);

//		pDC->EndFont();
	}

	for (int i = 0; i < LAYER_ALL+1; i++)
	{
		if (m_bReleaseTextOnDraw[i])
			m_BufferMap[i].clear();
	}
	InitEndLine();
}

void XScreenDebugger::SelLayer(_Layer layer)
{
	m_nLayer = layer;
}

void XScreenDebugger::Print(int x, int y, const wchar_t* szText, _Layer layer)
{
	//TIMEPROFILE_THIS_FUNCTION;

#ifdef _PUBLISH
	return;
#endif
	if (!IsVisible()) return;
	if (layer != LAYER_ALL && layer != m_nLayer) return;

	int key = MakeKey(x, y);
	m_BufferMap[layer].push_back(pair<int, wstring>(key, szText));
}

void XScreenDebugger::PrintLine(int x, const wchar_t* szText, _Layer layer)
{
#ifdef _PUBLISH
	return;
#endif
	if (!IsVisible()) return;
	if (layer != LAYER_ALL && layer != m_nLayer) return;

	Print(x, m_nEndLine[layer], szText, layer);
	m_nEndLine[layer] += LINE_PIXEL;
}

MPOINT XScreenDebugger::GetPosFromKey(int key)
{
	return MPOINT(key / 1000, key % 1000);
}

int XScreenDebugger::MakeKey(int x, int y)
{
	return (x*1000 + y);
}

void XScreenDebugger::PrevLayer()
{
	if (--m_nLayer < LAYER1) m_nLayer=LAYER_MAX;
}

void XScreenDebugger::NextLayer()
{
	if (++m_nLayer > LAYER_ALL) m_nLayer=0;
}

void XScreenDebugger::SetLine(int y, _Layer layer)
{
	if (layer == LAYER_ALL)
	{
		for (int i = 0; i < LAYER_ALL; i++)
			m_nEndLine[i] = y;
	}
	else
	{
		m_nEndLine[layer] = y;
	}
}

void XScreenDebugger::SetReleaseTextOnDraw( bool b, _Layer layer )
{
	m_bReleaseTextOnDraw[layer] = b;
}

void XScreenDebugger::ReleaseText( _Layer layer )
{
	m_BufferMap[layer].clear();	
}