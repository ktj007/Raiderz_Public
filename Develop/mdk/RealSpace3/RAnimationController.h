#pragma once

#include <map>

#include "RActorController.h"
#include "RAnimationControllerNode.h"

namespace rs3 {

class RAnimation;
class RActor;
class RActorNode;

enum RPLAYSTATE {
	RPS_STOP = 0,
	RPS_PLAY,
	RPS_PAUSE
};

class RAnimationEvent;

struct ANIMATION_EVENT
{
	RAnimationEvent*	pAnimationEvent;
	bool				bEventDone;			///< 이벤트가 실행되었는지 여부

	ANIMATION_EVENT()
	{
		pAnimationEvent = NULL;
		bEventDone		= false;
	}
};

enum RBLENDMODE
{
	RBLENDMODE_OVERWRITE,
	RBLENDMODE_BLEND,
//	RBLENDMODE_ADDITIVE,		///< 해당 애니메이션을 추가 적용 ( additive animation ), 추후 구현
};

class RS_API RActorNodeSelection
{
protected:
	typedef set<string>	RACTORNODESET;

	RACTORNODESET		m_nodes;

public:
	RActorNodeSelection( );

	bool IsExistNode(const char* szNodeName);
	bool SelectNode(RActor* pActor, const char* szNodeName);
	bool SelectNodeAndChildren(RActor* pActor, const char* szNodeName, bool bExceptSelectNode = false);

	void clear(){ m_nodes.clear(); }
};

struct RPLAYINFO
{
	RBLENDMODE				blendMode;	///< 블렌딩 모드, 덮어쓰던가, 기존것을 블렌딩한다
	float					fWeight;	///< 블렌딩에 사용할 가중치 0은 영향없음 1이면 전체
	float					fPlaySpeed;	///< 재생속도 기본값 : 1.0
	float					fBlendIn;	///< 시작될때의 블렌딩 시간
	float					fBlendOut;	///< 끝날때의 블렌딩 시간
	RActorNodeSelection*	pNodeSet;	///< 선택된 노드에만 영향을 주고 싶을때, NULL을 넣으면 전체

	RPLAYINFO()
		: blendMode( RBLENDMODE_OVERWRITE )
		, fWeight(1.0f)
		, fPlaySpeed(1.0f)
		, fBlendIn(0.f)
		, fBlendOut(0.f)
		, pNodeSet(NULL)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
// RAnimationFrameController
// 애니메이션의 프레임 진행과 애니메이션 event 처리를 담당하는 콘트롤러
class RS_API RAnimationFrameController : public RActorController
{
public:

	RAnimationFrameController(RSNCTRL_TYPE eType = RSNCTRL_UPDATE);

	virtual void	Update(RCameraSceneNode* pCamera) override;
	void			SetAnimation( RAnimation *pAnimation );
	void			Reset();

	bool			IsPlayDone() const;
	bool			IsOncePlayDone() const;
	DWORD			GetMaxFrame() const;
	float			GetSpeed() const;
	bool			GetLoop() const;
	RPLAYSTATE		GetState() const;

	DWORD			GetFrame() const;
	void			SetFrame( DWORD nFrame );
	void			SetSpeed( float fSpeed );
	void			SetLoop( bool bLoop );
	void			SetOncePlayDone( bool bOncePlayDone );
	void			SetForceAnimationControlPlayDoneForTest( bool bPlayDone );

	void			Play();
	void			Stop();
	void			Pause();

	vector<ANIMATION_EVENT>& GetAnimationEvent() { return m_AniamtionEvents; }
	void			SetAnimationEvent(vector<ANIMATION_EVENT>& aniamtionEvents);
	void			ResetEvent();
	void			CloseEvent();
	bool			GetEventActive() const;
	void			SetEventActive(bool b);

	void			GetNextFrameAndState(DWORD dwElapsedTime, DWORD& _nOutNextFrame, bool& _bPlayDone) const;

	void			CheckAnimationEventSend();

protected:
	void			UpdateFrame(DWORD dwElapsedTime);

