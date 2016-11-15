#include "stdafx.h"
#include "RClothEdgeAndTriangleInfo.h"
#include <stdio.h>


namespace rs3
{
	RClothEdge::RClothEdge( void )
	:m_edgeValue(0)
	{
	
	}

	RClothEdge::~RClothEdge( void )
	{
	}

	void RClothEdge::setVertexes( int vertex0Index,int vertex1Index )
	{
		_ASSERT(vertex0Index != vertex1Index);

		if( vertex0Index < vertex1Index )
		{
			m_vertex0Index = vertex0Index;
			m_vertex1Index = vertex1Index;
		}
		else
		{
			m_vertex0Index = vertex1Index;
			m_vertex1Index = vertex0Index;
		}

	}



	bool operator<( const RClothEdge& lhs,const RClothEdge& rhs )
	{
		if( lhs.getEdgeValue() < rhs.getEdgeValue() )	return true;
		return false;
	}

	bool operator==( const RClothEdge& lhs,const RClothEdge& rhs )
	{
		if( lhs.getEdgeValue() == rhs.getEdgeValue() )	return true;
		return false;
	}

	//---------------------------------------------------------------------
	RClothEdgeAndTriangleInfo::RClothEdgeAndTriangleInfo(void)
	:m_countingForPushedTriangles(0)
	{
		m_pPoints = NULL;
		m_pColors = NULL;
		m_theNumberOfPoints = 0;
	}

	RClothEdgeAndTriangleInfo::~RClothEdgeAndTriangleInfo(void)
	{
		releasePointsAndColors();
	}

	void RClothEdgeAndTriangleInfo::push_ThreeVertexIndexes_Of_AnTriangle(int vertex0Index,int vertex1Index,int vertex2Index)
	{
		//edge만들기
		RClothEdge	edge0;	edge0.setVertexes(vertex0Index,vertex1Index);
		RClothEdge	edge1;	edge1.setVertexes(vertex1Index,vertex2Index);
		RClothEdge	edge2;	edge2.setVertexes(vertex2Index,vertex0Index);
		
		push_ThreeEdges_Of_AnTriangle(edge0,edge1,edge2);
		increaseCountingForPushedTriangles();
	}

	void RClothEdgeAndTriangleInfo::push_ThreeEdges_Of_AnTriangle( const RClothEdge& edge0,const RClothEdge& edge1,const RClothEdge& edge2 )
	{
		push_Info_For_A_SetOfEdges(edge0);
		push_Info_For_A_SetOfEdges(edge1);
		push_Info_For_A_SetOfEdges(edge2);

		push_Info_For_A_Triangle_To_Three_Edges(getCountingForPushedTriangles(),edge0);
		push_Info_For_A_Triangle_To_Three_Edges(getCountingForPushedTriangles(),edge1);
		push_Info_For_A_Triangle_To_Three_Edges(getCountingForPushedTriangles(),edge2);

		push_Info_For_A_Edge_To_Triangles(edge0,getCountingForPushedTriangles());
		push_Info_For_A_Edge_To_Triangles(edge1,getCountingForPushedTriangles());
		push_Info_For_A_Edge_To_Triangles(edge2,getCountingForPushedTriangles());
	}

	void RClothEdgeAndTriangleInfo::push_Info_For_A_SetOfEdges( const RClothEdge& edge )
	{
		m_info_For_A_SetOfEdges.insert(edge);
	}

	void RClothEdgeAndTriangleInfo::push_Info_For_A_Triangle_To_Three_Edges( const RC_TRIANGLE_INDEX& trianlgeIndex,const RClothEdge& edge )
	{
		m_info_For_A_Triangle_To_Three_Edges.insert(make_pair(trianlgeIndex,edge));
	}

	void RClothEdgeAndTriangleInfo::push_Info_For_A_Edge_To_Triangles( const RClothEdge& edge,const RC_TRIANGLE_INDEX& trianlgeIndex )
	{
		m_info_For_A_Edge_To_Triangles.insert(make_pair(edge,trianlgeIndex));
	}

