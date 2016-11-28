#pragma once

class GKillRewardOwner
{
public:
	struct PARTYUID_CID
	{
		PARTYUID_CID(CID nCID, MUID nPartyUID)
		: m_nCID(nCID), m_nPartyUID(nPartyUID) {}
		CID		m_nCID;
		MUID	m_nPartyUID;
	};

	typedef std::tr1::unordered_map<CID, int> MAP_CANDIDATE_CID;
	typedef map<int, PARTYUID_CID> MAP_CANDIDATE_RANK;

public:
	GKillRewardOwner();
	~GKillRewardOwner(void);

	void GetBeneficiaryAndParty(vector<CID>& outvecBeneficiaryCID, MUID& outnPartyUID);
	void InsertBeneficiaryAndParty(const vector<CID>& vecBeneficiaryCID, const MUID& nPartyUID);
	bool IsEmpty();	

protected:
	void Insert(CID nCID, MUID nPartyUID);
	void Delete(CID nCID);
	void Delete(const vector<CID>& vecCID);

	virtual void Clear();

	CID GetRewardOwnerCID();
	bool IsCandidate(CID nCID);	

private:
	int NewCandidateRank();	
	MUID GetRewardOwnerPastPartyUID();
	

private:
	MAP_CANDIDATE_RANK	m_mapCandidateRank;	///< 보상 소유자 후보 key : Rank, value : CID
	MAP_CANDIDATE_CID	m_mapCandidateCID;	///< 보상 소유자 후보 key : CID, value : Rank	

	int	m_nCanditateRank;
};
