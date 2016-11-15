#include "stdafx.h"

#include "MMath.h"

#include "max.h"
#include "stdmat.h"

#include "RMaxMaterial.h"
#include "RMaterial.h"
#include "RToken.h"

#include "MFileSystem.h"
#include "ConvertTexture.h"
#include "RTTGroup/IRTTGroup.h"

#include "MCPlug3.h"

RMaxMaterial::RMaxMaterial(void) 
: m_bUsed(false)
, m_nIndex(-1)
, m_nSubMtrlID(-1)
, m_pMtl(NULL)
, m_uvTransform(new Matrix3)
, m_bScaleAni(false)
, m_bRotateAni(false)
, m_bOffsetAni(false)
, m_nUsingUVChannelNum(0)
{
	m_pRMaterial = new RMaterial;
	m_uvTransform->IdentityMatrix();
}

RMaxMaterial::~RMaxMaterial(void)
{
	delete m_uvTransform;
	delete m_pRMaterial;
}

RMaxMaterial* RMaxMaterial::GetMaterialByID(int nID)
{
	if(m_children.empty()) {
		m_pBaseMaterial->m_bUsed = true;
		return m_pBaseMaterial; // 실제 참조할 Base Material
	}

	// top level material 이면
	if(m_pParent==NULL) {
		RMaxMaterial *pChild = m_children[nID % m_children.size()];
		return pChild ? pChild->GetMaterialByID(0) : NULL;
	}

	return m_children[0]->GetMaterialByID(0);
}

RMaxMaterialKeeper::RMaxMaterialKeeper()
{
	// material 이 적용되어있지 않은 폴리곤을 위해 default material을 준비한다
	RMaxMaterial *pDefaultMaterial = new RMaxMaterial;
	pDefaultMaterial->m_pBaseMaterial = pDefaultMaterial;	// 자기 자신
	m_materials.push_back(pDefaultMaterial);

	m_timeStart = m_timeEnd = m_timeDelta = 0;
}

RMaxMaterialKeeper::~RMaxMaterialKeeper()
{
	for(size_t i=0;i<m_materials.size();i++)
	{
		delete m_materials[i];
	}
	m_materials.clear();

	for(size_t i=0;i<m_clearedMaterials.size();i++)
	{
		delete m_clearedMaterials[i];
	}
	m_clearedMaterials.clear();
}

// ID 는 Texture ID
// #define ID_DI 1   //!< Diffuse
// #define ID_BU 8   //!< Bump 
bool RMaxMaterialKeeper::CompareTexture(Mtl *pMtl1, Mtl *pMtl2, int nID)
{
	Texmap *pTexture1 = pMtl1->GetSubTexmap(nID);
	Texmap *pTexture2 = pMtl2->GetSubTexmap(nID);

	// 1. 둘다 없으면 같다.
	if( pTexture1 == NULL && pTexture2 == NULL )	
		return true;

	// 2. 하나가 없으면 다르다.
	if( (pTexture2 && NULL == pTexture1) || (pTexture1 && NULL == pTexture2 )  )
		return false;

	// 3. 클래스 아이디가 다르면 다르다.
	if( pTexture1->ClassID() != pTexture2->ClassID() )
		return false;

	// 4. 실제 텍스쳐를 찾아준다. ( NormalBump Slot을 쓰는 경우에는 SubTexture가 실제 텍스쳐 데이터이다. )
	Texmap* pCurrentTexture1 = NULL;
	Texmap* pCurrentTexture2 = NULL;

	if ( (pTexture1->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) && (pTexture2->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) )
	{
		pCurrentTexture1 = pTexture1;
		pCurrentTexture2 = pTexture2;
	}
	else if(nID == ID_BU)
	{
		// id 0 -> normal , 1 -> additional bump
		pCurrentTexture1 = pTexture1->GetSubTexmap(0);
		pCurrentTexture2 = pTexture2->GetSubTexmap(0);
	}
	
	if( pCurrentTexture1 && pCurrentTexture2 )
	{
		Bitmap *pBitmap1 = ((BitmapTex *)pCurrentTexture1)->GetBitmap(0);
		Bitmap *pBitmap2 = ((BitmapTex *)pCurrentTexture2)->GetBitmap(0);

		if( (pBitmap2 && NULL == pBitmap1) || (pBitmap1 && NULL == pBitmap2 )  )
			return false;

		if( pBitmap1 || pBitmap2 )
		{
			if( stricmp( pBitmap1->Storage()->bi.Name(), pBitmap2->Storage()->bi.Name() ) != 0 )
				return false;
		}
		else // 없으면
		{
			if( stricmp( ((BitmapTex *)pCurrentTexture1)->GetMapName(), ((BitmapTex *)pCurrentTexture2)->GetMapName() ) != 0 )
				return false;
		}
	}

	return true;
}

