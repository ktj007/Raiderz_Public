#include "stdafx.h"
#include "RClothUtility.h"
#include "MFileSystem.h"
#include "RMeshNodeLoadImpl.h"
#include "RMesh.h"

#pragma warning( disable : 4482 )  // 비표준 확장이 사용되었습니다. 정규화된 이름에 '~~:~~~~' 열거형을 사용했습니다.

namespace rs3
{
	

	RClothUtility::RClothUtility(void)
	{
	}

	RClothUtility::~RClothUtility(void)
	{
	}

	void RClothUtility::makeClothInfoFromELUFile( RClothInfo *pClothInfo,std::string fileName )
	{
		_ASSERT(0 != pClothInfo);

		RMeshNode *pClothMeshNode = RMesh::loadClothMeshNodeELUFile(fileName);	
		_ASSERT(0 != pClothMeshNode);

		makeClothInfoFromMeshNode(pClothInfo,pClothMeshNode);

		if( NULL != pClothMeshNode )
		{
			delete pClothMeshNode;
			pClothMeshNode = NULL;
		}
	}

	void RClothUtility::makeClothInfoFromMeshNode( RClothInfo *pClothInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		_ASSERT(0 != pClothInfo);
		_ASSERT(0 != pClothMeshNode);
		
		RClothEdgeAndTriangleInfo edgeAndTriInfo;
		makeETIFromMeshNode(&edgeAndTriInfo,pClothMeshNode);
		makeClothInfoFromETI(pClothInfo,&edgeAndTriInfo);
	}

	void RClothUtility::makeETIFromMeshNode( RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		_ASSERT(NULL != pClothMeshNode);
		_ASSERT(NULL != pEdgeAndTriInfo);

		makePointsAndColorsOfETI(pEdgeAndTriInfo,pClothMeshNode);
		makeBaseOfETI(pEdgeAndTriInfo,pClothMeshNode);
		makeSortEdgeInfo(pEdgeAndTriInfo,pClothMeshNode);
		makeTriangleOfETI(pEdgeAndTriInfo,pClothMeshNode);
	}

	void RClothUtility::makePointsAndColorsOfETI( RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		_ASSERT(NULL != pClothMeshNode);
		_ASSERT(NULL != pEdgeAndTriInfo);

		_ASSERT( pClothMeshNode->m_nVertexIndexTableCnt == pClothMeshNode->m_nPointCnt );
		pEdgeAndTriInfo->alllocateSpaceForPointsAndColors(pClothMeshNode->m_nVertexIndexTableCnt);
		RVector* pPoints = pEdgeAndTriInfo->getPoints();
		RVector* pColors = pEdgeAndTriInfo->getColors();
		for( int i = 0; i < pClothMeshNode->m_nVertexIndexTableCnt; ++i )
		{
			const WORD& index = pClothMeshNode->m_pVertexIndexTable[i].p;

			pPoints[i] = pClothMeshNode->m_pPointTable[index];
			if( 0		== pClothMeshNode->m_nPointColorCnt		|| 
				NULL	== pClothMeshNode->m_pPointColorTable		)
			{
				pColors[i] = 1;
			}
			else
			{
				pColors[i] = pClothMeshNode->m_pPointColorTable[index];
			}
		}
	}

	void RClothUtility::makeBaseOfETI(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		_ASSERT(NULL != pClothMeshNode);
		_ASSERT(NULL != pEdgeAndTriInfo);

		RVector* pPoints = pEdgeAndTriInfo->getPoints();
		RVector* pColors = pEdgeAndTriInfo->getColors();

		int theNumberOfTriangles = pClothMeshNode->m_nFaceIndexTableCnt/3;
		for( int i = 0; i < theNumberOfTriangles; ++i )
		{
			WORD index0 = pClothMeshNode->m_pFaceIndexTable[i*3+0];
			WORD index1 = pClothMeshNode->m_pFaceIndexTable[i*3+1];
			WORD index2 = pClothMeshNode->m_pFaceIndexTable[i*3+2];

			pEdgeAndTriInfo->push_ThreeVertexIndexes_Of_AnTriangle(index0,index1,index2);	//삼각형을 이루는 버텍스 추가
			pEdgeAndTriInfo->push_An_Face(	index0,				index1,				index2,
				pPoints[index0],	pPoints[index2],	pPoints[index2]	);
		}
	}

