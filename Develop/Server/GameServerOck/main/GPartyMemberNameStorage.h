#pragma once

class GPartyMemberNameStorage
{
public:
	typedef map<MUID, string>	NAME_MAP;

public:
	void AddName(MUID uidMember, string strMemberName);
	void DeleteName(MUID uidMember);
	string FindName(MUID uidMember) const;

private:
	NAME_MAP					m_mapName;
};
