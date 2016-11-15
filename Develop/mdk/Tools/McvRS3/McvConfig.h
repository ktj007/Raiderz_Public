#pragma once

#include <string>

using namespace rs3;

class McvConfig
{
public:
	McvConfig(void);
	~McvConfig(void);

	bool LoadConfig(const string& strFileName);

	static McvConfig& GetInstance();

	float	m_fFarZ;
	string	m_strRegistryRoot;
	string	m_strLightAni;		// 라이트 심은 에니메이션 이름지정하여 해당 에니메이션 강제 수행 하게 한다.
};
