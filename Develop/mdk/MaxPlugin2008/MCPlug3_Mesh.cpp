#include "stdafx.h"

#include "MCplug3.h"
#include "PHYEXP.H"

#include "modstack.h"
#include "iparamm2.h"
#include "iskin.h"
#include "meshadj.h"

#include "RMaxMaterial.h"
#include "RMaterial.h"
#include "MDebug.h"

#include "IGame.h"

//////////////////////////////////////////////////////////////
// object export

Modifier* FindPhysiqueModifier (INode* nodePtr)
{
	Object* ObjectPtr = nodePtr->GetObjectRef();

	if (!ObjectPtr) return NULL;

	if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID) {

		IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

		int ModStackIndex = 0;

		while (ModStackIndex < DerivedObjectPtr->NumModifiers()) {

			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)) {

				return ModifierPtr;
			}

			ModStackIndex++;
		}
	}

	return NULL;
}

Modifier* FindSkinModifier(INode* nodePtr)
{
	Object* ObjectPtr = nodePtr->GetObjectRef();

	if (!ObjectPtr) return NULL;

	while (ObjectPtr && ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);

		int ModStackIndex = 0;

		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == SKIN_CLASSID)
			{
				return ModifierPtr;
			}
			ModStackIndex++;
		}

		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}
	return NULL;
}

bool MCplug2::ExportSkin(INode* node,RMaxMeshNode* pMeshNode)
{
	Modifier *phyMod = FindSkinModifier(node);

    if (!phyMod) return false;    

	ISkin *pSkin = (ISkin*)phyMod->GetInterface(I_SKIN);

	if(!pSkin)	return false;

	ISkinContextData *pSkinMC = pSkin->GetContextInterface(node);

	int i, j;
	
	int numBone = pSkin->GetNumBones();

	//for (i = 0; i < numBone; i++) 
	//{ 
	//	INode *bone = pSkin->GetBone(i); 

	//	if (bone) {
	//		string Name = bone->GetName();
	//	}
	//} 

	int numVerts = pSkinMC->GetNumPoints();

	pMeshNode->m_PhysiqueCnt = numVerts;
	pMeshNode->m_PhysiqueTable = new RPhysiqueInfo [numVerts];
	memset(pMeshNode->m_PhysiqueTable,0,sizeof(RPhysiqueInfo)*numVerts);

	RPhysiqueInfo* pPData = NULL;

	int nVertBoneCnt = 0;

	string Name;
	float fWeight;

	for (i = 0; i<numVerts; i++) { 

		pPData = &pMeshNode->m_PhysiqueTable[i];

		nVertBoneCnt = pSkinMC->GetNumAssignedBones(i);

		pPData->Create(nVertBoneCnt);

		for(j = 0; j < nVertBoneCnt; j++) {

			int x = pSkinMC->GetAssignedBone(i, j);

			INode *Bone = pSkin->GetBone(x);			
			Name	= Bone->GetName();					// 디버그 용이하게 하기위해.

			fWeight = pSkinMC->GetBoneWeight(i, j);

			pPData->SetName(j,Name);
			pPData->SetWeight(j,fWeight);
		}
	}

	phyMod->ReleaseInterface(I_SKIN, pSkin);

	return true;
}


bool MCplug2::ExportPhysique( ObjectState *os, INode *node,RMaxMeshNode* pMeshNode)
{
    Modifier *phyMod = FindPhysiqueModifier(node);

    if (!phyMod) return false;    

    IPhysiqueExport *phyExport = (IPhysiqueExport *)phyMod->GetInterface(I_PHYINTERFACE);

	// Remove the non uniform scale

//	IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);
//	BipIface->RemoveNonUniformScale(1);

    if (phyExport) {

		IPhyContextExport *mcExport = (IPhyContextExport *)phyExport->GetContextInterface(node);

		if (mcExport) {

			mcExport->ConvertToRigid(TRUE);
			mcExport->AllowBlending(TRUE);		//<--

			int p_num = os->obj->NumPoints();

			pMeshNode->m_PhysiqueCnt = p_num;
			pMeshNode->m_PhysiqueTable = new RPhysiqueInfo [p_num];
			memset(pMeshNode->m_PhysiqueTable,0,sizeof(RPhysiqueInfo)*p_num);

			RPhysiqueInfo* pPData = NULL;

			phyMod->DisableMod();
			phyExport->SetInitialPose( true );
//			phyExport->GetInitNodeTM( this, MeshRefMatrix );

			for (int i = 0; i < p_num;  i++) {

				IPhyVertexExport *vtxExport = mcExport->GetVertexInterface(i);

				if (vtxExport) {

					pPData = &pMeshNode->m_PhysiqueTable[i];

					if (vtxExport->GetVertexType() & BLENDED_TYPE) {

						IPhyBlendedRigidVertex *vtxBlend = (IPhyBlendedRigidVertex *)vtxExport;
			
						int no = vtxBlend->GetNumberNodes();

						pPData->Create(no);

						Point3 BlendP(0.0f, 0.0f, 0.0f);

						for (int n = 0; n < no; n++) {

							INode *Bone		= vtxBlend->GetNode(n);
//							Point3 Offset	= vtxBlend->GetOffsetVector(n);
							float Weight	= vtxBlend->GetWeight(n);

//							pPData->SetOffset(n,D3DXVECTOR3(Offset.x,Offset.z,Offset.y));
							pPData->SetName(n,string(Bone->GetName()));
							pPData->SetWeight(n,Weight);
						}
		
					} else {

						IPhyRigidVertex *vtxNoBlend = (IPhyRigidVertex *)vtxExport;
						INode *Bone = vtxNoBlend->GetNode();
//						Point3 Offset = vtxNoBlend->GetOffsetVector();

						pMeshNode->m_PhysiqueTable[i].Create(1);

//						pPData->SetOffset(0,D3DXVECTOR3(Offset.x,Offset.z,Offset.y));
						pPData->SetName(0,string(Bone->GetName()));
						pPData->SetWeight(0,1.f);

					}

					mcExport->ReleaseVertexInterface(vtxExport);
					vtxExport = NULL;	

				}
			}

			phyExport->SetInitialPose( false );
			phyMod->EnableMod();

			phyExport->ReleaseContextInterface(mcExport);
		}
        phyMod->ReleaseInterface(I_PHYINTERFACE, phyExport);
    }

	return true;
}

