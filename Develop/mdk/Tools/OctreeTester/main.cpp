#include "..\..\Realspace3\stdafx.h"
#include "..\..\Realspace3\ROctreeManager.h"
#include "ROctreeManagerTest.h"
#include <GL/glut.h>
#include <queue>

using namespace rs3;

ROMT g_Tree;
std::queue<ROctreeItem*>	g_Queue;

RPlane g_plane[4];

void	IdleFunc()
{
	// Call the display routine next time through the main loop  
	glutPostRedisplay();
}


void	DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(0.001, -0.001, 1);
	
	g_Tree.Render(100.0f);
 	g_Tree.Cull_MC(g_plane,4,&g_Tree);
	g_Tree.RenderCullingList(100);
	

	glColor3f(1,0,1);
	glBegin(GL_LINES );
		glVertex2f(-g_plane[0].a * g_plane[0].d / 100.0f,-500);
		glVertex2f(-g_plane[0].a * g_plane[0].d / 100.0f,500);

		glVertex2f(-g_plane[1].a * g_plane[1].d / 100.0f,-500);
		glVertex2f(-g_plane[1].a * g_plane[1].d / 100.0f,500);

		glVertex2f(-500,g_plane[2].d / 100.0f);
		glVertex2f(500,g_plane[2].d / 100.0f);

		glVertex2f(-500,g_plane[3].d / 100.0f);
		glVertex2f(500,g_plane[3].d / 100.0f);

	glEnd();	

	// Draw frustum.
	glColor3f(1, 0, 0);
	glutSwapBuffers();
}


void	KeyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 't':
	case 'T':
		{
// 			DWORD time1,time2;
// 			RPlane plane[4];
// 			plane[0].a = 1;
// 			plane[0].b = 0;
// 			plane[0].c = 0;
// 			plane[0].d = 0;
// 
// 			plane[1].a = -1;
// 			plane[1].b = 0;
// 			plane[1].c = 0;
// 			plane[1].d = 30000;
// 
// 			plane[2].a = 0;
// 			plane[2].b = 0;
// 			plane[2].c = -1;
// 			plane[2].d = 30000;
// 
// 			plane[3].a = 0;
// 			plane[3].b = 0;
// 			plane[3].c = 1;
// 			plane[3].d = 0;
// 
// 			int COUNT = 10000;
// 
// 			time1 = GetTickCount();
// 			for(int i=0;i<COUNT;i++)
// 				g_Tree.Cull(plane,4,&g_Tree);
// 			time2 = GetTickCount();
// 			printf(" time : %d \n", time2 - time1);
// 
// 			time1 = GetTickCount();
// 			for(int i=0;i<COUNT;i++)
// 				g_Tree.Cull_M(plane,4,&g_Tree);
// 			time2 = GetTickCount();
// 			printf(" time : %d \n", time2 - time1);
// 
// 			time1 = GetTickCount();
// 			for(int i=0;i<COUNT;i++)
// 				g_Tree.Cull_MC(plane,4,&g_Tree);
// 			time2 = GetTickCount();
// 			printf(" time : %d \n", time2 - time1);
		}break;

	case 'a':
	case 'A':
		{
			int x,y,z,r;
			RSceneNode *pSceneNode = new RSceneNode;
			x = (rand() % 8000 - 4000) * 10;
			y = 0;
			z = (rand() % 8000 - 4000) * 10;
			r = rand() % 3000;
			pSceneNode->GetSphere().center = RVector(x,y,z);
			pSceneNode->GetSphere().radius = r;

			ROctreeItem* pItem = g_Tree.Add(pSceneNode);
			g_Queue.push(pItem);
			
		}break;

	case 'c':
	case 'C':
		{
			g_Tree.Cull(g_plane,4,&g_Tree);
		}break;

	case '6':
		{
			for(int i=0;i<2;i++)
				g_plane[i].d+= 400;
		}break;

	case '4':
		{
			for(int i=0;i<2;i++)
				g_plane[i].d-= 400;
		}break;

	case 'b':
	case 'B':
		{
			int x,y,z,r;

			static int inc = 10;
			inc+=1000;

			RSceneNode *pSceneNode = new RSceneNode;
			x = -10000+inc;
			y = -5000;
			z = 20000;
			r = 2000;
			pSceneNode->GetSphere().center = RVector(x,y,z);
			pSceneNode->GetSphere().radius = r;

			ROctreeItem* pItem = g_Tree.Add(pSceneNode);
			g_Queue.push(pItem);
		}break;

	case 'd':
	case 'D':
		{
			if( 0 == g_Queue.size() ) break;
			ROctreeItem* pItem = g_Queue.front();
			g_Queue.pop();
			g_Tree.Remove(pItem);
		}break;

	case 'm':
	case 'M':
		{
			if( 0 == g_Queue.size() ) break;
			ROctreeItem* pItem = g_Queue.front();
			g_Tree.Move(pItem,RSphere(RVector(pItem->X()+2000,pItem->Y(),pItem->Z()+300),pItem->R()));
		}break;

	}



}

void	OpenOGL()
{
	glutInitDisplayMode(GLUT_RGBA | /* GLUT_DEPTH | */ GLUT_DOUBLE);
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Octree test");
	
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
//	glutMouseFunc(MouseHandler);
//	glutMotionFunc(MotionHandler);
//  glutSpecialFunc(KeyboardSpecialFunc);
//  glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc);
//  
//  InitializeProgram();
	
	glutSwapBuffers();
//  glutMainLoop();
	
}

void main(void)
{
	g_plane[0].a = 1;
	g_plane[0].b = 0;
	g_plane[0].c = 0;
	g_plane[0].d = 0;

	g_plane[1].a = -1;
	g_plane[1].b = 0;
	g_plane[1].c = 0;
	g_plane[1].d = 30000;

	g_plane[2].a = 0;
	g_plane[2].b = 0;
	g_plane[2].c = -1;
	g_plane[2].d = 30000;

	g_plane[3].a = 0;
	g_plane[3].b = 0;
	g_plane[3].c = 1;
	g_plane[3].d = 0;

	OpenOGL();

	glutMainLoop();
}
/*
int AABBvsFrustum(AABB *b, FRUSTUM *f, int in_mask, int *out_mask)
{
	float m, n; int i, k = 1 << b->start_id, result = INSIDE;

	PLANE *sp = f->plane + b->start_id; *out_mask=0;

	if (k & in_mask) 
	{

		m = (sp->a * b->v[sp->nx].x) + (sp->b * b->v[sp->ny].y) + (sp->c * b->v[sp->nz].z);
		if (m > -sp->d) return OUTSIDE;

		n = (sp->a * b->v[sp->px].x) + (sp->b * b->v[sp->py].y) + (sp->c * b->v[sp->pz].z);
		if (n > -sp->d) { *out_mask |= k; result = INTERSECT; }

	}

	for (i = 0, k = 1; k <= in_mask; i++, k += k)
	{
		if ((i != b->start_id) && (k & in_mask)) 
		{ 
			PLANE *p = f->plane + i;

			m = (p->a * b->v[p->nx].x) + (p->b * b->v[p->ny].y) + (p->c * b->v[p->nz].z);

			if (m > -p->d) 
			{ 
				b->start_id = i; 
				return OUTSIDE; 
			}

			n = (p->a * b->v[p->px].x) + (p->b * b->v[p->py].y) + (p->c * b->v[p->pz].z);
			if (n > -p->d) 
			{ 
				*out_mask |= k; 
				result = INTERSECT; 
			}

		} 
	}
	return result;
}
*/
