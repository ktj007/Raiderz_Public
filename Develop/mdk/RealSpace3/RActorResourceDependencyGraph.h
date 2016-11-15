#pragma once

#include <map>
#include "RTypes.h"

namespace rs3
{
	class RActorResources
	{
	public:
		RMesh*								m_pMesh;
		std::map<std::string,RTexture*>		m_TexturesMap;

	};
}