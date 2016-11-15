#pragma once

class TNPC
{
protected:
	int		m_NPCID;

protected:
	void	SetNpcEquipItem(TNPCInfo* pNPCInfo);
	void	SetNpcColor(TNPCInfo* pNPCInfo);
	void	SetSkin(unsigned int nColor);
	void	SetHair(unsigned int nColor);

	SH_ITEM_SLOT NPCPartsSlot2Slot(int nSlot);

public:
	TNPC(void);

	bool	SetNPC( int NPCID );	//내부 npc id를 사용하지 않는다.
	void COLTEdit();
	void ScriptEdit();

	void CreateColtFile(String^ filename);
	//bool IsEqualString( System::String^ NPCName, System::String^ FindName );
	//void FindNPCGroup(System::String^ NPCID);
	void SetNPCScale();

	bool IsValidNPCID(int NPCID)
	{
		if (m_NPCID == NPCID)
			return true;

		return false;
	}

	void SetNPCID(int val) { m_NPCID = val; }
	int GetNPCID() const { return m_NPCID; }
	TNPCInfo* GetNPCInfo()
	{
		TNPCInfo* pNPCInfo = TNPCInfoMgr::GetInstance().Get(m_NPCID);
		return pNPCInfo;
	}
	TNPCInfo* GetNPCInfo(int nNPCID)
	{
		return TNPCInfoMgr::GetInstance().Get(nNPCID);
	}
};
