#pragma once

#include "RSceneNode.h"

namespace rs3 
{
	RS_API extern const char *RSID_OCCLUDER;

	class RS_API ROccluderSceneNode : public RSceneNode
	{
	public:
		ROccluderSceneNode(RRENDERPASS eRenderPass = RRP_OCCLUDER);
		virtual ~ROccluderSceneNode(void);

	public:
		MDeclareRTTI;
		DECLARE_NODE_ID_STRING(RSID_OCCLUDER);

		virtual bool			CreateFromXML(MXmlElement &element, bool bBackgroundCreation) override;
		virtual void			SaveToXML(MXmlElement &element) override;

		virtual RSceneNode*		Clone()	override;
		virtual void			OnUpdateTransform() override;
		virtual void			OnUpdateAABB() override;

	public:
		ROccluderPointsSet*			GetWorldTransformedOccluderPointsSet(void);
		void						AddOccluderPointsToOccluderPointsSet(const ROccluderPoints& OccluderPoints);
		ROccluderPointsSet*			GetOCcluderPointsSet(void)	{ return m_pOccluderPointsSet;	}
		void					SetLocalScale(RVector3 LocalScale)	{	m_LocalNonUniformScale = LocalScale;	NeedUpdateTransform(); NeedUpdateBoundingVolume();	}
		const	RVector3&		GetLocalScale(void)	{	return m_LocalNonUniformScale;	}
		
	private:
		bool						HasOccluderPointsSet(void)	{ return NULL==m_pOccluderPointsSet ? false : true ;	}

		void						DestroyOccluder(void);
		void						UpdateWorldTransformedOccluderPointsSet(const RMatrixA16 &WorldMatrix);
		bool						HasWorldTransformedOccluderPointsSet(void)	{	return NULL == m_pWorldTransformedOccluderPointsSet ? false : true; }

	private:
		ROccluderPointsSet*			m_pOccluderPointsSet;
		ROccluderPointsSet*			m_pWorldTransformedOccluderPointsSet;

		RVector3		m_LocalNonUniformScale;
	};
}