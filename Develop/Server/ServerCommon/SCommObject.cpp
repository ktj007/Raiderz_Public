#include "stdafx.h"
#include "SCommObject.h"

SCommObject::SCommObject(MUID& uid)
: m_UID(uid)
, m_bCreated(false)
{
	;
}

SCommObject::~SCommObject()
{
	;
}

void SCommObject::Create()
{
	m_bCreated = true;
}

void SCommObject::Destroy()
{
	if (!m_bCreated) return;

	m_bCreated = false;
}

const MUID SCommObject::GetUID(void) const
{
	return m_UID;
}
