#pragma once

#include "RActor.h"

namespace rs3 {

class RSceneNodeController;

enum AnimationLoopType;

////////////////////////////////////////////////////////////////////////////////////////////////////
// 일반적인 패스로 랜더링 안하고 환경 이펙트등이 적용되지 않게 개별 렌더링하는 이펙트액터
//
RS_API extern const char *RSID_EFFECTACTOR;

class RS_API REffectActor : public RActor
{
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_EFFECTACTOR);

	enum EFFECTACTOR_TRANSFORM_TYPE
	{
		ETT_NONE,
		ETT_ALIGN,
		ETT_ALIGN_ZFIXED,
		ETT_ROTATE_X,
		ETT_ROTATE_Y,
		ETT_ROTATE_Z,
		ETT_NUM
	};

protected:
	virtual void	OnUpdate();

	//변수 추가시 REffectBasic::CopyInstance() 에도 추가 해주기.
	EFFECTACTOR_TRANSFORM_TYPE	m_TransformType;
	RSceneNodeController*	m_pTransformController;	// 트랜스폼 콘트롤러. Align, AlignZFixed, Rotate

	float			m_fElapsedAfterStart;
	float			m_fStartTime;
	AnimationLoopType	m_AniLoopType;
	RPLAYSTATE		m_ePlayAnimationState;

	bool			m_bVisibleSwitch;

	RSceneNode*		m_pParentEffectSceneNode;
	int				m_nRenderOrder;

	RAnimation*		m_pEffectAnimation;

public:
	REffectActor(RRENDERPASS eRenderPass = RRP_ACTOR);
	virtual ~REffectActor();

	virtual void	OnLoadingComplete(RResource* pResource);
	virtual int		CalcPriority();
	virtual bool	IsLoadingCompleted();

	bool			IsAlive();

	bool			IsLooping();
	void			SetLooping(bool bLoop);

	void			EnableTransformController();
	void			RemoveTransformController();
	void			SetAlignController();
	void			SetAlignZFixedController();
	void			SetRotateXController( float fRotateMin, float fRotateMax );
	void			SetRotateYController( float fRotateMin, float fRotateMax );
	void			SetRotateZController( float fRotateMin, float fRotateMax );
	bool			IsAlign() { return ( ETT_ALIGN == m_TransformType ); }
	bool			IsAlignZFixed() { return ( ETT_ALIGN_ZFIXED == m_TransformType ); }
	bool			IsRotateX() { return ( ETT_ROTATE_X == m_TransformType ); }
	bool			IsRotateY() { return ( ETT_ROTATE_Y == m_TransformType ); }
	bool			IsRotateZ() { return ( ETT_ROTATE_Z == m_TransformType ); }

	bool			GetRotateMinMax( float& fOutMin, float& fOutMax );

	bool			GetVisibleSwitch() { return m_bVisibleSwitch; }
	void			SetVisibleSwitch( bool bFlag ) { m_bVisibleSwitch = bFlag; }
	virtual bool	GetVisible() const override { return ( RActor::GetVisible() && m_bVisibleSwitch ); }

	virtual RSceneNode* Clone();

	void	SetStartTime(float fStartTime)	{ m_fStartTime = fStartTime; }
	float	GetStartTime()					{ return m_fStartTime; }
	void	ResetStartElapsedTime()			{ m_fElapsedAfterStart = 0.0f; }

	virtual bool	CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void	SaveToXML(MXmlElement &element);

	void			EffectStart();
	void			EffectStop();
	void			EffectPause();

private:
	void			SetLoopType(AnimationLoopType aniType);
};


class RS_API REffectActorMLeaf : public REffectActor, public MMemPool<REffectActorMLeaf>
{
	MDeclareRTTI;
public:
	REffectActorMLeaf(RRENDERPASS eRenderPass = RRP_ACTOR) : REffectActor(eRenderPass) {}
	virtual ~REffectActorMLeaf() {}
};

}