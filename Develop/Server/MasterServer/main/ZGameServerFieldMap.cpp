#include "stdafx.h"
#include "ZGameServerFieldMap.h"
#include "ZSharedField.h"
#include "ZDynamicFieldMaster.h"

void ZGameServerFieldMap::AddSharedField(ZSharedField* pSharedField)
{
	pSharedField->AddRef();

	ZBaseFieldManager::AddSharedField(pSharedField);
}

void ZGameServerFieldMap::DelSharedField(MUID uidSharedField)
{
	ZBaseFieldManager::DelSharedField(uidSharedField); ///< 이곳에서 Drop() 호출
}

void ZGameServerFieldMap::AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster)
{
	pDynamicFieldMaster->AddRef();

	ZBaseFieldManager::AddDynamicFieldMaster(pDynamicFieldMaster);
}

void ZGameServerFieldMap::DelDynamicFieldMaster(MUID uidDynamicFieldMaster)
{
	ZBaseFieldManager::DelDynamicFieldMaster(uidDynamicFieldMaster); ///< 이곳에서 Drop() 호출
}

ZGameServerFieldMap::~ZGameServerFieldMap()
{
	
}
