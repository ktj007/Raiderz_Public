#pragma once

#include "RMeshUtil.h"

namespace rs3 {

const float ANIMATION_FRAME_PER_SECOND = 4800.f;

class RAnimationResource;

enum AnimationLoopType {
	RAniLoopType_Loop = 0,
	RAniLoopType_HoldLastFrame,
};

enum RANIMATIONTRANSITIONTYPE
{
	RAT_NONE = 0,				// 블렌딩 없음
	RAT_IMMEDIATE_BLEND,	// 현재 애니메이션은 정지, 즉시애니메이션이 시작되며 일정시간 블렌딩
	RAT_DELAYED_BLEND,		// 현재 애니메이션은 정지, 일정시간 블렌딩후 애니메이션이 시작된다
	RAT_CROSS_FADE,			// 현재 애니메이션이 플레이, 즉시애니메이션이 시작되며 일정시간 블렌딩

	RAT_COUNT
};

class RAnimationEvent
{
public:
	string			m_strEvent;		///< 이벤트 이름
	unsigned int	m_nFrame;		///< 이벤트가 발생하는 프레임
	string			m_strParam1;	///< 파라미터 1
	string			m_strParam2;	///< 파라미터 2
	string			m_strParam3;	///< 파라미터 3
	bool			m_bUnconditional; ///< 무조건 발생해야하는 이벤트
};

class RS_API RAnimation 
{
public:
	RAnimation(string& strName, RAnimationResource* pAnimation, int nID);
	~RAnimation();
	void				SetResource(RAnimationResource* pResource)	{	m_pResource = pResource;	}
	RAnimationResource*	GetResource();
	void				SetAliasName(const char* name);
	const string&		GetAliasName();

	int					GetID();
	void				SetSpeed(const float speed);
	float				GetSpeed();

	AnimationLoopType	GetAnimationLoopType();
	void				SetAnimationLoopType(AnimationLoopType type);

	void				LoadEvent(MXmlElement* pElement);
	void				SaveEvent(MXmlElement* pElement);

	// 모든 이벤트를 지운다
	void				ClearEvent();

	vector<RAnimationEvent*> m_animationEvents;

protected:
	string					m_strAliasName;
	RAnimationResource*		m_pResource;
	int						m_nID;
	float					m_fSpeed;
	AnimationLoopType		m_AniLoopType;
};

//////////////////////////////////////////////////////////////////////////
// inlines
inline RAnimationResource*	RAnimation::GetResource()				{ return m_pResource; }

inline void				RAnimation::SetAliasName(const char* name) { m_strAliasName = name; }
inline const string&	RAnimation::GetAliasName()				{ return m_strAliasName; }

inline int				RAnimation::GetID()						{ return m_nID; }
inline void				RAnimation::SetSpeed(const float speed)	{ m_fSpeed = speed;}
inline float			RAnimation::GetSpeed()					{ return m_fSpeed;}

inline AnimationLoopType RAnimation::GetAnimationLoopType()		{	return m_AniLoopType;	}
inline void				RAnimation::SetAnimationLoopType(AnimationLoopType type)	{	m_AniLoopType = type;	}


}