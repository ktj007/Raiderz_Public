#pragma once

#include "SNetClient.h"

class GNetClient : public SNetClient
{
private:
protected:
public:
	GNetClient(const MNetClientDesc& desc);
	virtual ~GNetClient();
};