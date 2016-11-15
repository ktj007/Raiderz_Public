#pragma once

class XChatChannel
{
public:
	XChatChannel(MUID uid, wstring strName, bool isOwner);
	virtual ~XChatChannel();

	MUID GetUID(void) const;
	wstring GetName(void) const;
	bool IsOwner(void) const;

	void SetOwner(bool isOwner);

private:
	MUID m_uid;
	wstring m_strName;
	bool m_isOwner;
};
