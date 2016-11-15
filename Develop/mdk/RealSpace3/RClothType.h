#pragma once
namespace rs3
{
	typedef unsigned short	RC_VERTEX_INDEX;
	typedef unsigned int	RC_EDGE_VALUE;
	typedef unsigned int	RC_TRIANGLE_INDEX;
	typedef struct tagRCFACE
	{
		RC_VERTEX_INDEX index0,index1,index2;
	}RC_TRIANGLE;

	typedef int	RC_TRIANGLE_TYPE;
	#define RC_TRIANGLE_TYPE_0_RED	0
	#define RC_TRIANGLE_TYPE_1_RED	1
	#define RC_TRIANGLE_TYPE_2_RED	2	
	#define RC_TRIANGLE_TYPE_3_RED	3

	static const float ATTACHING_RED_VALUE = 251.0f / 255.0f;
}