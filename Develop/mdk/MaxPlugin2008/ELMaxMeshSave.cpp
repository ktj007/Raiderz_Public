#include "stdafx.h"

#include "ELMaxMesh.h"
#include "RMaxMaterial.h"
#include "MDebug.h"

/*
void RMaxMtrlNode::Save(RMaxMeshFile& mfp)
{
//	mfp.Save(m_nMtrlID);
//	mfp.Save(m_nSubMtrlID);

	mfp.Save(m_vAmbient );
	mfp.Save(m_vDiffuse );
	mfp.Save(m_vSpecular);

	mfp.Save(m_fPower);

//	mfp.Save(m_nSubMtrlNum);

	mfp.Save(m_Name);
	mfp.Save(m_TexName);
	mfp.Save(m_OpaName);

	mfp.Save(m_nTwoSided);
	mfp.Save(m_nAdditive);
	mfp.Save(m_nAlphaTestRef);

	// uv animation

	int nUseUVKey = 0;

	if(m_pUVKey.m_Key) nUseUVKey = 1;

	mfp.Save(nUseUVKey);

	if(nUseUVKey) {

		int i,nCnt;

		for( i=0;i<RUVKeyType_End;i++ ) {
		
			if(m_pUVKey.m_Key[i]) {
			
				nCnt = m_pUVKey.m_Key[i]->m_nCnt;

				mfp.Save( nCnt );

				if(nCnt)	
					mfp.Save(m_pUVKey.m_Key[i]->m_pTable,sizeof(RUVKeySub)*nCnt);
			}
			else {
				nCnt = 0;
				mfp.Save( nCnt );
			}
			
		}
	}


}

void RMaxMtrlNode::SaveText(RMaxMeshFile& mfp)
{
	mfp.SaveText("mtrl_id",m_nMtrlID,m_nSubMtrlID);

	mfp.SaveText("ambient",m_vAmbient);
	mfp.SaveText("diffuse",m_vDiffuse);
	mfp.SaveText("specular",m_vSpecular);

	mfp.SaveText("power",m_fPower);
	mfp.SaveText("additive",m_nAdditive);

	mfp.SaveText("name",m_Name);
	mfp.SaveText("tex_name",m_TexName);
	mfp.SaveText("opa_name",m_OpaName);
	mfp.SaveText("UsedNodeName",m_bUsedNodeName);
	
	mfp.SaveText("sub_mtrl_num",m_nSubMtrlNum);
	mfp.SaveText("twoside",m_nTwoSided);
	mfp.SaveText("alpha_test_ref",m_nAlphaTestRef);
}
*/

//////////////////////////////////////////////////////////////////
// Polygon data ( tri-vertices, vertex-color ) 만 출력한다.

void RMaxMeshNode::GetPolygons(vector<vector<MVector3> > &outPolygonArray)
{
	if (m_PointCnt == 0)
		return;
	if ( m_FaceCnt == 0)
		return;
	
	vector<D3DXVECTOR3> vTransformVector;
	vTransformVector.resize(m_PointCnt);

	for ( int i = 0; i<m_PointCnt; ++i )
	{
		D3DXVECTOR3 &vertex = m_PointTable[i];
		D3DXVec3TransformCoord(&vTransformVector[i],&vertex,&m_matBase);
	}

	for ( int i = 0; i<m_FaceCnt; ++i )
	{
		RMaxFaceData* pFace = m_FaceTable[i];
		int nDeg = pFace->nDeg;
		vector<MVector3> polygon;
		polygon.resize(nDeg);

		for ( int n = 0; n<nDeg; ++n )
		{
			const D3DXVECTOR3 &Vertex = vTransformVector[pFace->index[n].p];
			polygon[n] = MVector3(Vertex.x, Vertex.y, Vertex.z);
		}

		outPolygonArray.push_back(polygon);
	}
}

