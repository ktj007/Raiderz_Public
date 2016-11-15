#pragma  once

#include "RCullingMgr.h"

namespace rs3
{
	class RViewFrustumCullingMgr : public RCullingMgr
	{
	public:
		RViewFrustumCullingMgr(void);
		virtual ~RViewFrustumCullingMgr(void);
	};
}