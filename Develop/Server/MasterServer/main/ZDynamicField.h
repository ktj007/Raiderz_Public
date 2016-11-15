#pragma once

#include "ZField.h"

class ZDynamicField : public ZField
{
private:
	MUID m_uidOwnerFieldGroup;

public:
	ZDynamicField(MUID uid, ZFieldInfo* pFieldInfo, int m_nGameServerID, MUID uidOwnerFieldGroup);
	~ZDynamicField();

	MUID	GetFieldGroupUID()			{ return m_uidOwnerFieldGroup; }
};
