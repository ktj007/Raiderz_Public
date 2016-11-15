#ifndef _RSCREENEFFECTMANAGER_H
#define _RSCREENEFFECTMANAGER_H

#include "RListener.h"
//#include "RActor.h"
#include "REffectActor.h"
#include "RAnimation.h"

namespace rs3 {

// 스크린 이펙트 존재 속성
enum RSCREENEFFECT_EXIST_ATT
{
	EXIST_ONE,								// 오직 하나
	EXIST_MULTI								// 여러개
};

// 정렬 방법
enum RSCREENEFFECT_LOCATION_TYPE
{
	LOCATION_LEFT,							// 왼쪽 정렬
	LOCATION_RIGHT,							// 오른쪽 정렬
	LOCATION_CENTER							// 중앙 정렬
};

// 문자 속성
enum RSCREENEFFECT_CHARACTER_ATT
{
	CHARACTER_NORMAL,						// 문자 일반(이미지 하나)
	CHARACTER_WORD							// 문자 조합
};

// 애니메이션 종료 후 처리 속성
enum RSCREENEFFECT_ANIMATION_END_ATT
{
	ANIMATION_END_SHOW = 0,					// 애니메이션 종료후 보여주기
	ANIMATION_END_HIDE,						// 애니메이션 종료후 숨기기
	ANIMATION_END_DEL,						// 애니메이션 종료후 삭제
};

class RScreenEffect;
class RScreenEffectUnit;

typedef map<string, RScreenEffect*>			CompositeScreenEfeectList;
typedef CompositeScreenEfeectList::iterator	CompositeScreenEfeectList_Iter;
typedef vector<RScreenEffect*>				SingleScreenEfeectList;
typedef SingleScreenEfeectList::iterator	SingleScreenEfeectList_Iter;

typedef vector<RScreenEffectUnit*>			ScreenEfeectUnitList;
typedef ScreenEfeectUnitList::iterator		ScreenEfeectUnitList_Iter;

//////////////////////////////////////////////////////////////////////////
// EffectActor 와 같으나 자체적으로 렌더링 하여 렌더패스가 다르다
class RScreenEffectActor : public REffectActor
{
public:
	RScreenEffectActor();

	void Update()
	{
		RActor::UpdateAnimation();
	}
};

//////////////////////////////////////////////////////////////////////////
// 스크린 이펙트 유닛
class RScreenEffectUnit : public RListener
{
private:
	RMesh *							m_pMesh;
	RAnimation *					m_pAnimation;

	RScreenEffectActor				m_RActor;
	RSceneNodeUpdateVisitor			m_ActorUpdator;
	RVector							m_Offset;

	bool							m_bVisible;

public:
	RScreenEffectUnit();
	~RScreenEffectUnit();

	bool							Render();
	void							Update(DWORD dwElapsed);
	bool							IsAnimationEnd();
	void							Create(RMesh* pMesh, 
											RAnimation* pAnimation, 
											vec3 offset = vec3(0,0,0));
	void							Destroy();

	RMesh *							GetMesh() {return m_pMesh; }
	RActor *						GetActor() { return &m_RActor; }
	bool							GetbVisible() { return m_bVisible; }

	void							SetOffset(vec3& offset) { m_Offset = offset; }
	void							SetbVisible(bool bVisible);
	void							SetAnimation(char* pOrder);
	void							SetAnimation(RAnimation* pAnimation);
	void							SetAnimation(char * pOrder, AnimationLoopType nAniLoop);
	void							SetPosition(vec3 vPos);
	void							AnimationStop();
	void							SetVisibility(float f);
	void							SetLastFrame();
};

//////////////////////////////////////////////////////////////////////////
// 스크린 이펙트 베이스
class RScreenEffect
{
protected:
	RSCREENEFFECT_LOCATION_TYPE		m_ScreenEffectLocation;
	RSCREENEFFECT_EXIST_ATT			m_ScreenEffectExist;
	RSCREENEFFECT_CHARACTER_ATT		m_ScreenEffectChar;
	RSCREENEFFECT_ANIMATION_END_ATT	m_ScreenEffectAniEndAtt;
	AnimationLoopType				m_ScreenEffectLoop;

	string							m_strScreenEffectName;
	string							m_strBaseFileName;

	vec3							m_vPosEffect;

	bool							m_bAnimationEnd;
	RSceneManager *					m_pSceneManager;

public:
	RScreenEffect(RSceneManager* pSceneManager);
	virtual ~RScreenEffect();

	virtual void					Create(string strScreenEffectName, 
											string strBaseFileName, 
											RSCREENEFFECT_EXIST_ATT existAtt,
											RSCREENEFFECT_CHARACTER_ATT charAtt,
											AnimationLoopType aniLoop);
	virtual void					Destroy();
	
	virtual bool					Render( RCameraSceneNode* pCurrenCamera);	// dubble. 렌더링쪽 리팩토링때문
	virtual bool					Update(DWORD dwElapsed);

	// 문자열 입력
	virtual void					SetString(string strContents);
	// 위치 셋팅
	virtual void					SetPosition(vec3 vPosEffect, RSCREENEFFECT_LOCATION_TYPE scrBase);
	// 애니메이션 변경
	virtual void					ChangeAnimation(string strAnimation, RSCREENEFFECT_ANIMATION_END_ATT aniDel);
	// 그리기 셋팅
	virtual void					SetDraw(bool bVisible);

