#pragma once

#include <Windows.h>
#include <string>
#include "d3d9types.h"
#include "RMeshNode.h"
#include "RClothInfo.h"
#include "RClothEdgeAndTriangleInfo.h"


namespace rs3
{
	class RClothUtility
	{
	public:
		RClothUtility(void);
		~RClothUtility(void);

	public:
		static void				makeClothInfoFromELUFile(RClothInfo *pClothInfo,std::string fileName);
		static void				makeClothInfoFromMeshNode(RClothInfo *pClothInfo,const rs3::RMeshNode *pClothMeshNode);
	protected:
		static void				makeETIFromMeshNode(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode);
			static void				makePointsAndColorsOfETI(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode);
			static void				makeBaseOfETI(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode);
			static void				makeSortEdgeInfo(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode);
			static void				makeTriangleOfETI(RClothEdgeAndTriangleInfo* pEdgeAndTriInfo,const rs3::RMeshNode *pClothMeshNode);

		static void				makeClothInfoFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo);
			static void				makeParticlesFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo);
			static void				makeTrianglesFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo);
			static void				makeConstraintsFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo);
				static void				makeDistanceConstraintsFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo);
				static void				makeStretchConstraintsFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo& edgeAndTriInfo);
			static void				makeGroupsOfFacesFromETI(RClothInfo *pClothInfo,RClothEdgeAndTriangleInfo* pEdgeAndTriInfo);



		static void		calculateIndexAndTriangleTypeByHeightAndRedColor(RC_VERTEX_INDEX& sortedIndex0,RC_VERTEX_INDEX& sortedIndex1,RC_VERTEX_INDEX& sortedIndex2,
																RC_TRIANGLE_TYPE& triType,const float r0,const float r1,const float r2);
	};
}