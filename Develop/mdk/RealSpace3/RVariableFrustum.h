#pragma once

#include "RTypes.h"
#include <vector>

namespace rs3 
{
	class RS_API RVariableFrustum
	{
	public:
		RVariableFrustum(void);
		~RVariableFrustum(void);

	public:
		void	AddPlane(const RPlane& Plane)	{ m_Planes.push_back(Plane); }
		RPlane	GetPlane(unsigned int index)	{ _ASSERT(IsValidIndex(index)); return m_Planes[index]; }
		unsigned int		GetNumberOfPlanes(void)			{	return m_Planes.size();		}


	private:
		bool	IsValidIndex(unsigned int index)	{	return index < m_Planes.size(); }

	private:
		std::vector<RPlane>		m_Planes;
	};
}