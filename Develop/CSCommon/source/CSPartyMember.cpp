#include "stdafx.h"
#include "CSPartyMember.h"
#include "CTransData.h"

CSPartyMember::CSPartyMember()
: m_nStatusFlag(0)
, m_nHP(0)
, m_nEN(0)
, m_nSTA(0)
, m_nLevel(0)
, m_isSetted(false)
{
	// do nothing
}

CSPartyMember::CSPartyMember(MUID uidMember, tstring strMemberName)
: m_UID(uidMember)
, m_nStatusFlag(0)
, m_nHP(0)
, m_nEN(0)
, m_nSTA(0)
, m_strName(strMemberName)
, m_nLevel(0)
, m_isSetted(false)
{
	// do noting
}

CSPartyMember::~CSPartyMember()
{
	// do nothing
}

void CSPartyMember::Assign(const TD_PARTY_MEMBER& tdPartyMember, const vector<int>& vecBuff)
{
	m_UID = tdPartyMember.m_uidPlayer;	
	m_nStatusFlag = tdPartyMember.nStatusFlag;
	m_nHP = tdPartyMember.nHP;
	m_nEN = tdPartyMember.nEN;
	m_nSTA = tdPartyMember.nSTA;
	m_strName = tdPartyMember.szName;
	m_nLevel = tdPartyMember.nLevel;

	m_vecBuff = vecBuff;

	m_isSetted = true;
}

void CSPartyMember::Export(TD_PARTY_MEMBER* pPartyMember, vector<int>* pVecBuff) const
{
	pPartyMember->m_uidPlayer = m_UID;
	pPartyMember->nStatusFlag = m_nStatusFlag;
	pPartyMember->nHP = m_nHP;
	pPartyMember->nEN = m_nEN;
	pPartyMember->nSTA = m_nSTA;
	pPartyMember->nLevel = m_nLevel;
	_tcsncpy_s(pPartyMember->szName, m_strName.c_str(), _TRUNCATE);
	

	*pVecBuff = m_vecBuff;
}

void CSPartyMember::Update(const CSPartyMember* pPartyMember)
{	
	m_nStatusFlag = pPartyMember->m_nStatusFlag;
	m_nHP = pPartyMember->m_nHP;
	m_nEN = pPartyMember->m_nEN;
	m_nSTA = pPartyMember->m_nSTA;
	m_strName = pPartyMember->m_strName;
	m_nLevel = pPartyMember->m_nLevel;

	m_vecBuff = pPartyMember->m_vecBuff;

	m_isSetted = true;
}

void CSPartyMember::DoOffline(void)
{
	m_nHP = 0;
	m_nEN = 0;
	m_nSTA = 0;
	m_nStatusFlag = 0;

	SetBitSet(m_nStatusFlag, UPS_OFFLINE);
}

void CSPartyMember::SetUID(MUID uidMember)
{
	m_UID = uidMember;
}

MUID CSPartyMember::GetUID(void) const
{
	return m_UID;
}

tstring CSPartyMember::GetName(void) const
{
	return m_strName;
}

bool CSPartyMember::IsSetted(void) const
{
	return m_isSetted;
}
