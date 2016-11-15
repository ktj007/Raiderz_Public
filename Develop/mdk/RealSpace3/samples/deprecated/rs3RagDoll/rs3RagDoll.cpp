#include <windows.h>
#include "crtdbg.h"
#include "d3dx9.h"

#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"
#include "RSkeleton.h"
#include "RSimpleBoxSceneNode.h"
#include "RSimpleSphereSceneNode.h"
#include "RSimpleCylinderSceneNode.h"
#include "RRenderHelper.h"
#include "RMeshUtil.h"
#include "RRagDollSkeletonController.h"
#include "RNewton.h"
#include "RTerrain.h"
#include "RAABBTreeSceneNode.h"
#include "RCollisionTree.h"
#include "RUtil.h"

using namespace rs3;

// 이것들을 어떻게 해야할지 -_-
namespace rs3 {
void  PhysicsApplyForceAndTorque (const NewtonBody* body);
void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix);
}


class myApplication : public sampleApp {
	RNewton			m_newton;
//	RMeshMgr		m_MeshMgr;
	RActor*			m_pActor;
//	RSkeleton		*m_pSkeleton;
    RTerrain		*m_pTerrain;
	RCollisionTree	m_collisionTree;

	int				m_nShowBone;

	// newton 테스트용
	RSimpleBoxSceneNode	*m_pFloor;
	RSimpleBoxSceneNode	*m_pBoxes;

	RRagDollSkeletonController	*m_pRagDollController;

	// newton 객체들
	NewtonBody* boxBody;
	NewtonBody* floorBody; 
	NewtonCollision* collision;

public:
	myApplication() : sampleApp(), m_nShowBone(false) { }

	char *GetName() { return "rs3 actor sample application"; }

	void OnInit();
	bool OnCreate();
	void OnDestroy();
	void OnRender();
	void OnUpdate();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	
	void OnPickTest();
	void OnShowBone();
//	void OnForce();

private:
	void Reset();
	void BeginRagDoll();

} g_App;

void myApplication::OnInit()
{
	sampleApp::OnInit();
	REngine::GetConfig().SetTexturePath("../Data/Texture/character");
	REngine::GetConfig().SetModelPath("../Data/Model");
}

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'R'	:Reset();break;
		case 'T'	:BeginRagDoll();break; 
		case 'P'	:OnPickTest();break;
		case 'B'	:OnShowBone();break;
//		case 'F'	:OnForce();break;
		}break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}
//
//void myApplication::OnForce()
//{
//	float veloc[3] = { 0.0f, 0.0f, 10.0f };
//   NewtonAddBodyImpulse (m_myBody, &veloc[0], &matrix.m_posit[0]);
//}

void myApplication::OnShowBone()
{
	m_nShowBone = (m_nShowBone+1)%3;
	m_pRagDollController->ShowBones(m_nShowBone==2);
}

void myApplication::OnPickTest()
{
	RECT rt;
	GetClientRect(GetWindowHandle(),&rt);

	RVector p, d, q;

	RGetScreenLine(400, 300, rt, m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix(),&d);

	RCollisionPickInfo info;
	info.m_inPickOrigin = m_pCamera->GetPosition();
	info.m_inPickTo = m_pCamera->GetPosition() + 10000.f*d;

	if(m_collisionTree.Pick(info, RCollisionTriangle::ALL, false))
		m_pActor->SetPosition(info.m_outPosition+RVector(0,0,400));
}

const int nBoxCount = 20;

