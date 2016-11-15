#pragma once

class PChatChannel;

class PChatChannelFactory
{
public:
	PChatChannel* NewChatChannel_Private(int nOwnerCID, const std::wstring& strChannelName);
	PChatChannel* NewChatChannel_Party(const MUID& uidParty);	
	PChatChannel* NewChatChannel_Auction(void);
	PChatChannel* NewChatChannel_Field(int nFieldID);
};
