#pragma once

#include "GUTHelper_Buff.h"
#include "GUTHelper_Talent.h"
#include "GUTHelper_Item.h"

class GEntityPlayer;
class GEntityActor;

class GUTHelper_Focus
{
private:
	GUTHelper_Buff		m_HelperBuff;
	GUTHelper_Talent	m_HelperTalent;
	GUTHelper_Item		m_HelperItem;
public:
	// 포커스를 초기화함
	void ClearFocus(GEntityPlayer* pUser);
	// 광포 포커스를 얻음, 히트 카운트가 -1일 경우에는 포커스 걸릴만큼 때려줌
	void GainBerserk(GEntityPlayer* pUser, bool bLearnLicense=true, int nHitCounter=-1);
	// 반격 포커스를 얻음
	void GainCounter(GEntityPlayer* pUser, bool bLearnLicense=true);
	// 반격용 라이센스 배움
	void LearnLicense_Counter( GEntityPlayer* pUser );
	//광폭용 라이센스 배움
	void LearnLicense_Berserk( GEntityPlayer* pUser );
	// 강신 포커스를 토글함
	void ToggleAdvent(GEntityActor* pUser);
	// 은신 포커스를 토글함
	void ToggleSneak(GEntityActor* pUser);
	// 저격 포커스를 토글함
	void TogglePrecision(GEntityActor* pUser);
	// 각성 포커스를 얻음, 히트 카운트가 -1일 경우에는 포커스 걸릴만큼 때려줌
	void GainEnlighten(GEntityPlayer* pUser, int nHitCounter=-1);
	// 각성 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForEnlighten(GEntityPlayer* pUser);
	// 강신 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForAdvent( GEntityPlayer* pUser );
	// 반격 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForCounter( GEntityPlayer* pUser );
	// 광포 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForBerserk( GEntityPlayer* pUser );
	// 저격 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForPrecision( GEntityPlayer* pUser );
	// 은신 포커스를 얻기 위해 필요한 아이템을 얻기
	void GainItemForSneak( GEntityPlayer* pUser );
};
