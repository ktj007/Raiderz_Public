#include <windows.h>
#include "d3dx9.h"
#include "mmsystem.h"
#include "MXml.h"

#include "../sampleApp.h"

#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"

#include "RCameraSceneNode.h"
#include "RDebugPassRenderer.h"
#include "RDirectionalLightSceneNode.h"
#include "RTextSceneNode.h"

#include "RActor.h"
#include "RMesh.h"
#include "RMeshMgr.h"

#include "REffectSceneNode.h"
#include "REffectResourceMgr.h"
#include "REffectResource.h"
#include "RAnimationResource.h"

#include "RRenderHelper.h"
#include "RUtil.h"

#pragma warning( disable : 4996 )
#include "MDebug.h"
#pragma warning( default : 4996 )

#include "../../../sdk/vld/vld.h"
#pragma comment(lib,"vldmt.lib")


#define EFFECT_SAMPLE_HF_IDLE					"2hd_idle"
#define EFFECT_SAMPLE_HF_WEAPON_MESH			"weapons/2hsword.elu"
#define EFFECT_SAMPLE_HF_WEAPON_MESH_NODE		"2hsword_01"
#define EFFECT_SAMPLE_HF_WEAPON_ACTOR_NODE		"dummy_2hs"
#define EFFECT_SAMPLE_HF_ATK					"2hd_atk1"

#define EFFECT_SAMPLE_GM_IDLE					"idle"
#define EFFECT_SAMPLE_GM_ATK					"atk_r"
#define EFFECT_SAMPLE_GM_SPELL					"spell_fire"

#define	EFFECT_SAMPLE_HF_ATK_SUCCESS			"hf_atk_success"
#define	EFFECT_SAMPLE_GM_ATK_SUCCESS			"gm_atk_success"

#define	EFFECT_SAMPLE_HF_ATK_END				"hf_atk_end"
#define	EFFECT_SAMPLE_GM_ATK_END				"gm_atk_end"

#define EFFECT_SAMPLE_HF_BITTEN					"2hd_MF_BT1_1"
#define EFFECT_SAMPLE_GM_BITTEN					"MF_BT2"

#define EFFECT_SAMPLE_HF_BITTEN_END				"hf_bitten_end"
#define EFFECT_SAMPLE_GM_BITTEN_END				"gm_bitten_end"

//by pok. -_- 지송.. 이 리소스 아니었는디...
//#define EFFECT_SAMPLE_HF_ATK_EFFECT				"meleeattack01.Effect.xml"
#define EFFECT_SAMPLE_HF_ATK_EFFECT				"ef_spread_blood_01.Effect.xml"
//#define EFFECT_SAMPLE_HF_ATK_EFFECT				"cast_stepup.Effect.xml"
#define EFFECT_SAMPLE_GM_ATK_EFFECT				"ef_poison_proj_l.Effect.xml"

using namespace rs3;
class myApplication : public sampleApp
{
	// Human Female
	RActor*			m_pHumanFemale;
	RMesh*			m_p2hSwordMesh;

	// Goblin Mage
	RActor*			m_pGoblinMage;

	// TextSceneNode
	RTextSceneNode*	m_pTextSceneNode;

	RDirectionalLightSceneNode* m_pLightSceneNode;

	// Effect
	REffectSceneNode* m_pMeleeEffect;
	REffectSceneNode* m_pProjectileEffect; // fade effect
	REffectSceneNode* m_pHeadEffect;

	bool m_bProjectileNeedToUpdate;

	enum E_TEST_TYPE
	{
		E_MELEE_ATK = 0,
		E_PROJECTILE_ATK,

		E_TEST_NUM
	};
	E_TEST_TYPE		m_eTestType;
	std::string		m_strTestTypeInfo[E_TEST_NUM];
	RSceneNodeUpdateVisitor m_updateVisitor;

public:
	myApplication()
		: m_pHumanFemale(NULL)
		, m_p2hSwordMesh(NULL)
		, m_pMeleeEffect(NULL)
		, m_pProjectileEffect(NULL)
		, m_pHeadEffect(NULL)
		, m_bProjectileNeedToUpdate(false)
	{
	}

