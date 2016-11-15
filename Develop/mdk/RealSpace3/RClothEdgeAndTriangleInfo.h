#pragma once

#include <Windows.h>
#include "RMeshNode.h"
#include <set>
#include <map>
#include "RClothType.h"
#include <functional>

namespace rs3
{
	class RClothEdge
	{
	public:
		RClothEdge(void);
		~RClothEdge(void);

	public:
		void	setVertexes(int vertex0Index,int vertex1Index);
		inline	RC_EDGE_VALUE	getEdgeValue(void)		const	{	return m_edgeValue;		}
		inline	RC_VERTEX_INDEX	getVertex0Index(void)	const	{	return m_vertex0Index;	}
		inline	RC_VERTEX_INDEX	getVertex1Index(void)	const	{	return m_vertex1Index;	}
		
	private:
		union
		{
			RC_EDGE_VALUE	m_edgeValue;
			struct  
			{
				RC_VERTEX_INDEX	m_vertex0Index;
				RC_VERTEX_INDEX	m_vertex1Index;
			};
		};
	};

	bool	operator==(const RClothEdge& lhs,const RClothEdge& rhs);
	bool	operator<(const RClothEdge& lhs,const RClothEdge& rhs);



	//----------------------------------------------------------------------------------------------
	class RClothEdgeAndTriangleInfo
	{
	public:
		RClothEdgeAndTriangleInfo(void);
		~RClothEdgeAndTriangleInfo(void);

		typedef std::set<RClothEdge>										EDGE_SET;
		typedef std::multimap<float,RClothEdge>								SORTED_EDGE_SET;
		typedef std::multimap<RC_TRIANGLE_INDEX,RClothEdge>					TRIANGLE_TO_EDGE_MULTIMAP;
		typedef	std::multimap<RClothEdge,RC_TRIANGLE_INDEX>					EDGE_TO_TRIANGLE_MULTIMAP;
		typedef std::multimap<float,RC_TRIANGLE,greater<float>>				SORTED_TRIANGLE_MAP;
		typedef	std::list<RC_TRIANGLE>										TRIANGLE_LIST;

	public://접근
		EDGE_SET&					getSetOfEdges(void)			{	return m_info_For_A_SetOfEdges;	}
		SORTED_EDGE_SET&			getSortedSetOfEdges(void)	{	return m_info_For_A_SortedSetOfEdges;	}
		EDGE_TO_TRIANGLE_MULTIMAP&	getEdgeToTriInfo(void)		{	return m_info_For_A_Edge_To_Triangles;	}
		SORTED_TRIANGLE_MAP&		getTriangles(void)			{	return m_info_For_Sorted_Triangles;	}
		rs3::RVector*				getPoints(void)				{	return m_pPoints;	}
		rs3::RVector*				getColors(void)				{	return m_pColors;	}
		int							getTheNumberOfPoints(void)	{	return m_theNumberOfPoints;	}
		void						setTheNumberOfPoints(int theNumberOfPoints)	{	m_theNumberOfPoints = theNumberOfPoints;	}

	public://가공
		void	push_ThreeVertexIndexes_Of_AnTriangle(int vertex0Index,int vertex1Index,int vertex2Index);
		void	push_An_Face(int vertex0Index,int vertex1Index,int vertex2Index,const rs3::RVector& v0,const rs3::RVector& v1,const rs3::RVector& v2);
		void	alllocateSpaceForPointsAndColors(int theNumberOfPoints);
		void	releasePointsAndColors(void);
		void	push_triangle(const RC_TRIANGLE &triangle,RC_TRIANGLE_TYPE triType,RVector* pColors);

	public://사용 
		void	getVertexesFromTriangleIndex(RC_TRIANGLE_INDEX triIndex,set<RC_VERTEX_INDEX>& setOfVertexes);
		void	getTriangleIndexesFromAEdge(RClothEdge edge,set<RC_TRIANGLE_INDEX>& setOfTriangles);
		void	getVertexesFromTriangles(set<RC_TRIANGLE_INDEX>& setOfTriangles,set<RC_VERTEX_INDEX>& setOfVertexes);

	public://디버그
		void	debugPrintOut(void);
	private:
		void	push_ThreeEdges_Of_AnTriangle(const RClothEdge& edge0,const RClothEdge& edge1,const RClothEdge& edge2);
			void	push_Info_For_A_SetOfEdges(const RClothEdge& edge);
			void	push_Info_For_A_Triangle_To_Three_Edges(const RC_TRIANGLE_INDEX& trianlgeIndex,const RClothEdge& edge);
			void	push_Info_For_A_Edge_To_Triangles(const RClothEdge& edge,const RC_TRIANGLE_INDEX& trianlgeIndex);

		inline	void			increaseCountingForPushedTriangles(void)	{	m_countingForPushedTriangles++;			}
		inline	RC_TRIANGLE_INDEX	getCountingForPushedTriangles(void)			{	return m_countingForPushedTriangles;	}

	private:
		EDGE_SET						m_info_For_A_SetOfEdges;
		SORTED_EDGE_SET					m_info_For_A_SortedSetOfEdges;
		TRIANGLE_TO_EDGE_MULTIMAP		m_info_For_A_Triangle_To_Three_Edges;
		EDGE_TO_TRIANGLE_MULTIMAP		m_info_For_A_Edge_To_Triangles;
		SORTED_TRIANGLE_MAP				m_info_For_Sorted_Triangles;
		RC_TRIANGLE_INDEX				m_countingForPushedTriangles;

		rs3::RVector*	m_pPoints;
		int				m_theNumberOfPoints;
		rs3::RVector*	m_pColors;

	public:
		TRIANGLE_LIST		m_type3TriangleList;
		TRIANGLE_LIST		m_type2TriangleList;
		TRIANGLE_LIST		m_type1TriangleList;
		TRIANGLE_LIST		m_type0TriangleList;
	};
}
