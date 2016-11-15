#pragma once

#include <string>
#include <set>
using namespace std;

class CCListGen
{
protected:
	bool					CheckAndGen(char *name);

	void CheckFile( char * name );
	int						m_nRecursionLevel;

	bool					Check_CCCheckFunc(char* szLine, string& outCCName);

	set<string>				setNames;
public:
	CCListGen();
	~CCListGen();

	void					GenCCList(const char *szPath);
	void Save(const char* szTargetFileName);
};

