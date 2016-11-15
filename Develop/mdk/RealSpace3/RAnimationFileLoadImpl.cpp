#include "StdAfx.h"
#include "RAnimationFileLoadImpl.h"
#include "RAnimationNode.h"
#include "RD3DVertexUtil.h"
#include <d3dx9.h>
#include "RFile.h"

namespace rs3 {



#define RFILEREAD1(str)															\
{	int _size = 0;																\
	if ( rf.Read( &_size, sizeof( int)) == false)	return false;				\
	str.resize( _size);															\
	if ( rf.Read( &str[ 0], _size) == false)		return false;				\
	if ( _size > 0  &&  str[ _size - 1] == 0)		str.resize( _size - 1);		\
}

#define RFILEREAD2(buff,read)													\
{	if ( rf.Read( (buff), (read)) == false)			return false;				\
}


//////////////////////////////////////////////////////////////////////////
//
void RAnimationFileLoadImpl_v6::LoadVertexAniBoundingBox(RFile &rf, RAnimationNode *pANode)
{
	pANode->m_bboxVertexAni.Initialize();
	for ( int j = 0; j<pANode->m_nVertexCnt; ++j )
	{
		for ( int i = 0; i<pANode->m_nVertexVCnt; ++i )
			pANode->m_bboxVertexAni.Add(pANode->m_vertex[j][i]);
	}
}

bool RAnimationFileLoadImpl_v6::LoadVertexAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	RFILEREAD1( pANode->m_Name);
	RFILEREAD2( &pANode->m_nVertexCnt, 4);	// vertex group (mesh-node) count
	if ( pANode->m_nVertexCnt )
		pANode->m_vertex = new RVector*[pANode->m_nVertexCnt];

	int vcnt = 0;	// vertex point count
	RFILEREAD2( &vcnt, 4);
	pANode->m_nVertexVCnt = vcnt;

	if ( pANode->m_nVertexCnt )
		pANode->m_pVertexFrame = new DWORD[pANode->m_nVertexCnt];

	RFILEREAD2( pANode->m_pVertexFrame, sizeof(DWORD)*pANode->m_nVertexCnt);
	for (int j = 0; j<pANode->m_nVertexCnt; ++j)
	{
		pANode->m_vertex[j] = new RVector[vcnt];
		RFILEREAD2( pANode->m_vertex[j], sizeof(RVector)*vcnt);
	}

	LoadVertexAniBoundingBox(rf, pANode);

	return true;
}

bool RAnimationFileLoadImpl_v6::LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	RFILEREAD1( pANode->m_Name);
	if (dwVersion>=EXPORTER_ANI_VER6)
		RFILEREAD1( pANode->m_parentName);

	RFILEREAD2( &pANode->m_matLocal, sizeof(D3DXMATRIX));


	// 초기 프레임에 대한 애니메이션 키 <- 오 후버댐!! 이런것도 있었네 하지만 지금은 안쓰이는 듯 - 090709, COMMENT OZ
	// 안쓰는듯 하여 지웁니다. - birdkr 2010/05/07
	//pANode->m_matLocal.Decompose(pANode->m_baseScale,pANode->m_baseTranslation,pANode->m_baseRotation);

	int pos_key_num = 0;
	int rot_key_num = 0;

	RFILEREAD2( &pos_key_num, 4);
	if ( pos_key_num )
	{
		pANode->m_positionKeyTrack.Create(pos_key_num+1);
		RFILEREAD2( (void*)&pANode->m_positionKeyTrack[0],sizeof(RPosKey)*pos_key_num);

		pANode->m_positionKeyTrack[pos_key_num] = pANode->m_positionKeyTrack[pos_key_num-1];

	}

	rf.Read(&rot_key_num, 4);
	if ( rot_key_num )
	{
		pANode->m_rotationKeyTrack.Create(rot_key_num+1);
		RFILEREAD2( (void*)&pANode->m_rotationKeyTrack[0],sizeof(RQuatKey)*rot_key_num);
		pANode->m_rotationKeyTrack[rot_key_num] = pANode->m_rotationKeyTrack[rot_key_num-1];

	}

	if ( dwVersion>=EXPORTER_ANI_VER5)
	{
		int nScaleCnt = 0;
		RFILEREAD2( &nScaleCnt,sizeof(DWORD));
		if(nScaleCnt) 
		{
			pANode->m_scaleKeyTrack.Create(nScaleCnt);
			RFILEREAD2( (void*)&pANode->m_scaleKeyTrack[0],sizeof(RScaleKey)*nScaleCnt);
		}
	}

	return true;
}

