#pragma once

#include "MWidget.h"

namespace mint3 {

	/// 움직일 수 없는 프레임으로 구성된 위젯
	class MSheet : public MWidget{
	protected:
		bool			m_bDragAndDrop;
	protected:

		virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
		virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
		virtual void OnShow(void);
		virtual void OnDraw(MDrawContext* pDC){
			// Draw Nothing
		}
	public:
		MSheet(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
		virtual ~MSheet(void);

		void EnableDragAndDrop( bool bEnable);
		virtual bool IsDropable(MWidget* pSender){ return m_bDragAndDrop; }

#define MINT_SHEET	"Sheet"
		virtual const char* GetClassName(void){ return MINT_SHEET; }
	};

} // namespace mint3