	void RClothUtility::makeSortEdgeInfo(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		_ASSERT(NULL != pEdgeAndTriInfo);
		_ASSERT(NULL != pClothMeshNode);

		RVector* pPoints = pEdgeAndTriInfo->getPoints();

		RClothEdgeAndTriangleInfo::EDGE_SET&					setOfEdges			= pEdgeAndTriInfo->getSetOfEdges();
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET&				sortedSetOfEdges	= pEdgeAndTriInfo->getSortedSetOfEdges();
		RClothEdgeAndTriangleInfo::EDGE_SET::iterator			iterSetOfEdges;


		for(iterSetOfEdges = setOfEdges.begin();iterSetOfEdges != setOfEdges.end();++iterSetOfEdges)
		{
			RClothEdge edge = (*iterSetOfEdges);
			RC_VERTEX_INDEX index0 = edge.getVertex0Index();
			RC_VERTEX_INDEX index1 = edge.getVertex1Index();

			float maxZ = max(pPoints[index0].z,pPoints[index1].z);
			sortedSetOfEdges.insert(make_pair(maxZ,edge));
		}

		_ASSERT( setOfEdges.size() == sortedSetOfEdges.size() );

	}

	
	void RClothUtility::makeTriangleOfETI( RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode )
	{
		RClothEdgeAndTriangleInfo::SORTED_TRIANGLE_MAP& triangles_map = pEdgeAndTriInfo->getTriangles();
		RClothEdgeAndTriangleInfo::SORTED_TRIANGLE_MAP::iterator iter;
		
		RVector*		pPoints		= pEdgeAndTriInfo->getPoints();
		RVector*		pColors		= pEdgeAndTriInfo->getColors();
		
		int count=0;
		for(iter = triangles_map.begin();iter != triangles_map.end(); ++iter)
		{
			RC_TRIANGLE& currentTriangle = iter->second;

			//index정보 수정
			const float &r0 = pColors[currentTriangle.index0].x;
			const float &r1 = pColors[currentTriangle.index1].x;
			const float &r2 = pColors[currentTriangle.index2].x;

			RC_VERTEX_INDEX		sortedIndex0 = currentTriangle.index0;
			RC_VERTEX_INDEX		sortedIndex1 = currentTriangle.index1;
			RC_VERTEX_INDEX		sortedIndex2 = currentTriangle.index2;
			RC_TRIANGLE_TYPE	triType = RC_TRIANGLE_TYPE_3_RED;

			calculateIndexAndTriangleTypeByHeightAndRedColor(sortedIndex0,sortedIndex1,sortedIndex2,triType,r0,r1,r2);

			RC_TRIANGLE triangle;
			triangle.index0	=	sortedIndex0;
			triangle.index1	=	sortedIndex1;
			triangle.index2	=	sortedIndex2;

			pEdgeAndTriInfo->push_triangle(triangle,triType,pColors);

			++count;
		}

		_ASSERT(triangles_map.size() == (	pEdgeAndTriInfo->m_type0TriangleList.size() +
											pEdgeAndTriInfo->m_type1TriangleList.size() +
											pEdgeAndTriInfo->m_type2TriangleList.size() +
											pEdgeAndTriInfo->m_type3TriangleList.size()  ) );
		_ASSERT(triangles_map.size() == count);
	}


