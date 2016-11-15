#pragma once

class GEntityNPC;

class GNPCFirstStrike : public MTestMemPool<GNPCFirstStrike>
{
public:
	GNPCFirstStrike(GEntityNPC* pOwner);
	~GNPCFirstStrike(void);

	// 업데이트 처리
	void Update(float fDelta);

	// 선제 공격 가능한지 여부
	bool IsEnable() const;

	// 선제 공격 가능여부 설정
	void SetEnableSpawnDelay(bool var);

private:
	GEntityNPC* m_pOwner;
	// 스폰후 가능해지는 시간
	MRegulator m_rgrFirstEnable;
	// 스폰후 선제공격 기능을 사용할지 여부
	bool m_bEnableSpawnDelay;
	// 스폰후 선제공격 가능한지 여부
	bool m_bSpawnDelay;
};
