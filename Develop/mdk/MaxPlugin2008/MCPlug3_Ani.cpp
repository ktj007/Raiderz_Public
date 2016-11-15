#include "stdafx.h"

#include "MCplug3.h"

/////////////////////////////////////////////////////////////
// ani out

BOOL EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > _ALMOST_ZERO)	return FALSE;
	if (fabs(p1.y - p2.y) > _ALMOST_ZERO)	return FALSE;
	if (fabs(p1.z - p2.z) > _ALMOST_ZERO)	return FALSE;

	return TRUE;
}

BOOL MCplug2::IsKnownController(Control* cont)
{
	ulong partA, partB;

	if (!cont)	return FALSE;

	partA = cont->ClassID().PartA();
	partB = cont->ClassID().PartB();

	if (partB != 0x00)	return FALSE;

	switch (partA) 
	{
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 애니메이션
void MCplug2::ExportAnimation(INode* node, RMaxMeshNode* mesh_node) 
{
	BOOL bPosAnim;
	BOOL bRotAnim;
	BOOL bScaleAnim;
	BOOL bDoKeys = FALSE;

	// dummy_loc 가 있고, bip01 이 있으면 bip01 을 고정하고 dummy_loc를 옮겨준다.
	bool bNeedToMove = 
		m_pBip01Node && (m_eDummyLocType == DUMMY_LOC || m_eDummyLocType == DUMMY_LOC_DIR) &&
		(node==m_pBip01Node || node==m_pDummyLocNode );

	// 키 애니메이션
	BOOL bExistAnimation = CheckForAnimation(node, bPosAnim, bRotAnim, bScaleAnim);
	if (bExistAnimation || bNeedToMove)
	{
		// 애니메이션이 없어도 bip01 과 dummy_loc 는 샘플링을 해야한다
		if(bNeedToMove)
		{
			bPosAnim = TRUE;
			bRotAnim = TRUE;
		}

		DumpSRTSample( node, mesh_node, bPosAnim, bRotAnim, bScaleAnim );
	}
	DumpVisSample(node, mesh_node);

	// 버텍스 애니메이션
	if(m_options.m_bVertexAnimationOut) {
		DumpVertexAni(node, mesh_node);
	}

	// 전체 애니메이션 프레임의 최대 AABB를 구한다. // 애니메이션 출력이 필요할때만 수집을 한다.
	if(m_options.m_bKeyAnimationOut || m_options.m_bVertexAnimationOut) {
		DumpMaxAniAABB(node, mesh_node);
	}
}

// 원점 기준으로 하나의 animation 파일안에 AABB 크기 전체의 합
// AABB * Bip01 (Z축 이동과 회전값을 제외한 Bip01 Transform, 나머지 TM은 Dummy에 기록된다.)
// Dummy와 Biped는 위치값으로 계산한다.
void MCplug2::DumpMaxAniAABB(INode* node, RMaxMeshNode* mesh_node)
{
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end	= ip->GetAnimRange().End();
	TimeValue t;

	int delta = GetTicksPerFrame() * GetKeyFrameStep();

	Matrix3 tm(1);
	tm.IdentityMatrix();
	mesh_node->m_MaxAniBoundingBox.Initialize();

	//////////////////////////////////////////////////////

	BOOL	needDel = FALSE;;
	int		i;
	Point3	v;

	string nodeName = node->GetName();

	Matrix3 tmBip01(1);	// 0 프레임의 Bip 01 위치 값
	Point3 vTransBip01(0,0,0);

	//if( m_pBip01Node )
	//{
	//	tmBip01 = m_pBip01Node->GetNodeTM(start);
	//	vTransBip01 = tmBip01.GetTrans();
	//}

	for ( t = start; t <= end; t += delta ) 
	{
		Matrix3 tmInvBip01(1);
		Matrix3 tmRemainBip01(1);
		Point3 vInvTransBip01(0, 0, 0);
		if( (m_eDummyLocType == DUMMY_LOC || m_eDummyLocType == DUMMY_LOC_DIR) && ( m_pBip01Node ) )
		{
			tmInvBip01 = m_pBip01Node->GetObjTMAfterWSM(t);
			vInvTransBip01 = tmInvBip01.GetTrans();
			vInvTransBip01 -= vTransBip01;	// start 프레임째의 Bip01 위치값을 제외시킨다.

			// 그 이후 정제된 Bip01 TM을 적용 시켜준다.
			Matrix3 tmDummyRefined;
			Matrix3 tmDummyRemains;
			if(m_eDummyLocType == DUMMY_LOC_DIR)
				GetRefinedTransform_loc_dir( m_pBip01Node, t, tmDummyRefined, tmDummyRemains );
			else
				GetRefinedTransform_loc( m_pBip01Node, t, tmDummyRefined, tmDummyRemains );

			// Remain 된 bip01 TM을 적용 시킨다.
			tmRemainBip01 = tmDummyRemains;// * Inverse( node->GetParentTM(t) );
		}

		tm = node->GetObjTMAfterWSM(t);

		PolyObject *poly = NULL;
		Object *obj = node->EvalWorldState(t).obj;
		if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0)))
		{
			poly = (PolyObject*)obj->ConvertToType(t, Class_ID(POLYOBJ_CLASS_ID, 0));

			if (obj != poly) 
				needDel = TRUE;
		}

		if(poly)
		{
			int point_num = poly->mm.VNum();

			for ( i = 0; i < point_num; i++ ) 
			{
				Point3 v = poly->mm.P(i) * tm;
				v -= vInvTransBip01;	// Bip01 의 이동값을 제외 시킨다.
				v = v * tmRemainBip01;
				mesh_node->m_MaxAniBoundingBox.Add( MVector3(v.x, v.y, v.z) );
			}

			if (needDel)
			{
				delete poly;
			}
		}
		else if( m_pDummyLocNode != node )	// Dummy_loc 의 경우 Bip01 에 영향을 받지 않기 때문에 제외 시킨다.
		{
			// 가진 점이 없다면 위치값을 사용
			Point3 v = tm.GetTrans();
			v -= vInvTransBip01;
			v = v * tmRemainBip01;
			mesh_node->m_MaxAniBoundingBox.Add( MVector3(v.x, v.y, v.z) );
		}
	}
}

