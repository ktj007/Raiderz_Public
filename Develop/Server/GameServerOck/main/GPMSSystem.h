#pragma once

#include "PMPMSConnector.h"

class GPMSObject;

class GPMSSystem : public PMPMSConnector, public MTestMemPool<GPMSSystem>
{
public:
	GPMSSystem(void);
	~GPMSSystem(void);

	// PMS 콜백 오브젝트 구하기
	virtual IPMSObject* GetPMSObject() override;
private:
	GPMSObject* pCallbackObj;
};