	void RClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor( RC_VERTEX_INDEX& sortedIndex0,RC_VERTEX_INDEX& sortedIndex1,RC_VERTEX_INDEX& sortedIndex2, RC_TRIANGLE_TYPE& triType,const float r0,const float r1,const float r2 )
	{
		//RC_TRIANGLE_TYPE_3_RED
		if( r0 > ATTACHING_RED_VALUE && r1 > ATTACHING_RED_VALUE && r2 > ATTACHING_RED_VALUE)
		{
			triType = RC_TRIANGLE_TYPE_3_RED;	//전부 고정점. 인덱스변환 필요없음
			return;
		}


		//RC_TRIANGLE_TYPE_2_RED
		if( r0 > ATTACHING_RED_VALUE && r1 > ATTACHING_RED_VALUE && r2 < ATTACHING_RED_VALUE)
		{
			triType = RC_TRIANGLE_TYPE_2_RED;
			return;
		}
		else if ( r0 > ATTACHING_RED_VALUE && r1 < ATTACHING_RED_VALUE && r2 > ATTACHING_RED_VALUE)
		{
			triType = RC_TRIANGLE_TYPE_2_RED;
			swap(sortedIndex1,sortedIndex2);
			return;
		}
		else if ( r0 < ATTACHING_RED_VALUE && r1 > ATTACHING_RED_VALUE && r2 > ATTACHING_RED_VALUE)
		{
			triType = RC_TRIANGLE_TYPE_2_RED;
			swap(sortedIndex0,sortedIndex2);
			return;
		}

		//RC_TRIANGLE_TYPE_1_RED
		if( r0 > ATTACHING_RED_VALUE && r1 < ATTACHING_RED_VALUE && r2 < ATTACHING_RED_VALUE )
		{
			triType = RC_TRIANGLE_TYPE_1_RED;
			return;
		}
		else if( r0 < ATTACHING_RED_VALUE && r1 > ATTACHING_RED_VALUE && r2 < ATTACHING_RED_VALUE )
		{
			triType = RC_TRIANGLE_TYPE_1_RED;
			swap(sortedIndex0,sortedIndex1);
			return;
		}
		else if( r0 < ATTACHING_RED_VALUE && r1 < ATTACHING_RED_VALUE && r2 > ATTACHING_RED_VALUE)
		{
			triType = RC_TRIANGLE_TYPE_1_RED;
			swap(sortedIndex0,sortedIndex2);
			return;
		}

		//RC_TRIANGLE_TYPE_0_RED
		if( r0 < ATTACHING_RED_VALUE && r1 < ATTACHING_RED_VALUE && r2 < ATTACHING_RED_VALUE )
		{
			triType = RC_TRIANGLE_TYPE_0_RED;
			return;
		}

		_ASSERT(false);
		return;
	}



