#pragma once

#include "RCullingMgr.h"

namespace rs3
{
	class ROcclusionCullingMgr	: public RCullingMgr
	{
	public:
		ROcclusionCullingMgr(void);
		virtual ~ROcclusionCullingMgr(void);
	};

	class ROcclusionCullingMgrSet
	{
	public:
		ROcclusionCullingMgrSet(void);
		virtual ~ROcclusionCullingMgrSet(void);

	public:
		void			OcclusionCull(RWorldSceneNode* pWorldScene, const RVector3& ViewPosition ,ROccluderList& OccluderList);
		void			SetOccludedSceneNodes( void );
		void			ResetOccludedSceneNodes(void);

	private:
		unsigned int	GetTheNumberOfOcclusionMgrs(void)	{	return NUM_OCCLUSION_CULLING_MGR;	}
		bool			IsEceptionalPass(int pass);
	private:
		static const int	NUM_OCCLUSION_CULLING_MGR;
		RCullingMgr*		m_pOcclusionCullingMgrs;

		int		m_UsedOcclusionCullingMgrIndex;
		int		m_TheNumberOfOccludedSceneNodes;

		enum	CALL_ORDER
		{
			OCMS_OCCLUSIONCULL = 0,
			OCMS_SETOCCLUDEDSCENENODES,
			OCMS_RESETOCCLUDEDSCENENODES
		}m_CallOrderValidator;
	};
}