// 더미와 일반 오브젝트 정보 출력.
void MCplug2::ExportGeometryObject(INode* node)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());

	if (!os.obj)	return;
	
	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))	return;

	string szNodeName = FixupName( node->GetName() );

	// ground_ 는 pathengine용이므로 export 대상에서 제외
	const char *prefixGround = "ground_";
	if (strstr(szNodeName.c_str(), prefixGround)) return;

	// physx_ 는 PhysX Scene Export용이므로 export 대상에서 제외
	const char *prefixPhysX = "physx_";
	if (strstr(szNodeName.c_str(), prefixPhysX)) return;

	// external_는 shape 따는 용도이므로 export 대상에서 제외
	const char *prefixExternal = "ex_";

	RMaxMeshNode* pMeshNode = new RMaxMeshNode;

	pMeshNode->m_Name = szNodeName;

	mlog("%s\n",pMeshNode->m_Name.c_str());

	pMeshNode->CalcName2Type(node);

	INode* parent = node->GetParentNode();

	if (parent && !parent->IsRootNode()) {
		pMeshNode->m_ParentName = FixupName( parent->GetName() );
	}

	DWORD iFlags = node->GetTMController()->GetInheritanceFlags();
	Matrix3 pivot = node->GetNodeTM(GetStaticFrame());

	if(TMNegParity(pivot))
		pivot = matNegative * pivot; 

	ConvertToD3DXMatrix(pMeshNode->m_matBase, pivot );

	// local matrix 구함
	Matrix3 matParent = node->GetParentTM( GetStaticFrame());
	if(TMNegParity(matParent))
		matParent = matNegative * matParent; 

	pivot = pivot	* Inverse( matParent );
	ConvertToD3DXMatrix(pMeshNode->m_matLocal, pivot );

	// base visiblity
	pMeshNode->m_fBaseVisibility = node->GetVisibility(GetStaticFrame());

	const char *pcNodeName = pMeshNode->m_Name.c_str();

	if ( strstr(szNodeName.c_str(), prefixExternal))
	{
		ExportMesh(node, GetStaticFrame(), pMeshNode);
		m_meshList.m_ObstacleMeshList.push_back(pMeshNode);
		return;
	}

	if( pMeshNode->CheckFlag(RM_FLAG_COLLISION_MESHONLY) ||			// Gunz2 에서 벽충돌용도로 사용되는 캡슐
		pMeshNode->CheckFlag(RM_FLAG_COLLISION_CLOTH_MESH) )	// 충돌용 메쉬이다
	{
		ExportCollision(node, pMeshNode);
		delete pMeshNode;
		return;
	}

	if( pMeshNode->CheckFlag(RM_FLAG_OCCLUDER) )
	{
		ExportOccluder(node, pMeshNode);
		delete pMeshNode;
		return;
	}

	// 무시하는 노드인지 체크
	bool bIgnore = false;
	GetUserPropertyString( node, "ignore_at_export", &bIgnore );

	if( bIgnore == false )
		GetUserPropertyString( node, "set_export_ignore", &bIgnore );

	if( bIgnore ) return;

	// 물 메시
	bool bWaterMesh = false;
	GetUserPropertyString( node, "set_water", &bWaterMesh );
	if( bWaterMesh )
	{
		m_waterList.push_back( node );
	}

	// static mesh 또는 elu mesh
	bool bExportPhysique = false;
	if( !pMeshNode->CheckFlag(RM_FLAG_DUMMY_MESH) &&
		!pMeshNode->CheckFlag(RM_FLAG_BONE_MESH) ) // 더미또는 본 노드는 내용을 채울 필요가 없다..
	{
		if(m_options.m_bStaticMesh)
		{
			bool bStatic = false;
			GetUserPropertyString( node, "set_static", &bStatic );

			// Static Mesh 출력일때 Static 속성값을 가지는 메쉬만 출력한다.
			if (bStatic)
				ExportStaticMesh( node, GetStaticFrame() );	
		}
		else
		{
			// 애니메이션만 익스포트하는 경우에도 AABB를 계산하기 위해 메시 익스포트 해야 한다.
			if( m_options.m_bMeshOut || m_options.m_bKeyAnimationOut || m_options.m_bVertexAnimationOut )
			{
				// XRef도 아닌 Mesh만 출력한다
				Object *obj = node->GetObjectRef();
				if ( obj->ClassID()!=XREFOBJ_CLASS_ID ) 
				{
					ExportMesh(node, GetStaticFrame(), pMeshNode);
					bExportPhysique = true;
				}
			}
		}
	}

	ExportEffect( node, pMeshNode );

	Mtl* mtl = node->GetMtl();
	// 더미및 헬퍼 오브젝트에 재질이 필요한가 ?
	pMeshNode->m_pMtrl = m_meshList.m_pMaterialKeeper->GetMaterial(mtl);

	// 특수효과 셰이더 컴포넌트 플래그 정보를 연결된 마테리얼에 전달.
	pMeshNode->m_pMtrl->GetRMaterial()->AddShaderComponent(pMeshNode->m_rscShaderComponentFlag);
	
	if(bExportPhysique)
	{
		if(ExportPhysique(&os,node,pMeshNode)==false)	// physique 가 없을경우 skin 도 시도해 본다.
			ExportSkin(node,pMeshNode);

		if(pMeshNode->m_PhysiqueCnt!=0 && (pMeshNode->m_PointCnt!=pMeshNode->m_PhysiqueCnt))
		{
			mlog3("%s 오브젝트의 (%d) 리깅된 피지크 버텍스 개수와 폴리오브젝트의 버텍스 개수(%d) 가 다릅니다. 문제가 되는 버텍스를 찾아서 수정해주세요", 
				pMeshNode->m_Name.c_str(), pMeshNode->m_PhysiqueCnt, pMeshNode->m_PointCnt );
		}
	}

	if( m_options.m_bKeyAnimationOut || m_options.m_bVertexAnimationOut )	
	{
		// 애니메이션 익스포트 때 physique 인지 참조하는부분이 있어서 제일 뒤에 한다
		ExportAnimation(node, pMeshNode);
	}

	m_meshList.Add(pMeshNode);
}

