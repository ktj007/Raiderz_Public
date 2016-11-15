#include "stdafx.h"
#include "TPick.h"

TPick::TPick()
{

}

TPick::~TPick()
{

}

bool TPick::IntersectTriangle( const vec3& orig, const vec3& dir, vec3& v0, vec3& v1, vec3& v2, float *t, float *u, float *v )
{
	// Find vectors for two edges sharing vert0
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	vec3 pvec;
	pvec = dir.CrossProduct(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1.DotProduct(pvec);

	vec3 tvec;
	if( det > 0 )
	{
		tvec = orig - v0;
	}
	//else
	//{
		// 이부분 모야!? ㅡㅡ;;
		// 리턴하닌깐... 더 정확한 Picking이 되잖앙
		// tvec = v0 - orig;
		// det = -det;
	//}

	if( det < 0.0001f )
		return false;

	// Calculate U parameter and test bounds
	*u = tvec.DotProduct(pvec);
	if( *u < 0.0f || *u > det )
		return false;

	// Prepare to test V parameter
	vec3 qvec = tvec.CrossProduct(edge1);

	// Calculate V parameter and test bounds
	*v = dir.DotProduct(qvec);
	if( *v < 0.0f || *u + *v > det )
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = edge2.DotProduct(qvec);
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;	
}

void TPick::_2DTo3DCoord( vec3* pOutDir , vec3* pOutOrig , MPoint ScreenPt, MRect rtClient , MMatrix* CameraView , MMatrix* ProjWorld )
{
	vec3 vt;

	MMatrix matinverCameraInver;
	MMatrix mat;

	matinverCameraInver.MakeIdentity();
	CameraView->GetInverse(&matinverCameraInver);

	vt.x =  -( ( ( 2.0f * ScreenPt.x ) / rtClient.GetWidth() ) - 1 ) / ProjWorld->_11;
	vt.y =  ( ( ( 2.0f * ScreenPt.y ) / rtClient.GetHeight() ) - 1 ) / ProjWorld->_22;
	vt.z =  1.0f;//카메라 좌표계에서  값 
	//Cuboid 안에서의 방향 벡터 	
	//vT.z(Cuboid 안 항상 1) == zF(월드) 	

	//스크린 상에서의 선택한 정점의  월드 공간에서의 값  
	// 맵에 향한 방향 벡터터
	vec3 vPickRayDir;
	vPickRayDir.x  = vt.x*matinverCameraInver._11 + vt.y*matinverCameraInver._21 + vt.z*matinverCameraInver._31;
	vPickRayDir.y  = vt.x*matinverCameraInver._12 + vt.y*matinverCameraInver._22 + vt.z*matinverCameraInver._32;
	vPickRayDir.z  = vt.x*matinverCameraInver._13 + vt.y*matinverCameraInver._23 + vt.z*matinverCameraInver._33;

	*pOutDir = vPickRayDir;

	//Cuboid 안에서의 방향 벡터 	

	// 카메라의 위치
	//카메라 원점의  월드 공간에서의 값
	vec3 vPickRayOrig;
	vPickRayOrig.x = matinverCameraInver._41;
	vPickRayOrig.y = matinverCameraInver._42;
	vPickRayOrig.z = matinverCameraInver._43;

	*pOutOrig = vPickRayOrig;
}

bool TPick::AABBPicking( RBoundingBox& box, MPoint pt, MRect rect, RMatrix& matWorld, RMatrix& matCamera, RMatrix& matProject, float& fOutDist )
{
	TPick pickTest;

	vec3 vPickRayDir = 0;
	vec3 vPickRayOrig = 0;

	pickTest._2DTo3DCoord(&vPickRayDir, &vPickRayOrig, pt, rect, &matCamera, &matProject);

	vector<vec3> vecVertices;
	pickTest.GetAABBVertexData(box, vecVertices, matWorld);
	
	const static int IndexArray[] =
	{
		0, 1, 2, 1, 3, 2, 
		1, 5, 3, 5, 7, 3, 
		5, 4, 7, 4, 6, 7, 
		4, 0, 6, 0, 2, 6,
		4, 5, 0, 5, 1, 0,
		2, 3, 6, 3, 7, 6
	};

	for(int i = 0; i < 12; i++)
	{
		int nStartIndex = i * 3;

		float fu = 0.0f;
		float fv = 0.0f;

		if(pickTest.IntersectTriangle(vPickRayOrig, vPickRayDir, vecVertices[IndexArray[nStartIndex]], vecVertices[IndexArray[nStartIndex + 1]], vecVertices[IndexArray[nStartIndex + 2]], &fOutDist, &fu, &fv))
		{
			return true;
		}
	}

	return false;
}

void TPick::GetAABBVertexData( RBoundingBox& box, vector<vec3>& vecVertices, RMatrix& matWorld )
{
	const static RVector baseVertices[] = 
	{
		RVector(-1, 1, 1),
		RVector( 1, 1, 1),
		RVector(-1,-1, 1),
		RVector( 1,-1, 1),
		RVector(-1, 1,-1),
		RVector( 1, 1,-1),
		RVector(-1,-1,-1),
		RVector( 1,-1,-1)
	}; // 8개 vertex

	for ( int i = 0; i<8; ++i )
	{
		RVector vec, pushVec;
		vec.x = ( baseVertices[i].x < 0 ? box.vmin.x : box.vmax.x );
		vec.y = ( baseVertices[i].y < 0 ? box.vmin.y : box.vmax.y );
		vec.z = ( baseVertices[i].z < 0 ? box.vmin.z : box.vmax.z );
		matWorld.TransformVect(vec, pushVec);
		vecVertices.push_back(pushVec);
	}
}