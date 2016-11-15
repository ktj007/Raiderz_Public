#pragma once

#include "MScrollBar.h"



// namespace : mint3
namespace mint3
{


/// class MSlider
class MSlider : public MScrollBar
{
/// Member functions
public:
	MSlider( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual bool IsShowArrow() const				{ return false;								}



#define MINT_SLIDER		"Slider"
	virtual const char* GetClassName() override		{ return MINT_SLIDER;						}
};


} // namespace mint3