//////////////////////////////////////////////////////////////////////////////
// 한평면위의 convex 이면 true
bool MCplug2::CheckCoplanarAndConvex(MNMesh& mm, int iFace)	
{

#define MVECTOR(x) MVector3((float*)mm.P(x))

	// fnormal을 법선으로 하고 vertex 0을 지나는 평면의 방정식을 만든다
	Point3 center;
	Point3 fnormal;
	mm.ComputeNormal(iFace,fnormal,&center);

	MNFace *f=mm.F(iFace);
	MPlane plane(MVector3((float*)fnormal),MVector3((float*)mm.P(f->vtx[0])));

	// 폴리곤들이 모두 한 평면 위에 있고, 볼록 다각형이면 쪼갤 필요가없다
	bool bCoplanar=true;
	bool bConvex=true;

	int nSign=0;

	for(int j=0;j<f->deg;j++)
	{
		MVector3 normal;
		MVector3 edgea,edgeb;

#define COPLANAR_TOLER	1.f
		MVector3 testpoint = MVECTOR(f->vtx[j]);

		if(fabs(plane.GetDistanceTo(testpoint))>COPLANAR_TOLER)		// 동일 평면위에 있는지 확인
		{
			bCoplanar=false;
			break;
		}

		edgea=MVECTOR(f->vtx[j]);
		edgeb=MVECTOR(f->vtx[(j+1)%f->deg]);
		normal=CrossProduct(MVector3((float*)fnormal),edgeb-edgea);
		normal.Normalize();

		MPlane testplane(normal,edgea);

		// 각각의 edge 에 대해서 평면을 세우고 그 평면의 + 쪽에 다음 버텍스가 있는지 확인한다
#define CONVEX_TOLER	1.f

		double ftestv = testplane.GetDistanceTo(MVECTOR(f->vtx[(j+2)%f->deg]));
#define CONVEX_SIGN(x) ((x)<-CONVEX_TOLER ? -1 : (x)>CONVEX_TOLER ? 1 : 0)

		if(nSign==0)
			nSign=CONVEX_SIGN(ftestv);
		else
		{
			int sign=CONVEX_SIGN(ftestv);
			if(nSign!=sign && 0!=sign)
			{
				bConvex=false;
				break;
			}
		}
	}
	return bCoplanar && bConvex;
}

