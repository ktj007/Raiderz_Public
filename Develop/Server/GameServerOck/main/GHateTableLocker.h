#pragma once

// 타켓을 일정시간동안 고정하는 역할
class GHateTableLocker
{
public:
	GHateTableLocker();

	// 사용가능한지 여부 반환
	bool IsEnable();
	// 사용가능한지 여부
	void Enable(bool bEnable);
	// 대상 액터를 상대로 고정함
	void Lock(MUID uidTarget, float fDurationTime);
	// 고정한 상태를 취소함
	void Unlock();
	// 고정한 액터가 있는지 여부
	bool IsLock() const;
	// 고정된 상대 반환
	MUID GetTarget() const;
	// 틱 처리
	void Update(float fDelta);
private:
	// 사용가능한지 여부
	bool m_bEnable;
	// 고정된 상대 UID
	MUID m_uidTarget;
	// 시간 해제
	MRegulator m_rgrReleaser;
};
