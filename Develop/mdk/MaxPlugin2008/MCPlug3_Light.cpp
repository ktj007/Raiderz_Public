#include "stdafx.h"
#include "MCplug3.h"
#include "LightList.h"

void MCplug2::ExportLight(INode* node)
{
	// 무시하는 노드인지 체크
	bool bIgnore = false;	// 이전 포멧
	GetUserPropertyString( node, "ignore_at_export", &bIgnore );
	if( bIgnore == false )	// 새 포멧
		GetUserPropertyString( node, "set_export_ignore", &bIgnore );
	if( bIgnore == true)	// 둘다 실패면 출력하지 않는다
		return;

	// 항상 출력하되 static 이면 staticmesh 의 자식으로 아니면 actor 의 자식으로 등록
	ExportLightObject(node);

// 	if (m_options.m_bStaticMesh)
// 	{
// 		bool bStatic = false;
// 		GetUserPropertyString( node, "set_static", &bStatic );
// 		if(bStatic)	// Static Mesh 출력일때 Static 속성값을 가지는 메쉬만 출력한다.
// 		{					
// 			ExportLightObject(node);
// 		}
// 	}
// 	else	// 아니면 아예 출력하지 않는다.
// 	{
// 		bool bStatic = false;
// 		GetUserPropertyString( node, "set_static", &bStatic );
// 
// 		if(!bStatic) // elu는 static이 아니고
// 		{
// 			// XRef도 아닌 Mesh만 출력한다
// 			Object *obj = node->GetObjectRef();
// 			if ( obj->ClassID() != XREFOBJ_CLASS_ID ) 
// 			{
// 				ExportLightObject(node);
// 			}
// 		}
// 	}
}

void MCplug2::ExportLightObject(INode* node)
{
	TimeValue t = GetStaticFrame();

	ObjectState os = node->EvalWorldState(t);
	if (!os.obj) return;

	GenLight* light = (GenLight*)os.obj;
	struct LightState ls;
	Interval valid = FOREVER;
	Interval animRange = ip->GetAnimRange();

	light->EvalLightState(t, valid, &ls);

	if(ls.type==OMNI_LIGHT || ls.type==DIR_LIGHT || ls.type==FSPOT_LIGHT || ls.type==TSPOT_LIGHT)
	{
		MCLIGHT *plight=new MCLIGHT;
		Matrix3 pivot = node->GetNodeTM(GetStaticFrame());
		AffineParts ap;
		decomp_affine(pivot, &ap);

		Matrix3 matParentNodeMT(true);
		INode* parent = node->GetParentNode();
		if (parent && !parent->IsRootNode()) {
			plight->ParentName = FixupName( parent->GetName() );

			matParentNodeMT = parent->GetNodeTM(GetStaticFrame());
			matParentNodeMT.Invert();
		}

		plight->Name=FixupName( node->GetName() );
		plight->Color=RVector(ls.color.r,ls.color.g,ls.color.b);

		Point3 vPositon = ap.t * matParentNodeMT;
		plight->Position=RVector(vPositon.x, vPositon.y, vPositon.z);

		RMatrix matTM;
		ConvertToMMatrix(matTM, pivot * matParentNodeMT);

		// -z 방향이 direction, y 방향이 up
		plight->Direction = -RVector(matTM._31, matTM._32, matTM._33);
		plight->Up = RVector(matTM._21, matTM._22, matTM._23);

		plight->fIntensity=ls.intens;
		plight->fAttnStart=ls.attenStart;
		plight->fAttnEnd=ls.attenEnd;
		plight->lightType = ls.type;

		plight->dwFlags=0;
		if(light->GetShadow()!=FALSE)
			plight->dwFlags|=RL_FLAG_CASTSHADOW;

		bool bStatic = false;
		GetUserPropertyString( node, "set_static", &bStatic );
		plight->bStatic = bStatic;

		plight->fFalloff = ls.fallsize;

		plight->CalcName2Type(node);

		m_lightList.push_back(plight);
	}
}