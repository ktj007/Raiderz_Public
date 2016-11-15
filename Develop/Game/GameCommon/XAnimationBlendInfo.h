#pragma once

#include <tchar.h>
#include "RMeshAnimationMgr.h"
#include "MTstring.h"

struct BLEND_DATA 
{
	float				fDurationTime;
	BLEND_DATA(float _fTime) : fDurationTime(_fTime) {}
	BLEND_DATA() { fDurationTime = 0.0f; }

	static const float				DEFAULT_BLEND_DURATION;
};

class XAnimationBlendInfo
{
public:
	XAnimationBlendInfo();
	virtual ~XAnimationBlendInfo();

	void Clear();

	/**
	@brief 애니메이션 블렌딩 타입을 추가하거나 변경한다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@param aniTransType 변경 할 애니메이션 블렌딩 타입
	@param nDuration 애니메이션 블렌딩 시간
	@return 타입을 추가하거나 변경에 성공하면 true를 반환한다.
	*/
	bool SetAnimationTransData( std::tstring& source, std::tstring& target, float fDurationTime );

	/**
	@brief 애니메이션 블렌딩 타입을 얻어온다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@return 타입을 반환한다. 해당 케이스에 대한 데이터가 없으면 기본 타입을 반환한다.
	*/
	BLEND_DATA GetAnimationTransData( std::tstring& source, std::tstring& target, RMeshAnimationMgr* AnimationMgr );

	/**
	@brief 애니메이션 블렌딩 타입을 삭제한다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@return 삭제에 성공하면 true를 반환한다.
	*/
	bool DeleteAnimationTransType( std::tstring& source, std::tstring& target );

	bool CheckExistAnimation(RMeshAnimationMgr* AnimationMgr, std::tstring& source, std::tstring& target);

	/// 기본 변환 타입 상수
	static const BLEND_DATA					DEFAULT_ANIMATION_TRANSITION_DATA;

public:
	typedef std::pair<std::tstring, std::tstring> AnimationBlendPair;
	typedef map<AnimationBlendPair, BLEND_DATA> AnimationBlendData;

private:
	bool IsAnimationNameMatch( const TCHAR* source, const TCHAR* target );

public:
	bool IsNotDefaultValue( BLEND_DATA blendData );
	AnimationBlendData* GetAnimationBlendData() { return &m_AnimationBlendData; }

protected:
	AnimationBlendData m_AnimationBlendData;
};
