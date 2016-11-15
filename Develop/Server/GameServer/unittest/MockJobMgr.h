#ifndef _MOCK_JOB_MGR_H_
#define _MOCK_JOB_MGR_H_

#include "GJobMgr.h"

class MockJobMgr : public GJobMgr
{
public:
	bool PopJob()			{ return GJobMgr::PopJob(); }
};

#endif //_MOCK_JOB_MGR_H_