void MCplug2::DumpVertexAni(INode* node, RMaxMeshNode* mesh_node)
{
	if( mesh_node->m_PointCnt==0 )//가진점이 없다면...
		return;

	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end	= ip->GetAnimRange().End();
	TimeValue t;

	int delta = GetTicksPerFrame() * GetKeyFrameStep();

	Matrix3 tm(1);

	int cnt = 0;

	for (t=start; t<=end; t+=delta) {
		cnt++;
	}

	mesh_node->m_VertexAniCnt = cnt;
	mesh_node->m_VertexAniFrame = new DWORD[cnt];
	mesh_node->m_VertexAniTable = new D3DXVECTOR3*[cnt];

	//////////////////////////////////////////////////////

	BOOL needDel = FALSE;;
//	Mesh* mesh;
//	int vcnt;
	int frame_cnt = 0;
	int i;

	Point3 v;

	D3DXVECTOR3* pVert = NULL;
	TriObject* tri = NULL;

	for (t=start; t<=end; t+=delta) {

		tm = node->GetObjTMAfterWSM(t);

		PolyObject *poly = NULL;
		Object *obj = node->EvalWorldState(t).obj;
		if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) { 
			poly = (PolyObject *) obj->ConvertToType(t, 
				Class_ID(POLYOBJ_CLASS_ID, 0));

			if (obj != poly) needDel = TRUE;
		}
		if(!poly) return;

		int point_num = poly->mm.VNum();
		int face_num  = poly->mm.FNum();

		pVert = new D3DXVECTOR3[point_num];

		mesh_node->m_VertexAniTable[frame_cnt] = pVert;
		mesh_node->m_VertexAniFrame[frame_cnt] = t-m_nBeginFrame;

		for (i=0; i<point_num; i++) {

			Point3 v = poly->mm.P(i) * tm;

			pVert[i].x = v.x;
			pVert[i].y = v.y;
			pVert[i].z = v.z;
		}
		if (needDel) {
			delete poly;
		}

		frame_cnt++;
	}
}

