#include "stdafx.h"
#include "MCplug3.h"
#include "MCollision.h"
#include "RMaterial.h"

void CopyMatirx(MMatrix& dest, Matrix3& src)
{
	dest._11 = src.GetRow(0).x;
	dest._12 = src.GetRow(0).y;
	dest._13 = src.GetRow(0).z;
	dest._14 = 0;

	dest._21 = src.GetRow(1).x;
	dest._22 = src.GetRow(1).y;
	dest._23 = src.GetRow(1).z;
	dest._24 = 0;

	dest._31 = src.GetRow(2).x;
	dest._32 = src.GetRow(2).y;
	dest._33 = src.GetRow(2).z;
	dest._34 = 0;

	dest._41 = src.GetRow(3).x;
	dest._42 = src.GetRow(3).y;
	dest._43 = src.GetRow(3).z;
	dest._44 = 1;
}

bool MCplug2::ExportCollision(INode* node, RMaxMeshNode* mesh_node)
{
	TimeValue t = GetStaticFrame();
	Mtl* nodeMtl	= node->GetMtl();
	Matrix3 tm		= node->GetObjTMAfterWSM(t);
	BOOL negScale	= TMNegParity(tm);

	WORD index = 0;

	Matrix3 matParent = node->GetParentTM( GetStaticFrame());
	if(TMNegParity(matParent))
		matParent *= matNegative;

	tm = tm	* Inverse( matParent );

	ObjectState os = node->EvalWorldState(t);

	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return false ; 
	}

	/*
	if(os.obj->ClassID()==Class_ID(CYLINDER_CLASS_ID,0))
	{

		IParamArray *iCylParams = os.obj->GetParamBlock();
		assert(iCylParams);

		// Get the value of radius, height
		int rad = os.obj->GetParamBlockIndex(CYLINDER_RADIUS);
		assert(rad>=0);

		Interval interv;
		float fRadius;
		iCylParams->GetValue(rad,0,fRadius,interv);

		int height = os.obj->GetParamBlockIndex(CYLINDER_HEIGHT);
		assert(height>=0);

		float fHeight;
		iCylParams->GetValue(height,0,fHeight,interv);

		MCollision *pCollision = new MCollision(MCollision::CYLINDER);
		pCollision->m_fRadius = fRadius;
		pCollision->m_fHeight = fHeight;
		CopyMatirx(pCollision->m_matLocal,tm);
		pCollision->m_strParentName = mesh_node->m_ParentName;
		m_mesh_list.m_collision_list.push_back(pCollision);

	}else
	if(os.obj->ClassID()==Class_ID(SPHERE_CLASS_ID,0))
	{
		MCollision *pCollision = new MCollision(MCollision::SPHERE);

		IParamArray *iParams = os.obj->GetParamBlock();
		assert(iParams);

		Interval interv;
		int		iParam;
		iParam = os.obj->GetParamBlockIndex(SPHERE_RADIUS);
		assert(iParam>=0);
		iParams->GetValue(iParam,0,pCollision->m_fRadius,interv);

		pCollision->m_strParentName = mesh_node->m_ParentName;
		CopyMatirx(pCollision->m_matLocal,tm);
		m_mesh_list.m_collision_list.push_back(pCollision);

	}else
	*/

	// User Property
	GetUserPropertyString( node, "set_cloth_related_name", &mesh_node->m_strRelatedNodeNames );

	// 디버그로 알아낸 값, 이건뭐 sdk문서에도 없어서 디버깅으로 찾았다 -_-;