bool RMaxMaterialKeeper::Compare(RMaxMaterial *pMaxMtl1, RMaxMaterial *pMaxMtl2)
{
	Mtl *pMtl1 = pMaxMtl1->m_pMtl;
	Mtl *pMtl2 = pMaxMtl2->m_pMtl;

 	TSTR szMtrl1 = pMtl1->GetName();
 	TSTR szMtrl2 = pMtl2->GetName();
// 	szMtrl1.toLower();
// 	szMtrl2.toLower();

	//if( strcmp( szMtrl1, szMtrl2 ) == 0 )
	//	bMtrlName = true;

	if( stricmp( pMtl1->GetName(), pMtl2->GetName() ) != 0 )
		return false;

	if( pMaxMtl1->m_nSubMtrlID != pMaxMtl2->m_nSubMtrlID )
		return false;

	if( pMtl1->GetAmbient() != pMtl2->GetAmbient() )
		return false;
	if( pMtl1->GetDiffuse() != pMtl2->GetDiffuse() )
		return false;
	if( pMtl1->GetSpecular() != pMtl2->GetSpecular() )
		return false;
	if( pMtl1->GetShininess() != pMtl2->GetShininess() )
		return false;
	if( pMtl1->GetShinStr() != pMtl2->GetShinStr() )
		return false;

	if( ( pMtl1->ClassID() == Class_ID(DMTL_CLASS_ID, 0) ) && ( pMtl2->ClassID() == Class_ID(DMTL_CLASS_ID, 0) ) )
	{
		StdMat* pStd1 = (StdMat*)pMtl1;
		StdMat* pStd2 = (StdMat*)pMtl2;
		if( pStd1->GetTwoSided() != pStd2->GetTwoSided() )
			return false;
	}
	
	if( pMtl1->NumSubMtls() != pMtl2->NumSubMtls() )
		return false;
	if( pMtl1->NumSubTexmaps() != pMtl2->NumSubTexmaps() )
		return false;

	// 텍스쳐 비교 
	for(int i = 0; i <= ID_DP; ++i)
	{
		if( CompareTexture(pMtl1, pMtl2, i) == false )
			return false;
	}
	
	return true;
}

RMaxMaterial* RMaxMaterialKeeper::GetExistMaterial(RMaxMaterial *pMaxMaterial)
{
	_ASSERT( pMaxMaterial );

	// 적용된 material 이 없으면 default material;
	if(NULL== pMaxMaterial->m_pMtl) return m_materials[0];

	for(int i=0;i<(int)m_materials.size();i++)
	{
		if( m_materials[i]->m_pMtl ) 
		{
			if( Compare(m_materials[i], pMaxMaterial) )
				return m_materials[i];
		}
	}

	return NULL;
}

RMaxMaterial* RMaxMaterialKeeper::GetMaterial(Mtl *mtl)
{
	// 적용된 material 이 없으면 default material;
	if(NULL==mtl) return m_materials[0];

	for(int i=0;i<(int)m_materials.size();i++)
		if(m_materials[i]->m_pMtl == mtl) return m_materials[i];
	
	return NULL;
}

bool RMaxMaterialKeeper::IsExistMaterial(RMaxMaterial *pMaxMaterial)
{
	for( size_t i = 0; i < m_materials.size(); ++i )
	{
		if( m_materials[i] == pMaxMaterial )
			return true;
	}

	return false;
}

bool RMaxMaterialKeeper::CheckForAnimation(Texmap *tex, bool &bOffset, bool &bScale, bool &bRotate)
{
	if ( m_timeDelta == 0 )		return false;
	if ( m_timeStart == m_timeEnd )		return false;

	if ( tex == NULL )	return false;
	if ( tex->ClassID() != Class_ID(BMTEX_CLASS_ID, 0x00))	return false;

	StdUVGen *uvGen = ((BitmapTex*)tex)->GetUVGen();
	if ( uvGen == NULL)		return false;

	TimeValue t = m_timeStart;

	RVector2 vOffset(uvGen->GetUOffs(t), uvGen->GetVOffs(t));
	RVector2 vScale (uvGen->GetUScl(t) , uvGen->GetVScl(t));
	RVector vRotate (uvGen->GetUAng(t), uvGen->GetVAng(t), uvGen->GetWAng(t));

	bOffset = false;
	bScale = false;
	bRotate = false;
	
	// 변화가 있는지 검사, material 제어에 해당하는 controller를 얻을 수 있을 때 까지 이 방법 밖에 없는 것 같다.
	for ( ; t<=m_timeEnd; t += m_timeDelta )
	{
		RVector2 vOffsetN, vScaleN;
		RVector3 vRotateN;

		vOffsetN = RVector2(uvGen->GetUOffs(t), uvGen->GetVOffs(t));
		vScaleN = RVector2(uvGen->GetUScl(t) , uvGen->GetVScl(t));
		vRotateN = RVector(uvGen->GetUAng(t), uvGen->GetVAng(t), uvGen->GetWAng(t));


		if ( vOffset != vOffsetN )	bOffset = true;
		if ( vScale != vScaleN )	bScale = true;
		if ( vRotate != vRotateN )	bRotate = true;

		vOffset = vOffsetN;
		vScale = vScaleN;
		vRotate = vRotateN;
	}

	return ( bOffset || bScale || bRotate );
}

