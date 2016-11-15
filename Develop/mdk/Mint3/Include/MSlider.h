#pragma once

#include "MScrollBar.h"


// namespace : mint3
namespace mint3
{

#define MINT_SLIDER	"Slider"


// 슬라이더 클래스 선언
class MSlider : public MScrollBar
{
// Member functions
public:
	// 생성자
	MSlider( const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
};

} // namespace mint3
