#pragma once

#include "ZFieldManager.h"

/// 거임 서버 오브젝트가 가지고 있는 필드 인스턴스
/// - ZField 인스턴스는 ZFieldManager가 가지고 있다.
class ZGameServerFieldMap : public ZBaseFieldManager
{
public:
	ZGameServerFieldMap() : ZBaseFieldManager() {}
	virtual ~ZGameServerFieldMap();

	void	AddSharedField(ZSharedField* pSharedField) override;
	void	DelSharedField(MUID uidSharedField) override;

	void	AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster) override;
	void	DelDynamicFieldMaster(MUID uidDynamicFieldMaster) override;
};