void MCplug2::ExportMesh(INode* node, TimeValue t, RMaxMeshNode* pMeshNode)
{
	// Normal Data 출력 시에 Ascii 방식으로는 Normal 이 제대로 나오지 않는 버그가 있어서
	// IGameMesh 로 변환 시켜서 Normal Data 를 얻게함
	IGameNode* pGameNode = m_pIGame->GetIGameNode(node);
	if(!pGameNode)
	{
		MessageBox(NULL, "GetIGameNode() 실패", "ExportMesh", MB_OK);
		return;
	}

	// Skin Mesh 인데 참조하는 본이 없으면 출력하지 않는다.
	Modifier *phyMod = FindSkinModifier(node);
	if (phyMod)
	{
		ISkin *pSkin = (ISkin*)phyMod->GetInterface(I_SKIN);
		if(pSkin)
		{
			int numBone = pSkin->GetNumBones();
			if( numBone <= 0 )
			{
				char szErrorMessage[MAX_PATH];
				sprintf(szErrorMessage, "<%s> 메쉬가 Skin Modify 이면서 참조하는 본이 0개 입니다.", pMeshNode->m_Name.c_str());
				MessageBox(NULL, szErrorMessage, "Mesh Export Failed!!", MB_OK);
				return;
			}
		}
	}

	//string strGameNodeName = pGameNode->GetName();
	IGameObject* obj = pGameNode->GetIGameObject();
	IGameMesh* gmesh = (IGameMesh*)obj;
	bool bGMeshInitializeSuccess = gmesh->InitializeData();
	// 느려서 일단 주석처리
	if(bGMeshInitializeSuccess)
		gmesh->SetCreateOptimizedNormalList();


	Mtl* nodeMtl	= node->GetMtl();
	Matrix3 tm		= node->GetObjTMAfterWSM(t);
	BOOL negScale	= TMNegParity(tm);
	Matrix3 pivot = tm;
	pivot.NoTrans();
	Matrix3 nodetm = node->GetNodeTM(0);
	Matrix3 invNodetm = Inverse(nodetm);
	Matrix3 matSign(1);
	if(negScale)
		matSign = matNegative * matSign;

	ObjectState os = node->EvalWorldState(t);
	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID)
		return; 

	BOOL needDel = FALSE;
	TriObject* tri = GetTriObjectFromNode(node, t, needDel);
	if (!tri)
		return;

	Mesh* mesh = &tri->GetMesh();

	mesh->buildNormals();

	int point_num = mesh->getNumVerts();
	int face_num  = mesh->getNumFaces();

	pMeshNode->m_PointCnt = point_num;
	pMeshNode->m_PointTable = new D3DXVECTOR3[point_num];
	memset( pMeshNode->m_PointTable,0,sizeof(D3DXVECTOR3)*point_num );

	if( mesh->vcFace )
	{
		pMeshNode->m_PointColorCnt = point_num;
		pMeshNode->m_PointColorTable = new D3DXVECTOR3[ point_num ];// 정점과 순서 보장
	}

	// vertex position
	for (int i=0; i<mesh->getNumVerts(); i++)
	{
		Point3 v = tm * mesh->verts[i] * invNodetm * matSign;
		//         -------------------
		//				world vertex
		// 최종적으로 world space modifier가 적용된 로컬 좌표가 된다

		pMeshNode->m_PointTable[i].x = v.x;
		pMeshNode->m_PointTable[i].y = v.y;
		pMeshNode->m_PointTable[i].z = v.z;
	}

	// material
	RMaxMaterial *pNodeMaterial = m_meshList.m_pMaterialKeeper->GetMaterial(nodeMtl);
	_ASSERT(NULL!=pNodeMaterial);

	for ( int i = 0; i < face_num; ++i ) 
	{
		FaceEx* face = bGMeshInitializeSuccess ? gmesh->GetFace(i) : NULL;
		Face* f = &mesh->faces[i];
		RMaxFaceData *pFaceData = new RMaxFaceData;

		pFaceData->nDeg = 3;
		pFaceData->index = new RFaceSubData[3];

		// material id
		RMaxMaterial *pMaterial = NULL;
		if(pNodeMaterial)
		{
			pMaterial = pNodeMaterial->GetMaterialByID( mesh->faces[i].getMatID() );
		}

		pFaceData->pMaterial		= pMaterial;
		pFaceData->nSubMaterialID	= mesh->faces[i].getMatID();

		pFaceData->triangleIndices = new unsigned char[3];
		pFaceData->triangleIndices[0] = 0;
		pFaceData->triangleIndices[1] = 1;
		pFaceData->triangleIndices[2] = 2;
		pMeshNode->m_FaceTable.push_back(pFaceData);

		for( int j = 0; j < 3; ++j )
		{
			int ncorner = negScale ? 3 - j -1 : j;	// 2 1 0
			// negative Mesh 일때 IGameMesh 는 삼각형 구축 순서가 다르다
			// 2 1 0 -> 1 2 0 으로 삼각형 구축 순서 변경
			int ncornernormal = negScale ? (j + 1) % 3 : j; // 1 2 0
			// Position
			pFaceData->index[j].p = (WORD)f->v[ncorner];

			// IGameMesh 를 이용해 Normal 을 출력한다.
			// Normal
			Point3 n;
			if( bGMeshInitializeSuccess )
			{
				n = gmesh->GetNormal(face, ncornernormal, true);
			}
			else
			{
				int vert = f->getVert(ncorner);
				n = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
			}
			Point3 vnSign = VectorTransform( matSign, n );
			pFaceData->index[j].n = pMeshNode->m_NormalTable.Add( (float*)vnSign );

			// uv //////////////////////////////////////////////////////////////////////////

			// DiffuseMap Channel
			pFaceData->index[j].uv  = 0;	// 초기화
			pFaceData->index[j].uv2 = 0;

			// 0: Vertex Color channel.
			// 1: Default mapping channel (the TVert array).
			// 2 through MAX_MESHMAPS-1: The new mapping channels available in release 3.0.
			if ( mesh->mapSupport(0) || mesh->mapSupport(1) ) 
			{
				DWORD	tvert	= mesh->tvFace[i].t[ncorner];
				UVVert	tv		= mesh->tVerts[tvert];
				RVector uv;
				uv.x = tv.x;
				uv.y = 1.f - tv.y;
				uv.z = 0;

				pFaceData->index[j].uv = pMeshNode->m_TexCoordTable.Add( uv );
			}

			if ( mesh->mapSupport(2) )
			{
				DWORD	tvert	= mesh->mapFaces(2)[i].t[ncorner];
				UVVert	tv		= mesh->mapVerts(2)[tvert];
				RVector uv;
				uv.x = tv.x;
				uv.y = 1.f - tv.y;
				uv.z = 0;

				pFaceData->index[j].uv2 = pMeshNode->m_TexCoordTableExtra.Add( uv );
			}

			// vcolor
			if(mesh->vcFace)
			{
				int VertexColorIndex = -1;
				VertexColorIndex = mesh->vcFace[i].t[j];

				pMeshNode->m_PointColorTable[pFaceData->index[j].p] = (float*)mesh->vertCol[ VertexColorIndex ];
			}			
		}
	}

	pMeshNode->m_FaceCnt  = pMeshNode->m_FaceTable.size();

	// vertex color
	if( mesh->vcFace )
	{
		// check 모두 1인 컬러값이면 출력 안한다..
		// 디자이너가 컬러값을 모두 지운다고 흰색으로 만들어버린 경우도 
		// max 에서는 지워지지 않아서 다시 작업해야 할수도 있다..
		D3DXVECTOR3* pColor = NULL;
		bool bCheck = true;
		for( int i = 0; i < point_num; ++i )
		{
			pColor = &pMeshNode->m_PointColorTable[i];
			if( (pColor->x != 1.f) || (pColor->y != 1.f) ||	(pColor->z != 1.f)) {
				bCheck = false;
				break;
			}
		}

		if(bCheck)	//모두 흰색인 경우는 출력할 필요없다..
		{
			pMeshNode->m_PointColorCnt = 0;
			delete [] pMeshNode->m_PointColorTable;
			pMeshNode->m_PointColorTable = NULL;
		}
	}

	pGameNode->ReleaseIGameObject();
}

