#ifndef _MOCK_PARTY_H_
#define _MOCK_PARTY_H_

#include "SUnitTestUtil.h"
#include "GParty.h"

class MockParty : public GParty
{
public:
	MockParty(MUID uidParty): GParty(uidParty)	{}
};

#endif//_MOCK_PARTY_H_