	void RClothEdgeAndTriangleInfo::debugPrintOut( void )
	{
		int count=0;

		/*
		count=0;
		EDGE_SET::iterator	iterSetOfEdges;
		for(iterSetOfEdges = m_info_For_A_SetOfEdges.begin();iterSetOfEdges !=  m_info_For_A_SetOfEdges.end();iterSetOfEdges++)
		{
			RClothEdge edge = (*iterSetOfEdges);
			printf("\n [%d]_ v0 : %d, v1 : %d",count,edge.getVertex0Index(),edge.getVertex1Index() );
			count++;
		}
		*/


	/*
		count=0;
		EDGE_TO_TRIANGLE_MULTIMAP::iterator iterE2T;
		for( iterE2T = m_info_For_A_Edge_To_Triangles.begin(); iterE2T != m_info_For_A_Edge_To_Triangles.end() ; iterE2T++)
		{
			RClothEdge			edge	 = iterE2T->first;
			RC_TRIANGLE_INDEX	triIndex = iterE2T->second;
			printf("\n [%d]_ v0 : %d , v1 : %d , triIndex : %d",count,edge.getVertex0Index(),edge.getVertex1Index(),triIndex);

			count++;
		}*/


		count=0;
		SORTED_TRIANGLE_MAP::iterator	iterFace;
		for(iterFace = m_info_For_Sorted_Triangles.begin();iterFace != m_info_For_Sorted_Triangles.end();++iterFace)
		{
			RC_TRIANGLE face = iterFace->second;
			printf("\n [%d]_ i0: %d , i1: %d , i2: %d",count,face.index0,face.index1,face.index2);
			++count;
		}


	}


	void RClothEdgeAndTriangleInfo::getVertexesFromTriangleIndex( RC_TRIANGLE_INDEX triIndex,set<RC_VERTEX_INDEX>& setOfVertexes )
	{
		pair<TRIANGLE_TO_EDGE_MULTIMAP::iterator,TRIANGLE_TO_EDGE_MULTIMAP::iterator>	range;
		range = m_info_For_A_Triangle_To_Three_Edges.equal_range(triIndex);

		TRIANGLE_TO_EDGE_MULTIMAP::iterator iter;
		for( iter = range.first ; iter != range.second ; ++iter)
		{
			RClothEdge edge = iter->second;
			setOfVertexes.insert(edge.getVertex0Index());
			setOfVertexes.insert(edge.getVertex1Index());
		}
	}

	void RClothEdgeAndTriangleInfo::getTriangleIndexesFromAEdge( RClothEdge edge,set<RC_TRIANGLE_INDEX>& setOfTriangles )
	{
		//TODO : find_if로 찾는 것으로 바꾸어야 한다.
		//EDGE_TO_TRIANGLE_MULTIMAP iter = m_info_For_A_Edge_To_Triangles.begin();
		//iter = m_info_For_A_Edge_To_Triangles.find_if(iter,m_info_For_A_Edge_To_Triangles.end(),)
		
		EDGE_TO_TRIANGLE_MULTIMAP::iterator iter;
		for(iter = m_info_For_A_Edge_To_Triangles.begin(); iter != m_info_For_A_Edge_To_Triangles.end() ; ++iter)
		{
			RClothEdge		currentEdge		= iter->first;
			RC_TRIANGLE_INDEX	currentTriIndex = iter->second;
			if( edge == currentEdge )
			{
				setOfTriangles.insert(currentTriIndex);
			}
		}

	}

