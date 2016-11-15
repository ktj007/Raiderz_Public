#include "stdafx.h"
#include "GVectorMUID.h"
#include "GGlobal.h"

GVectorMUID::GVectorMUID()
{
	m_object = gmgr.pSharedMemMUIDPool->AcquireVector();
}

GVectorMUID::~GVectorMUID()
{
	gmgr.pSharedMemMUIDPool->ReleaseVector(m_object.first);
}

vector<MUID>& GVectorMUID::Vector()
{
	return *m_object.second;
}