TriObject* MCplug2::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if( obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)) )
	{
		TriObject *tri = (TriObject*)obj->ConvertToType( t, Class_ID(TRIOBJ_CLASS_ID, 0) );

		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri)
			deleteIt = TRUE;

		return tri;
	}
	else
	{
		return NULL;
	}
}

// mesh 의 폴리곤 set 을 하나의 폴리곤으로 만든다. 실패하면 null 을 리턴
rpolygon* MCplug2::MergePolygon( vector<rpolygon*>& triangles )
{
	vector<rvertex> vecVertices;

	RMaxMaterial*	pMaterial = NULL;
	int				nLightMapID = -1;
	DWORD			dwFlags = 0;

	int nCount = 0;
	for (size_t i=0; i<triangles.size(); i++) 
	{
		rpolygon* pPolygon = triangles[i];
		if(!pPolygon) return NULL;	// 이미 합쳐져서 더해진 폴리곤이다
		
		// 첫 폴리곤일경우
		if( nCount == 0 )
		{
			for(int j=0;j<3;j++)
				vecVertices.push_back( pPolygon->v[j] );

			pMaterial = pPolygon->pMaterial;
			nLightMapID = pPolygon->nLightMapID;
			dwFlags = pPolygon->dwFlags;
		}else
		{
			if(pMaterial!=pPolygon->pMaterial) return NULL;
			if(dwFlags!=pPolygon->dwFlags) return NULL;

			// 맞는 edge 를 찾는다
			int nEdge = -1;
			int nVertexToAdd = -1;	// 공유하는 버텍스가 아닌 나머지 하나의 버텍스
			for(size_t j=0; j< vecVertices.size(); j++)
			{
				// 만드는 폴리곤의 edge = ea, eb
				rvertex* ea = &vecVertices[j];
				rvertex* eb = &vecVertices[(j+1) % vecVertices.size() ];

				for( int k=0; k<3; k++)
				{
					// 추가할 삼각형의 edge = ec, ed
					rvertex* ec = &pPolygon->v[k];
					rvertex* ed = &pPolygon->v[(k+1) % 3 ];

					// 맞는 edge 를 찾아본다
					if(*ea == *ed && *eb == *ec)
					{
						// convex가 되는지 검사

						// edge를 이루는 두 버텍스아닌 남은 하나의 vertex
						rvertex* remain = &pPolygon->v[(k+2) % 3 ];

						// 만들고 있는 polygon 에서 edge 의 이웃 edge 를 지난 평면에 대해 테스트

						// edge-a쪽 이전 edge 를 지나고 폴리곤에 수직으로 교차하는 평면에 대해 테스트 
						MVector3 ea1 = vecVertices[(j-1 + vecVertices.size() ) % vecVertices.size() ].coord;
						MVector3 normal = CrossProduct(ea->coord - ea1, eb->coord - ea->coord );
						normal.Normalize();	// 폴리곤의 normal

						MVector3 planeNormal = CrossProduct( normal, ea->coord - ea1 );
						planeNormal.Normalize();

						MPlane testplaneA(planeNormal,ea->coord);

						// edge-b 쪽 다음 edge 를 지나고 폴리곤에 수직으로 교차하는 평면 대해 테스트
						MVector3 eb1 = vecVertices[(j+2) % vecVertices.size() ].coord;

						MVector3 planeBNormal = CrossProduct( normal, eb1 - eb->coord );
						planeBNormal.Normalize();

						MPlane testplaneB(planeBNormal,eb->coord);

						float fDistTestPlaneA = testplaneA.GetDistanceTo(remain->coord);
						float fDistTestPlaneB = testplaneB.GetDistanceTo(remain->coord);
						// convex 로 구성할수 있다
						if( CONVEX_SIGN(fDistTestPlaneA) == CONVEX_SIGN(fDistTestPlaneB) )
						{
							nEdge = j+1;	// 끼워 넣을 위치, 맨앞에 넣으나 맨 뒤에 넣으나 같아서 %n 필요없다
							nVertexToAdd = (k+2)%3;
							break;
						}
					}
				}
				if(nEdge!=-1)
					break;
			}

			// 맞는 edge 가 없으면 포기
			if(nEdge==-1) return NULL;

			vecVertices.insert( vecVertices.begin() + nEdge, pPolygon->v[nVertexToAdd] );
		}
		nCount++;
	}

	rpolygon* pMerged = new rpolygon;
	pMerged->nCount = vecVertices.size();
	pMerged->v = new rvertex[pMerged->nCount];
	pMerged->dwFlags = dwFlags;
	pMerged->pMaterial = pMaterial;
	pMerged->nLightMapID = nLightMapID;
	memcpy( &pMerged->v[0], &vecVertices[0], sizeof(rvertex)*pMerged->nCount );

	return pMerged;
}

void MCplug2::ExportStaticMesh(INode* node, TimeValue t)
{
	ObjectState os = node->EvalWorldState(t);

	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return; 
	}

	BOOL needDel = FALSE;
	Object *obj = node->EvalWorldState(t).obj;

	TriObject* tri = GetTriObjectFromNode(node, t, needDel);
	if (!tri) {
		return;
	}

	Mtl* nodeMtl	= node->GetMtl();
	Matrix3 tm		= node->GetObjTMAfterWSM(t);
	BOOL negScale	= TMNegParity(tm);

	Matrix3 pivot = tm;
	pivot.NoTrans();

	//Matrix3 nodetm = node->GetNodeTM(0);
	//Matrix3 invNodetm = Inverse(nodetm);

	RMaxMaterial *pNodeMaterial = m_meshList.m_pMaterialKeeper->GetMaterial(nodeMtl);
	_ASSERT(NULL!=pNodeMaterial);

	// lightmap id
