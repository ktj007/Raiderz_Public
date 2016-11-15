#pragma once

class LogListBox
{
public:
	LogListBox( CListBox* pListBox );
	virtual ~LogListBox();

	void			PutLog( const char* szLog, ... );
	void			Dump();

private:
	CListBox*		m_pLogListBox;

};
