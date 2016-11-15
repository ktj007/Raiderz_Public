#include "StdAfx.h"
#include "ZDynamicField.h"

ZDynamicField::ZDynamicField(MUID uid, ZFieldInfo* pFieldInfo, int m_nGameServerID, MUID uidOwnerFieldGroup)
: ZField(uid, pFieldInfo, m_nGameServerID)
, m_uidOwnerFieldGroup(uidOwnerFieldGroup)
{
}

ZDynamicField::~ZDynamicField()
{
}