bool myApplication::OnCreate()
{
	m_collisionTree.Load("Gunz2Test/Gunz2Test.cl2");

	if(!GetSceneManager()->LoadRootSceneFromXML("Gunz2Test/Gunz2Test.zone.xml") )
		return false;

	m_pCamera->SetClearColor(0xff404040);

	RSceneManager* pSceneManager = GetSceneManager();
	RMesh* pMesh = pSceneManager->CreateResource< RMesh >("player/hf/hf.elu");
	if( !pMesh->BuildAll() )
	{
		pSceneManager->ReleaseResource( pMesh );
		return false;
	}

	m_pActor = new RActor;
	m_pActor->Create(pMesh);

	m_pActor->SetAnimation("none_walk");
	m_pActor->Play();

	RVector ragDollActorPosition(-100,100,200);	// indoor
	GetSceneManager()->AddSceneNode(m_pActor);
	m_pActor->SetDirection(RVector(-1,0,0),RVector(0,0,1));
	m_pActor->SetPosition(ragDollActorPosition);

	m_pActor->m_dwBoneColor[RBIPID_RCLAVICLE] = 0xffffff00;
	m_pActor->m_dwBoneColor[RBIPID_RUPPERARM] = 0xffff0000;
//	m_pSkeleton->m_dwBoneColor[RBIPID_RFOREARM] = 0xffff0000;
//	m_pSkeleton->m_dwBoneColor[RBIPID_RHAND] = 0xffff0000;
	
	m_fDistToCamera = 0.f;
	m_cameraTargetPos = -m_cameraDir*400.f;

	// newton 초기화
	m_newton.Create();

	///////////////// 바닥을 만든다
	RVector size (1000.0f, 1000.0f, 2.0f);

	RMatrix location;
	location.MakeIdentity();
	location._43= -5.0f; 

	// 바닥 메쉬
	RBoundingBox boxFloor;
	boxFloor.vmin = -.5f*size;
	boxFloor.vmax = .5f*size;
	m_pFloor = new RSimpleBoxSceneNode;
	m_pFloor->SetBox(boxFloor);
	m_pFloor->SetColor(0xffff0000);
//	m_pFloor->SetTransform(location);
	GetSceneManager()->AddSceneNode(m_pFloor);

	const NewtonWorld *nWorld = m_newton.GetNewtonWorld();

	// create the the floor collision, and body with default values
	collision = NewtonCreateBox (nWorld, size.x, size.y, size.z, NULL); 
	floorBody = NewtonCreateBody (nWorld, collision);
	NewtonReleaseCollision (nWorld, collision);

	// set the transformation for this rigid body
	NewtonBodySetMatrix (floorBody, (float*)location);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (floorBody, m_pFloor);

	// set the transform call back function
	NewtonBodySetTransformCallback (floorBody, PhysicsSetTransform);

	// set the force and torque call back function
	NewtonBodySetForceAndTorqueCallback (floorBody, PhysicsApplyForceAndTorque);

	/*
	//////////////// 상자를 만든다
	// set the initial size
	size = RVector(10.f, 10.f, 10.f);

	// create the collision 
	collision = NewtonCreateBox (nWorld, size.x, size.y, size.z, NULL); 
	
	m_pBoxes = new RSimpleBoxSceneNode[nBoxCount];
	for(int i=0;i<nBoxCount;i++)
	{

		//create the rigid body
		boxBody = NewtonCreateBody (nWorld, collision);

		location.MakeIdentity();
		location.SetRotationMatrix(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);

		location._41 = (i%3)*20 -70.f;
		location._43 = 40.f + (i/3)*20;
		NewtonBodySetMatrix (boxBody, (float*)location);


		// 상자 메쉬
		RBoundingBox box;
		box.vmin = -.5f*size;
		box.vmax = .5f*size;
		m_pBoxes[i].SetBox(box);
		GetSceneManager()->AddSceneNode(&m_pBoxes[i]);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (boxBody, &m_pBoxes[i]);

		// set a destructor for this rigid body
	//	NewtonBodySetDestructorCallback (boxBody, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (boxBody, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyForceAndTorque);

		// set the mass matrix
		//NewtonBodySetMassMatrix (boxBody, 1.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f  / 6.0f);
		NewtonBodySetMassMatrix (boxBody, 1.0f, 1.0f, 1.0f, 1.0f);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (boxBody, (float*)location);
		PhysicsSetTransform (boxBody, (float*)location);
	}

	NewtonReleaseCollision (nWorld, collision);
	*/

	m_pRagDollController = new RRagDollSkeletonController(nWorld,PhysicsApplyForceAndTorque);

	m_newton.CreateCollision(&GetSceneManager()->GetRootSceneNode());

	return true;
}

void myApplication::OnRender()
{
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,25,20,20,0x80808080);
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,100,5,5,0xffffffff);

	if(m_nShowBone==1)
	{
		m_pDevice->SetDepthEnable(false,false);
		m_pActor->RenderSkeleton();
		m_pDevice->SetDepthEnable(true);
	}

	// _draw_matrix

	/*
	RMatrix matRUpperArm = bones[1].pSceneNode->GetTransform();
	_draw_matrix(matRUpperArm ,20);	// r-upper-arm

	matRUpperArm = m_pSkeleton->m_matResult[RBIPID_RUPPERARM] * m_pSkeleton->GetTransform();
	_draw_matrix(matRUpperArm ,10);	// r-upper-arm
	*/

}

void myApplication::OnUpdate()
{
	m_newton.Update();

//	m_pRagDollController->UpdateRagDoll();

	sampleApp::OnUpdate();

	RVector cameraPos = m_pCamera->GetPosition();

	char szBuffer[256];
	sprintf_s(szBuffer,"FPS %3.1f camera ( %3.3f, %3.3f, %3.3f ), R = reset , T = begin, B = show bones, P = pick position",
			GetDevice()->GetFrameRate(),cameraPos.x,cameraPos.y,cameraPos.z);
	SetText(szBuffer);

}

void myApplication::OnDestroy()
{
	delete m_pRagDollController;

	m_newton.Destory();

	if(m_pFloor)
	{
		m_pFloor->RemoveFromParent();
		SAFE_DELETE(m_pFloor);
	}
	if(m_pBoxes)
	{
		for(int i=0;i<nBoxCount;i++)
		{
			m_pBoxes[i].RemoveFromParent();
		}
		SAFE_DELETE_ARRAY(m_pBoxes);
	}

	m_pActor->RemoveFromParent();
	SAFE_DELETE(m_pActor);
}

void myApplication::BeginRagDoll()
{
	m_pActor->RemoveController(m_pRagDollController);
	m_pActor->AddController(m_pRagDollController);
}

void myApplication::Reset()
{
	m_pActor->RemoveController(m_pRagDollController);
	m_pActor->SetAnimation("none_walk");
	m_pActor->Stop();
//	m_pRagDollController->DestroyRagDoll();
//	m_pRagDollController->BuildRagDoll();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	_CrtSetBreakAlloc(22698);
	return g_App.Run();
}

