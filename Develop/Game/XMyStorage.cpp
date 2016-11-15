#include "StdAfx.h"
#include "XMyStorage.h"

XMyStorage::XMyStorage(void)
: XItemContainer(MAX_STORAGE_SIZE)
, m_bExpended(false)
{
	
}

XMyStorage::~XMyStorage(void)
{
}