BOOL MCplug2::CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale)
{
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3 firstPos;
	float rotAngle, firstRotAngle;
	Point3 rotAxis, firstRotAxis;
	Point3 firstScaleFactor;

	bPos = bRot = bScale = FALSE;

	for (t=start; t<=end; t+=delta) {

		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

		decomp_affine(tm, &ap);

		AngAxisFromQ(ap.q, &rotAngle, rotAxis);

		if (t != start) {

			if (!bPos) {

				if (!EqualPoint3(ap.t, firstPos)) 						bPos = TRUE;
			}

			if (!bRot) {
				if (fabs(rotAngle - firstRotAngle) > _ALMOST_ZERO) 		bRot = TRUE;
				else if (!EqualPoint3(rotAxis, firstRotAxis)) 			bRot = TRUE;
			}

			if (!bScale) {
				if (!EqualPoint3(ap.k, firstScaleFactor)) 				bScale = TRUE;
			}

		} else {

			firstPos = ap.t;
			firstRotAngle = rotAngle;
			firstRotAxis = rotAxis;
			firstScaleFactor = ap.k;
		}

		if (bPos && bRot && bScale)		break;
	}

	return bPos || bRot || bScale;
}

void MCplug2::GetRefinedTransform_loc_dir(INode* node, TimeValue t, Matrix3& tmDummyRefined, Matrix3& tmDummyRemains)
{
	//////////////////////////////////////////////////////////////////////////
	// node(bip01) 의 애니메이션 중
	// (높이를 제외한)이동 및 (z-up 정렬된) 회전 성분을 tmDummyRefined 로 
	// 나머지 성분을 tmDummyRemains (bip01 및 나머지 toplevel노드 ) 로 분리한다.

	// tmBip01   : Bip01 의 현재 tm
	// tmbip01_0 : Bip01 의 첫프레임 tm
	// tmDummy	: 첫프레임에 대한 ( WORLD 기준 ) 상대적인 애니메이션
	//
	// 일반적인 tm      : tmBip01 * WVP
	
	// 구하는 순서

	// 1. tmDummy 를 구한다
	// tmDummy			= Inv( tmBip01_0_r ) * tmBip01 * Inv(tmBip01_0_t)

	// 2. tmDummyRefined 를 구한다
	// tmDummy 로 부터 재처리 하여 tmDummyRefined 를 얻는다

	// 3. tmDummyRemains를 구한다
	// tmDummyRefined 쪽으로 빠진성분을 tmBip01 로 부터 제거한다


	// static frame 의 tm
	Matrix3 tmBip01_0 = node->GetNodeTM( GetStaticFrame() );

	// static frame 의 회전
	Matrix3 tmBip01_0_r = tmBip01_0;
	tmBip01_0_r.NoTrans();

	// static frame 의 translation
	Matrix3 tmBip01_0_t;
	tmBip01_0_t.IdentityMatrix();
	tmBip01_0_t.SetTrans( tmBip01_0.GetTrans() );

	Matrix3 tmBip01 = node->GetNodeTM( t );

	// 1. tmDummy 를 구한다
	Matrix3 tmDummy = Inverse( tmBip01_0_r ) * tmBip01 * Inverse(tmBip01_0_t);

	// 2. tmDummyRefined 를 구한다

	// tmDummy 에서 부터 z 방향/움직임 성분 제거하여 tmDummyRefined 를 생성

	// 위 아래 방향으로 끄덕거림 성분 없앤다
	Point3 vecDir = tmDummy.GetRow(1);		// rs3 에서 y 가 방향으로 쓰인다.
	vecDir.z = 0;
	vecDir = Normalize(vecDir);

	Point3 vecUp(0,0,1);
	Point3 vecRight = Normalize( CrossProd( vecDir, vecUp ) );
	vecUp = CrossProd(vecRight,vecDir);

	tmDummyRefined;
	tmDummyRefined.IdentityMatrix();
	tmDummyRefined.SetRow( 0, vecRight );
	tmDummyRefined.SetRow( 1, vecDir );
	tmDummyRefined.SetRow( 2, vecUp );

	// z이동 (점프) 성분 제거
	Point3 vecPos = tmDummy.GetTrans();
	vecPos.z = 0;
	tmDummyRefined.SetTrans( vecPos );
	//*/

	// 3. tmDummyRemains를 구한다

	// dummy 값으로 빠진 값이외의 성분들은 원래 tm 에 넣는다
	
	// tmDummyRefined 를 rotation 성분과 translation 성분으로 분해
	Matrix3 tmRefined_r = tmDummyRefined;
	tmRefined_r.NoTrans();

	Matrix3 tmRefined_t;
	tmRefined_t.IdentityMatrix();
	tmRefined_t.SetTrans( tmDummyRefined.GetTrans() );

	Matrix3 tmBip01_t;
	tmBip01_t.IdentityMatrix();
	tmBip01_t.SetTrans( tmBip01.GetTrans() );

	tmDummyRemains = tmBip01 * Inverse(tmBip01_t) * Inverse(tmRefined_r) * tmBip01_t * Inverse( tmRefined_t ) ;
	//                         -----------------------------------------------------   -----------------------
	//                            회전에서 빠진성분 (이동성분빼고 회전후 다시이동)        이동에서 빠진성분
}