bool RAnimationFileLoadImpl_v6::LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	if( dwVersion>=EXPORTER_ANI_VER5)
	{
		int nVisCnt = 0;
		RFILEREAD2( &nVisCnt, 4);

		if(nVisCnt) 
		{
			pANode->m_visKeyTrack.Create(nVisCnt);
			RFILEREAD2( (void*)&pANode->m_visKeyTrack[0],sizeof(RVisKey)*nVisCnt);

			// Visiblity값이 0보다 작을 수 있다. 맥스에서 Off용도로 0보다 작은값을 사용.
			for( int i = 0; i < nVisCnt; ++i )
			{
				if( pANode->m_visKeyTrack[i].v < 0.0f )
					pANode->m_visKeyTrack[i].v = 0.0f;
			}

			return true;
		}
	}
	else
	{
		int nVisCnt = 0;
		RFILEREAD2( &nVisCnt, 4);

		struct ROldVisKey 
		{
			float v;
			int frame;
		} oldKey;

		if(nVisCnt) 
		{
			pANode->m_visKeyTrack.Create(nVisCnt);
			for( int i=0; i<nVisCnt; i++)
			{
				// Visiblity값이 0보다 작을 수 있다. 맥스에서 Off용도로 0보다 작은값을 사용.
				RFILEREAD2( &oldKey,sizeof(ROldVisKey));
				if( oldKey.v < 0.0f )
					oldKey.v = 0.0f;

				pANode->m_visKeyTrack[i].frame = oldKey.frame;
				pANode->m_visKeyTrack[i].v = oldKey.v;
			}

			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
void RAnimationFileLoadImpl_v7::LoadVertexAniBoundingBox(RFile &rf, RAnimationNode *pANode)
{
	RBoundingBox *pAABB = &pANode->m_bboxVertexAni;
	rf.Read( pAABB->vmin.v, sizeof(float)*3);
	rf.Read( pAABB->vmax.v, sizeof(float)*3);
}

//////////////////////////////////////////////////////////////////////////
//
bool RAnimationFileLoadImpl_v9::LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	int nVisCnt = 0;
	RFILEREAD2( &nVisCnt, 4);

	if(nVisCnt) 
	{
		pANode->m_visKeyTrack.Create(nVisCnt);
		RFILEREAD2( (void*)&pANode->m_visKeyTrack[0],sizeof(RVisKey)*nVisCnt);

		return true;
	}

	return false;
};

bool RAnimationFileLoadImpl_v11::LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	RFILEREAD1( pANode->m_Name);
	RFILEREAD1( pANode->m_parentName);

	RFILEREAD2( &pANode->m_matLocal, sizeof(D3DXMATRIX));

	// 초기 프레임에 대한 애니메이션 키 <- 오 후버댐!! 이런것도 있었네 하지만 지금은 안쓰이는 듯 - 090709, COMMENT OZ
	// 안쓰는듯 하여 지웁니다. - birdkr 2010/05/07
	//pANode->m_matLocal.Decompose(pANode->m_baseScale,pANode->m_baseTranslation,pANode->m_baseRotation);
	RAnimType anim_type1;
	RFILEREAD2( &anim_type1, 12); //RAnimType
	if ( anim_type1.Count > 0 )
	{
		pANode->m_positionKeyTrack.Create(anim_type1.Count + 1);
		RFILEREAD2( (void*)&pANode->m_positionKeyTrack[0],sizeof(RPosKey)*anim_type1.Count);
		pANode->m_positionKeyTrack[anim_type1.Count] = pANode->m_positionKeyTrack[anim_type1.Count - 1];
	}

	RAnimType anim_type2;
	RFILEREAD2( &anim_type2, 12); //RAnimType
	if ( anim_type2.Count > 0 )
	{
		pANode->m_rotationKeyTrack.Create(anim_type2.Count+1);
		RFILEREAD2( (void*)&pANode->m_rotationKeyTrack[0],sizeof(RQuatKey)*anim_type2.Count);
		pANode->m_rotationKeyTrack[anim_type2.Count] = pANode->m_rotationKeyTrack[anim_type2.Count - 1];
	}
	RAnimType anim_type3;
	RFILEREAD2( &anim_type3, 12); //RAnimType
	if(anim_type3.Count > 0) 
	{
		pANode->m_scaleKeyTrack.Create(anim_type3.Count);
		RFILEREAD2( (void*)&pANode->m_scaleKeyTrack[0],sizeof(RScaleKey)*anim_type3.Count);
	}

	return true;
}