// 	string strLightMapName;
// 	GetUserPropertyString( node, "lightmap", &strLightMapName );
//	int nLightMapID = m_meshList.m_pMaterialKeeper->FindLightMap(node);
	int nLightMapID = 0;

	Mesh* mesh = &tri->GetMesh();
	mesh->buildNormals();

	//////////////////////////////////////////////////////////////////////////
	// 정점 타입에 따라 RGeometry를 생성한다
	// 기본 포멧이면		: RFVF_XYZ
	// 정점 컬러를 사용하면 : RFVF_DIFFUSE
	//////////////////////////////////////////////////////////////////////////
	bool bVertexColor = false;
	GetUserPropertyString( node, "set_vertexcolor", &bVertexColor );
	bool bShade = true;
	GetUserPropertyString( node, "set_shade", &bShade );
	if( bShade == false )
		bVertexColor = true;

	//////////////////////////////////////////////////////////////////////////
	// 폴리곤 FLAG 조합
	// RM_FLAG_COLLISION_MESH : 충돌 정보로 cl2 출력할 폴리곤인가
	//////////////////////////////////////////////////////////////////////////
	DWORD dwPolygonFlag = 0;
	bool bCollision = true;
	GetUserPropertyString( node, "set_collision", &bCollision );
	if( bCollision )
		dwPolygonFlag |= RM_FLAG_COLLISION_MESH;
	
	// 충돌 전용 메쉬 (Box, Sphere, Capsule 만 가능하다)
	bool bCollisionOnly = false;
	GetUserPropertyString( node, "set_collisiononly", &bCollisionOnly );
	if( bCollision )
		dwPolygonFlag |= RM_FLAG_COLLISION_MESHONLY;


	//////////////////////////////////////////////////////////////////////////
	// Visible
	// InVisible 이면 FVF 는 0 이다. FVF가 0이면 렌더링 하지 않는다.
	//////////////////////////////////////////////////////////////////////////
	bool bVisible = true;
	GetUserPropertyString( node, "set_visible", &bVisible );

	float fVisibility = 1.0f;
	fVisibility = node->GetVisibility(t);
	
	DWORD idFVF = 0;

	if( bVisible )
	{
		idFVF |= RFVF[RFVF_XYZ].fvf;
		idFVF |= RFVF[RFVF_NORMAL].fvf;
		idFVF |= RFVF[RFVF_TEX2].fvf;
		if( bVertexColor || (fVisibility < 1.0f) )
			idFVF |= RFVF[RFVF_DIFFUSE].fvf;
	}

	RGeometry* pCurrentGeometry = m_pCurrentCell->GetGeometry(idFVF);
	
	vector<rpolygon*> vecTriangles;
	for (int i=0; i<mesh->getNumFaces(); i++) 
	{
		// material id
		RMaxMaterial *pPolygonMaterial = NULL;
		if(pNodeMaterial)
		{
			pPolygonMaterial = pNodeMaterial->GetMaterialByID( mesh->faces[i].getMatID() );
		}

		rpolygon *rf= new rpolygon;
		rf->v		= new rvertex[3];
		rf->nCount	= 3;
		rf->dwFlags = dwPolygonFlag;
		rf->pMaterial = pPolygonMaterial;
		rf->nLightMapID = nLightMapID;
		rf->bColor	= false;

		Face* f = &mesh->faces[i];

		for(int j=0;j<3;j++)
		{
			int ncorner = negScale ? 3 - j -1 : j;

			Point3 coord = tm * mesh->verts[ f->v[ncorner] ];
			rf->v[j].coord	= MVector3( (float*)coord );

			DWORD vert = f->getVert(ncorner);
			Point3 vn = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
			Point3 p		= pivot * vn;
			rf->v[j].normal=MVector3((float*)p);

			//0: Vertex Color channel.
			//1: Default mapping channel (the TVert array).
			//2 through MAX_MESHMAPS-1: The new mapping channels available in release 3.0
			if (mesh->mapSupport(0) || mesh->mapSupport(1)) {
				DWORD tvert = mesh->tvFace[i].t[ncorner];
				UVVert tv = mesh->tVerts[tvert];
				rf->v[j].u = tv.x;
				rf->v[j].v = 1.f - tv.y;
			}

			// mesh->vertCol가 사용하지 않는 VColor를 포함하는 경우가 있다. 이를 방지하기 위해 vcFace에서 인덱스를 얻는다.
			int VertexColorIndex = -1;
			if(mesh->vcFace)
				VertexColorIndex = mesh->vcFace[i].t[j];

			if( fVisibility < 1.0f )	// 최우선 조건 // Visibility 가 있으면 정점 컬러로 넣어준다.
				rf->v[j].color = MVector3(fVisibility, fVisibility, fVisibility);
			else if( mesh->numCVerts && ( VertexColorIndex >= 0 ) )
				rf->v[j].color = MVector3( (float*)mesh->vertCol[ VertexColorIndex ] );//f->v[ncorner] ] );
			else
				rf->v[j].color = MVector3(1.0f, 1.0f, 1.0f);

			// 3번 맵채널
			if (mesh->mapSupport(3)) {
				DWORD tvert3 = mesh->mapFaces(3)[i].t[ncorner];
				UVVert tv3 = mesh->mapVerts(3)[tvert3];
				rf->v[j].u2 = tv3.x;
				rf->v[j].v2 = 1.f - tv3.y;
			}

		}
		vecTriangles.push_back(rf);
	}

	//////////////////////////////////////////////////////////////////////////
	// 충돌용 메쉬 생성
	for(size_t i=0;i<vecTriangles.size();i++)
	{
		rpolygon *rf= vecTriangles[i];
		DWORD dwMaterialID = m_collisionTreeBuilder.GetZoneMaterialID(rf->pMaterial->GetRMaterial()->GetName());
		m_collisionTreeBuilder.AddPolygon(rf->v[0].coord, rf->v[1].coord, rf->v[2].coord, 0, 0, dwMaterialID);
	}

	// polygon 으로 만들어보고 더한다
	BitArray bitAdded;
	bitAdded.SetSize ( mesh->getNumFaces() );
	bitAdded.ClearAll();
	BitArray bitCurrentPolygon;
	bitCurrentPolygon.SetSize ( mesh->getNumFaces() );

	MeshTempData* pTemp = new MeshTempData;
	pTemp->SetMesh (mesh);
	AdjFaceList *af = pTemp->AdjFList ();

	for (int i=0; i<mesh->getNumFaces(); i++) 
	{
		if(bitAdded[i]) continue;

		bitCurrentPolygon.ClearAll();

		// max 에서 기본으로 polygon 선택하는 옵션
		bool ignoreVisEdge = false;
		mesh->PolyFromFace (i, bitCurrentPolygon, DegToRad( 45 ) , ignoreVisEdge, af);

		vector<rpolygon*> vecCurrentPolygon;
		for (int j=0; j<mesh->getNumFaces(); j++) 
		{
			if(!bitCurrentPolygon[j]) continue;
			vecCurrentPolygon.push_back( vecTriangles[j] );
		}

		rpolygon *rf = MergePolygon( vecCurrentPolygon );
		if(rf)
		{
			// 폴리곤으로 만들기 성공, 삼각형들은 지우고
			for (size_t j=0; j<vecCurrentPolygon.size(); j++) 
			{
				if(!bitCurrentPolygon[j]) continue;
				delete vecTriangles[j];
				vecTriangles[j] = NULL;
			}
			// 더해진 삼각형을 체크한다
			bitAdded |= bitCurrentPolygon;
		}
		else	// 실패하면 triangle을 직접 더한다
			rf = vecTriangles[i];

		// 현재 Geometry 에 폴리곤을 더한다
		pCurrentGeometry->m_sourcePolygons.push_back(rf);
	}

	return;
}

