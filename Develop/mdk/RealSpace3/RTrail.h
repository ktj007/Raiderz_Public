#pragma once

#include "RVType.h"
#include "RPType.h"
#include "RTimer.h"

#include "REffectBase.h"
#include "RPCollection.h"

#include "RTechnique.h"

namespace rs3 {

	class RParticleShader;

	class RS_API RTrail : public REffectBase
	{
	private:
		MDeclareRTTI;
		DECLARE_NODE_ID_STRING("RTRAIL");

		class RTrailCreater
		{
		public:
			virtual int AddVertex( PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
				int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition ) = 0;
		};

		static class RTrailCreaterUVLeft : public RTrailCreater
		{
		public:
			virtual int AddVertex( PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
				int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition );
		} m_sTrailCreaterUVLeft;

		static class RTrailCreaterUVBottom : public RTrailCreater
		{
		public:
			virtual int AddVertex( PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
				int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition );
		} m_sTrailCreaterUVBottom;

		static class RTrailCreaterUVRight : public RTrailCreater
		{
		public:
			virtual int AddVertex( PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
				int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition );
		} m_sTrailCreaterUVRight;

		static class RTrailCreaterUVTop : public RTrailCreater
		{
		public:
			virtual int AddVertex( PVERTEXFORMAT *pVertex, const RVector &p1, const RVector &p2,
				int nSegment, const DWORD color, const RVector4 *pTexUV, float uvPosition );
		} m_sTrailCreaterUVTop;

	public:
		RTrail();
		virtual ~RTrail();

		virtual void Start() override;
		virtual void Stop() override;
		virtual void Pause() override { }
		virtual bool IsAlive() override;
		virtual int	GetCurrentCount()
		{
			return m_queueTrails.count();
		}

		// RTrail은 Resource를 사용하지 않으므로, NULL 을 리턴한다.
		RResource *GetResource();

		void SetUVGenLeft();
		void SetUVGenTop();
		void SetUVGenRight();
		void SetUVGenBottom();
		bool IsUVGenLeft() const;
		bool IsUVGenTop() const;
		bool IsUVGenRight() const;
		bool IsUVGenBottom() const;

		void SetLooping(bool loop)	{ m_bLoop = loop; }
		bool IsLooping()			{ return m_bLoop; }

		void SetAlign(bool b)	{	m_bAlign = b; }
		bool GetAlign()			{	return m_bAlign; }

		void SetLife(float f);
		float GetLife()			{	return m_fTrailLife; }

		void SetWidth(float f)	{	m_fTrailWidth = f; }
		float GetWidth()		{	return m_fTrailWidth; }

		void SetSegment(int n);
		int	GetSegment()		{	return m_nSegment; }

		int GetCount()			{	return m_queueTrails.size(); }

		const RVector4* GetTexUV()					{ return &m_vTexUV;		}
		void SetTexUV(const RVector4 &v )			{ m_vTexUV = v;			}

		void SetFrontColor(const RVector4 &color)	{ m_vFrontColor = color; }
		void SetBackColor(const RVector4 &color)	{ m_vBackColor = color; }
		const RVector4& GetFrontColor()				{ return m_vFrontColor; }
		const RVector4& GetBackColor()				{ return m_vBackColor; }

		virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
		virtual void OnUpdate();
		virtual void OnUpdateAABB() override;

		virtual bool				CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
		virtual void				SaveToXML(MXmlElement &element);

		virtual RSceneNode*			Clone();

		virtual bool				IsLoadingCompleted();

	private:
		bool CreateQueue();

		int		RenderTrail( RCameraSceneNode* pCurrentCamera);
		int		RenderAlignTrail( RCameraSceneNode* pCurrentCamera);
		void	RenderPrimitive( RCameraSceneNode* pCurrentCamera);

		bool	UpdateAABB();

		RTimer m_kTimer;

		// debug 용으로 안정화되면 삭제하자.
		int m_nVertexSize;
		int m_nIndexSize;

		float m_fTrailLife;
		float m_fTrailWidth;
		int m_nSegment;

		RVector4 m_vFrontColor;
		RVector4 m_vBackColor;
		RVector4 m_vTexUV;

		bool m_bAlign;
		bool m_bCreateState;
		bool m_bLoop;

		float m_fSumElpasedTIme;

		enum RTRAILUV_TYPE
		{
			RTRAILUV_LEFT = 0,
			RTRAILUV_BOTTOM,
			RTRAILUV_RIGHT,
			RTRAILUV_TOP
		};
		RTRAILUV_TYPE	m_UVType;
		RTrailCreater*	m_pTrailCreater;

		struct RMarks
		{
			RVector vPosition;
			RVector vNormal;
			float fLife;
		};
		CParticleQueue<RMarks> m_queueTrails;
	};

	inline void RTrail::SetLife(float f)
	{
		m_fTrailLife = f;
		CreateQueue();
	}
	
	inline void RTrail::SetSegment(int n)
	{
		_ASSERT(n>=1);

		m_nSegment = n;
		CreateQueue();
	}
}
