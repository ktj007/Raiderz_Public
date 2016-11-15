#include "stdafx.h"
#include "SDsnFactory.h"
#include "SBaseDsnFactory.h"


SDsnFactory::~SDsnFactory()
{
	Release();
}

void SDsnFactory::Set( SBaseDsnFactory* pFactory )
{
	if (NULL != m_pFactory)
		SAFE_DELETE(m_pFactory);

	m_pFactory = pFactory;
}

void SDsnFactory::Release()
{
	if (NULL != m_pFactory)
		SAFE_DELETE(m_pFactory);
}