Point3 MCplug2::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;
	
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) {
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}

RCell* MCplug2::MakeCell(INode* node)
{
	TimeValue t = GetStaticFrame();

	int i;
	Matrix3 tm		= node->GetObjTMAfterWSM(t);
	BOOL negScale	= TMNegParity(tm);

	ObjectState os = node->EvalWorldState(t);

	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return NULL; 
	}

	BOOL needDel = FALSE;
	Object *obj = node->EvalWorldState(t).obj;

	PolyObject *poly = NULL;
	if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) { 
		poly = (PolyObject *) obj->ConvertToType(t, 
			Class_ID(POLYOBJ_CLASS_ID, 0));

		if (obj != poly) needDel = TRUE;
	}
	if(!poly) return NULL;

	Matrix3 normalTM;
	normalTM = tm;
	normalTM.NoTrans();

	RCell* pCell = new RCell;
	pCell->m_strName = node->GetName();
	// polygons
	for (i=0; i<poly->mm.FNum(); i++) 
	{

		// 같은평면에 있고 convex 이면 하나로 취급, 그렇지 않으면 삼각형으로 쪼갠다
		if(!CheckCoplanarAndConvex(poly->mm,i))
		{
			mlog3( "cell 을 이루는 폴리곤 중, 한 평면위에 있지 않거나 볼록다각형이 아닌 경우가 있습니다\n" );
			return false;
		}

		// world 기준의 평면을 구한다

		Point3 center;
		Point3 fnormal;
		poly->mm.ComputeNormal(i,fnormal,&center);
		fnormal = normalTM * fnormal;

		MNFace *f = poly->mm.F(i);
		Point3 aPoint = tm * poly->mm.P(f->vtx[0]);

		MPlane plane(MVector3((float*)fnormal),MVector3((float*)aPoint));
		pCell->m_planes.push_back( plane );

		// face vertices
		RCellFace* pCellFace = new RCellFace;
		pCellFace->nVerts = f->deg;
		pCellFace->pVerts = new RVector[f->deg];
		for (int j=0; j< f->deg; j++) 
		{
			Point3 facePoint = tm * poly->mm.P(f->vtx[j]);
			pCellFace->pVerts[j].Set( facePoint.x, facePoint.y, facePoint.z );
		}
		pCell->m_cellFaces.push_back( pCellFace );
	}

	return pCell;
}

// property string 을 찾는다, 
// ex) target = test1 이 프로퍼티에 있을경우 GetUserPro..( node, "target" ) 는 pOutString으로 "test1" 을 넣어준다, 찾으면 true 없으면 false를 리턴
bool MCplug2::GetUserPropertyString(INode* node, const char* szPropertyName, string* pOutString)
{
	TSTR userPropBuffer;
	node->GetUserPropBuffer(userPropBuffer);

	istringstream iStringBuffer(userPropBuffer.data());
	string aLine;
	while(getline(iStringBuffer,aLine))
	{
		// trim right
		int n = aLine.find_last_not_of(" \n\r");
		if(n==string::npos) continue;
		string buf = aLine.substr(0,n+1);

		stringstream sstream(buf); // string을 stream에 넣는다
		sstream >> aLine;

		if( (strncmp(aLine.c_str(),szPropertyName,aLine.length())==0) 
			&& (strncmp(aLine.c_str(),szPropertyName,strlen(szPropertyName))==0) )
		{
			if(pOutString)
			{
				string strTargetName = buf.substr(buf.find_first_of("=")+1);
				int n = strTargetName.find_first_not_of(" \t\v\n");	// trim left
				if(n!=string::npos) strTargetName = strTargetName.substr(n);
				*pOutString = strTargetName;
			}
			return true;
		}
	}

	return false;
}

