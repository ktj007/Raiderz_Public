#pragma once

class ZFixedPartyInfoManager
{
public:
	typedef map<wstring, int>	fixedmember_map;
	typedef map<int, wstring>	fixedparty_map;

public:
	ZFixedPartyInfoManager();
	virtual ~ZFixedPartyInfoManager();

	bool Load(wstring strFileName);
	void ParseParty(MXmlElement* pElement, MXml* pXML);
	void ParseMember(MXmlElement* pElement, int nFixedPartyID, MXml* pXML);

	void AddPartyInfo(int nFixedPartyID, wstring strPartyName);
	void AddMemberInfo(int nFixedPartyID, wstring strMemberName);	
	virtual int FindFixedPartyID(wstring strName);
	virtual wstring FindFixedPartyName(int nFixedPartyID);

private:
	fixedmember_map	m_mapFixedMember;
	fixedparty_map m_mapFixedParty;
};
