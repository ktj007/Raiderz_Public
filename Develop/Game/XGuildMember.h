#ifndef _X_GUILD_MEMBER_H_
#define _X_GUILD_MEMBER_H_

class XGuildMember
{
public:
	wstring				m_strName;
	int					m_nLevel;
	GUILD_MEMBER_GRADE	m_nGrade;

	bool			m_bOnline;
	int				m_nFieldID;
	int				m_nChannelID;

public:

	XGuildMember(const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade, bool bOnline, int nFieldID, int nChannelID);
	~XGuildMember();
};

#endif
