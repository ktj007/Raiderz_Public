#ifndef _TPICK_H
#define _TPICK_H

#pragma once

class TPick
{
private:
	void						_2DTo3DCoord(vec3* pOutDir , vec3* pOutOrig , MPoint ScreenPt, MRect rtClient , MMatrix* CameraView , MMatrix* ProjWorld);
	bool						IntersectTriangle(const vec3& orig, const vec3& dir, vec3& v0, vec3& v1, vec3& v2, float *t, float *u, float *v);

	void						GetAABBVertexData(RBoundingBox& box, vector<vec3>& vecVertices, RMatrix& matWorld);

public:
	TPick();
	virtual ~TPick();

	static bool					AABBPicking(RBoundingBox& box, MPoint pt, MRect rect, RMatrix& matWorld, RMatrix& matCamera, RMatrix& matProject, float& fOutDist);



};
#endif