	void RClothEdgeAndTriangleInfo::getVertexesFromTriangles( set<RC_TRIANGLE_INDEX>& setOfTriangles,set<RC_VERTEX_INDEX>& setOfVertexes )
	{
		set<RC_TRIANGLE_INDEX>::iterator iter_Tri;
		for(iter_Tri = setOfTriangles.begin();iter_Tri != setOfTriangles.end(); ++iter_Tri)
		{
			RC_TRIANGLE_INDEX triIndex = *iter_Tri;
			getVertexesFromTriangleIndex(triIndex,setOfVertexes);
		}

	}

	void RClothEdgeAndTriangleInfo::push_An_Face( int vertex0Index,int vertex1Index,int vertex2Index,const RVector& v0,const RVector& v1,const RVector& v2 )
	{
		RC_TRIANGLE face;
		float averageZ = (v0.z + v1.z + v2.z)/3.0f;

		if( v0.z >= v1.z && v0.z >= v2.z )
		{
			face.index0 = vertex0Index;
			face.index1 = vertex1Index;
			face.index2 = vertex2Index;
		}
		else if( v1.z >= v0.z && v1.z >= v2.z )
		{
			face.index0 = vertex1Index;
			face.index1 = vertex0Index;
			face.index2 = vertex2Index;
		}
		else
		{
			face.index0 = vertex2Index;
			face.index1 = vertex0Index;
			face.index2 = vertex1Index;
		}

		m_info_For_Sorted_Triangles.insert(make_pair(averageZ,face));
	}

	void RClothEdgeAndTriangleInfo::alllocateSpaceForPointsAndColors( int theNumberOfPoints )
	{
		_ASSERT(theNumberOfPoints > 0);
		m_theNumberOfPoints = theNumberOfPoints;
		m_pPoints = new RVector[theNumberOfPoints];
		m_pColors = new RVector[theNumberOfPoints];
	}

	void RClothEdgeAndTriangleInfo::releasePointsAndColors( void )
	{
		if( NULL != m_pPoints )	delete[] m_pPoints;
		if( NULL != m_pColors )	delete[] m_pColors;
		m_theNumberOfPoints = 0;
	}

	void RClothEdgeAndTriangleInfo::push_triangle( const RC_TRIANGLE &triangle,RC_TRIANGLE_TYPE triType,RVector* pColors )
	{
		switch(triType)
		{
			case RC_TRIANGLE_TYPE_3_RED:
			{
				m_type3TriangleList.push_back(triangle);

				_ASSERT( (pColors[triangle.index0].x > ATTACHING_RED_VALUE) );
				_ASSERT( (pColors[triangle.index1].x > ATTACHING_RED_VALUE) );
				_ASSERT( (pColors[triangle.index2].x > ATTACHING_RED_VALUE) );
			}break;

			case RC_TRIANGLE_TYPE_2_RED:
			{
				m_type2TriangleList.push_back(triangle);

				_ASSERT(  (pColors[triangle.index0].x > ATTACHING_RED_VALUE) );
				_ASSERT(  (pColors[triangle.index1].x > ATTACHING_RED_VALUE) );
				_ASSERT( !(pColors[triangle.index2].x > ATTACHING_RED_VALUE) );
			}break;

			case RC_TRIANGLE_TYPE_1_RED:
			{
				m_type1TriangleList.push_back(triangle);

				_ASSERT(  (pColors[triangle.index0].x > ATTACHING_RED_VALUE) );
				_ASSERT( !(pColors[triangle.index1].x > ATTACHING_RED_VALUE) );
				_ASSERT( !(pColors[triangle.index2].x > ATTACHING_RED_VALUE) );
			}break;

			case RC_TRIANGLE_TYPE_0_RED:
			{
				m_type0TriangleList.push_back(triangle);

				_ASSERT( !(pColors[triangle.index0].x > ATTACHING_RED_VALUE) );
				_ASSERT( !(pColors[triangle.index1].x > ATTACHING_RED_VALUE) );
				_ASSERT( !(pColors[triangle.index2].x > ATTACHING_RED_VALUE) );
			}break;

			default:
			{
				_ASSERT(false);
			}break;
		}		
	}
}//rs3