#pragma once

/// 내 보관함
class XMyStorage : public XItemContainer
{
public:
	XMyStorage(void);
	virtual ~XMyStorage(void);

	virtual int		GetSlotSize()	{ return m_bExpended ? MAX_STORAGE_SIZE : BASE_STORAGE_SIZE;}

private:
	bool	m_bExpended;
};
