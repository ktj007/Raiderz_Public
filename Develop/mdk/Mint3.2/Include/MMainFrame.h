#ifndef _MMAIN_FRAME_H
#define _MMAIN_FRAME_H


#include "MWidget.h"



namespace mint3
{


/// class : MMainFrame	
class MMainFrame : public MWidget
{

public:
	MMainFrame( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual bool IsLayered() const override		{ return true;					}
};


}


#endif