#define CAPSULE_CLASSID			Class_ID(0x6d3d77ac,0x79c939a9)

	const int PARAM_CAPSULE_RADIUS	= 0;
	const int PARAM_CAPSULE_HEIGHT	= 1;

	if(os.obj->ClassID()==CAPSULE_CLASSID)
	{
		IParamArray *iCylParams = os.obj->GetParamBlock();
		assert(iCylParams);

		// Get the value of radius, height
		int rad = os.obj->GetParamBlockIndex(PARAM_CAPSULE_RADIUS);	// 실린더랑 파라미터 순서는 같은듯
		assert(rad>=0);

		Interval interv;
		float fRadius;
		iCylParams->GetValue(rad,0,fRadius,interv);

		int height = os.obj->GetParamBlockIndex(PARAM_CAPSULE_HEIGHT);
		assert(height>=0);

		float fHeight;
		iCylParams->GetValue(height,0,fHeight,interv);

		MCollision *pCollision = new MCollision(MCollision::CAPSULE);
		pCollision->m_fRadius = fRadius;
		pCollision->m_fHeight = fHeight;
		CopyMatirx(pCollision->m_matLocal,tm);
		pCollision->m_strName = mesh_node->m_Name;
		pCollision->m_strParentName = mesh_node->m_ParentName;
		pCollision->m_strRelatedNodeNames = mesh_node->m_strRelatedNodeNames;

		if( mesh_node->CheckFlag(RM_FLAG_COLLISION_MESHONLY) )
			pCollision->m_meshType = MCollision::MESHTYPE_NORMAL;
		else if( mesh_node->CheckFlag(RM_FLAG_COLLISION_CLOTH_MESH) )
			pCollision->m_meshType = MCollision::MESHTYPE_CLOTH;

		m_meshList.m_collisionList.push_back(pCollision);

	}
	else if(os.obj->ClassID()==Class_ID(SPHERE_CLASS_ID,0))
	{
		MCollision *pCollision = new MCollision(MCollision::SPHERE);

		IParamArray *iParams = os.obj->GetParamBlock();
		assert(iParams);

		Interval interv;
		int		iParam;
		iParam = os.obj->GetParamBlockIndex(SPHERE_RADIUS);
		assert(iParam>=0);
		iParams->GetValue(iParam,0,pCollision->m_fRadius,interv);

		pCollision->m_strName = mesh_node->m_Name;
		pCollision->m_strParentName = mesh_node->m_ParentName;
		pCollision->m_strRelatedNodeNames = mesh_node->m_strRelatedNodeNames;

		CopyMatirx(pCollision->m_matLocal,tm);
		if( mesh_node->CheckFlag(RM_FLAG_COLLISION_MESHONLY) )
			pCollision->m_meshType = MCollision::MESHTYPE_NORMAL;
		else if( mesh_node->CheckFlag(RM_FLAG_COLLISION_CLOTH_MESH) )
			pCollision->m_meshType = MCollision::MESHTYPE_CLOTH;

		m_meshList.m_collisionList.push_back(pCollision);

	}
	else if(os.obj->ClassID()==Class_ID(BOXOBJ_CLASS_ID,0))
	{
		MCollision *pCollision = new MCollision(MCollision::BOX);

		IParamArray *iParams = os.obj->GetParamBlock();
		assert(iParams);

		Interval interv;
		int		iParam;

		iParam = os.obj->GetParamBlockIndex(BOXOBJ_WIDTH);
		assert(iParam>=0);
		iParams->GetValue(iParam,0,pCollision->m_fRadius,interv);

		iParam = os.obj->GetParamBlockIndex(BOXOBJ_LENGTH);
		assert(iParam>=0);
		iParams->GetValue(iParam,0,pCollision->m_fLength,interv);

		iParam = os.obj->GetParamBlockIndex(BOXOBJ_HEIGHT);
		assert(iParam>=0);
		iParams->GetValue(iParam,0,pCollision->m_fHeight,interv);

		pCollision->m_strName = mesh_node->m_Name;
		pCollision->m_strParentName = mesh_node->m_ParentName;
		pCollision->m_strRelatedNodeNames = mesh_node->m_strRelatedNodeNames;

		CopyMatirx(pCollision->m_matLocal,tm);

		if( mesh_node->CheckFlag(RM_FLAG_COLLISION_MESHONLY) )
			pCollision->m_meshType = MCollision::MESHTYPE_NORMAL;
		else if( mesh_node->CheckFlag(RM_FLAG_COLLISION_CLOTH_MESH) )
			pCollision->m_meshType = MCollision::MESHTYPE_CLOTH;

		m_meshList.m_collisionList.push_back(pCollision);
	}
	else
	{
		return false;
	}

	return true;
}