#pragma once

class GCollisionTree;

class GCollisionTreeMgr : public map<int, GCollisionTree* >, public MTestMemPool<GCollisionTreeMgr>
{
public:
private:
public:
	GCollisionTreeMgr(void) {}
	virtual ~GCollisionTreeMgr(void);
	void Insert( int nZoneID, GCollisionTree* pCollisionTree );
	GCollisionTree* Find(int nZoneID);
	void Clear();
};