	RPLAYSTATE		m_nState;			///< 플레이 상태
	DWORD			m_nFrame;			///< 현재 애니메이션 프레임
	DWORD			m_nMaxFrame;		///< 현재 애니메이션 최대 프레임
	bool			m_bLoop;			///< loop 형식인지 여부
	bool			m_bPlayDone;		// Loop 일때 이 값이 true 가 될수 없다.
	bool			m_bOncePlayDone;	// Loop 일때도 플레이가 한번 되면 true 가 될수 있다.
	float			m_fSpeed;			///< 재생속도
	bool			m_bEventActive;		///< 이벤트 활성화 여부

	vector<ANIMATION_EVENT> m_AniamtionEvents;
};

//////////////////////////////////////////////////////////////////////////
inline DWORD RAnimationFrameController::GetFrame() const { return m_nFrame; }
inline void RAnimationFrameController::SetFrame(DWORD nFrame) { m_nFrame = min(nFrame,m_nMaxFrame); }
inline float RAnimationFrameController::GetSpeed() const	{ return m_fSpeed; }
inline void RAnimationFrameController::SetSpeed( float fSpeed ) { m_fSpeed = fSpeed; }
inline bool RAnimationFrameController::IsPlayDone()	const	{	return m_bPlayDone;	}
inline bool	RAnimationFrameController::IsOncePlayDone()	const {	return m_bOncePlayDone;	}
inline DWORD RAnimationFrameController::GetMaxFrame() const	{ return m_nMaxFrame; }
inline bool RAnimationFrameController::GetLoop() const		{	return m_bLoop;	}
inline void	RAnimationFrameController::SetLoop( bool bLoop ) { m_bLoop = bLoop; }
inline RPLAYSTATE RAnimationFrameController::GetState() const{ return m_nState;	}
inline void	RAnimationFrameController::SetOncePlayDone( bool bOncePlayDone ) { m_bOncePlayDone = bOncePlayDone; }
inline bool	RAnimationFrameController::GetEventActive() const	{ return m_bEventActive; }
inline void RAnimationFrameController::SetEventActive(bool b)	{ m_bEventActive = b; }


/// AnimationController/ overlay animation controller / blend animation controller 를 대채할 새 콘트롤러
class RS_API RAnimationController : public RActorController
{
	friend RActor;

public:

	RAnimationController(RSNCTRL_TYPE eType = RSNCTRL_POSTANIMATION, RSNCTRL_TYPE eFrameControllerType = RSNCTRL_UPDATE);
	virtual ~RAnimationController(void);

	void			Destroy();

	void			Reset();
	RAnimation*		GetAnimation();

	bool			SetAnimation( const char* szAnimation, RPLAYINFO* pPlayInfo = NULL);

	virtual void	Update(RCameraSceneNode* pCamera) override;

	RBLENDMODE		GetBlendMode() const;
	void			SetBlendMode(RBLENDMODE blendMode);
	void			SetWeight( float fWeight );
	float			GetWeight() const;

	void			SetFrame(DWORD nFrame);
	DWORD			GetFrame() const;
	DWORD			GetMaxFrame() const;
	void			SetSpeed(float fSpeed);
	float			GetSpeed() const;

	bool			GetLoop() const;

	RPLAYSTATE		GetState() const;

	bool			IsPlayDone();
	bool			IsOncePlayDone();

	RAnimationControllerNode* GetNodeController(const string& strName);
	RAnimationControllerNode* GetNodeController(int i);
	size_t			GetNodeCount();			///< 해당 actor 의 전체 노드 수
	size_t			GetActiveNodeCount();	///< 현재 사용하는 노드 수

	void			Play();
	void			Stop();
	void			Pause();

	vector<ANIMATION_EVENT>& GetAnimationEvent() { return m_frameController.GetAnimationEvent(); }
	void			ResetEvent();
	void			CloseEvent();
	bool			GetEventActive() const;
	void			SetEventActive(bool b);

	void			GetNextFrameAndState(DWORD dwElapsedTime, DWORD& _nOutNextFrame, bool& _bPlayDone);

	RAnimationFrameController& GetFrameController() { return m_frameController; }

protected:
	void			ConnectAnimation(RAnimation *pAnimation, RActorNodeSelection* pSelection );
	void			Disconnect();

	void			SetActorUseVertexAnimation( RAnimation* pAnimation );