RMaxMaterial* RMaxMaterialKeeper::AddMtl(Mtl *mtl,RMaxMaterial *pParent, int nSubMtrlID)
{
	if(!mtl) return NULL;

	RMaxMaterial* pExistMaterial = GetMaterial(mtl);
	if(pExistMaterial) return pExistMaterial;

	// 새로운 재질은 무조건 추가 하고 ( 맥스에서 중복이 제거 되지 않은 상태의 FaceMtrlID를 사용하므로 )
	// 실제 Export에서는 중복되지 않게 m_pBaseMaterial를 참조한다. 출력도 m_pBaseMaterial을 출력한다.
	RMaxMaterial *newMaxMaterial = new RMaxMaterial;
	newMaxMaterial->m_pMtl = mtl;
	newMaxMaterial->m_pParent = pParent;
	newMaxMaterial->m_nSubMtrlID = nSubMtrlID;

	pExistMaterial = GetExistMaterial(newMaxMaterial);
	if(pExistMaterial) 
		newMaxMaterial->m_pBaseMaterial = pExistMaterial;
	else
		newMaxMaterial->m_pBaseMaterial = newMaxMaterial;	// 자기 자신
	

	m_materials.push_back(newMaxMaterial);
	return newMaxMaterial;
}

void RMaxMaterialKeeper::GetMaterialListRecurse(INode* node, MCplug2* pExporter)
{
//	MCHAR* name = node->GetName(); // for debug

	// 액터 익스포트 할 때는 XRefGroup과 그 자식들은 마테리얼을 익스포트 하지 않는다.
	if( !pExporter->m_options.m_bStaticMesh )
	{
		if ( node->IsGroupHead() && pExporter->IsXRefExport(node) )
			return;
	}

	if( !pExporter->IsExportSelected() || node->Selected() )
		AddMtl(node->GetMtl());

	for (int c = 0; c < node->NumberOfChildren(); c++) {
		GetMaterialListRecurse(node->GetChildNode(c), pExporter);
	}
}

