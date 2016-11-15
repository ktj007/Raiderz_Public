#ifndef	_M_AUTO_TEST_RUNNER_H_
#define	_M_AUTO_TEST_RUNNER_H_

#include <fstream>
#include "MMaietTestResult.h"

using namespace std;
//using std::ofstream;

class MAutoTest;

class MAutoTestRunner
{
private:
	MMaietTestResultList	m_TestResulList;
	int m_nRepeatCount;
	bool m_bExit;
	bool m_bFailure;

private:
	virtual MAutoTest* NewTest()=0;	
	const char* GetOutputFileName() { return "AutoTestResult.xml"; }
	void SaveTestResult();	
	void Report();
	void DecisionSuccessOrFailure(MMaietTestResult& refTestResult);

protected:
	MAutoTest* m_pTest;

protected:		
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual bool OnUpdate(float fDelta) { return true;	}

public:
	MAutoTestRunner();
	virtual ~MAutoTestRunner();

	bool Create();
	void Destroy();
	bool Update(float fDelta);	

	bool IsExit()		{ return m_bExit; }	
	bool IsFailure()	{ return m_bFailure; }
};

#endif