	void RClothUtility::makeClothInfoFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo )
	{
		makeParticlesFromETI(pClothInfo,pEdgeAndTriInfo);
		makeTrianglesFromETI(pClothInfo,pEdgeAndTriInfo);	
		makeConstraintsFromETI(pClothInfo,*pEdgeAndTriInfo);
		makeGroupsOfFacesFromETI(pClothInfo,pEdgeAndTriInfo);
	}

	void RClothUtility::makeParticlesFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo)
	{
		_ASSERT(0 != pClothInfo);
		_ASSERT(0 != pEdgeAndTriInfo);

		pClothInfo->allocateSpaceForParticles(pEdgeAndTriInfo->getTheNumberOfPoints());
		RClothParticle*	pParticles	= pClothInfo->getParticles();
		RVector*		pPoints		= pEdgeAndTriInfo->getPoints();
		RVector*		pColors		= pEdgeAndTriInfo->getColors();

		for( int i = 0; i < pEdgeAndTriInfo->getTheNumberOfPoints(); ++i)
		{
			pParticles[i].oriP  = pPoints[i];
			pParticles[i].p		= pPoints[i];
			pParticles[i].newP	= pPoints[i];
			pParticles[i].color = pColors[i];

			if(pColors[i].x > ATTACHING_RED_VALUE)
			{
				pParticles[i].inverseMass = 0.0;
			}
			else
			{
				pParticles[i].inverseMass = 1.0 / pColors[i].x;
			}
			
		}
	}

	void RClothUtility::makeTrianglesFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo)
	{
		pClothInfo->allocateSpaceForTrianglesAndTypes(	pEdgeAndTriInfo->m_type0TriangleList.size(),
														pEdgeAndTriInfo->m_type1TriangleList.size(),
														pEdgeAndTriInfo->m_type2TriangleList.size(),
														pEdgeAndTriInfo->m_type3TriangleList.size()  );

		int count;
		RC_TRIANGLE *pTriangle;
		RClothEdgeAndTriangleInfo::TRIANGLE_LIST::iterator	iter;

		count = 0;
		pTriangle = pClothInfo->getType0_Triangles();
		for( iter = pEdgeAndTriInfo->m_type0TriangleList.begin() ;iter != pEdgeAndTriInfo->m_type0TriangleList.end();++iter)
		{
			pTriangle[count] = (*iter);
			++count;		
		}

		count = 0;
		pTriangle = pClothInfo->getType1_Triangles();
		for( iter = pEdgeAndTriInfo->m_type1TriangleList.begin() ;iter != pEdgeAndTriInfo->m_type1TriangleList.end();++iter)
		{
			pTriangle[count] = (*iter);
			++count;		
		}

		count = 0;
		pTriangle = pClothInfo->getType2_Triangles();
		for( iter = pEdgeAndTriInfo->m_type2TriangleList.begin() ;iter != pEdgeAndTriInfo->m_type2TriangleList.end();++iter)
		{
			pTriangle[count] = (*iter);
			++count;		
		}

		count = 0;
		pTriangle = pClothInfo->getType3_Triangles();
		for( iter = pEdgeAndTriInfo->m_type3TriangleList.begin() ;iter != pEdgeAndTriInfo->m_type3TriangleList.end();++iter)
		{
			pTriangle[count] = (*iter);
			++count;		
		}
	}

	void RClothUtility::makeConstraintsFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo )
	{
		_ASSERT(0 != pClothInfo);

		makeDistanceConstraintsFromETI(pClothInfo,edgeAndTriInfo);
		makeStretchConstraintsFromETI(pClothInfo,edgeAndTriInfo);
	}

	void RClothUtility::makeDistanceConstraintsFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo )
	{
		_ASSERT(0 != pClothInfo);

		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET&	sortedSetOfEdges = edgeAndTriInfo.getSortedSetOfEdges();
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET::reverse_iterator	riter;

		pClothInfo->allocateSpaceForDistanceConstraints(sortedSetOfEdges.size());
		RClothConstraint	*pConstraints	= pClothInfo->getDistanceConstraints();	_ASSERT(NULL != pConstraints);
		RClothParticle		*pParticles		= pClothInfo->getParticles();	_ASSERT(NULL != pParticles);

		int count=0;
		RC_VERTEX_INDEX	vertex0Index;
		RC_VERTEX_INDEX	vertex1Index;
		for(riter = sortedSetOfEdges.rbegin();riter != sortedSetOfEdges.rend(); ++riter)
		{
			RClothEdge edge = riter->second;
			if( pParticles[edge.getVertex0Index()].p.z > pParticles[edge.getVertex1Index()].p.z )
			{
				vertex0Index = edge.getVertex0Index();
				vertex1Index = edge.getVertex1Index();
			}
			else
			{
				vertex0Index = edge.getVertex1Index();
				vertex1Index = edge.getVertex0Index();
			}


			pConstraints[count].setParticleIndices(vertex0Index,vertex1Index);
			pConstraints[count].setConstraintType(DISTANCE_COSTRAINT);
			pConstraints[count].setRestDistance( (pParticles[vertex0Index].p - pParticles[vertex1Index].p).Length() );

			++count;
		}
	}

	void RClothUtility::makeStretchConstraintsFromETI( RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo )
	{
		_ASSERT(0 != pClothInfo);
		
		static RVector AXIS(RVector::AXISZ);
		float	PHI_4	=	3.141592f / 4.0f;	// PHI/4
		
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET&				sortedSetOfEdges = edgeAndTriInfo.getSortedSetOfEdges();
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET::iterator	iter;
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET				sortedSetOfStretchEdges;
		RVector*		pPoints		= edgeAndTriInfo.getPoints();

		//strecth edge수집
		for(iter = sortedSetOfEdges.begin();iter != sortedSetOfEdges.end(); ++iter)
		{
			set<RC_TRIANGLE_INDEX>	setOfTriangles;
			set<RC_VERTEX_INDEX>	setOfVertexIndexes;
			RClothEdge			edge = iter->second;
			edgeAndTriInfo.getTriangleIndexesFromAEdge(edge,setOfTriangles);
			edgeAndTriInfo.getVertexesFromTriangles(setOfTriangles,setOfVertexIndexes);

			setOfVertexIndexes.erase(edge.getVertex0Index());
			setOfVertexIndexes.erase(edge.getVertex1Index());

			if( setOfVertexIndexes.size() == 1 )
			{//이 edge를 포함하는 삼각형이 하나밖에 없다.(맨 외곽 삼각형)
			}
			else if( setOfVertexIndexes.size() == 2 )
			{
				RC_VERTEX_INDEX vertex0Index = (*setOfVertexIndexes.begin());
				RC_VERTEX_INDEX vertex1Index = (*++setOfVertexIndexes.begin());
				RClothEdge stretchEdge;
				stretchEdge.setVertexes(vertex0Index,vertex1Index);

				float maxZ = max(pPoints[vertex0Index].z,pPoints[vertex1Index].z);
				sortedSetOfStretchEdges.insert(make_pair(maxZ,stretchEdge));
			}
			else
			{
				_ASSERT(false);
			}
		}


		int count = 0;
		RC_VERTEX_INDEX vertex0Index; 
		RC_VERTEX_INDEX vertex1Index;
		pClothInfo->allocateSpaceForStretchConstraints(sortedSetOfStretchEdges.size());
		RClothConstraint	*pConstraints	= pClothInfo->getStretchConstraints();	_ASSERT(NULL != pConstraints);
		RClothParticle		*pParticles		= pClothInfo->getParticles();			_ASSERT(NULL != pParticles);
		RClothEdgeAndTriangleInfo::SORTED_EDGE_SET::reverse_iterator	riter;

		for(riter = sortedSetOfStretchEdges.rbegin();riter != sortedSetOfStretchEdges.rend(); ++riter)
		{
			RClothEdge			stretchEdge = riter->second;

			if( pParticles[stretchEdge.getVertex0Index()].p.z > pParticles[stretchEdge.getVertex1Index()].p.z )
			{
				vertex0Index = stretchEdge.getVertex0Index();
				vertex1Index = stretchEdge.getVertex1Index();
			}
			else
			{
				vertex0Index = stretchEdge.getVertex1Index();
				vertex1Index = stretchEdge.getVertex0Index();
			}

			RVector	v =	(pParticles[vertex0Index].p - pParticles[vertex1Index].p);	v.Normalize();
			float cosTheta = v.DotProduct(AXIS);	
			cosTheta = fabs(cosTheta);
			if( cosTheta < PHI_4 )
			{
				pConstraints[count].setConstraintType(STRETCH_VERTICAL_CONSTRAINT);		
			}
			else
			{
				pConstraints[count].setConstraintType(STRETCH_HORIZONTAL_CONSTRAINT);
			}

			pConstraints[count].setParticleIndices(vertex0Index,vertex1Index);
			pConstraints[count].setRestDistance( (pParticles[vertex0Index].p - pParticles[vertex1Index].p).Length() );			

			++count;
		}

	}

	void RClothUtility::makeGroupsOfFacesFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo)
	{
		//-- partitionedTriangle 생성
		RClothEdgeAndTriangleInfo::SORTED_TRIANGLE_MAP				sortedTriangles = pEdgeAndTriInfo->getTriangles();
		RClothEdgeAndTriangleInfo::SORTED_TRIANGLE_MAP::iterator	iter;
		float currentValue;
		float beforeValue;
		float difference;

		RVector*		pPoints		= pEdgeAndTriInfo->getPoints();
		static float	partitioningGap = 5.0f;

		vector<RC_TRIANGLE>			arrayTriangle;
		vector<vector<RC_TRIANGLE>>	partitionedTriangls;
		typedef vector<vector<RC_TRIANGLE>>::iterator ITER_PARTITIONED_TRIANGLES;
		typedef vector<RC_TRIANGLE>::iterator			ITER_ARRAY_TRIANGLE;

		for(iter = sortedTriangles.begin(); iter != sortedTriangles.end() ; ++iter)
		{
			RC_TRIANGLE triangleIndex = iter->second;
			RC_VERTEX_INDEX &index0 = triangleIndex.index0;
			RC_VERTEX_INDEX &index1 = triangleIndex.index1;
			RC_VERTEX_INDEX &index2 = triangleIndex.index2;

			currentValue = (pPoints[index0].z + pPoints[index1].z + pPoints[index2].z) / 3.0f;
				if( iter == sortedTriangles.begin() ) beforeValue  = currentValue;
			difference   = fabs(currentValue - beforeValue);

			
			if( difference < partitioningGap )//같은부분
			{
				arrayTriangle.push_back(triangleIndex);
			}
			else//다른부분
			{
				if( arrayTriangle.size() == 0 ) 
				{
					arrayTriangle.push_back(triangleIndex);	
				}
				else
				{
					partitionedTriangls.push_back(arrayTriangle);	//전에것들 처리
					arrayTriangle.clear();
					arrayTriangle.push_back(triangleIndex);			//현재거 처리
				}
			}

			beforeValue = currentValue;
		}
		partitionedTriangls.push_back(arrayTriangle);

		//-- partitionedTriangle를 정보를 RClothInfo의 형식에 맞게 변환,넣어줌
		pClothInfo->m_partitionedTriangles.allocateForTriangleGroups(partitionedTriangls.size());

		for(unsigned int i=0; i<partitionedTriangls.size() ; ++i )
		{
			vector<RC_TRIANGLE>& triangleArray = partitionedTriangls[i];
			unsigned int triangle_size = triangleArray.size();

			pClothInfo->m_partitionedTriangles.m_pTriangleGroups[i].allocateForATriangleGroup(triangle_size);
			for(unsigned int j=0; j<triangle_size ; ++j)
			{
				pClothInfo->m_partitionedTriangles.m_pTriangleGroups[i].m_pTriangles[j] = triangleArray[j];
			}
		}

	}//RClothUtility::makeGroupsOfFacesFromETI







}//namespace rs3