/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#include "PhysXDebugRenderer.h"
#include "PhysXSDK/SDKs/Foundation/include/NxDebugRenderable.h"

// Enigma
#define ENIGMA_CLIENTSIDE // Set to import headers
#include "../../../../Enigma/Source/Precompiled.h"
#include "../../../../Enigma/Source/Enigma.h"

#define GL_POINTS 0
#define GL_LINES 1
#define GL_TRIANGLES 2


void DebugRenderer::renderBuffer(float* pVertList, float* pColorList, int type, int num)
{
	if (type == GL_LINES)
	{
		for (int i=0; i<num; i+=2)
		{
			const uint32 vtxNum1 = i	*3;
			const uint32 vtxNum2 = (i+1)*3;
			MCore::Vector3 p0(pVertList[vtxNum1+0], pVertList[vtxNum1+1], pVertList[vtxNum1+2]);
			MCore::Vector3 p1(pVertList[vtxNum2+0], pVertList[vtxNum2+1], pVertList[vtxNum2+2]);
			MCore::RGBAColor color(pColorList[vtxNum1+0], pColorList[vtxNum1+1], pColorList[vtxNum1+2], 1.0f);

			ENIGMA_GRAPHICS->GetLine()->DrawLine3D( &p0, &p1, color.ToDWORD() );
		}
	}
}

void DebugRenderer::renderData(const NxDebugRenderable& data) const
{
	unsigned int NbPoints = data.getNbPoints();
	if(NbPoints)
	{
		float* pVertList = new float[NbPoints*3];
    	float* pColorList = new float[NbPoints*4];
    	int vertIndex = 0;
    	int colorIndex = 0;
		const NxDebugPoint* Points = data.getPoints();
		while(NbPoints--)
		{
        	pVertList[vertIndex++] = Points->p.x;
        	pVertList[vertIndex++] = Points->p.y;
        	pVertList[vertIndex++] = Points->p.z;
        	pColorList[colorIndex++] = (float)((Points->color>>16)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)((Points->color>>8)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)(Points->color&0xff)/255.0f;
        	pColorList[colorIndex++] = 1.0f;
	      	Points++;
		}
		
		renderBuffer(pVertList, pColorList, GL_POINTS, data.getNbPoints());
    	
    	delete[] pVertList;
    	delete[] pColorList;
	}

	unsigned int NbLines = data.getNbLines();
	if(NbLines)
	{
		float* pVertList = new float[NbLines*3*2];
    	float* pColorList = new float[NbLines*4*2];
    	int vertIndex = 0;
    	int colorIndex = 0;
		const NxDebugLine* Lines = data.getLines();
		while(NbLines--)
		{
        	pVertList[vertIndex++] = Lines->p0.x;
        	pVertList[vertIndex++] = Lines->p0.y;
        	pVertList[vertIndex++] = Lines->p0.z;
        	pColorList[colorIndex++] = (float)((Lines->color>>16)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)((Lines->color>>8)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)(Lines->color&0xff)/255.0f;
        	pColorList[colorIndex++] = 1.0f;

        	pVertList[vertIndex++] = Lines->p1.x;
        	pVertList[vertIndex++] = Lines->p1.y;
        	pVertList[vertIndex++] = Lines->p1.z;
        	pColorList[colorIndex++] = (float)((Lines->color>>16)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)((Lines->color>>8)&0xff)/255.0f;
        	pColorList[colorIndex++] = (float)(Lines->color&0xff)/255.0f;
        	pColorList[colorIndex++] = 1.0f;

	      	Lines++;
		}
		
		renderBuffer(pVertList, pColorList, GL_LINES, data.getNbLines()*2);
    	
    	delete[] pVertList;
    	delete[] pColorList;
	}

	unsigned int NbTris = data.getNbTriangles();
	if(NbTris)
	{
		float* pVertList = new float[NbTris*3*3];
    	float* pColorList = new float[NbTris*4*3];
    	int vertIndex = 0;
    	int colorIndex = 0;
		const NxDebugTriangle* Triangles = data.getTriangles();
		while(NbTris--)
		{
        	pVertList[vertIndex++] = Triangles->p0.x;
        	pVertList[vertIndex++] = Triangles->p0.y;
        	pVertList[vertIndex++] = Triangles->p0.z;

        	pVertList[vertIndex++] = Triangles->p1.x;
        	pVertList[vertIndex++] = Triangles->p1.y;
        	pVertList[vertIndex++] = Triangles->p1.z;

        	pVertList[vertIndex++] = Triangles->p2.x;
        	pVertList[vertIndex++] = Triangles->p2.y;
        	pVertList[vertIndex++] = Triangles->p2.z;

			for(int i=0;i<3;i++)
			{
        		pColorList[colorIndex++] = (float)((Triangles->color>>16)&0xff)/255.0f;
        		pColorList[colorIndex++] = (float)((Triangles->color>>8)&0xff)/255.0f;
        		pColorList[colorIndex++] = (float)(Triangles->color&0xff)/255.0f;
        		pColorList[colorIndex++] = 1.0f;
			}

	      	Triangles++;
		}
		
		renderBuffer(pVertList, pColorList, GL_TRIANGLES, data.getNbTriangles()*3);
  	
    	delete[] pVertList;
    	delete[] pColorList;
	}
}