void MCplug2::GetRefinedTransform_loc(INode* node, TimeValue t, Matrix3& tmDummyRefined, Matrix3& tmDummyRemains)
{
	//////////////////////////////////////////////////////////////////////////
	// node(bip01) 의 애니메이션 중 이동값을 dummy_loc 로 옮겨준다

	// static frame 의 tm
	Matrix3 tmBip01_0 = node->GetNodeTM( GetStaticFrame() );

	// static frame 의 회전
	Matrix3 tmBip01_0_r = tmBip01_0;
	tmBip01_0_r.NoTrans();

	// static frame 의 translation
	Matrix3 tmBip01_0_t;
	tmBip01_0_t.IdentityMatrix();
	tmBip01_0_t.SetTrans( tmBip01_0.GetTrans() );

	Matrix3 tmBip01 = node->GetNodeTM( t );

	// 1. tmDummy 를 구한다
	Matrix3 tmDummy = Inverse( tmBip01_0_r ) * tmBip01 * Inverse(tmBip01_0_t);

	// 2. tmDummyRefined 를 구한다

	tmDummyRefined;
	tmDummyRefined.IdentityMatrix();

	// z이동 (점프) 성분 제거
	Point3 vecPos = tmDummy.GetTrans();
	vecPos.z = 0;
	tmDummyRefined.SetTrans( vecPos );
	//*/

	// 3. tmDummyRemains를 구한다

	// dummy 값으로 빠진 값이외의 성분들은 원래 tm 에 넣는다

	tmDummyRemains = tmBip01 * Inverse( tmDummyRefined ) ;
}

void MCplug2::DumpSRTSample(INode* node, RMaxMeshNode* pMeshNode, BOOL pos, BOOL rot, BOOL scale)
{
	// dummy_loc_dir 이름을 가진노드이면 강제로 이름을 dummy_loc 로 맞춰준다
	if( node==m_pDummyLocNode && (m_eDummyLocType != DUMMY_LOC_NONE) )
		pMeshNode->m_Name = "dummy_loc";

	pMeshNode->m_isHavePosAni = (pos!=FALSE);
	pMeshNode->m_isHaveRotAni = (rot!=FALSE);

	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end	= ip->GetAnimRange().End();
	TimeValue t;

	int delta = GetTicksPerFrame() * GetKeyFrameStep() ;

	for (t=start; t<=end; ) 
	{
		Matrix3 tm,ptm;
		ptm = node->GetParentTM(t);
		tm = node->GetNodeTM(t);
		tm = tm * Inverse(ptm);
		if(TMNegParity( tm ))	// mirror 되어있는경우 mesh 를 뒤집어놨다.
		{
			static Matrix3 Negative = ScaleMatrix(Point3 ( -1.0f, -1.0f, -1.0f ));
			tm = Negative * tm; 
		}

		if( (m_eDummyLocType == DUMMY_LOC || m_eDummyLocType == DUMMY_LOC_DIR) &&
			( node==m_pBip01Node || node==m_pDummyLocNode ) )
		{

			Matrix3 tmDummyRefined;
			Matrix3 tmDummyRemains;
			if(m_eDummyLocType == DUMMY_LOC_DIR)
				GetRefinedTransform_loc_dir( m_pBip01Node, t, tmDummyRefined, tmDummyRemains );
			else
				GetRefinedTransform_loc( m_pBip01Node, t, tmDummyRefined, tmDummyRemains );

			// bip01 에 대해
			if( node== m_pBip01Node )
				tm = tmDummyRemains * Inverse( node->GetParentTM(t) );
			else
			{
				// dummy_loc에 대해서...
				_ASSERT( m_pDummyLocNode == node );
				tm = node->GetNodeTM(t) * tmDummyRefined * Inverse( node->GetParentTM(t) );
			}
		}

		pMeshNode->AddKeys(tm, t-m_nBeginFrame, pos, rot, scale );

		// 샘플링 간격을 다르게 설정한 구간 처리
		if(m_bCustomSampling && m_nCustomSamplingStart<=t && t<m_nCustomSamplingEnd)
			t+= m_nCustomSamplingInterval;
		else
			t+=delta;
	}
}