void RMaxMaterialKeeper::GetTextureMap(RMtrlTexInfo* pTexInfo, Texmap* tex, int subNo)
{
	//Set Texture Type
	pTexInfo->SetTextureType( ConvertTextureTyepFromMaxToRS3(subNo) );


	if(tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
	{
		StdUVGen* uvGen = ((BitmapTex *)tex)->GetUVGen();
		if(uvGen)
		{
			int texAddressMode = uvGen->GetTextureTiling();

			if(texAddressMode & U_MIRROR)
				pTexInfo->SetTextureAddressU(RTADDRESS_MIRROR);
			else
			if(texAddressMode & U_WRAP)
				pTexInfo->SetTextureAddressU(RTADDRESS_WRAP);
			else
				pTexInfo->SetTextureAddressU(RTADDRESS_BORDER);

			if(texAddressMode & V_MIRROR)
				pTexInfo->SetTextureAddressV(RTADDRESS_MIRROR);
			else
			if(texAddressMode & V_WRAP)
				pTexInfo->SetTextureAddressV(RTADDRESS_WRAP);
			else
				pTexInfo->SetTextureAddressV(RTADDRESS_BORDER);

			pTexInfo->SetMapChannel( uvGen->GetMapChannel() - 1 );

			RUVTransform UVTransform;
			UVTransform.SetOffset( RVector2(uvGen->GetUOffs(0), uvGen->GetVOffs(0)) );
			UVTransform.SetScale( RVector2(uvGen->GetUScl(0), uvGen->GetVScl(0)) );
			pTexInfo->SetUVTransform(UVTransform);
		}


		//Set Texture file name
		pTexInfo->SetTextureName( GetTextureName(tex) );


		//Set UV-Animation
		{
			bool bOffsetAni, bScaleAni, bRotateAni;

			StdUVGen *uvGen = ((BitmapTex*)tex)->GetUVGen();

			// default texture transform
			RVector4 vUVTransform(0,0,1,1);

			// animation track
			if ( CheckForAnimation(tex, bOffsetAni, bScaleAni, bRotateAni))
			{
				MCKeyTrack<MCVec3Key> trackOffset;
				MCKeyTrack<MCVec3Key> trackScale;
				MCKeyTrack<MCVec3Key> trackRotate;

				int nFrame = 0;
				for ( TimeValue t = m_timeStart; t <=m_timeEnd; t += m_timeDelta, ++nFrame )
				{
					// 시작 프레임이 0이 아니라도 실제로 저장되는 값은 0프레임 기준으로 저장되어야 한다.
					TimeValue zeroBaseFrame = t - m_timeStart;

					if ( bOffsetAni || t == m_timeStart )
					{
						MCVec3Key vFrame;
						vFrame.x = uvGen->GetUOffs(t);
						vFrame.y = uvGen->GetVOffs(t);
						vFrame.z = 0.f;
						vFrame.frame = zeroBaseFrame;

						trackOffset.push_back(vFrame);
					}
					if ( bScaleAni || t == m_timeStart )
					{
						MCVec3Key vFrame;
						vFrame.x = uvGen->GetUScl(t);
						vFrame.y = uvGen->GetVScl(t);
						vFrame.z = 0.f;
						vFrame.frame = zeroBaseFrame;

						trackScale.push_back(vFrame);
					}
					if ( bRotateAni || t == m_timeStart )
					{
						MCVec3Key vFrame;
						vFrame.x = uvGen->GetUAng(t);
						vFrame.y = uvGen->GetVAng(t);
						vFrame.z = uvGen->GetWAng(t);
						vFrame.frame = zeroBaseFrame;

						trackRotate.push_back(vFrame);
					}
				}

				// 키를 최적화 한다
				trackOffset.Optimize();
				trackScale.Optimize();
				trackRotate.Optimize();

				// 최적화된 키를 RMaterial 로 복사한다
#define Copy_MCKeyTrack_To_RAnimationTrack( _dest, _src )	if(_src.size()) {	\
_dest.Create( _src.size() );	\
int i=0;	\
for(MCKeyTrack<MCVec3Key>::iterator itr = _src.begin();itr!=_src.end(); ++itr, ++i)	\
_dest.at(i) = *itr;	\
				}

				Copy_MCKeyTrack_To_RAnimationTrack( pTexInfo->GetUVAnimationRef().GetOffsetKeyTrack(), trackOffset);
				Copy_MCKeyTrack_To_RAnimationTrack( pTexInfo->GetUVAnimationRef().GetScaleKeyTrack(), trackScale);
				Copy_MCKeyTrack_To_RAnimationTrack( pTexInfo->GetUVAnimationRef().GetRotateKeyTrack(), trackRotate);
			}
		}


	}
	else
	{
		// normal bump 일때 따로 처리 // 중복 처리 삭제
		if(subNo==ID_BU) 
		{
			// id 0 -> normal , 1 -> additional bump
			Texmap* subTex = tex->GetSubTexmap(0);
			if(subTex && (subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)))
				GetTextureMap(pTexInfo, subTex, ID_BU);
		}
	}
}

