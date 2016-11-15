#pragma once

#include "RTypes.h"
#include "RVariableFrustum.h"
#include "ROccluderList.h"
#include <vector>


namespace rs3
{

class RS_API ROccluderPoints
{
public:
	void				AddPoint(const RVector3& Point) {	m_Points.push_back(Point);	}
	unsigned int		GetNumberOfPoints(void)	const	{	return m_Points.size();		}
	const RVector3&		GetPoint(unsigned int index)	const
	{	
		_ASSERT( IsValidIndex(index));	
		return m_Points[index]; 
	}
	void				SetPoint(const RVector3& Point,unsigned int index)
	{
		_ASSERT( IsValidIndex(index));
		m_Points[index] = Point;
	}

	RVector3 			GetCenterPoint(void);

	RAABB				GetAABB(void);

private:
	bool				IsValidIndex(unsigned int index) const	{ return index<GetNumberOfPoints(); }

private:
	std::vector<RVector3> m_Points;
};


class RS_API ROccluderPointsSet
{
public:
	ROccluderPointsSet();
	~ROccluderPointsSet();

	void					AddOccluderPoints(const ROccluderPoints& OccluderPoints)	{ m_OccluderPointsSet.push_back(OccluderPoints); }
	unsigned int			GetTheNumberOfOccludePoints(void)							{ return m_OccluderPointsSet.size(); }
	ROccluderPoints&		GetOCcluderPoints(unsigned int index)
	{
		_ASSERT( IsValidIndex(index) );
		return m_OccluderPointsSet[index];
	}

	RAABB					GetAABB(void);
	
private:
	bool	IsValidIndex(unsigned int index)	{	return index < m_OccluderPointsSet.size(); }

private:
	std::vector<ROccluderPoints>		m_OccluderPointsSet;
};

class RS_API ROccluderUtil
{
public:// 오클루더 포인터로 부터 프러스텀을 만드는 함수들
	static void ConvertOccluderPointsToVariableFrusum(const RVector3& ViewCenter,ROccluderPoints& OccluderPoints,RVariableFrustum& VariableFrustum);
	static void ConvertOccluderPointsSetToVariableFrustums(const RVector3& ViewCenter,ROccluderPointsSet& OccluderPointsSet,vector<RVariableFrustum>& VariableFrustums);
	static void ConvertOccluderListToVariableFrustumsList(const RVector3& ViewCenter,ROccluderList& OccluderList,vector<vector<RVariableFrustum>>& VariableFrustumsList);

public:// 오클루전 정보를 렌더링 하는 함수들
	static void RenderOcclusionPoints(ROccluderPoints& OccluderPoints,DWORD dwColor = 0xffffffff);
	static void RenderOcclusionPointsSet(ROccluderPointsSet& OccluderPointsSet,DWORD dwColor = 0xffffffff);
};

}
