#ifndef _RSPTManager_H_
#define _RSPTManager_H_

#include "RCullingMgr.h"

namespace rs3
{
	class RSphereTreeNode;

	class RS_API RSPTManager
	{
	public:
		RSPTManager(void);
		virtual ~RSPTManager(void){};

	public://유틸리티 함수들
		virtual RSphereTreeNode*	AddSphereToSPT(const RVector& pos, float fRadius, void* pUserData)	= 0;
		virtual void				FrustumTest(const RViewFrustum& frustum)							= 0;
		virtual void				PlanesTest(const RPlane *pPlanes, int planesNum)					= 0;
		virtual	void				Update(void)														= 0;
		virtual	void				RemoveSphereFromSPT(RSphereTreeNode* pNode)							= 0;


	public://컬링 정보
		void				SetCullingInfo(RCullingMgr *pCullingMgr,RRENDERPASS begin,RRENDERPASS end);
		void				ClearCullingInfo(void);

		RCullingMgr*		GetCullingMgr(void)			{	return m_pCullingMgr;	}
		RRENDERPASS			GetCullingBeginPass(void)	{	return m_BeginPass;		}
		RRENDERPASS			GetCullingEndPass(void)		{	return m_EndPass;		}

	private:
		RCullingMgr*				m_pCullingMgr;
		RRENDERPASS					m_BeginPass;	//컬링될 씬노드 시작패스
		RRENDERPASS					m_EndPass;		//컬링될 씬노드 마지막패스
	};

}

#endif