void RMaxMaterialKeeper::GetSubMaterial(RMaxMaterial *material, bool bSetForceUsed)
{
	if(!material) return;

	Mtl *mtl = material->m_pMtl;
	if(!mtl) return;

	// DirectX Shader 를 사용하는지 판별.
	BOOL bUseHWShader = mtl->TestMtlFlag(MTL_HW_MAT_ENABLED);

	int numSubMtls = mtl->NumSubMtls();
	if (numSubMtls > 0)
	{
		for (int i=0; i<numSubMtls; i++)
		{
			Mtl* subMtl = mtl->GetSubMtl(i);
			if (subMtl) // 중간에 빈 material 이 있을수도 있다
			{
				RMaxMaterial *subMaterial = NULL;
				
				bool bExist = GetMaterial(subMtl)!=NULL;

				// 09.03.04 - 부모의 Sub Material ID 를 그대로 사용한다.
				// 서브 재질을 사용하는 DXSHADER 의 경우 DXSHADER 를 상속 받는 실제 재질을 항상 0번째 서브로 가지는데,
				// 이를 무효화 시키기 위한 서브ID 값 복사
				if( bUseHWShader )
					subMaterial = AddMtl(subMtl, material, material->m_nSubMtrlID);
				else
					subMaterial = AddMtl(subMtl,material,i);

				material->m_children.push_back(subMaterial);

				//새로운 매테리얼일때만 데이터 생성
				if(!bExist)
					GetSubMaterial(subMaterial, bSetForceUsed);
			}
			else
			{
				material->m_children.push_back(m_materials[0]);	// default material
			}
		}
		// Root Multi/Sub는 출력하지 않는다.
		material->m_bUsed = false;
	}
	else
	if( bSetForceUsed )
	{
		material->m_bUsed = true;
	}

	rs3::RMaterial* m = material->m_pRMaterial;

	TimeValue t = 0;

	// 부모가 DirectX Shader 를 사용하고 그 자식 재질(Render Style Material) 인지 판단.
	BOOL bParentUseHWShader = false;
	if( material->m_pParent )
	{
		if( material->m_pParent->m_pMtl )
			bParentUseHWShader = material->m_pParent->m_pMtl->TestMtlFlag(MTL_HW_MAT_ENABLED);
	}
	// 부모가 DirectX Shader 를 사용하고 그 자식 재질(Render Style Material) 이라면 부모의 이름을 가지게 한다.
	if( bParentUseHWShader )
	{
		char buff[MAX_PATH] = "";
		if( material->m_nSubMtrlID >= 0 )
		{
			sprintf_s( buff, MAX_PATH, "%d", material->m_nSubMtrlID );
		}

		m->SetName( string(material->m_pParent->m_pMtl->GetName()) ); // + string(buff);	// 재질 교체시 찾아야할 이름이 바뀌므로 제거함
	}
	else
	{
		m->SetName( string(mtl->GetName()) );
	}


	// standard material 일때
	if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		StdMat* std = (StdMat*)mtl;
		m->SetAmbient( RVector(std->GetAmbient(t).r,std->GetAmbient(t).g,std->GetAmbient(t).b) ) ;
		m->SetDiffuse( RVector(std->GetDiffuse(t).r,std->GetDiffuse(t).g,std->GetDiffuse(t).b) );
		m->SetSpecular( RVector(std->GetSpecular(t).r,std->GetSpecular(t).g,std->GetSpecular(t).b) );
		m->SetSpecularLevel( std->GetShinStr(t) * 100.0f );	// Max에서의 수치와 맞추어 준다.
		m->SetGlossiness( std->GetShininess(t) * 100.0f );// Max에서의 수치와 맞추어 준다.
		m->SetFlag(0);
		if(std->GetTransparencyType()==TRANSP_ADDITIVE) 
			m->AddFlag(RM_FLAG_ADDITIVE);
		if(std->GetTwoSided()==TRUE)
			m->AddFlag(RM_FLAG_TWOSIDED);
		if(_strnicmp(m->GetName().c_str(),"at_",3)==0)
			m->AddFlag(RM_FLAG_USEALPHATEST);
		if(_strnicmp(m->GetName().c_str(),"at_less_",8)==0) 
		{
			m->AddFlag(RM_FLAG_USEALPHATEST);
			m->SetAlphaCmpFunc(RCMP_LESS);
		}


		vector<Texmap*> enabledTextures;
		vector<int>		enabledTexturesIDs;
		vector<Texmap*> enabledCompositeTextures;
		vector<int> enabledCompositeTextureIDs;
		GatherTexture(material, enabledTextures, enabledTexturesIDs, enabledCompositeTextures, enabledCompositeTextureIDs);


		//-------------------------------------------------------------
		//Add Texture Info
		m->CreateTexInfo((int)enabledTextures.size());
		for(size_t i=0;i<enabledTextures.size();i++) 
		{
			Texmap* subTex = enabledTextures[i];

			RMtrlTexInfo TexInfo;
			GetTextureMap(&TexInfo, subTex,enabledTexturesIDs[i]);
			material->UpdateUVChannelNum( TexInfo.GetMapChannel() );

			// 디퓨즈 맵이면 uv transform 을 구한다
			if(enabledTexturesIDs[i]==ID_DI) 
			{
				StdUVGen* uvGen = ((BitmapTex *)subTex)->GetUVGen();
				if(uvGen) 
				{
					uvGen->GetUVTransform(*material->m_uvTransform);
				}

				Point3 scale = Point3((TexInfo.GetTextureAddressU()==RTADDRESS_MIRROR) ? 2.f : 1.f ,
										(TexInfo.GetTextureAddressV()==RTADDRESS_MIRROR) ? 2.f : 1.f ,
										1.f);
				Matrix3 matScale;
				matScale.IdentityMatrix();
				matScale.Scale(scale);
				
				*material->m_uvTransform *= matScale;
			}

			// 알파테스트가 활성화 되어있으면 오파서티 플래그는 무시한다
			if(TexInfo.GetTextureType()== RTT_OPACITY && !m->CheckFlag(RM_FLAG_USEALPHATEST)) 
			{
				m->AddFlag(RM_FLAG_USEOPACITY);
			}

			m->AddMtrlTexInfo(i, TexInfo);
		}
		//Add Texture Info
		//-------------------------------------------------------------
		


		//Composite Layer Info
		AddCompositeMap(m, enabledCompositeTextures, enabledCompositeTextureIDs);
				


		// Self-Illumination
		BOOL bSIOn = mtl->GetSelfIllumColorOn();
		if( bSIOn )
		{
			static const int MAXSELFILLUMINATIONSCALE = 4;
			// 0 ~ 1 -> 1 ~ 4(MAXSELFILLUMINATIONSCALE)
			float fScale = mtl->GetSelfIllumColor().r;
			fScale = 1 + fScale * ( MAXSELFILLUMINATIONSCALE - 1 );
			m->SetSelfIlluminationScale( fScale );
		}
	}
}

