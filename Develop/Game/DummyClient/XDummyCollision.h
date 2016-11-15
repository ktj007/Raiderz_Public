#pragma once

#include <map>
using namespace std;

#include "XWorld.h"

class XDummyCollision
{
protected:
	XCollisionTree*		m_pCollisionTree;
public:
	XDummyCollision();
	~XDummyCollision();
	void Load(const char* szFileName);

	bool Pick(RCollisionPickInfo& pickInfo);
};

class XDummyCollisionMgr : map<int, XDummyCollision*>
{
public:
	XDummyCollisionMgr()
	{

	}
	virtual ~XDummyCollisionMgr();
	static XDummyCollisionMgr* GetInstance();

	XDummyCollision* Get(int nFieldID);
	void Load(int nFieldID, const char* szCollisionFileName);
};