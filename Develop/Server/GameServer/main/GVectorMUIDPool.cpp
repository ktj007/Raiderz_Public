#include "stdafx.h"
#include "GVectorMUIDPool.h"

GVectorMUIDPool::GVectorMUIDPool(size_t poolSize, size_t objectVectorSize)
: m_objectVectorSize(objectVectorSize)
{
	for(size_t i = 0; i < poolSize; i++)
	{
		vector<MUID>* pObject = new vector<MUID>;
		pObject->reserve(m_objectVectorSize);

		m_vecObject.push_back(pObject);
		m_queueReleaseObjectID.push(i);
	}
}

GVectorMUIDPool::~GVectorMUIDPool()
{	
	for each(vector<MUID>* pObject in m_vecObject)
	{
		delete pObject;
	}
}

VectorMUIDObject GVectorMUIDPool::AcquireVector(void)
{
	if (m_queueReleaseObjectID.empty())
	{
		mlog3("Not enough VectorMUIDPool. RequireSize(%d)\n", m_vecObject.size() + 1);
		
		vector<MUID>* pObject = new vector<MUID>;
		pObject->reserve(m_objectVectorSize);
		
		m_vecObject.push_back(pObject);
		m_queueReleaseObjectID.push(m_vecObject.size() - 1);
	}

	VectorID id = m_queueReleaseObjectID.front();
	m_queueReleaseObjectID.pop();

	return VectorMUIDObject(id, m_vecObject[id]);
}

void GVectorMUIDPool::ReleaseVector(VectorID vectorID)
{
	VALID(vectorID < m_vecObject.size());

	static size_t nRuntimeLargestVectorSize = m_objectVectorSize;
	if (nRuntimeLargestVectorSize < m_vecObject[vectorID]->size())
	{
		nRuntimeLargestVectorSize = m_vecObject[vectorID]->size();
		mlog3("Not enough VectorMUID vector size. RequireSize(%d)\n", nRuntimeLargestVectorSize);
	}

	m_queueReleaseObjectID.push(vectorID);
	m_vecObject[vectorID]->clear();
}
