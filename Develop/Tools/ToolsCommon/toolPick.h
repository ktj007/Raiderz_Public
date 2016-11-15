#pragma once

#include "RViewFrustum.h"
#include "RDeviceD3D.h"

class CToolPick
{
private:
	void						_2DTo3DCoord(vec3* pOutDir , vec3* pOutOrig , MPoint ScreenPt, MRect rtClient , MMatrix* CameraView , MMatrix* ProjWorld);
	bool						IntersectTriangle(const vec3& orig, const vec3& dir, vec3& v0, vec3& v1, vec3& v2, float *t, float *u, float *v);

	void						GetAABBVertexData(MBox& box, vector<vec3>& vecVertices, MMatrix& matWorld);
	void						GetBillboardVertexData(vec3 vCenter, float fW, float fH, vector<vec3>& vecVertices, MMatrix& matView);

public:
	CToolPick();
	virtual ~CToolPick();

	static bool					AABBPicking(MBox& box, MPoint pt, MRect rect, MMatrix& matWorld, MMatrix& matView, MMatrix& matProject, float& fOutDist);

	static bool					BillboardPicking(vec3 vCenter, float fW, float fH, MPoint pt, MRect rect, MMatrix& matView, MMatrix& matProject, float& fOutDist);
	static bool					BillboardPicking(vec3 vCenter, float fW, float fH, vec3 vOrg, vec3 vDir, MMatrix& matView, float& fOutDist);
	static bool					BillboardIntersection(vec3 vCenter, float fW, float fH, rs3::RViewFrustum& _refFrustum, MMatrix& matView);

	static vec3					CalObejectMovefrom2D(vec2& vStart, vec2& vEnd, vec3& vAxis, vec3& vCenter, RECT& rt, rs3::RDeviceD3D* pDevice, vec3 vCameraPos, vec3 vCameraDir);

};
