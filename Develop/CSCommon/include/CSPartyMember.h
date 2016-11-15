#pragma once

#include "CSCommonLib.h"

struct TD_PARTY_MEMBER;

class CSCOMMON_API CSPartyMember
{
public:
	CSPartyMember();
	CSPartyMember(MUID uidMember, tstring strMemberName);
	virtual ~CSPartyMember();

	void Assign(const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBUff);
	void Export(TD_PARTY_MEMBER* tdPartyMember, vector<int>* vecBuff) const;
	void Update(const CSPartyMember* pPartyMember);
	virtual void DoOffline(void);

	void SetUID(MUID uidMember);
	MUID GetUID(void) const;
	tstring GetName(void) const;
	bool IsSetted(void) const;

private:
	MUID	m_UID;	
	uint32	m_nStatusFlag;
	int		m_nHP;
	int		m_nEN;
	int		m_nSTA;
	tstring	m_strName;
	int		m_nLevel;

	vector<int> m_vecBuff;
	
	bool	m_isSetted;
};
