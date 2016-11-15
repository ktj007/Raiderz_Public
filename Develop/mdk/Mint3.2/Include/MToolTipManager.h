#pragma once

#include <string>
using namespace std;


namespace mint3
{

class MWidget;





/// class : MToolTipManager
class MToolTipManager
{
protected:
	MWidget*		m_pWidget;
	MWidget*		m_pToolTip;
	DWORD			m_dwTimer;
	DWORD			m_dwOpacityTimer;
	MPOINT			m_ptCursor;
	bool			m_bQueryToolTip;

public:
	MToolTipManager();
	virtual ~MToolTipManager();

	void Reset();
	void ResetCursor();

	bool PostProcessToolTipEvent( MWidget* pMainFrame, MEvent* pEvent);
	bool PostProcessToolTipRun( MWidget* pMainFrame, DWORD dwCurrTime);
	bool ChangedToolTipText( MWidget* pWidget);
	void DrawToolTipImage( MDrawContext* pDC);

	static MPOINT CalcToolTipPosition(const MPOINT& ptCursor, const MPOINT& ptOffset, const MSIZE& workspaceSize, const MSIZE& toolTipSize);
};





} // namespace mint3
