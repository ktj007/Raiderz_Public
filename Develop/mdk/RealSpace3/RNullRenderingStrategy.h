#pragma once

#include "RRenderingStrategy.h"
#include "RVType.h"
#include "RShaderConstant.h"


namespace rs3 {

	class RS_API RNullRenderingStrategy : public RRenderingStrategy
	{
	public:
		RNullRenderingStrategy();
		virtual ~RNullRenderingStrategy();
		virtual void Init( int nW, int nH);
	};
}