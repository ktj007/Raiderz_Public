#ifndef _RAnimationNode_h
#define _RAnimationNode_h

#include "RAnimationDef.h"
#include "RMeshUtil.h"
#include "RAnimationKey.h"

namespace rs3 {

class RMesh;

// key controller 형식으로 바꿔주기..여러가지 보간법 적용가능하도록..
class RS_API RAnimationNode : public RBaseObject
{
public:
	RAnimationNode();
	~RAnimationNode();


	//////////////////////////////////////////////////////////////////////////
	// Data Getter
	//	keyIterator 는 현재 프레임의 키가 다음프레임에도 쓰일 확률이 높기 때문에, 
	//	현재 키인덱스를 저장해놓은 값입니다.
	//	쓰지않으시려면 변수에 0을 할당하시고 변수를 파라미터로 사용하세요. - dubble
	//
	bool			GetRotValue(RQuaternion* pOut, int frame,unsigned int &rotKeyIterator);
	bool			GetPosValue(RVector* pOut, int frame,unsigned int &posKeyIterator);
	int				GetVecValue(int frame,RVector* pVecTable);
	bool			GetVisValue(float* pOut, int frame, unsigned int &visKeyIterator);
	bool			GetScaleValue(RVector* pOut, int frame,unsigned int &scaleKeyIterator);

	void			GetSamplingData( int nStartFrame, int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt, const RMatrix* pBaseCoordIfUse );

public:

	string			m_parentName;

	RMatrix			m_matLocal;				///< 초기 프레임의 (Local) tm

	// 사용하지 않는 base 변수 주석처리합니다. - birdkr
	RVector			m_baseScale;			///< 초기 프레임의 tm 중 스케일값
	RQuaternion		m_baseRotation;			///< 초기 프레임의 tm 중 애니메이션의 회전값
	RVector			m_baseTranslation;		///< 초기 프레임의 tm 중 애니메이션의 이동값

	int				m_nNodeID;

	/// 모든 프레임에 대한 바운드 박스
	RBoundingBox	m_bboxVertexAni;

	RAnimationTrackTest<RPosKey>	m_positionKeyTrack;
	RAnimationTrackTest<RPos2Key>	m_position2KeyTrack;
	RAnimationTrackTest<RQuatKey>	m_rotationKeyTrack;
	RAnimationTrackTest<RScaleKey>	m_scaleKeyTrack;
	RAnimationTrackTest<RVisKey>	m_visKeyTrack;

	int				m_nVertexCnt;	// 버텍스 애니메이션 프레임 수
	int				m_nVertexVCnt;	// 버텍스 애니메이션할 메쉬의 버텍스 숫자
	DWORD*			m_pVertexFrame;	// 버텍스 애니메이션의 키프레임 배열
	RVector**		m_vertex;		// 실제 버텍스 애니메이션 데이터
};


}

#endif//_RAnimationNode_h