	bool			SetAnimation( RAnimation* pAnimation, RPLAYINFO* pPlayInfo );

	/// 노드를 활성화 한다 셀렉션을 따로 주지 않으면 모든 노드를 활성화한다
	void			ActivateNodes( RActorNodeSelection* pSelection = NULL);

	void			UpdateAnimation();

	virtual void	OnAdd() override;
	virtual void	OnRemove() override;
	virtual void	ReNew() override;

	vector<RAnimationControllerNode>	m_nodeControllers;		///< 모든 노드를 담고있다
	vector<RAnimationControllerNode*>	m_activeControllers;	///< 활성화된 노드셋

	//// playinfo 를 가지고 있을까.,, by pok. playinfo를 가지게 했다.
	//RBLENDMODE				m_blendMode;		///< 블렌딩 모드
	//float						m_fWeight;			///< 목표 섞이는 농도 (0=영향을 전혀 안미침,1=완전히 덮어쓰게됨)
	//float						m_fBlendIn;			///< 시작될때의 블렌딩 시간
	//float						m_fBlendOut;		///< 끝날때의 블렌딩 시간
	//RActorNodeSelection*		m_pNodeSelection;	///< 현재 선택된 노드
	RActorNodeSelection			m_nodeSelection;
	RPLAYINFO					m_playInfo;

	RAnimation					*m_pAnimation;

	RAnimationFrameController	m_frameController;	///< 애니메이션 프레임진행과 이벤트 처리
};

//////////////////////////////////////////////////////////////////////////
// inlines

inline size_t RAnimationController::GetNodeCount() { return m_nodeControllers.size(); }
inline size_t RAnimationController::GetActiveNodeCount() { return m_activeControllers.size(); }
inline RAnimation* RAnimationController::GetAnimation() { return m_pAnimation; }
inline void RAnimationController::SetWeight( float fWeight ) { m_playInfo.fWeight = fWeight; }
inline void	RAnimationController::SetBlendMode(RBLENDMODE blendMode) { m_playInfo.blendMode = blendMode; }

//////////////////////////////////////////////////////////////////////////
// 헉헉.. GetFrameController 하나만 빼주고 다 없앨까...
inline DWORD RAnimationController::GetFrame() const		{	return m_frameController.GetFrame();	}
inline bool RAnimationController::IsPlayDone()			{	return m_frameController.IsPlayDone();	}
inline bool	RAnimationController::IsOncePlayDone()		{	return m_frameController.IsOncePlayDone();	}
inline DWORD RAnimationController::GetMaxFrame() const	{	return m_frameController.GetMaxFrame(); }
inline float RAnimationController::GetSpeed() const		{	return m_frameController.GetSpeed(); }
inline float RAnimationController::GetWeight() const	{	return m_playInfo.fWeight; }
inline RBLENDMODE RAnimationController::GetBlendMode() const		{	return m_playInfo.blendMode; }
inline bool RAnimationController::GetLoop() const		{	return m_frameController.GetLoop();	}
inline RPLAYSTATE RAnimationController::GetState() const{	return m_frameController.GetState();	}
inline void	RAnimationController::ResetEvent()			{ m_frameController.ResetEvent(); }
inline void	RAnimationController::CloseEvent()			{ m_frameController.CloseEvent(); }
inline void	RAnimationController::SetFrame(DWORD nFrame) { m_frameController.SetFrame(nFrame); }
inline void	RAnimationController::SetSpeed(float fSpeed) { m_frameController.SetSpeed(fSpeed); }
	inline void	RAnimationController::Play()				{ m_frameController.Play(); }
inline void	RAnimationController::Stop()				{ m_frameController.Stop(); }
inline void	RAnimationController::Pause()				{ m_frameController.Pause(); }
inline void	RAnimationController::GetNextFrameAndState(DWORD dwElapsedTime, DWORD& _nOutNextFrame, bool& _bPlayDone) { m_frameController.GetNextFrameAndState(dwElapsedTime, _nOutNextFrame, _bPlayDone); }
inline bool	RAnimationController::GetEventActive() const	{ return m_frameController.GetEventActive(); }
inline void RAnimationController::SetEventActive(bool b)	{ m_frameController.SetEventActive(b); }

}