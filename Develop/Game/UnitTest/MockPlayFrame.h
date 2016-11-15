#pragma once

#include "XPlayFrame.h"

class MockPlayFrame : public XPlayFrame
{
public:
	MockPlayFrame() : XPlayFrame() {}
	virtual ~MockPlayFrame() {}

	virtual bool Open(const wchar_t* args);

	virtual void Close();

};