bool MCplug2::GetUserPropertyString(INode* node, const char* szPropertyName, bool* pBool)
{
	string strValue;
	if(!GetUserPropertyString(node,szPropertyName,&strValue))
		return false;

	if(pBool)
	{ 
		if(_stricmp( strValue.c_str(), "true" ) == 0 )
			*pBool = true;
		else
			*pBool = false;
	}
	return true;
}

RPortal* MCplug2::MakePortal(INode* node)
{
	TimeValue t = GetStaticFrame();

	Matrix3 tm		= node->GetObjTMAfterWSM(t);
	BOOL negScale	= TMNegParity(tm);

	ObjectState os = node->EvalWorldState(t);

	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return NULL; 
	}

	BOOL needDel = FALSE;
	Object *obj = node->EvalWorldState(t).obj;

	PolyObject *poly = NULL;
	if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) { 
		poly = (PolyObject *) obj->ConvertToType(t, 
			Class_ID(POLYOBJ_CLASS_ID, 0));

		if (obj != poly) needDel = TRUE;
	}
	if(!poly) return NULL;

	if(poly->mm.FNum()!=1 || !CheckCoplanarAndConvex(poly->mm,0))
	{
		mlog3("%s : portal 을 이루는 폴리곤은 단하나의 볼록다각형이어야 합니다\n", node->GetName());
		return NULL;
	}

	const char* TARGET = "target";
	string strTargetName;
	GetUserPropertyString( node, TARGET, &strTargetName);

	if( m_pCurrentCell->m_strName == strTargetName )
	{
		mlog3("%s : portal의 부모 셀과 연결하려는 셀이 같아서 익스포트에서 제외되었습니다.\n", node->GetName());
		return NULL;
	}

	RPortal* pPortal = new RPortal;
	pPortal->m_strName = node->GetName();

	MNFace *f = poly->mm.F(0);
	pPortal->nVertices = f->deg;
	pPortal->m_pVertices = new Point3[f->deg];

	for(int j=0; j< f->deg; j++)
	{
		pPortal->m_pVertices[j] = tm * poly->mm.P(f->vtx[j]);
	}
	pPortal->m_strParentCellName = m_pCurrentCell->m_strName;
	pPortal->m_strTargetCellName = strTargetName;

	return pPortal;
}

void MCplug2::ExportEffect( INode* node, RMaxMeshNode* pMeshNode)
{
	const char *ADD_EFFECT = "add_effect";
	const char *SET_ADD_EFFECT = "set_addeffect";
	const char *SET_EFFECT_NAME = "set_effect_name";
	const char *SET_EFFECT_SCALE = "set_effect_scale";
	const char *SET_STATIC = "set_static";

	float fEffectScale = 1.0f;

	bool bStatic = false;
	GetUserPropertyString( node, SET_STATIC, &bStatic );

	bool bAddEffect = false;

	TSTR userPropBuffer;
	node->GetUserPropBuffer(userPropBuffer);

	istringstream iStringBuffer(userPropBuffer.data());
	string aLine;
	string aValue;
	while(getline(iStringBuffer,aLine))
	{
		// trim right
		int n = aLine.find_last_not_of(" \n\r");
		if(n==string::npos) continue;
		string buf = aLine.substr(0,n+1);

		stringstream::pos_type spos;
		stringstream sstream(buf); // string을 stream에 넣는다
		sstream >> aLine;

		if(strncmp(aLine.c_str(),ADD_EFFECT,strlen(ADD_EFFECT))==0) 
		{
			string strEffectName = buf.substr(buf.find_first_of("=")+1);
			int n = strEffectName.find_first_not_of(" \t\v\n");	// trim left
			if(n!=string::npos) strEffectName = strEffectName.substr(n);
			m_effectList.Add( pMeshNode, node->GetName(),strEffectName, bStatic, fEffectScale);
		}
		else if(strncmp(aLine.c_str(),SET_ADD_EFFECT,strlen(SET_ADD_EFFECT))==0) 
		{
			sstream >> aLine;	// =

			// set_addeffect = true // 새 룰
			if(strcmp(aLine.c_str(), "=") == 0)	
			{
				sstream >> aLine;
				if(strcmp(aLine.c_str(), "true") == 0)
					bAddEffect = true;
			}
		}
		else if(strncmp(aLine.c_str(),SET_EFFECT_SCALE,strlen(SET_EFFECT_SCALE))==0) 
		{
			sstream >> aLine;	// =

			// set_effect_scale = 1.0 // 새 룰
			if(strcmp(aLine.c_str(), "=") == 0)	
			{
				sstream >> fEffectScale;
			}
		}
		else if(strncmp(aLine.c_str(),SET_EFFECT_NAME,strlen(SET_EFFECT_NAME))==0) 
		{
			if( bAddEffect == true )
			{
				string strEffectName = buf.substr(buf.find_first_of("=")+1);
				int n = strEffectName.find_first_not_of(" \t\v\n");	// trim left
				if(n!=string::npos) strEffectName = strEffectName.substr(n);
				m_effectList.Add(pMeshNode, FixupName( node->GetName() ),strEffectName, bStatic, fEffectScale);

				bAddEffect = false;
			}
		}
	}
}