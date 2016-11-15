#pragma once

#include "RealSound.h"


class XSoundAdapter : public SoundAdapter
{
	int m_nNum;

public:
	XSoundAdapter( int num) : m_nNum( num)	{}

	virtual bool getPosition( FMOD_VECTOR* pPos) override;
};




class XRealSound : public RealSound
{
};