void MCplug2::DumpVisSample(INode* node, RMaxMeshNode* mesh_node)
{
	Control* cont = node->GetVisController();

//	RVisKey* pVis = NULL;

//	float fVis = node->GetVisibility();
	//시작과 끝에 하나씩 꼭 넣어준다
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end	= ip->GetAnimRange().End();

	if(cont) {

		int i;
		IKeyControl *ikc = NULL;
		ikc = GetKeyControlInterface(cont);

		if(ikc)	{

			//시작
			MCVisKey viskey;
			viskey.frame = start - m_nBeginFrame;
			viskey.v = node->GetVisibility(start);
			// 3DMax에서 내부적으로 Visibility값이 0보다 작으면 Off로 취급하기 때문에 값이 0보다 작을 수 있다.
			if( viskey.v < 0.0f )
				viskey.v = 0.0f;

			mesh_node->m_visKey.push_back(viskey);

			int numKey = ikc->GetNumKeys();

			for (i=0; i<numKey; i++) {
				if(cont->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0)) {
					ITCBFloatKey key;
					ikc->GetKey(i, &key);
//					pVis->frame = key.time / GetTicksPerFrame();
					viskey.frame = key.time - m_nBeginFrame;
					viskey.v		= key.val;
				}
				else if(cont->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0))
				{
					IBezFloatKey key;
					ikc->GetKey(i, &key);
					viskey.frame = key.time - m_nBeginFrame;
					viskey.v		= key.val;
				}
				else if(cont->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) {
					ILinFloatKey key;
					ikc->GetKey(i, &key);

					// 소울헌트 익스포트 약속
					// 컨트롤러 타입이 Linear Float이면 보간되지 않도록 키를 작게 넣는다.
					if( numKey > 1 && i > 0 && i < numKey )
					{
						ILinFloatKey prevKey;
						ikc->GetKey(i-1, &prevKey);

							// 이전 키가 시작 프레임이면 스킵
						if( prevKey.time != m_nBeginFrame &&
							// 키값이 같으면 스킵
							key.val != prevKey.val )
						{
							// 프레임 간격을 1차이만큼만 나도록 아주 작게
							viskey.frame = prevKey.time - m_nBeginFrame + 1;
							viskey.v = key.val;
						}
						else
						{
							viskey.frame = key.time - m_nBeginFrame;
							viskey.v = key.val;
						}
					}
					else
					{
						viskey.frame = key.time - m_nBeginFrame;
						viskey.v = key.val;
					}
				}

				// Visibility키값은 0 ~ 1 범위를 벗어나면 안된다.
				if( viskey.v < 0.0f )
				{
					viskey.v = 0.0f;
					mlog3( "%s %d프레임의 Visilibity키값이 0보다 작습니다. ", mesh_node->m_Name.c_str(), viskey.frame );
				}
				else
				if( viskey.v > 1.0f )
				{
					viskey.v = 1.0f;
					mlog3( "%s %d프레임의 Visilibity키값이 1보다 큽니다. ", mesh_node->m_Name.c_str(), viskey.frame );
				}

				mesh_node->m_visKey.push_back(viskey);
			}
			//끝
			viskey.frame = end - m_nBeginFrame;
			viskey.v = node->GetVisibility(end);
			// 3DMax에서 내부적으로 Visibility값이 0보다 작으면 Off로 취급하기 때문에 값이 0보다 작을 수 있다.
			if( viskey.v < 0.0f )
				viskey.v = 0.0f;

			mesh_node->m_visKey.push_back(viskey);
		}
	}
}
