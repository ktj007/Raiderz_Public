#ifndef _AnimationBlendTable_h_
#define _AnimationBlendTable_h_

namespace rs3 {

enum RANIMATIONTRANSITIONTYPE;
class RMeshAnimationMgr;

struct RS_API BLENDDATA {
	RANIMATIONTRANSITIONTYPE	eType;
	unsigned int				nDuration;
	BLENDDATA(RANIMATIONTRANSITIONTYPE _eType,unsigned int _nDuration) : eType(_eType), nDuration(_nDuration) {}
	BLENDDATA() {}

	static const RANIMATIONTRANSITIONTYPE	DEFAULT_BLEND_TYPE;
	static const unsigned int				DEFAULT_BLEND_DURATION;
};

/**
	@class RAnimationBlendTable
	@brief 애니메이션 전환 블렌딩 옵션 관리 클래스
*/
class RS_API RAnimationBlendTable
{
public:
	RAnimationBlendTable();
	~RAnimationBlendTable();

	void Clear();

	/**
	@param animationMgr 초기화 된 애니메이션 매니저 클래스, NULL값을 넣으면 안된다.
	@note 이미 셋팅된 RAnimationMgr 클래스 포인터와 같은 파라메터 값이 넘어오면 기존 데이터를 삭제 하지 않는다.
	      RAnimationMgr클래스에서 애니메이션 파일을 추가 하는 경우가 있기 때문이다.
	*/
	void Reset( RMeshAnimationMgr* const animationMgr );

	/**
	@brief 애니메이션 블렌딩 타입을 추가하거나 변경한다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@param aniTransType 변경 할 애니메이션 블렌딩 타입
	@param nDuration 애니메이션 블렌딩 시간
	@return 타입을 추가하거나 변경에 성공하면 true를 반환한다.
	*/
	bool SetAnimationTransData( const char* source, const char* target, RANIMATIONTRANSITIONTYPE aniTransType, unsigned int nDuration );

	/**
	@brief 애니메이션 블렌딩 타입을 추가하거나 변경한다.
	@param source 원본 애니메이션 id 값
	@param target 대상 애니메이션 id 값
	@param aniTransType 변경 할 애니메이션 블렌딩 타입
	@param nDuration 애니메이션 블렌딩 시간
	@return 타입을 추가하거나 변경에 성공하면 true를 반환한다.
	*/
	bool SetAnimationTransData( int source, int target, RANIMATIONTRANSITIONTYPE aniTransType, unsigned int nDuration );

	/**
	@brief 애니메이션 블렌딩 타입을 얻어온다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@return 타입을 반환한다. 해당 케이스에 대한 데이터가 없으면 기본 타입을 반환한다.
	*/
	BLENDDATA GetAnimationTransData( const char* source, const char* target );

	/**
	@brief 애니메이션 블렌딩 타입을 얻어온다.
	@param source 원본 애니메이션 id 값
	@param target 대상 애니메이션 id 값
	@return 타입을 반환한다. 해당 케이스에 대한 데이터가 없으면 기본 타입을 반환한다.
	*/
	BLENDDATA GetAnimationTransData( int source, int target );

	/**
	@brief 애니메이션 블렌딩 타입을 삭제한다.
	@param source 원본 애니메이션 이름
	@param target 대상 애니메이션 이름
	@return 삭제에 성공하면 true를 반환한다.
	*/
	bool DeleteAnimationTransType( const char* source, const char* target );

	/**
	@brief 애니메이션 블렌딩 타입을 삭제한다.
	@param source 원본 애니메이션 id 값
	@param target 대상 애니메이션 id 값
	@return 삭제에 성공하면 true를 반환한다.
	*/
	bool DeleteAnimationTransType( int source, int target );

	/**
	@brief 변환 테이블을 XML데이터로 저장한다.
	@param parentXmlElement XML 상위 노드 값.
	*/
	void SaveXml( MXmlElement* parentXmlElement );

	/**
	@brief 변환 테이블을 XML데이터로 불러온다.
	@param animationMgr 초기화 된 애니메이션 매니저 클래스.
	@param parentXmlElement XML 상위 노드 값.
	@param szFilter 필터 스트링
	*/
	void LoadXml( RMeshAnimationMgr* const animationMgr, MXmlElement* parentXmlElement, const char* szFilter ="");

	/// 기본 변환 타입 상수
	static const BLENDDATA					DEFAULT_ANIMATION_TRANSITION_DATA;

public:
	typedef std::pair<int, int> AnimationBlendPair;
	typedef map<AnimationBlendPair, BLENDDATA> AnimationBlendData;
//	typedef std::pair<AnimationBlendPair, RANIMATIONTRANSITIONTYPE> AnimationBlendDataPair;
//	typedef AnimationBlendData::const_iterator AnimationBlendDataConstIter;
//	typedef AnimationBlendData::iterator AnimationBlendDataIter;

private:
	bool IsAnimationNameMatch( const char* source, const char* target );
	int GetTransTypeId( const char* typestr );

public:
	bool IsValid();
	bool IsNotDefaultValue( BLENDDATA blendData );
	static const char* GetTransTypeString( int type );
	AnimationBlendData* GetAnimationBlendData() { return &m_AnimationBlendData; }

private:
	RMeshAnimationMgr* m_AnimationMgr;
	AnimationBlendData m_AnimationBlendData;
};

};

#endif // _AnimationBlendTable_h_
