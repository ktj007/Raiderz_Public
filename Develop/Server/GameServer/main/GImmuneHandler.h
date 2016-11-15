#pragma once

#include "GImmuneInfo.h"

enum DAMAGE_ATTRIB;
enum TALENT_SKILL_TYPE;
enum BUFF_PASSIVE_EXTRA_ATTRIB_;

class GImmuneHandler
{
public:
	GImmuneHandler(void);
	~GImmuneHandler(void);

	// 초기화
	void Init(immune::INFO info);

	// 데미지타입에 대한 면역여부
	bool IsDamageTypeImmune(DAMAGE_ATTRIB nDamageType) const;
	// 스킬타입에 대한 면역여부
	bool IsSkillTypeImmune( TALENT_SKILL_TYPE nSkillType ) const;
	// 버프타입에 대한 면역여부
	bool IsBuffImmune(BUFF_PASSIVE_EXTRA_ATTRIB_ nBuffExtraType) const;
	// 커스덤타입에 대한 면역여부
	bool IsImmune(immune::CUSTOM_TYPE nCustomType) const;
private:
	bool			m_bInit;
	immune::INFO	m_Info;
};