void RMaxMaterialKeeper::GetMaterialList(INode*	node, MCplug2* pExporter)
{
	// 각 노드의 Mtl을 수집한후
	GetMaterialListRecurse(node, pExporter);

	// subMtl 들도 더한다, 갯수가 늘어나므로 topLevel 에만 한다.
	int nTopLevelMaterials = (int)m_materials.size();
	for(int i=0;i<nTopLevelMaterials;i++)
		GetSubMaterial(m_materials[i],false);
}

void RMaxMaterialKeeper::AddSubMtl(Mtl *mtl,RMaxMaterial *pParent, bool bSetForceUsed)
{
	AddMtl(mtl);
	GetSubMaterial(m_materials[(int)m_materials.size()-1],bSetForceUsed);
}

void RMaxMaterialKeeper::ClearVoidMaterial()
{
	vector<RMaxMaterial*>::iterator it = m_materials.begin();
	while(it!=m_materials.end())
	{
		RMaxMaterial* pMtrl = (*it);

		// 레어어 수가 하나이고( 0개는 없으므로 ) UNKNOWN 일때 데이터가 없는 재질 이므로 삭제
		if( pMtrl->m_pRMaterial->GetMaxLayer() == 1 &&
			pMtrl->m_pRMaterial->GetTexLayer(0)->GetTextureType() == RTT_UNKNOWN )
		{
			m_clearedMaterials.push_back(pMtrl);
			it = m_materials.erase(it);
		}
		else if(!pMtrl->m_bUsed)
		{
			m_clearedMaterials.push_back(pMtrl);
			it = m_materials.erase(it);
		}
		else
		{
			it++;
		}
	}

	for(size_t i=0;i<m_materials.size();i++)
		m_materials[i]->m_nIndex = i;
}



void RMaxMaterialKeeper::ExportXML(MXmlElement *pElement)
{
	MXmlElement* pMeterialListElement = new MXmlElement(RTOK_MATERIALLIST);
	pElement->LinkEndChild(pMeterialListElement);

	for(size_t i=0;i<m_materials.size();i++)
	{
		RMaterial *pMaterial= m_materials[i]->m_pRMaterial;

		// 텍스쳐 이름앞의 경로를 지워준다
		for( int j=0; j<pMaterial->GetMaxLayer(); j++)
		{
			RMtrlTexInfo* pTexInfo = pMaterial->GetTexLayer(j);
			pTexInfo->SetTextureName(  MGetRemovePathName(pTexInfo->GetTextureName()) );
		}

		MXmlElement* pMeterialElement = new MXmlElement(RTOK_MATERIAL);
		pMeterialListElement->LinkEndChild(pMeterialElement);
		pMaterial->Save(pMeterialElement);
	}
}

#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