#if 0
void RMaxMeshNode::GetPolygonData(vector<MVector3> &outVertices, vector<pair<int, int> > &outEdges)
{
	for ( int i = 0; i<m_PointCnt; ++i )
	{
		D3DXVECTOR3 &vertex = m_PointTable[i];
		D3DXVECTOR3 vec;
		
		D3DXVec3TransformCoord(&vec,&vertex,&m_matLocal);
		outVertices.push_back(MVector3(vec.x, vec.y, vec.z));
	}

	for ( int i = 0; i<m_FaceCnt; ++i )
	{
		char pcBuffer[256];
		pcBuffer[0] = 0;

		RMaxFaceData* pFace = m_FaceTable[i];
		for ( int n = 0; n<pFace->nDeg; ++n )
		{
			pair<int, int> nEdge;
			nEdge.first = pFace->index[n].p;
			nEdge.second = pFace->index[(n+1)%pFace->nDeg].p;
			outEdges.push_back(nEdge);
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////

void RMaxMeshNode::Save(RMaxMeshFile& mfp)
{
	int nSize = 0;
	int nParentNodeID = -1;

	mfp.Save(m_Name);
	mfp.Save(m_ParentName);
	
	mfp.Save(m_nParentID );

	mfp.Save(m_dwFlag);
	mfp.Save(m_nAlign);

//	mfp.Save(DWORD(m_AniPartsType));
//	int nAniPartsType = 0;
//	mfp.Save(DWORD(nAniPartsType));	// -> 사용하지 말자
//	mfp.Save(DWORD(m_PartsPosInfoType));
//	mfp.Save(DWORD(m_PartsType));	// version 11 에 제거

	////////////////////////////////////////////////////

//	mfp.Save(m_matBase);
	mfp.Save(m_matLocal);
	mfp.Save(m_fBaseVisibility); // version 11에 추가

	//////////////////////////////////////////////////////////////////////////

	mfp.Save(m_PointCnt);

	if(m_PointCnt)	mfp.Save(m_PointTable,m_PointCnt);

	nSize = m_NormalTable.size();

	mfp.Save(nSize);

	if(nSize)	mfp.Save((D3DXVECTOR3*)&m_NormalTable[0],nSize);

	nSize = m_TangentTanTable.size();

	mfp.Save(nSize);

	// version 16에서 vector4로 변경
	if(nSize)	mfp.Save((D3DXVECTOR4*)&m_TangentTanTable[0],nSize);

	nSize = m_TangentBinTable.size();

	mfp.Save(nSize);

	if(nSize)	mfp.Save((D3DXVECTOR3*)&m_TangentBinTable[0],nSize);

	nSize = m_TexCoordTable.size();

	mfp.Save(nSize);

	if(nSize)	mfp.Save((D3DXVECTOR3*)&m_TexCoordTable[0],nSize);

	nSize = m_TexCoordTableExtra.size();

	mfp.Save(nSize);

	if(nSize)	mfp.Save((D3DXVECTOR3*)&m_TexCoordTableExtra[0],nSize);

	mfp.Save(m_FaceCnt);
	
	// polygon 정보
	if(m_FaceCnt)
	{
		int nTotalDeg = 0;	// 버텍스 수
		int nTotalTriangles	= 0;	// 삼각형 수
		for(int i=0;i<m_FaceCnt;i++)
		{
			RMaxFaceData* pFace = m_FaceTable[i];
			nTotalDeg += pFace->nDeg;
			nTotalTriangles += pFace->nDeg-2;
		}
		mfp.Save(nTotalDeg);
		mfp.Save(nTotalTriangles);
		for(int i=0;i<m_FaceCnt;i++)
		{
			RMaxFaceData* pFace = m_FaceTable[i];
			mfp.Save(pFace->nDeg);
			mfp.Save(pFace->index,sizeof(RFaceSubData)*pFace->nDeg);
			short nMaterial = pFace->pMaterial->GetIndex();
			mfp.Save(nMaterial);
			
//			mfp.Save(pFace->triangleIndices,sizeof(unsigned char)*(pFace->nDeg-2)*3);
		}
	}

	// point-color 처리. 
	bool bNoShade = false;
	bool bVertexColor = false;

	if ( FindPropertyString("vertexcolor") || FindPropertyString("set_vertexcolor = true") )
		bVertexColor = true;

	if ( bVertexColor )
	{
		mfp.Save(m_PointColorCnt);	// 0이더라도 파일포맷상 항상 저장하도록 했습니다. dubble
		if ( m_PointColorCnt )		// vertex color가 있고, 실제로 그 데이터가 있을 경우
		{
			mlog("- save vertex color data from %s\n", m_Name.c_str());
			RVector3 *pFillTable = new RVector3[m_PointColorCnt];
			for ( int i = 0; i<m_PointColorCnt; ++i )
			{
				pFillTable[i].x = m_PointColorTable[i].x;
				pFillTable[i].y = m_PointColorTable[i].y;
				pFillTable[i].z = m_PointColorTable[i].z;
			}
			mfp.Save(pFillTable, m_PointColorCnt*sizeof(RVector3));
			delete [] pFillTable;
		}
	}
	else
	{
		// property에 'vertexcolor'라는 단어가 있지 않으면, 저장하지 않는다.
		int nPointColorCount = 0;
		mfp.Save(nPointColorCount);
	}

	int nMtrlID = m_pMtrl ? m_pMtrl->GetIndex() : -1;
	mfp.Save(nMtrlID);

	mfp.Save(m_PhysiqueTable,m_PhysiqueCnt);

	mfp.Save(m_BoneTableCnt);

	if(m_BoneTableCnt) {
		mfp.Save(m_BoneTable,m_BoneTableCnt);
		mfp.Save((void*)m_BoneTableIndex,sizeof(WORD)*m_BoneTableCnt);
	}

	// vertexbuffer 구성정보..로딩 시간을 줄이기 위해 미리 계산해서 출력..

	mfp.Save(m_VertexIndexTableCnt);

	if(m_VertexIndexTableCnt)
		mfp.Save((void*)m_VertexIndexTable,sizeof(RVertexIndex)*m_VertexIndexTableCnt);

	// 현재 triangle list 만 쓰이나 strip 등 확장을 위해 primitive type 을 기록한다
	int nPrimitiveType = 0;
	mfp.Save(nPrimitiveType);

	mfp.Save(m_FaceIndexTableCnt);
	if(m_FaceIndexTableCnt)
	{
		mfp.Save((void*)m_FaceIndexTable,sizeof(WORD)*m_FaceIndexTableCnt);
	}

	mfp.Save(m_MtrlTableInfoCnt);

	if(m_MtrlTableInfoCnt)
		mfp.Save((void*)m_MtrlTableInfo,sizeof(RMtrlTableInfo)*m_MtrlTableInfoCnt);

	// Bounding-box 데이터 - EXPORTER_MESH_VER13 추가 사항
	mfp.Save((void*)m_BoundingBox.vmin.v, sizeof(float)*3);
	mfp.Save((void*)m_BoundingBox.vmax.v, sizeof(float)*3);
}