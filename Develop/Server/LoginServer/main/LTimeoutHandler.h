#pragma once

class LTimeoutHandler
{
public:
	virtual void OnTimeout(MUID uidPlayer) = 0;
};
