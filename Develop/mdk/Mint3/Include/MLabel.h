#pragma once

#include "MWidget.h"
#include "MDrawContext.h"
#include "MLookNFeel.h"

namespace mint3 {

class MLabel;

/// Label의 Draw 코드가 있는 클래스, 이 클래스를 상속받아서 커스텀룩을 가질 수 있다.
class MLabelLook{
public:
	virtual void OnDraw(MLabel* pLabel, MDrawContext* pDC);
	virtual MRECT GetClientRect(MLabel* pLabel, MRECT& r);
};

/// Label
class MLabel : public MWidget{
protected:
	MCOLOR			m_TextColor;
	MAlignmentMode	m_AlignmentMode;

	DECLARE_LOOK(MLabelLook)
	DECLARE_LOOK_CLIENT()

protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

public:
	MLabel(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	/// 텍스트 컬러 지정
	virtual void SetTextColor(MCOLOR color);

	/// 텍스트 컬러 얻기
	MCOLOR GetTextColor(void);

	/// 정렬 얻기
	MAlignmentMode GetAlignment(void);
	/// 정렬 지정
	virtual MAlignmentMode SetAlignment(MAlignmentMode am);

	void		glueSetTextColor(unsigned int color );

#define MINT_LABEL	"Label"
	virtual const char* GetClassName(void){ return MINT_LABEL; }
};

} // namespace mint3