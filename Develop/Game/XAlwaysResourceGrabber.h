#pragma once 

#include "XBaseResourceGrabber.h"

class XAlwaysResourceGrabber : public XBaseAlwaysResourceGrabber
{
public:
	XAlwaysResourceGrabber() {}
	virtual ~XAlwaysResourceGrabber() {}
	virtual void Grab();
};