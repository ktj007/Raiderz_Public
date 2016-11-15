#include "StdAfx.h"
#include "RAnimationNode.h"

namespace rs3 {

// TODO: vertex animation 도 key iterator 처리

RAnimationNode::RAnimationNode()
{
	m_nNodeID = -1;

	m_nVertexCnt = 0;
	m_nVertexVCnt = 0;

	m_vertex = NULL;
	m_pVertexFrame = NULL;

	m_matLocal.MakeIdentity();
	//m_matLocal.Decompose( m_vLocalScale, m_vLocalTran, m_qLocalRot);	// 쿼터니언과 이동 값 

	m_bboxVertexAni.Initialize();
}

RAnimationNode::~RAnimationNode() 
{
	if(m_nVertexCnt) {
		for(int i=0;i<m_nVertexCnt;i++){
			SAFE_DELETE_ARRAY(m_vertex[i]);
		}
		SAFE_DELETE_ARRAY(m_vertex);
	}

	SAFE_DELETE_ARRAY(m_pVertexFrame);
}

bool RAnimationNode::GetVisValue(float* pOut, int frame, unsigned int &visKeyIterator)
{
	if(m_visKeyTrack.empty()) return false;

	RVisKey key;
	m_visKeyTrack.GetInterpolatedKey(&key,frame,visKeyIterator);
	*pOut = key.v;
	return true;
}

bool RAnimationNode::GetRotValue(RQuaternion* pOut, int frame,unsigned int &rotKeyIterator)
{
	if(m_rotationKeyTrack.empty()) return false;

	RQuatKey key;
	m_rotationKeyTrack.GetInterpolatedKey(&key,frame,rotKeyIterator);
	*pOut = key;
	return true;
}

bool RAnimationNode::GetPosValue(RVector* pOut, int frame,unsigned int &posKeyIterator)
{
	if(m_positionKeyTrack.empty()) return false;

	RPosKey key;
	m_positionKeyTrack.GetInterpolatedKey(&key,frame,posKeyIterator);
	*pOut = key;
	return true;
}

bool RAnimationNode::GetScaleValue(RVector* pOut, int frame,unsigned int &scaleKeyIterator)
{
	if( m_scaleKeyTrack.empty()) return false;

	RScaleKey key;
	m_scaleKeyTrack.GetInterpolatedKey(&key,frame,scaleKeyIterator);
	*pOut = key;
	return true;
}

int	RAnimationNode::GetVecValue(int frame,RVector* pVecTable)
{
	DWORD dwFrame = frame;

	int j;
	for (j=0;j<m_nVertexCnt;j++) {
		if ( m_pVertexFrame[j] >= dwFrame) 
			break;
	}

	if( j>= m_nVertexCnt) {

		int vcnt = m_nVertexVCnt;

		RVector* v1 = m_vertex[vcnt-1];
		memcpy(pVecTable,v1,sizeof(RVector)*vcnt);

		return vcnt;
	}

	if(j)	j--;

	int   s = m_pVertexFrame[j+1] - m_pVertexFrame[j];

	float d = 1;

	if (s != 0)	d = (frame - m_pVertexFrame[j] )/(float)s;

	RVector* v1 = m_vertex[j];
	RVector* v2 = m_vertex[j+1];

	RVector v;

	int vcnt = m_nVertexVCnt;

	for(int k=0;k<vcnt;k++) {
		pVecTable[k] = v2[k].GetInterpolated(v1[k],d);
	}

	return vcnt;
}

void RAnimationNode::GetSamplingData( int nStartFrame, int nEndFrame, int nSampleInterval, std::vector< RMatrix >& _rOUt, const RMatrix* pBaseCoordIfUse )
{
	_ASSERT( nStartFrame <= nEndFrame );
	RQuaternion quot;
	RVector pos, scale;
	unsigned int nIterQuot(0), nIterPos(0), nIterScale(0);
	RMatrix mat(RMatrix::IDENTITY);

#define _PUT_MAT( frame ) {\
	bool bRot = GetRotValue(&quot, frame, nIterQuot); \
	bool bPos = GetPosValue(&pos, frame, nIterPos); \
	bool bScale = GetScaleValue(&scale, frame, nIterScale); \
	if (bRot) quot.SetMatrixRotation(mat); \
	if (bPos) mat.SetTranslation(pos); \
	if (bScale ) mat.SetScale(scale); \
	if ( pBaseCoordIfUse != NULL ) mat = mat * (*pBaseCoordIfUse); \
	_rOUt.push_back(mat); }

	while (nStartFrame < nEndFrame)
	{
		_PUT_MAT( nStartFrame );
		mat.MakeIdentity();
		nStartFrame += nSampleInterval;
	}

	_PUT_MAT( nEndFrame );
}

}