bool RMaxMaterialKeeper::ExportTextures(/*BOOL bUsePartsColor,*/ BOOL bUseDDS, BOOL bExportTexture, 
										D3DFORMAT plainFormat, D3DFORMAT alphaFormat, const string& targetPath )
{
	// export texture 이면 use dds 여야 한다
	_ASSERT(bUseDDS || !bExportTexture);


	LPDIRECT3D9				g_pD3D=NULL;
	LPDIRECT3DDEVICE9		g_pd3dDevice=NULL;
	D3DPRESENT_PARAMETERS	g_d3dpp; 

	HRESULT hr;

	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferWidth = 640;
	g_d3dpp.BackBufferHeight = 480;
	g_d3dpp.BackBufferCount = 1;
	g_d3dpp.Windowed   = true;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.Flags=NULL;

	DWORD BehaviorFlags=D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,GetDesktopWindow(),BehaviorFlags,&g_d3dpp,&g_pd3dDevice);
	if( FAILED( hr ) )
	{
		g_pD3D->Release();
		return false;
	}

	for(size_t i=0;i<m_materials.size();i++)
	{
		RMaterial* pMaterial= m_materials[i]->m_pRMaterial;
		for( int j=0; j<pMaterial->GetMaxLayer(); j++)
		{
			RMtrlTexInfo* pTexInfo = pMaterial->GetTexLayer(j);
			string targetName = targetPath + MGetRemovePathName(pTexInfo->GetTextureName());
			string targetExtName = MGetPureExtension(targetName);

			if(bUseDDS && targetExtName != ".dds" )
				targetName += ".dds";

			// 실제로 쓰는 채널만 익스포트
			if( RTT_UNKNOWN < pTexInfo->GetTextureType() && pTexInfo->GetTextureType() < RTT_MAX )
			{
				if(bExportTexture &&
					!ConvertTexture(g_pd3dDevice,pTexInfo->GetTextureName().c_str(), plainFormat, alphaFormat, targetName.c_str()))
				{
					mlog3("can't convert texture %s",MGetRemovePathName(pTexInfo->GetTextureName()).c_str());
				}
				pTexInfo->SetTextureName(targetName);
			}

			// diffuse 텍스쳐 이름에 _pc. 이 들어가 있으면 parts color 를 가진 것이다.
			if( pTexInfo->GetTextureType() == RTT_DIFFUSE && 
				strstr(pTexInfo->GetTextureName().c_str(), "_pc.")!=0 )
				pMaterial->AddFlag(RM_FLAG_USEPARTSCOLOR);

			pTexInfo->SetTextureName( MGetRemovePathName(pTexInfo->GetTextureName()) );
		}
	}

	g_pd3dDevice->Release();
	g_pD3D->Release();

	return true;
}

int RMaxMaterialKeeper::AddLightMap(const string& lightMapName, const string& strProperty, IRTTGroup* pRTTGroup)
{
	LightMap newMap;
	newMap.strLightMapName = lightMapName;
	newMap.strUserProperty = strProperty;
	newMap.pRTTGroup = pRTTGroup;

	m_defaultLightMaps.push_back( newMap );
	return m_defaultLightMaps.size()-1;
}

const TCHAR* RMaxMaterialKeeper::GetTextureName(Texmap* tex)
{
	// 실제 bitmap 이 로딩되어있는 경로
	Bitmap *pBitmap = ((BitmapTex *)tex)->GetBitmap(0);
	if(pBitmap)
	{
		return pBitmap->Storage()->bi.Name();
	}
	else // 없으면
	{
		// material 에 기술되어있는 경로
		return ((BitmapTex *)tex)->GetMapName();
	}
}

void RMaxMaterialKeeper::GatherTexture(RMaxMaterial *material, vector<Texmap*>& Textures, vector<int>& TexturesIDs, vector<Texmap*>& CompositeTextures, vector<int>& CompositeTextureIDs)
{
	Mtl *mtl = material->m_pMtl;
	rs3::RMaterial* m = material->m_pRMaterial;

	// texture map 정보를 기록한다
	for (int i=0; i<mtl->NumSubTexmaps(); i++) 
	{
		Texmap* subTex = mtl->GetSubTexmap(i);
		if (subTex && 
			(subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)))
		{
			// If it is a standard material we can see if the map is enabled.
			if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
			{
				if (!((StdMat*)mtl)->MapEnabled(i))
					continue;
			}

			Textures.push_back(subTex);
			TexturesIDs.push_back(i);
		}
		else if(subTex && (subTex->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0x00)))
		{
			int texcount = subTex->NumSubTexmaps();
			for(int iLayer=0; iLayer<texcount; iLayer+=2)
			{
				Texmap* Layertex = subTex->GetSubTexmap(iLayer);
				Texmap* OpacityTex = subTex->GetSubTexmap(iLayer+1);

				if(Layertex)
				{
					Textures.push_back(Layertex);
					TexturesIDs.push_back(i);
				}

				if(OpacityTex)
				{
					Textures.push_back(OpacityTex);
					TexturesIDs.push_back(i);
				}
			}

			CompositeTextures.push_back(subTex);
			CompositeTextureIDs.push_back(i);
		}
		else if(i == ID_BU)	// Bump (Normal Map)
		{
			if ( ( (StdMat*)mtl )->MapEnabled(i) )
			{
				Textures.push_back(subTex);
				TexturesIDs.push_back(i);
			}
		}

		// Specular Level값은 Specular Map 정보에 Level 값이 잇으면 이 값을 사용한다.
		// 아니면 default 항목의 Specular Level값을 사용한다.
		if(i == ID_SH)
		{
			if ( ( (StdMat*)mtl )->MapEnabled(i) )
			{
				StdMat* std = (StdMat*)mtl;
				m->SetSpecularLevel( std->GetTexmapAmt(i, 0) * 100.0f );
			}
		}
	}
}

