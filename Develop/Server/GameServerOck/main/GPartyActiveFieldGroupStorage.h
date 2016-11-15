#pragma once

class GPartyActiveFieldGroupStorage : public MTestMemPool<GPartyActiveFieldGroupStorage>
{
public:	
	typedef map<int,MUID>		MAP_FIELDGROUP;		// <FieldGroupID, FieldGroupUID>

public:
	// Field
	void AddField(int nFieldGroupID, MUID uidFieldGroup);
	void RemoveField(int nFieldGroupID);
	bool HasField(int nFieldGroupID) const;
	MUID Find(int nFieldGroupID) const;

	// Policy
	void AddPolicy(MUID uidFieldGroup);
	void RemovePolicy(MUID uidFieldGroup);
	const set<MUID>& GetFieldPolicy(void) const;

	// All
	void ClearField(void);

private:
	MAP_FIELDGROUP				m_mapFieldGroups;
	set<MUID>					m_setFieldPolicy;
};
