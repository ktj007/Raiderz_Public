#pragma once

class PLogicChecker;

class PCheckRunner
{
public:
	PCheckRunner();
	virtual ~PCheckRunner();
	
	void AddChecker(PLogicChecker* pChecker);
	bool Run(void);
	void Clear(void);

private:
	vector<PLogicChecker*>	m_vecChecker;
};
