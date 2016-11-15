#include "..\..\Realspace3\stdafx.h"
#include "ROctreeManagerTest.h"
#include <GL/glut.h>

namespace rs3
{
	ROMT::ROMT(void)
	{
	}

	ROMT::~ROMT(void)
	{
	}

	void	ROMT::Render(float div)
	{
		if( m_pRootNode )	Render(m_pRootNode,div);
	}

	void	ROMT::Render(ROctreeNode* pNode,float div)
	{
		float	d = (pNode->m_Depth / 5.0) + 0.2;
		if (d > 1) d = 1;

		// Draw this node.
		if( pNode->m_State == CS_OUTSIDE )			glColor3f(1, 0, 0);
		else if( pNode->m_State == CS_INSIDE)		glColor3f(1, 1, 0);
		else if( pNode->m_State == CS_PARTIAL )		glColor3f(0, 1, 1);
		else if( pNode->m_State == CS_NONE )		glColor3f(0, 0, 1);
		else _ASSERT(false);


		static const int	CT = 50;
		static float	Coords[CT][2];
		static bool	Inited = false;

		if (!Inited) {
			for (int i = 0; i < CT; i++) {
				float	theta = float(i) / CT * 2 * 3.141592;
				Coords[i][0] = cos(theta);
				Coords[i][1] = sin(theta);
			}
			
			Inited = true;
		}
		
		float	x, y, r;

		x = pNode->m_X		/ div;
		y = pNode->m_Z		/ div;
		r = pNode->m_Radius / div;

		int	inc = 1;

		glBegin(GL_POLYGON /* TRIANGLE_FAN */);
		for (int i = 0; i < CT; i += inc) {
			glVertex3f(x - Coords[i][0] * r, y - Coords[i][1] * r, 1);
		}
		glEnd();
		
		for(int i=0;i<O_HEIGHT;i++)
		{
			for(int j=0;j<O_WIDTH;j++)
			{
				if( pNode->m_pChildren[i][j] )
				{
					Render(pNode->m_pChildren[i][j],div);
				}
			}
		}

		ROctreeItem* pItem = pNode->m_pItemsChain;
		while( pItem )
		{
			Render(pItem,div,1,0,0);
			pItem = pItem->m_pNext;
		}

	}

	void	ROMT::Render(ROctreeItem* pItem,float div,float r,float g,float b)
	{
		static const int	CT = 50;
		static float	Coords[CT][2];
		static bool	Inited = false;

		if (!Inited) {
			for (int i = 0; i < CT; i++) {
				float	theta = float(i) / CT * 2 * 3.141592;
				Coords[i][0] = cos(theta);
				Coords[i][1] = sin(theta);
			}
			
			Inited = true;
		}
		
		float	x, y, radius;

		x = pItem->X() / div;
		y = pItem->Z() / div;
		radius = pItem->R() / div;

		int	inc = 1;

		glColor3f(r,g,b);
		glBegin(GL_POLYGON /* TRIANGLE_FAN */);
		for (int i = 0; i < CT; i += inc) {
			glVertex3f(x - Coords[i][0] * radius, y - Coords[i][1] * radius, 1);
		}
		glEnd();

		glColor3f(0,0,1);
		glBegin(GL_LINES );
			glVertex2f(x,y);
			glVertex2f(pItem->m_pOtreeNode->m_X/div,pItem->m_pOtreeNode->m_Z/div);
		glEnd();
	}

	void ROMT::CallBack_Cull(const RPlane* pPlanes,int planesNum, ROctreeItem* pItem,CULL_STATE state )
	{
	#ifdef USE_CC
		if( CS_INSIDE == state || CS_PARTIAL == state )
		{
			m_CullingSet.insert(pItem);
		}
		else if( CS_OUTSIDE == state )
		{
			m_CullingSet.erase(pItem);
		}
		else
		{
			_ASSERT(false);
		}
	#else
		if (state == CS_INSIDE )
		{
			Render(pItem,100,1,1,0);
		}
		else if (state == CS_PARTIAL)
		{
			Render(pItem,100,0,1,1);
		}
	#endif
	}

	void ROMT::CallBack_Cull( const RPlane* pPlanes,int planesNum,ROctreeItem* pItem,CULL_STATE before_state,CULL_STATE current_state )
	{
		if( current_state == CS_PARTIAL || current_state == CS_INSIDE )
		{
			_ASSERT( m_CullingSet.end() == m_CullingSet.find(pItem) );
			m_CullingSet.insert(pItem);
		}
		else if( current_state == CS_OUTSIDE )
		{
			_ASSERT( m_CullingSet.end() != m_CullingSet.find(pItem) );
			m_CullingSet.erase(pItem);
		}
		else _ASSERT(false);
	}


	void ROMT::RenderCullingList( float div )
	{
		std::set<ROctreeItem*>::iterator iter;
		for(iter = m_CullingSet.begin(); iter != m_CullingSet.end(); iter++ )
		{
			ROctreeItem* pItem = *iter;
			if( pItem->GetState() == CS_INSIDE )			Render(pItem,div,1,1,0);
			else if( pItem->GetState() == CS_PARTIAL )		Render(pItem,div,0,1,1);
			else _ASSERT(false);
		}
	}
}
