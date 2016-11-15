#pragma once

#include "GVectorMUIDPool.h"

class GVectorMUID
{
public:
	GVectorMUID();
	virtual ~GVectorMUID();

	vector<MUID>& Vector(void);

private:	// Noncopyble idiom
	GVectorMUID (const GVectorMUID &);
	GVectorMUID & operator = (const GVectorMUID &);

private:
	VectorMUIDObject	m_object;
};
