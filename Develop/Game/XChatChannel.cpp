#include "stdafx.h"
#include "XChatChannel.h"

XChatChannel::XChatChannel(MUID uid, wstring strName, bool isOwner)
: m_uid(uid)
, m_strName(strName)
, m_isOwner(isOwner)
{
	// do nothing
}

XChatChannel::~XChatChannel()
{
	// do nothing
}

MUID XChatChannel::GetUID(void) const
{
	return m_uid;
}

wstring XChatChannel::GetName(void) const
{
	return m_strName;
}

bool XChatChannel::IsOwner(void) const
{
	return m_isOwner;
}

void XChatChannel::SetOwner(bool isOwner)
{
	m_isOwner = isOwner;
}
