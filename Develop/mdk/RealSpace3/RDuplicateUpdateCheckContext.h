#pragma once

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// Duplicate Update Check Context
//
//////////////////////////////////////////////////////////////////////////
class RDuplicateUpdateCheckContext
{
public:
	RDuplicateUpdateCheckContext();
	~RDuplicateUpdateCheckContext();

	void	NeedUpdate() { m_bNeedUpdate = true; }
	int		GetUpdatedID() { return m_nUpdatedID; }

	// 업데이트가 필요한지 체크하고 중복 업데이트 되지 않도록 파라메터 갱신
	bool	IsNeedUpdate( bool bNeedUpdateHierarchy, int nParentUpdatedID );

	// 계층 업데이트 로직과 상관없이 강제 업데이트 한 후 더이상 Update가 필요하지 않을 때 불러줌
	void	SetForceUpdated( bool bNeedUpdateHierarchy, int nParentUpdatedID );	

protected:
	bool	m_bNeedUpdate;		// 업데이트 되어야 하는지
	int		m_nUpdatedID;		// 자신의 업데이트 ID, 업데이트 할때마다 증가
	int		m_nParentUpdatedID;	// 기억하고 있는 부모 업데이트 ID, 이게 바뀌면 다시 계산한다
};

}