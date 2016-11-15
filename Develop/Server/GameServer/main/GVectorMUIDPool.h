#pragma once

#include <queue>

typedef	size_t							VectorID;
typedef pair<VectorID, vector<MUID>*>	VectorMUIDObject;

class GVectorMUIDPool : public MTestMemPool<GVectorMUIDPool>
{
public:
	GVectorMUIDPool(size_t poolSize, size_t objectVectorSize);
	virtual ~GVectorMUIDPool();

	VectorMUIDObject	AcquireVector(void);
	void				ReleaseVector(VectorID vectorID);

private:	
	vector<vector<MUID>*>	m_vecObject;
	queue<VectorID>			m_queueReleaseObjectID;

	size_t					m_objectVectorSize;
};
