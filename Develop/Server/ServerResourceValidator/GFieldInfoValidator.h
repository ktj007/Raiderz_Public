#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"
#include "GFieldInfo.h"

class GVALIDATOR_API GFieldInfoValidator : public CSBaseValidator
{
private:
	void CheckLoadCollAndPath(GFieldInfo* pFieldInfo);	
	void CheckSpawn(GFieldInfo* pFieldInfo);	
	void CheckFieldSize( GFieldInfo* pFieldInfo );	
	void CheckSensor(GFieldInfo* pFieldInfo);
	void CheckSoulBinding(GFieldInfo* pFieldInfo);

public:
	virtual bool Check();
	
};

