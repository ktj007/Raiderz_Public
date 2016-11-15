#pragma once

#include "RLib.h"
#include "MVector2.h"
#include "MVector3.h"
#include "MMath.h"
#include <vector>
#include <functional>
#include <algorithm>


class CML2_API MConvexHullAlgorithm
{
public:
	//Points : 입력으로 들어온 점들
	//SortedPoints : ConvexHull을 이루는 점들을 CounterClockWise방향으로 정렬되게 모음
	static void MakeConvexHullByCounterClockWise2D(const std::vector<MVector2>	&Points,std::vector<MVector2>	&SortedPoints);
	static bool MakeConvexHullByCounterClockWise3D(const std::vector<MVector3>	&Points,std::vector<MVector3>	&SortedPoints);

public://MakeConvexHullByCounterClockWise()에 관계된 함수들
	static int		BottomRightMostPointIndex(const std::vector<MVector2> &Points);
	static float	GetAngle(const MVector2& A,const MVector2& B);
	static void		SortByAngle(std::vector<MVector2>& Points);
	static float	IsLeft(const MVector2& P0,const MVector2& P1,const MVector2& P2);
	static bool		IsConvexPoint(const MVector2& P0,const MVector2& P1,const MVector2& P2);
	static MMatrix	TransformPlanePointsToXYPlane(const std::vector<MVector3>	&Points,std::vector<MVector3>& TransformedPoints);
	


public:
	//3점이 이루는 4각형의 넓이. 결과값이 음수로도 나올 수 있다.
	static float QuadrangleArea(const MVector2 &A,const MVector2 &B,const MVector2 &C);

	//true값 리턴 : ABC가 positive area, A->B->C가 counterclockwise
	static bool	 CounterClockWise(const MVector2 &A,const MVector2 &B,const MVector2 &C) { return QuadrangleArea(A,B,C) > 0; }
};



