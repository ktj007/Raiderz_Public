#ifndef	_M_AUTO_TEST_H_
#define	_M_AUTO_TEST_H_

#include "MMaietTestResult.h"
#include "MAutoTestInfo.h"

class MAutoTest
{
private:
	MAutoTestInfo m_AutoTestInfo;

protected:
	virtual void OnCreate() {} 
	virtual void OnDestroy() {} 
	virtual void OnUpdate(float fDelta) {}
	
	
public:
	MAutoTest();
	virtual ~MAutoTest();
	
	void Create();
	void Destroy();
	void Update(float fDelta);
	virtual void SaveTestResult(MMaietTestResult& outTestResult) {}

	MAutoTestInfo& GetInfo() { return m_AutoTestInfo; }
	virtual	const char* GetName() = 0;
};

#endif