void RMaxMaterialKeeper::AddCompositeMap(rs3::RMaterial* m, vector<Texmap*>& CompositeTextures, vector<int>& CompositeTextureIDs)
{
	for(size_t i=0; i<CompositeTextures.size(); ++i)
	{
		RCompositeLayer CompositeLayer;
		RTextureType type = ConvertTextureTyepFromMaxToRS3(CompositeTextureIDs[i]);
		CompositeLayer.SetTextureType(type);


		Texmap* pCompositeMap = CompositeTextures[i];
	
		vector<COMPOSITE_LAYER_INFO> LayerInfo;
		if(GetCompositeLayerInfo(m, pCompositeMap, type, LayerInfo) == false)
			continue;

		for(size_t iLayer=0; iLayer<LayerInfo.size(); ++iLayer)
		{
			CompositeLayer.AddLayer(LayerInfo[iLayer]);
		}

		m->AddCompositeLayer(CompositeLayer);
	}	
}

bool RMaxMaterialKeeper::GetCompositeLayerInfo(rs3::RMaterial* m, Texmap* tex, RTextureType type, vector<COMPOSITE_LAYER_INFO>& OutLayer)
{
	IGameScene* pIGameScene = GetIGameInterface();

	IPropertyContainer* IProperties = pIGameScene->GetIGameTextureMap(tex)->GetIPropertyContainer();

	IParamBlock2* pBlendModeParam =NULL;
	IParamBlock2* pOpacityParam =NULL;
	ParamID BlendModeID, OpacityID;
	
	if( GetPropertyParamBlock(IProperties, "blendmode", pBlendModeParam, BlendModeID)==false ||
		GetPropertyParamBlock(IProperties, "opacity", pOpacityParam, OpacityID)==false )
		return false;


	int BlendModeCount= pBlendModeParam->Count(BlendModeID);
	int OpacityCount= pOpacityParam->Count(OpacityID);

	if(BlendModeCount!=OpacityCount)
		return false;


	int LayerCount = BlendModeCount;
	
	for(int i=0; i<LayerCount; ++i)
	{
		Texmap* Layertex = tex->GetSubTexmap(i*2);
		Texmap* OpacityTex = tex->GetSubTexmap(i*2+1);

		COMPOSITE_LAYER_INFO LayerInfo;

		if(Layertex)
		{
			if( strcmp(pIGameScene->GetIGameTextureMap(Layertex)->GetTextureClass(), "Normal Bump")==0 )
				Layertex = Layertex->GetSubTexmap(0);

			LayerInfo.pLayerMap = m->GetTexLayerByNameFromType(type, GetTextureName(Layertex));				
		}


		if(OpacityTex)
		{
			if( strcmp(pIGameScene->GetIGameTextureMap(OpacityTex)->GetTextureClass(), "Normal Bump")==0 )
				OpacityTex = OpacityTex->GetSubTexmap(0);

			LayerInfo.pMaskMap = m->GetTexLayerByNameFromType(type, GetTextureName(OpacityTex));				
		}


		Interval ivalid;
		int BlendMode =0;
		float fOpacity =0.0f;

		pBlendModeParam->GetValue(BlendModeID, 0, BlendMode, ivalid, i);
		pOpacityParam->GetValue(OpacityID, 0, fOpacity, ivalid, i);
		fOpacity /=100.0f;	// [0,100] => [0,1]		

		LayerInfo.fOpacity = fOpacity;
		LayerInfo.BlendMode = (rs3::COMPOSITE_LAYER_BLEND_MODE)BlendMode;

		OutLayer.push_back(LayerInfo);
	}

	return true;
}

RTextureType RMaxMaterialKeeper::ConvertTextureTyepFromMaxToRS3(int MaxType)
{
	switch(MaxType)
	{
	case ID_OP:	return RTT_OPACITY;
	case ID_DI:	return RTT_DIFFUSE;
	case ID_SH:
	case ID_SP:	return RTT_SPECULAR;
	case ID_SS:	return RTT_SHININESS;
	case ID_BU:	return RTT_NORMAL;
	case ID_SI: return RTT_SELFILLUMINATION;
	case ID_RL: return RTT_REFLECT;
	default: return RTT_UNKNOWN;
	}
}

bool RMaxMaterialKeeper::GetPropertyParamBlock(IPropertyContainer* IProperties, const TCHAR* name, IParamBlock2*& pOutParamBlock, ParamID& Outid)
{
	IGameProperty* pProperty = IProperties->QueryProperty(name);
	if(pProperty==NULL || (!pProperty->IsParamBlock() || !pProperty->IsPBlock2()))
		return false;

	pOutParamBlock = pProperty->GetMaxParamBlock2();
	
	int index = pProperty->GetParamBlockIndex();
	Outid = pOutParamBlock->IndextoID(index);

	return true;
}