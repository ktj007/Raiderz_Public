///////////////////////////////////////////////////////////////////////  
//	Constants
//
//	(c) 2003 IDV, Inc.
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com
//

#pragma once


// grass
const   float   c_fDefaultGrassFarLod = 200.0f;
const   float   c_fGrassFadeLength = 50.0f;
const   float   c_fMinBladeNoise = -0.2f;
const   float   c_fMaxBladeNoise = 0.2f;
const   float   c_fMinBladeThrow = 8.0f;		// 풀이 좌우로 흔들리는 정도 min값
const   float   c_fMaxBladeThrow = 10.0f;		// 풀이 좌우로 흔들리는 정도 max값
const   float   c_fMinBladeSize = 3.0f;
const   float   c_fMaxBladeSize = 8.0f;
const	int		c_nNumBladeMaps = 2;
//const   float   c_fGrassBillboardWideScalar = 3.0f;
const   float   c_fGrassBillboardWideScalar = 1.0f;
const   float   c_fWalkerHeight = 12.0f;
const   int     c_nDefaultGrassBladeCount = 30000;
const   int     c_nGrassRegionCount = 20;
const	int		c_nGrassSubDivs = 5; // must be an odd number

// grass vertex attribute sizes
const   int     c_nGrassVertexTexture0Size = 2 * sizeof(float);         // base map coordinate
const   int     c_nGrassVertexTexture1Size = 4 * sizeof(float);         // vertex index, blade size, wind weight, noise factor
const   int     c_nGrassVertexColorSize = 4 * sizeof(unsigned char);    // (rgba)
const   int     c_nGrassVertexPositionSize = 3 * sizeof(float);         // (x, y, z)

const   int     c_nGrassVertexTotalSize = c_nGrassVertexTexture0Size +
                                          c_nGrassVertexTexture1Size +
                                          c_nGrassVertexColorSize +
                                          c_nGrassVertexPositionSize;

                                          
