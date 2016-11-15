#include "stdafx.h"
#include "XChatChannelManager.h"
#include "XChatChannel.h"

XChatChannelManager::XChatChannelManager()
{
	// do nothing
}

XChatChannelManager::~XChatChannelManager()
{
	for(map_channel::iterator it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
	{
		delete it->second;
	}
	m_mapChannel.clear();
}

bool XChatChannelManager::Join(MUID uidChannel, wstring strChannelName, bool isOwner)
{
	map_channel::iterator it = m_mapChannel.find(uidChannel);
	if (it != m_mapChannel.end())	return false;

	XChatChannel* pChannel = new XChatChannel(uidChannel, strChannelName, isOwner);
	m_mapChannel.insert(map_channel::value_type(uidChannel, pChannel));

	return true;
}

void XChatChannelManager::Leave(MUID uidChannel)
{
	map_channel::iterator it = m_mapChannel.find(uidChannel);
	if (it == m_mapChannel.end())	return;

	delete it->second;
	m_mapChannel.erase(it);
}

XChatChannel* XChatChannelManager::Find(MUID uidChannel)
{
	map_channel::iterator it = m_mapChannel.find(uidChannel);
	if (it == m_mapChannel.end())	return NULL;

	return it->second;
}

XChatChannel* XChatChannelManager::Find(wstring strChannelName)
{
	for(map_channel::iterator it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
	{
		XChatChannel* pChannel = it->second;
		if (strChannelName == pChannel->GetName())
		{
			return pChannel;
		}
	}

	return NULL;
}

bool XChatChannelManager::IsEmpty(void) const
{
	return m_mapChannel.empty();
}
