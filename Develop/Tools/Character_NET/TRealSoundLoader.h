#pragma once

#include "TRealSound.h"


/// class : TRealSoundLoader
class TRealSoundLoader
{
/// Member variables
protected:



/// Member functions
public:
	TRealSoundLoader();
	virtual ~TRealSoundLoader();

	bool LoadSamples( const char* szFileName);
};
