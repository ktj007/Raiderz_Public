#include "stdafx.h"
#include "MPath.h"
#include "MPathFinder.h"
#include "MPathCollision.h"

MPath::MPath()
: m_pPath(NULL)
{
}

MPath::~MPath()
{
}

void MPath::Release()
{
	if ( m_pPath )
	{
		delete m_pPath;
		m_pPath = NULL;
	}
}

unsigned long MPath::GetLength()
{
	if ( m_pPath )
		return m_pPath->getLength();
	return 0;
}

unsigned long MPath::GetSize()
{
	if ( m_pPath )
		return m_pPath->size();

	return 0;
}
