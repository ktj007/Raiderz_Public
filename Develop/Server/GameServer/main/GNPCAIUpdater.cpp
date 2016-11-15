#include "stdafx.h"
#include "GNPCAIUpdater.h"

GNPCAIUpdater::GNPCAIUpdater(GNPCAI* pSubject)
: m_pSubject(pSubject)
{
	_ASSERT(m_pSubject);
}

GNPCAIUpdater::~GNPCAIUpdater()
{
	// do nothing
}