	char *GetName() { return "rs3 2Char's Effect Tester"; }
	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnRender();

	// 리스너로서의 sampleApp
	virtual void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
	{
		if (m_pHumanFemale == NULL || m_pGoblinMage == NULL)
			return;

		if (pEvent->m_strEvent == EFFECT_SAMPLE_HF_ATK_END || pEvent->m_strEvent == EFFECT_SAMPLE_HF_BITTEN_END)
		{
			m_pHumanFemale->SetAnimation(EFFECT_SAMPLE_HF_IDLE);
		}
		else if (pEvent->m_strEvent == EFFECT_SAMPLE_GM_ATK_END || pEvent->m_strEvent == EFFECT_SAMPLE_GM_BITTEN_END)
		{
			m_pGoblinMage->SetAnimation(EFFECT_SAMPLE_GM_IDLE);
		}
		else if (pEvent->m_strEvent == EFFECT_SAMPLE_HF_ATK_SUCCESS )
		{
			RAnimation* pAnimation = m_pGoblinMage->m_pMesh->m_AniMgr.GetAnimation(EFFECT_SAMPLE_GM_BITTEN);
			_ASSERT(pAnimation);
			int nLastCount = pAnimation->GetResource()->GetMaxFrame() - 1;
			MXmlElement* pDummyParentElement = new MXmlElement("DummyEvent");
			MXmlElement* pEventElementBitten = new MXmlElement("EVENT");
			_SetAttribute(pEventElementBitten,"name", EFFECT_SAMPLE_GM_BITTEN_END);
			_SetAttribute(pEventElementBitten,"frame", nLastCount);
			pDummyParentElement->LinkEndChild(pEventElementBitten);
			pAnimation->ClearEvent();
			pAnimation->LoadEvent(pDummyParentElement);
			delete pDummyParentElement;
			m_pGoblinMage->SetAnimation(pAnimation);

			//////////////////////////////////////////////////////////////////////////
			//
			// TODO : Melee Effect
			if (m_pMeleeEffect)
			{
				//m_pMeleeEffect->RemoveFromParent();
				delete m_pMeleeEffect;
			}

			m_pMeleeEffect = new REffectSceneNode;
			m_pMeleeEffect->Create(EFFECT_SAMPLE_HF_ATK_EFFECT);

			RVector vDir = -m_pHumanFemale->GetWorldDirection();
			RVector vPos = m_pGoblinMage->GetAABB().GetCenter();
			RMatrix matEffect;
			matEffect.SetLocalMatrix( vPos, vDir, RVector(0,0,1) );
			GetSceneManager()->AttachEffect(m_pMeleeEffect, NULL, NULL, matEffect);
		}
		else if (pEvent->m_strEvent == EFFECT_SAMPLE_GM_ATK_SUCCESS )
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// TODO : Projectile Effect
			if (m_pProjectileEffect)
			{
				//m_pProjectileEffect->RemoveFromParent();
				delete m_pProjectileEffect;
			}

			m_pProjectileEffect = new REffectSceneNode;
			m_pProjectileEffect->Create(EFFECT_SAMPLE_GM_ATK_EFFECT);
			GetSceneManager()->AttachEffect(m_pProjectileEffect, NULL, NULL);

			RVector vDir = -m_pGoblinMage->GetWorldDirection();
			RVector vPos = m_pGoblinMage->GetAABB().GetCenter();
			vPos = vPos + (vDir * 10);
			m_bProjectileNeedToUpdate = true;
			m_pProjectileEffect->SetTransform( vPos, vDir );
			m_pProjectileEffect->Traverse(&m_updateVisitor);
//			m_pProjectileEffect->Update();
		}
	}

	// 키 메시지 핸들러
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OnWndMessage 에서 호출되는 함수
	//
	void OnRequestAnimation(int _nAniNum)
	{
		if(m_pHumanFemale == NULL || m_pGoblinMage == NULL)
			return;

		if ( _nAniNum == 1 &&  m_eTestType == E_MELEE_ATK )
		{
			RAnimation* pAnimation = m_pHumanFemale->m_pMesh->m_AniMgr.GetAnimation(EFFECT_SAMPLE_HF_ATK);
			_ASSERT(pAnimation);
			int nLastCount = pAnimation->GetResource()->GetMaxFrame() - 1;

			MXmlElement* pDummyParentElement = new MXmlElement("DummyEvent");

			MXmlElement* pEventElementAtkSuccess = new MXmlElement("EVENT");
			_SetAttribute(pEventElementAtkSuccess,"name", EFFECT_SAMPLE_HF_ATK_SUCCESS);
			_SetAttribute(pEventElementAtkSuccess,"frame", nLastCount/2);
			pDummyParentElement->LinkEndChild(pEventElementAtkSuccess);

			MXmlElement* pEventElementAtkEnd = new MXmlElement("EVENT");
			_SetAttribute(pEventElementAtkEnd,"name", EFFECT_SAMPLE_HF_ATK_END);
			_SetAttribute(pEventElementAtkEnd,"frame", nLastCount);
			pDummyParentElement->LinkEndChild(pEventElementAtkEnd);

			pAnimation->ClearEvent();
			pAnimation->LoadEvent(pDummyParentElement);
			delete pDummyParentElement;

			m_pHumanFemale->SetAnimation(pAnimation);
		}
		else if ( _nAniNum == 1 && m_eTestType == E_PROJECTILE_ATK )
		{
			RAnimation* pAnimation = m_pGoblinMage->m_pMesh->m_AniMgr.GetAnimation(EFFECT_SAMPLE_GM_ATK);
			_ASSERT(pAnimation);
			int nLastCount = pAnimation->GetResource()->GetMaxFrame() - 1;

			MXmlElement* pDummyParentElement = new MXmlElement("DummyEvent");

			MXmlElement* pEventElementAtkSuccess = new MXmlElement("EVENT");
			_SetAttribute(pEventElementAtkSuccess,"name", EFFECT_SAMPLE_GM_ATK_SUCCESS);
			_SetAttribute(pEventElementAtkSuccess,"frame", nLastCount/3);
			pDummyParentElement->LinkEndChild(pEventElementAtkSuccess);

			MXmlElement* pEventElementAtkEnd = new MXmlElement("EVENT");
			_SetAttribute(pEventElementAtkEnd,"name", EFFECT_SAMPLE_GM_ATK_END);
			_SetAttribute(pEventElementAtkEnd,"frame", nLastCount);
			pDummyParentElement->LinkEndChild(pEventElementAtkEnd);

			pAnimation->ClearEvent();
			pAnimation->LoadEvent(pDummyParentElement);
			delete pDummyParentElement;

			m_pGoblinMage->SetAnimation(pAnimation);
		}
	}

	void OnModeChange(bool bNextMode = true)
	{
		if (bNextMode)
		{
			int nNextMode = (int)m_eTestType;
			nNextMode =  ( nNextMode + 1) % (int)E_TEST_NUM;
			m_eTestType = (E_TEST_TYPE)nNextMode;
		}

		m_pTextSceneNode->SetText( m_strTestTypeInfo[m_eTestType].c_str() );

		if (m_eTestType == E_MELEE_ATK)
		{
			// 설정할 position, at
			RVector vPosHuman(-100, 0, -100);
			RVector vPosGoblin(100, 0, -100);
			RVector vAt(0,0,-100);

			m_pHumanFemale->SetAnimation( EFFECT_SAMPLE_HF_IDLE );
			m_pHumanFemale->SetTransform( vPosHuman, vPosHuman - vAt );
			m_pHumanFemale->Traverse(&m_updateVisitor);
//			m_pHumanFemale->Update();

			m_pGoblinMage->SetAnimation( EFFECT_SAMPLE_GM_IDLE );
			m_pGoblinMage->SetTransform( vPosGoblin, vPosGoblin - vAt );
			m_pGoblinMage->Traverse(&m_updateVisitor);
//			m_pGoblinMage->Update();

			// 카메라 설정
			m_fDistToCamera = 400.f;
			SetCameraTarget( vAt );
		}
		else if (m_eTestType == E_PROJECTILE_ATK)
		{
			// 설정할 position, at
			RVector vPosHuman(-500, 0, -100);
			RVector vPosGoblin(500, 0, -100);
			RVector vAt(0,0,-100);

			m_pHumanFemale->SetAnimation( EFFECT_SAMPLE_HF_IDLE );
			m_pHumanFemale->SetTransform( vPosHuman, vPosHuman - vAt );
			m_pHumanFemale->Traverse(&m_updateVisitor);
//			m_pHumanFemale->Update();

			m_pGoblinMage->SetAnimation( EFFECT_SAMPLE_GM_IDLE );
			m_pGoblinMage->SetTransform( vPosGoblin, vPosGoblin - vAt );
			m_pGoblinMage->Traverse(&m_updateVisitor);
//			m_pGoblinMage->Update();

			m_fDistToCamera = 800.f;
			SetCameraTarget( vAt );
		}
	}

	void OnRotation( bool bClockWise )
	{
		RMatrix matRot;

		if (bClockWise)
			matRot.SetRotationZ(-0.05f);
		else
			matRot.SetRotationZ(0.05f);

		/*m_pHumanFemale->SetTransform( m_pHumanFemale->GetWorldTransform() * matRot );
		m_pGoblinMage->SetTransform( m_pGoblinMage->GetWorldTransform() * matRot );*/
		m_pHumanFemale->SetTransform( matRot * m_pHumanFemale->GetWorldTransform() );
		m_pHumanFemale->Traverse(&m_updateVisitor);
//		m_pHumanFemale->Update();
		m_pGoblinMage->SetTransform( matRot * m_pGoblinMage->GetWorldTransform() );
		m_pGoblinMage->Traverse(&m_updateVisitor);
//		m_pGoblinMage->Update();
	}

} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
	#endif
	#ifndef WHEEL_DELTA
	#define WHEEL_DELTA 10
	#endif

	switch (message)
	{
		case WM_KEYDOWN:
		{
			// 애니메이션 설정
			if('0'<wParam &&  wParam<'9')
			{
				OnRequestAnimation( int(wParam-'0') );
			}
			else
			{
				switch(wParam)
				{
					case 'C'	:	OnModeChange();			break;
					case 'Q'	:	OnRotation(true);		break;
					case 'E'	:	OnRotation(false);		break;
					case 'F' :
						{
							m_pHeadEffect = new REffectSceneNode;
							//m_pHeadEffect->Create("test_emmit.Effect.xml");
							m_pHeadEffect->Create("ef_critical_debuff.Effect.xml");
							RMatrix matTrasOffset;
							matTrasOffset.MakeIdentity();
							matTrasOffset.SetTranslation( RVector(0,0,0));
							GetSceneManager()->AttachEffect(m_pHeadEffect, m_pGoblinMage, m_pGoblinMage->GetActorNode("dummy_ef_body"), matTrasOffset);
						}
						break;
					case 'G' : //great scale
						{
							const RVector scale = m_pGoblinMage->GetScale();
							m_pGoblinMage->SetScale( scale + RVector( 0.1f, 0.1f, 0.1f) );
						}
						break;
					case 'H' :
						{
							const RVector scale = m_pGoblinMage->GetScale();
							m_pGoblinMage->SetScale( scale - RVector( 0.1f, 0.1f, 0.1f) );

						}
						break;
				}
			}
		}
		break;

		case WM_MOUSEWHEEL:
		{
			m_fDistToCamera += (float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA;
		}
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

bool myApplication::OnCreate()
{
	// 환경 설정
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	// 리소스 매니저 설정
	pSceneManager->GetResourceMgr< REffectResourceMgr >()->SetCacheEnable(true);

	// Human Female --
	m_pHumanFemale = static_cast< RActor* >( pSceneManager->CreateSceneNode( rs3::RSID_ACTOR ) );
	_ASSERT(m_pHumanFemale != NULL );
	bool bRet = pSceneManager->LoadSceneNode( m_pHumanFemale, "hf/hf.elu" );
	_ASSERT(bRet);
	m_pHumanFemale->SetListener(this);
	pSceneManager->AddSceneNode(m_pHumanFemale);

	// 대검달기
	// 1 자원로딩
	m_p2hSwordMesh = pSceneManager->CreateResource<RMesh>(EFFECT_SAMPLE_HF_WEAPON_MESH);
	bRet = m_p2hSwordMesh->BuildAll();
	_ASSERT(bRet);
	RMeshNode* pSwordMeshNode = m_p2hSwordMesh->GetNode(EFFECT_SAMPLE_HF_WEAPON_MESH_NODE);
	_ASSERT(pSwordMeshNode != NULL );
	RActorNode* pSwordDummyActorNode = m_pHumanFemale->GetActorNode(EFFECT_SAMPLE_HF_WEAPON_ACTOR_NODE);
	_ASSERT(pSwordDummyActorNode != NULL );
	// 2 인스턴스에 자원 붙이기
	m_pHumanFemale->AddMeshNode(pSwordMeshNode, pSwordDummyActorNode);

	// globin_mage --
	m_pGoblinMage = static_cast< RActor* >( pSceneManager->CreateSceneNode( rs3::RSID_ACTOR ));
	_ASSERT( m_pGoblinMage );
	bRet = pSceneManager->LoadSceneNode( m_pGoblinMage, "goblin_mage/goblin_Mage.elu" );
	_ASSERT( bRet );
	m_pGoblinMage->SetListener(this);
	pSceneManager->AddSceneNode(m_pGoblinMage);

	m_pHeadEffect = new REffectSceneNode;
	//m_pHeadEffect->Create("test_emmit.Effect.xml");
	/*RMatrix matTrasOffset;
	matTrasOffset.MakeIdentity();
	matTrasOffset.SetTranslation( RVector(0,0,0));
	pSceneManager->AttachEffect(m_pHeadEffect, m_pGoblinMage, NULL, matTrasOffset);
	m_pHeadEffect->SetAlignTarget(m_pGoblinMage);*/
	//m_pCamera->GetPassRenderer< rs3::RDebugPassRenderer >()->AddDebugBoxAndAxis( pSwordDummyActorNode->GetWorldTransform(), 10, 10 );


	// 빛 생성
	m_pLightSceneNode = static_cast< RDirectionalLightSceneNode* >(pSceneManager->CreateSceneNode(rs3::RSID_DIRECTIONALLIGHTSCENENODE));
	m_pLightSceneNode->SetDirection( RVector(0, 1, 0) );
	pSceneManager->AddSceneNode( m_pLightSceneNode );

	// 폰트 / 텍스트 씬노드 생성
	m_pFont->Destroy();
	m_pFont->Create(m_pDevice,_T("굴림"),12);
	m_pTextSceneNode = static_cast< RTextSceneNode* >( pSceneManager->CreateSceneNode( rs3::RSID_TEXTSCENENODE ) );
	m_pTextSceneNode->SetFont(m_pFont);
	m_pTextSceneNode->SetPosition(0,540);
	pSceneManager->AddSceneNode(m_pTextSceneNode);

	// Test 타입 / Text Info 설정
	m_strTestTypeInfo[ E_MELEE_ATK ] =  "Melee Atk Test Mode : press 'c' if you want to change mode\n";
	m_strTestTypeInfo[ E_MELEE_ATK ] += "Key 1 : 2hd_atk1\n";
	m_strTestTypeInfo[ E_MELEE_ATK ] += "Key Q : clockwise rotation\n";
	m_strTestTypeInfo[ E_MELEE_ATK ] += "Key E : r_clockwise rotation\n";

	m_strTestTypeInfo[ E_PROJECTILE_ATK ] =  "Projectile Atk Test Mode : press 'c' if you want to change mode\n";
	m_strTestTypeInfo[ E_PROJECTILE_ATK ] += "Key 1 : spell_fire\n";
	m_strTestTypeInfo[ E_PROJECTILE_ATK ] += "Key Q : clockwise rotation\n";
	m_strTestTypeInfo[ E_PROJECTILE_ATK ] += "Key E : r_clockwise rotation\n";

	m_eTestType = E_MELEE_ATK;
	OnModeChange(false);

	// 커서
	SetCursor(NULL);
	return true;
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();

	REffectSceneNode::ControlEffectSceneNodeLife(&m_pProjectileEffect);
	REffectSceneNode::ControlEffectSceneNodeLife(&m_pMeleeEffect);

	if (m_pProjectileEffect != NULL && m_pHumanFemale != NULL)
	{
		if (m_bProjectileNeedToUpdate == false)
			return;

		RVector vPos = m_pProjectileEffect->GetWorldPosition();
		if (m_pHumanFemale->GetWorldPosition().DistanceToSq(vPos) <= 10000.f)
		{
			m_pProjectileEffect->Stop();
			m_bProjectileNeedToUpdate = false;

			// 애니메이션 이벤트 설정
			RAnimation* pAnimation = m_pHumanFemale->m_pMesh->m_AniMgr.GetAnimation(EFFECT_SAMPLE_HF_BITTEN);
			_ASSERT(pAnimation);
			int nLastCount = pAnimation->GetResource()->GetMaxFrame() - 1;

			MXmlElement* pDummyParentElement = new MXmlElement("DummyEvent");
			MXmlElement* pEventElementBitten = new MXmlElement("EVENT");
			_SetAttribute(pEventElementBitten,"name", EFFECT_SAMPLE_HF_BITTEN_END);
			_SetAttribute(pEventElementBitten,"frame", nLastCount);
			pDummyParentElement->LinkEndChild(pEventElementBitten);

			pAnimation->ClearEvent();
			pAnimation->LoadEvent(pDummyParentElement);
			delete pDummyParentElement;

			m_pHumanFemale->SetAnimation(pAnimation);
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//
			// TODO : Projectile Effect
			RVector vDir = (m_pHumanFemale->GetWorldPosition() - m_pProjectileEffect->GetWorldPosition()).Normalize();
			vPos = vPos + ( vDir * ( static_cast<float>(REngine::GetTimer().GetElapsedTime())/4.f ) );
			m_pProjectileEffect->SetTransform(vPos, vDir);
			m_pProjectileEffect->Traverse(&m_updateVisitor);
//			m_pProjectileEffect->Update();
		}
	}
}

void myApplication::OnRender()
{
	sampleApp::OnRender();
}

#define SAFE_REMOVE_AND_DELETE(p) if(p) { (p)->RemoveFromParent(); SAFE_DELETE(p); }

void myApplication::OnDestroy()
{
	// Text
	SAFE_REMOVE_AND_DELETE(m_pTextSceneNode);

	// Effect
	SAFE_REMOVE_AND_DELETE(m_pMeleeEffect);
	SAFE_REMOVE_AND_DELETE(m_pProjectileEffect);
	SAFE_REMOVE_AND_DELETE(m_pHeadEffect);

	// Actor
	SAFE_REMOVE_AND_DELETE(m_pHumanFemale);
	SAFE_REMOVE_AND_DELETE(m_pGoblinMage);

	// 액터가 참조하므로 액터 지운 이후에 지워야 한다
	if(m_p2hSwordMesh)
		GetSceneManager()->ReleaseResource( m_p2hSwordMesh );

	//light
	SAFE_REMOVE_AND_DELETE(m_pLightSceneNode);

	GetSceneManager()->GetResourceMgr< REffectResourceMgr >()->ClearCache();

	sampleApp::OnDestroy();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	return g_App.Run();
}
