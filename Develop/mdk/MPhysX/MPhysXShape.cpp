#include "stdafx.h"

#include "MPhysXShape.h"
#include "MPhysX.h"

#include "MPhysXStream.h"
#include "NxCooking.h"

#include "mmsystem.h"

namespace physx {

unsigned int MPhysXShape::m_nUnNamedShapeCnt = 0;

bool MPhysXShape::CreateSphereCCD( const MSphere &sphere, const int nRings /*= 20*/, const int nSegments /*= 20*/ )
{
	if (NULL != m_pCCDVB || NULL != m_pCCDIB)
		return true;

	NxSphereShapeDesc* pSphereShapeDesc = dynamic_cast<NxSphereShapeDesc*>(m_pShapeDesc);

	_ASSERT(m_eShapeType == PSHT_GENERAL);
	_ASSERT(pSphereShapeDesc);
	_ASSERT(m_pCCDVB == NULL);
	_ASSERT(m_pCCDIB == NULL);

	MMatrix mat(MMatrix::IDENTITY);
	mat.SetTranslationMatrix(sphere.center);

	NxVec3* pVertex = NULL;
	NxU32*	pIndex	= NULL;

	int		nCntVertex	= 0;
	int		nCntIndex	= 0;

	// allocate the vertex buffer
	nCntVertex = (nRings + 1) * (nSegments+1);
	pVertex = new NxVec3[nCntVertex];
	m_pCCDVB = pVertex;

	// allocate index buffer
	nCntIndex = 6 * nRings * (nSegments + 1);
	pIndex = new NxU32[nCntIndex];
	m_pCCDIB = pIndex;

	float fDeltaRingAngle	= (MMath::PI / nRings);
	float fDeltaSegAngle	= (2 * MMath::PI / nSegments);
	unsigned short wVerticeIndex = 0;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) 
	{
		float r0 = sphere.radius * sinf (ring * fDeltaRingAngle);
		float y0 = sphere.radius * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) 
		{
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			pVertex++;

			//Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
			//*pVertex++ = vNormal.x;
			//*pVertex++ = vNormal.y;
			//*pVertex++ = vNormal.z;

			//*pVertex++ = (float) seg / (float) nSegments;
			//*pVertex++ = (float) ring / (float) nRings;

			if (ring != nRings) 
			{
				// each vertex (except the last) has six indices pointing to it
				*pIndex++ = wVerticeIndex + nSegments + 1;
				*pIndex++ = wVerticeIndex;               
				*pIndex++ = wVerticeIndex + nSegments;
				*pIndex++ = wVerticeIndex + nSegments + 1;
				*pIndex++ = wVerticeIndex + 1;
				*pIndex++ = wVerticeIndex;
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring

	NxSimpleTriangleMesh stm;
	stm.numVertices			= nCntVertex;
	stm.numTriangles		= nCntIndex;
	stm.pointStrideBytes	= sizeof(NxVec3);
	stm.triangleStrideBytes = sizeof(NxU32) * 3;

	stm.points		= pVertex;
	stm.triangles	= pIndex;
	stm.flags		|= NX_MF_FLIPNORMALS;

	pSphereShapeDesc->ccdSkeleton = m_pPhysX->m_pSDK->createCCDSkeleton(stm);
	if (!pSphereShapeDesc->ccdSkeleton)
		return false;

	return true;
}

bool MPhysXShape::CreateStaticMeshShape( std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, bool bHeightField )
{
	if (NULL != m_pShapeDesc)
		return true;

	_ASSERT(m_eShapeType == PSHT_STATIC_MESH);

	//mlog("%u px write buffer start\n", timeGetTime());

	// mesh desc
	NxTriangleMeshDesc triMeshDesc;

	triMeshDesc.numVertices				= (NxU32)_refVertices.size();
	triMeshDesc.numTriangles			= (NxU32)_refFaces.size() / 3;
	triMeshDesc.pointStrideBytes		= sizeof(NxVec3);
	triMeshDesc.triangleStrideBytes		= 3 * sizeof(NxU32);
	triMeshDesc.points					= &_refVertices[0];
	triMeshDesc.triangles				= &_refFaces[0];
	triMeshDesc.flags					= NX_MF_HARDWARE_MESH;

	// Height field option
	if(bHeightField)
	{
		triMeshDesc.heightFieldVerticalAxis		= NX_Z;
		triMeshDesc.heightFieldVerticalExtent	= -1000.f;
	}

	// mesh shape desc
	NxTriangleMeshShapeDesc * pTriangleMeshShapeDes = new NxTriangleMeshShapeDesc;

	MPhysXMemoryWriteBuffer wb;
	assert(triMeshDesc.isValid());

	if ( m_pPhysX->m_pCooking->NxCookTriangleMesh(triMeshDesc, wb) == false)
	{
		delete pTriangleMeshShapeDes;
		mlog("Triangle Mesh Cook Failed!!\n");
		return false;
	}

	//mlog("%u px write buffer end\n", timeGetTime());

	MPhysXMemoryReadBuffer rb(wb.data);
	pTriangleMeshShapeDes->meshData = m_pPhysX->m_pSDK->createTriangleMesh(rb);

	m_pShapeDesc = pTriangleMeshShapeDes;
	return true;
}

bool MPhysXShape::CreateConvexShape( std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces )
{
	if (NULL != m_pShapeDesc)
		return true;

	_ASSERT(m_eShapeType == PSHT_CONVEX);

	// convex desc
	NxConvexMeshDesc convexMeshDesc;
	convexMeshDesc.numVertices			= (NxU32)_refVertices.size();
	convexMeshDesc.numTriangles			= (NxU32)_refFaces.size() / 3;
	convexMeshDesc.pointStrideBytes		= sizeof(NxVec3);
	convexMeshDesc.triangleStrideBytes	= 3 * sizeof(NxU32);
	convexMeshDesc.points				= &_refVertices[0];
	convexMeshDesc.triangles			= &_refFaces[0];							
	convexMeshDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// convex shape desc
	NxConvexShapeDesc* pConvexShapeDesc = new NxConvexShapeDesc;

	MPhysXMemoryWriteBuffer wb;
	assert(convexMeshDesc.isValid());

	if ( m_pPhysX->m_pCooking->NxCookConvexMesh(convexMeshDesc, wb) == false )
	{
		delete pConvexShapeDesc;
		mlog("Convex Mesh Cook Failed!!\n");
		return false;
	}

	MPhysXMemoryReadBuffer rb(wb.data);
	pConvexShapeDesc->meshData = m_pPhysX->m_pSDK->createConvexMesh(rb);

	m_pShapeDesc = pConvexShapeDesc;
	return true;
}

MPhysXShape::~MPhysXShape()
{
	_ASSERT(m_nRefCnt == 0);

	// CCD buffer
	SAFE_DELETE(m_pCCDVB);
	SAFE_DELETE(m_pCCDIB);

	if(m_eShapeType == PSHT_CONVEX)
	{
		NxConvexShapeDesc* pDesc = (NxConvexShapeDesc*)m_pShapeDesc;
		if(pDesc && pDesc->meshData)
			m_pPhysX->m_pSDK->releaseConvexMesh(*pDesc->meshData);
	}
	else if(m_eShapeType == PSHT_STATIC_MESH)
	{
		NxTriangleMeshShapeDesc* pDesc = (NxTriangleMeshShapeDesc*)m_pShapeDesc;
		if(pDesc && pDesc->meshData)
			m_pPhysX->m_pSDK->releaseTriangleMesh(*pDesc->meshData);
	}

	delete m_pShapeDesc;
}

} // namespace physx