bool RAnimationFileLoadImpl_v12::LoadBoneAni(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	RFILEREAD1( pANode->m_Name);
	//if (dwVersion>=EXPORTER_ANI_VER6)
	//	RFILEREAD1( pANode->m_parentName);
	
	int unk = 0;
	RVector3 baseTrans;

	RFILEREAD2( &pANode->m_baseTranslation, sizeof(RVector3));
	RFILEREAD2( &pANode->m_baseRotation, sizeof(RQuaternion));
	RFILEREAD2( &pANode->m_baseScale, sizeof(RVector3));

	// Affine 을 가정한다


	/*
MVector3 vScale = MVector3(pANode->m_baseScale.x * 1.f , pANode->m_baseScale.y * 1.f, pANode->m_baseScale.z * 1.f);

MMatrix mtxLocal = pANode->m_baseRotation.Conv2Matrix();

float Mtx[] = {  mtxLocal._11 / vScale.x, mtxLocal._12 / vScale.x, mtxLocal._13 / vScale.x, 0,
                mtxLocal._21 / vScale.y, mtxLocal._22 / vScale.y, mtxLocal._22 / vScale.y, 0,
                 mtxLocal._32 / vScale.z, mtxLocal._32 / vScale.z, mtxLocal._33 / vScale.z, 0,
                 pANode->m_baseTranslation.x, pANode->m_baseTranslation.y, pANode->m_baseTranslation.z, 1
               };

	pANode->m_matLocal = Mtx;
	*/

	// 초기 프레임에 대한 애니메이션 키 <- 오 후버댐!! 이런것도 있었네 하지만 지금은 안쓰이는 듯 - 090709, COMMENT OZ
	// 안쓰는듯 하여 지웁니다. - birdkr 2010/05/07
	//pANode->m_matLocal.Decompose(pANode->m_baseScale,pANode->m_baseTranslation,pANode->m_baseRotation);
	RAnimType anim_type1;
	RFILEREAD2( &anim_type1, 12); //RAnimType
	if ( anim_type1.Count > 0 )
	{
		if(anim_type1.CountType == 10)
		{
			RAnimationTrackTest<RPos2Key> rPosKeys;
			rPosKeys.Create(anim_type1.Count);
			int j = 0;
			for (j = j; j < anim_type1.Count; ++j){
			RFILEREAD2( (void*)&rPosKeys.at(j),10);
			}

			if(!rPosKeys.empty())
			{
				pANode->m_positionKeyTrack.Create(anim_type1.Count + 1);
				int i = 0;
				for (i = 0; i < anim_type1.Count; ++i){
						pANode->m_positionKeyTrack.at(i).frame = rPosKeys.at(i).frame;
					//	mlog("frame after: %i frame before:%i\n", pANode->m_positionKeyTrack.at(i).frame,  rPosKeys.at(i).frame);
						pANode->m_positionKeyTrack.at(i).x = rPosKeys.at(i).sVector.X();
					//	mlog("x after: %f, x before: %i\n", pANode->m_positionKeyTrack.at(i).x, rPosKeys.at(i).sVector.m_x);
						pANode->m_positionKeyTrack.at(i).y = rPosKeys.at(i).sVector.Y();
					//	mlog("y after: %f, y before: %i\n", pANode->m_positionKeyTrack.at(i).y, rPosKeys.at(i).sVector.m_y);
						pANode->m_positionKeyTrack.at(i).z = rPosKeys.at(i).sVector.Z();
					//	mlog("z after: %f, z before: %i\n", pANode->m_positionKeyTrack.at(i).z, rPosKeys.at(i).sVector.m_z);
					}
				pANode->m_positionKeyTrack[anim_type1.Count] = pANode->m_positionKeyTrack[anim_type1.Count - 1];
			}
		}
		else if(anim_type1.CountType == 16)
		{
			pANode->m_positionKeyTrack.Create(anim_type1.Count + 1);
			RFILEREAD2( (void*)&pANode->m_positionKeyTrack[0],sizeof(RPosKey)*anim_type1.Count);
			pANode->m_positionKeyTrack[anim_type1.Count] = pANode->m_positionKeyTrack[anim_type1.Count - 1];
		}
		else
		{
			mlog_tool("invalid ELU attempted to be loaded!");
			return false;
		}
	}

	RAnimType anim_type2;
	RFILEREAD2( &anim_type2, 12); //RAnimType
	if ( anim_type2.Count > 0 )
	{
		if(anim_type2.CountType == 10)
		{
			RAnimationTrackTest<RPos2Key> rPosKeys;
			rPosKeys.Create(anim_type2.Count);
			int j = 0;
			for (j = j; j < anim_type1.Count; ++j){
				RFILEREAD2( (void*)&rPosKeys.at(j),10);
			}
			if(!rPosKeys.empty())
			{
				pANode->m_rotationKeyTrack.Create(anim_type2.Count + 1);
				int i = 0;
				for (i = 0; i < anim_type2.Count; ++i){
						pANode->m_rotationKeyTrack.at(i).frame = rPosKeys.at(i).frame;
						pANode->m_rotationKeyTrack.at(i).x = rPosKeys.at(i).sVector.X();
						pANode->m_rotationKeyTrack.at(i).y = rPosKeys.at(i).sVector.Y();
						pANode->m_rotationKeyTrack.at(i).z = rPosKeys.at(i).sVector.Z();
						float fTol = pANode->m_rotationKeyTrack.at(i).x * pANode->m_rotationKeyTrack.at(i).x + pANode->m_rotationKeyTrack.at(i).y * pANode->m_rotationKeyTrack.at(i).y + pANode->m_rotationKeyTrack.at(i).z * pANode->m_rotationKeyTrack.at(i).z;
						//mlog("anim: %f %f %f", pANode->m_rotationKeyTrack.at(i).x, pANode->m_rotationKeyTrack.at(i).y, pANode->m_rotationKeyTrack.at(i).z);
						if(fTol <= 1.0f)
						{
							float sub1 = 1.0f - fTol;
							float fSqrt = sqrt(sub1);
							pANode->m_rotationKeyTrack.at(i).w = fSqrt;
						}
						else
						{
							pANode->m_rotationKeyTrack.at(i).w = 0.0f;
						}
					}
					pANode->m_rotationKeyTrack[anim_type2.Count] = pANode->m_rotationKeyTrack[anim_type2.Count - 1];
			}
		}
		else if(anim_type2.CountType == 16)
		{
			RAnimationTrackTest<RRPosKey> rPosKeys;
			rPosKeys.Create(anim_type2.Count);
			int j = 0;
			for (j = j; j < anim_type2.Count; ++j){
				RFILEREAD2( (void*)&rPosKeys.at(j),16);
			}
			if(!rPosKeys.empty())
			{
				pANode->m_rotationKeyTrack.Create(anim_type2.Count + 1);
				int i = 0;
				for (i = 0; i < anim_type2.Count; ++i){
						pANode->m_rotationKeyTrack.at(i).frame = rPosKeys.at(i).frame;
						pANode->m_rotationKeyTrack.at(i).x = rPosKeys.at(i).Vector.x;
						pANode->m_rotationKeyTrack.at(i).y = rPosKeys.at(i).Vector.y;
						pANode->m_rotationKeyTrack.at(i).z = rPosKeys.at(i).Vector.z;
						float fTol = pANode->m_rotationKeyTrack.at(i).x * pANode->m_rotationKeyTrack.at(i).x + pANode->m_rotationKeyTrack.at(i).y * pANode->m_rotationKeyTrack.at(i).y + pANode->m_rotationKeyTrack.at(i).z * pANode->m_rotationKeyTrack.at(i).z;
						//mlog("anim: %f %f %f", pANode->m_rotationKeyTrack.at(i).x, pANode->m_rotationKeyTrack.at(i).y, pANode->m_rotationKeyTrack.at(i).z);
						if(fTol <= 1.0f)
						{
							float sub1 = 1.0f - fTol;
							float fSqrt = sqrt(sub1);
							pANode->m_rotationKeyTrack.at(i).w = fSqrt;
						}
						else
						{
							pANode->m_rotationKeyTrack.at(i).w = 0.0f;
						}
				}
				pANode->m_rotationKeyTrack[anim_type2.Count] = pANode->m_rotationKeyTrack[anim_type2.Count - 1];
			}
		}
		else if(anim_type2.CountType == 20)
		{
			pANode->m_rotationKeyTrack.Create(anim_type2.Count + 1);
			RFILEREAD2( (void*)&pANode->m_rotationKeyTrack[0],sizeof(RQuatKey)*anim_type2.Count);
			pANode->m_rotationKeyTrack[anim_type2.Count] = pANode->m_rotationKeyTrack[anim_type2.Count - 1];
		}
	}
	RAnimType anim_type3;
	RFILEREAD2( &anim_type3, 12); //RAnimType
	if(anim_type3.Count > 0) 
	{
		pANode->m_scaleKeyTrack.Create(anim_type3.Count);
		RFILEREAD2( (void*)&pANode->m_scaleKeyTrack[0],sizeof(RScaleKey)*anim_type3.Count);
	}

	return true;
}



bool RAnimationFileLoadImpl_v11::LoadVisibilityKey(RFile &rf, RAnimationNode *pANode, DWORD dwVersion)
{
	RAnimType anim_type;
	RFILEREAD2( &anim_type, sizeof(RAnimType)); //RAnimType
	if(anim_type.Count > 0) 
	{
		pANode->m_visKeyTrack.Create(anim_type.Count);
		RFILEREAD2( (void*)&pANode->m_visKeyTrack[0],sizeof(RVisKey)*anim_type.Count);

	}

	return true;
};

};