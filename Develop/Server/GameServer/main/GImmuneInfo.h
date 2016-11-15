#pragma once

namespace immune
{
	enum CUSTOM_TYPE
	{
		IMMUNE_DRAIN = 0,
		IMMUNE_DISPEL,
		IMMUNE_MAX,
	};

	static const wchar_t* CUSTOM_TYPE_STR[] = 
	{
		L"DRAIN",
		L"DISPEL",
	};

	struct INFO
	{
		// 버프에 대한 면역 비트셋
		uint32			nImmuneBuff;		
		// 데미지타입에 대한 면역 비트셋
		uint32			nImmuneDamageType;	
		// 스킬타입에 대한 면역 비트셋
		uint32			nImmuneSkillType;	
		// 커스덤타입에 대한 면역 비트셋
		int32			nImmuneCustomType;	

		INFO()
		{
			nImmuneBuff = 0L;
			nImmuneDamageType = 0L;
			nImmuneSkillType = 0L;
			nImmuneCustomType = 0L;
		}
	};
} // namespace immune