	virtual void					AnimationStop()	{}

	virtual bool					IsAnimationEnd() { return false; }

	// 스크린 이펙트 유닛 셋팅
	bool							SetScreenEffectUnit(RScreenEffectUnit* pUnit, string strFileName, vec3 vPosEffect);

	virtual void					SetVisibility(float f) {}
	virtual void					SetLastFrame() {}
};

//////////////////////////////////////////////////////////////////////////
// 싱글용 스크린 이펙트
class RSingleScreenEffect : public RScreenEffect
{
private:
	RScreenEffectUnit *				m_pUnit;

	void							SetScreenEffect();

public:
	RSingleScreenEffect(RSceneManager* pSceneManager);
	virtual ~RSingleScreenEffect();

	virtual void					Create(string strScreenEffectName, 
											string strBaseFileName, 
											RSCREENEFFECT_EXIST_ATT existAtt,
											RSCREENEFFECT_CHARACTER_ATT charAtt,
											AnimationLoopType aniLoop);
	virtual void					Destroy();

	virtual bool					Render( RCameraSceneNode* pCurrenCamera);	// dubble. 렌더링쪽 리팩토링때문
	virtual bool					Update(DWORD dwElapsedTime);
	
	// 위치 셋팅
	virtual void					SetPosition(vec3 vPosEffect, RSCREENEFFECT_LOCATION_TYPE scrBase);
	// 애니메이션 변경
	virtual void					ChangeAnimation(string strAnimation, RSCREENEFFECT_ANIMATION_END_ATT aniDel);
	// 그리기 셋팅
	virtual void					SetDraw(bool bVisible);
	virtual	void					AnimationStop();
	virtual bool					IsAnimationEnd();

	virtual void					SetVisibility(float f);
	virtual void					SetLastFrame();
};

//////////////////////////////////////////////////////////////////////////
// 단어 조합용 스크린 이펙트
class RCompositeScreenEffect : public RScreenEffect
{
private:
	string							m_strContents;

	ScreenEfeectUnitList			m_vecUnit;

	void							SetScreenEffect();
	void							ReSetScreenEffect();
	vec3							SetWordPosition(const vec3& vBasePosEffect, int nCount, int nTotalCount, RSCREENEFFECT_LOCATION_TYPE scrBase);

public:
	RCompositeScreenEffect(RSceneManager* pSceneManager);
	virtual ~RCompositeScreenEffect();

	virtual void					Create(string strScreenEffectName, 
											string strBaseFileName, 
											RSCREENEFFECT_EXIST_ATT existAtt,
											RSCREENEFFECT_CHARACTER_ATT charAtt,
											AnimationLoopType aniLoop);
	virtual void					Destroy();

	virtual bool					Render( RCameraSceneNode* pCurrentCamera);	// dubble. 렌더링쪽 리팩토링때문
	virtual bool					Update(DWORD dwElapsedTime);

	// 문자열 입력
	virtual void					SetString(string strContents);
	// 위치 셋팅
	virtual void					SetPosition(vec3 vPosEffect, RSCREENEFFECT_LOCATION_TYPE scrBase);
	// 애니메이션 변경
	virtual void					ChangeAnimation(string strAnimation, RSCREENEFFECT_ANIMATION_END_ATT aniDel);
	// 그리기 셋팅
	virtual void					SetDraw(bool bVisible);
	virtual void					AnimationStop();
};

//////////////////////////////////////////////////////////////////////////
// 스크린 이펙트 매니저
class RS_API RScreenEffectManager
{
private:
	RSceneManager *					m_pSceneManager;
	RScreenEffect*					m_pLastInputSingleScreenEffect;	
	CompositeScreenEfeectList		m_mapCompositeScreenEffect;
	SingleScreenEfeectList			m_vecSingleScreenEffect;

public:
	RScreenEffectManager();
	~RScreenEffectManager();

	void							AddScreenEffect(string strListFileName, 
													RSCREENEFFECT_CHARACTER_ATT charAtt = CHARACTER_NORMAL, 
													AnimationLoopType aniLoop = RAniLoopType_HoldLastFrame);
	void							RegisterScreenEffect(string strScreenEffectName, 
														string strListFileName,
														RSCREENEFFECT_CHARACTER_ATT charAtt = CHARACTER_NORMAL,
														AnimationLoopType aniLoop = RAniLoopType_HoldLastFrame);
	void							Destroy();

	// 문자열 입력
	void							SetString(string strScreenEffectName, string strContents);
	// 위치 셋팅
	void							SetPosition(string strScreenEffectName, vec3 vPosEffect, RSCREENEFFECT_LOCATION_TYPE scrBase);
	// SceneManager 인스턴스 셋팅
	void							SetSceneManager(RSceneManager * pSceneManager) { m_pSceneManager = pSceneManager; }
	// 그리기 시작
	void							SetDraw(string strScreenEffectName, bool bVisible);

	void							SetVisibility(string strScreenEffectName, float f);
	void							SetLastFrame(string strScreenEffectName);

	// 애니메이션 변경
	void							ChangeAnimation(string strScreenEffectName, string strAnimation, RSCREENEFFECT_ANIMATION_END_ATT aniDel);	

	void							Update();
	void							Render();

	// 스크린 이펙트 있는가?
	bool							IsScreenEffect();
	void							AnimationStop(string strScreenEffectName);
	bool							IsAnimationEnd(string strScreenEffectName);
};

}

#endif