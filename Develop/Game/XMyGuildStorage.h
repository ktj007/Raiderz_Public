#pragma once


class XMyGuildStorage : public XItemContainer
{
public:
	XMyGuildStorage(void);
	~XMyGuildStorage(void);

	virtual int		GetSlotSize()	{ return m_bExpended ? MAX_GUILDSTORAGE_SIZE : BASE_GUILDSTORAGE_SIZE;}

private:
	bool	m_bExpended;
};
