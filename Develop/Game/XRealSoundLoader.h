#pragma once

#include "XRealSound.h"


/// class : XRealSoundLoader
class XRealSoundLoader
{
/// Member variables
protected:



/// Member functions
public:
	XRealSoundLoader();
	virtual ~XRealSoundLoader();

	bool LoadSamples( const char* szFileName);
};
