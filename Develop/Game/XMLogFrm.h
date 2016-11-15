#ifndef _XMLOG_H_
#define _XMLOG_H_

#include "MTextFrame.h"

class XMLogFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	XMLogFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_MLOGFRM		"mlogfrm"
	virtual const char* GetClassName(void){ return MINT_MLOGFRM; }

};


#endif
