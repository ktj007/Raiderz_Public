#pragma once

class XChatChannel;

class XChatChannelManager
{
public:
	typedef map<MUID, XChatChannel*>	map_channel;

public:
	XChatChannelManager();
	virtual ~XChatChannelManager();

	bool Join(MUID uidChannel, wstring strChannelName, bool isOwner);
	void Leave(MUID uidChannel);
	XChatChannel* Find(MUID uidChannel);
	XChatChannel* Find(wstring strChannelName);

	bool IsEmpty(void) const;

private:
	map_channel	m_mapChannel;
};
