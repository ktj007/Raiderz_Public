#pragma once

#include "XCharacter.h"

class XBPartCharacter : public XCharacter
{
protected:
	MDeclareRTTI;

	virtual void CreateCharacterParts();

public:
	XBPartCharacter();
};
