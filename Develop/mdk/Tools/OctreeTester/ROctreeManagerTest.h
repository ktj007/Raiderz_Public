#pragma once

#include "..\..\Realspace3\ROctreeManager.h"

namespace rs3
{
	class ROMT	: public ROctreeManager , public ROctreeCallBack
	{
	public:
		ROMT(void);
		~ROMT(void);
	public:
		void	Render(float div);
		void	Render(ROctreeNode* pNode,float div);
		void	Render(ROctreeItem* pItem,float div,float r,float g,float b);
		void	RenderCullingList(float div);

	public:
		virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE state) override;
		virtual void CallBack_Cull(const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state) override;
		
	private:
		std::set<ROctreeItem*> m_CullingSet;
	};
}