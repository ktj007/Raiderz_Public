#include "EMotionFXManager.h"



// determine if this actor instance is visible or not
void CallBack::OnUpdateVisibilityFlags(ActorInstance* actorInstance, const float timePassedInSeconds)
{
	// we don't perform visibility checks in the examples, just update and render them always
	actorInstance->SetVisible( true );
}


// update a given actor instance (calculate world space transforms of all nodes etc)
void CallBack::OnUpdate(ActorInstance* actorInstance, const float timePassedInSeconds)
{
	// update the transformation of the actor instance and deform its meshes
	actorInstance->UpdateTransformations( timePassedInSeconds, true);
	actorInstance->UpdateMeshDeformers( timePassedInSeconds );
}



//////////////////////////////////////////////////////////////////////////
//						MActorInstance
//////////////////////////////////////////////////////////////////////////
MActorInstance::MActorInstance() :
m_ActorInstance(NULL),
m_RActor(NULL),
m_IKSolver(NULL), 
m_headNodeIndex(-1),
m_LookAtSpeed(1)
{

}


MActorInstance::~MActorInstance()
{
	m_Parts.clear();
	m_Motions.clear();
	if( m_IKSolver != NULL) delete m_IKSolver;
}


bool MActorInstance::LoadMotion(char* name)
{
	// try to load the motion
	Motion* motion = EMFX_IMPORTER.LoadSkeletalMotion(name);
	if (motion == NULL)
	{
		return false;
	}

	MCore::String strName = name;
	motion->SetName( strName.ExtractFilename() );
	m_Motions.push_back(motion);

	return true;
}



bool MActorInstance::PlayMotion(char* name, PlayBackInfo* setting)
{
	Motion* motion = NULL;

	for( vector<Motion*>::iterator it = m_Motions.begin(); it != m_Motions.end(); it++)
	{
		if( strcmp( (*it)->GetName(), name ) == 0 )
		{	
			motion = (*it);
			break;
		}
	}

	if( setting == NULL)
	{
		m_ActorInstance->GetMotionSystem()->PlayMotion( motion );
	}
	else
	{
		m_ActorInstance->GetMotionSystem()->PlayMotion( motion , setting );
	}
	return true;
}



void MActorInstance::StopMotion()
{
	m_ActorInstance->GetMotionSystem()->StopAllMotions();
}



bool MActorInstance::Create(char* name, RActor* maietActor)
{
	if( GET_EMOTION_MGR.GetActor(name) == NULL || maietActor == NULL)
	{
		return false;
	}

	m_RActor = maietActor;
	m_ActorInstance = EMFX_ACTORMANAGER.CreateInstance( GET_EMOTION_MGR.GetActor(name) );
	return true;
}


void MActorInstance::UpdateAnimation()
{
	for(vector<MCollateEMotion>::iterator it= m_Parts.begin(); it != m_Parts.end() ; it++)
	{
		if(	strcmp( it->partName, "Bip01") ==	0)
			continue;

		RMatrix matTransform = ConvertToMaietMatrix(m_ActorInstance->GetTransformData()->GetLocalTM(it->EmotionFXIndex)); 

		m_RActor->GetActorNodes()[it->MaietIndex]->SetTransform( matTransform );
	}
}


// initialize and add the CCD ik solver
void MActorInstance::CreateSolver(char* startNodeName, char* endNodeName)
{
	// get the start and end node
	Node* endNode   = m_ActorInstance->GetActor()->FindNodeByName( endNodeName);
	Node* startNode = m_ActorInstance->GetActor()->FindNodeByName( startNodeName);

	// create and add the two link ik solver
	m_IKSolver = new CCDIKSolver(m_ActorInstance, startNode->GetNodeIndex(), endNode->GetNodeIndex());
	m_ActorInstance->AddWorldSpaceController( m_IKSolver );

	// activate the controller
	m_IKSolver->Activate( 0.0f );
}



void MActorInstance::StartIK()
{
	m_IKSolver->Activate();
}



void MActorInstance::StopIK()
{
	m_IKSolver->Deactivate();
}


void MActorInstance::SetWeight(float weight, float blendTimeInSeconds)
{
	if( weight < 0 || weight > 1)
	{
		return;
	}

	m_IKSolver->SetWeight(weight, blendTimeInSeconds);
}




void MActorInstance::SetIKGoal(RVector pos)
{
	m_IKSolver->SetGoal( ConvertToEMotionVector(pos) );
}




void MActorInstance::SetupLookAtControllers()
{
	// get corresponding actor data
	Actor* actor = m_ActorInstance->GetActor();

	// we first have to constraint the node which is nearest to the root node, since the next controller
	// will use the already rotated skeleton so we have to use the forward kinematics node order

	// find the first spine node and apply the controller to it
	Node* spine1Node = actor->FindNodeByName("Bip01 Spine1");
	if (spine1Node)
	{
		// create the look-at controller
		// please note that we do NOT have to delete this manually
		// after we have added it to the actor instance, it will get deleted automatically when we delete the actor instance
		m_LookAtController[0] = new LookAtController(m_ActorInstance, spine1Node->GetNodeIndex());

		// set the interpolation speed factor, which is how fast/smooth the node will rotate to point at the goal
		m_LookAtController[0]->SetInterpolationSpeed(0.5f);

		// setup the pre rotation matrix
		Matrix m;
		m.SetRotationMatrixZ(Math::halfPi);
		m_LookAtController[0]->SetPreRotation(m);

		// setup the post rotation matrix
		m.SetRotationMatrixZ(0.0f);
		m_LookAtController[0]->SetPostRotation(m);

		// setup the rotation limits
		Vector3 vmin(-0.4f,-0.40f,-0.07f);
		Vector3 vmax( 0.4f, 0.20f, 0.07f);
		m_LookAtController[0]->SetEulerConstraints(vmin, vmax);

		// use ellipse constraints
		m_LookAtController[0]->SetConstraintType(LookAtController::CONSTRAINT_ELLIPSE);

		// apply the controller to the character's first spine
		// add the look-at controller as world space controller
		m_ActorInstance->AddWorldSpaceController( m_LookAtController[0] );

		// activate the controller
		m_LookAtController[0]->Deactivate();
	}

	// find the second spine node and apply the controller to it
	Node* spine2Node = actor->FindNodeByName("Bip01 Spine2");
	if (spine2Node)
	{
		// create the look-at controller
		// please note that we do NOT have to delete this manually
		// after we have added it to the actor instance, it will get deleted automatically when we delete the actor instance
		m_LookAtController[1] = new LookAtController(m_ActorInstance, spine2Node->GetNodeIndex());

		// set the interpolation speed factor, which is how fast/smooth the node will rotate to point at the goal
		m_LookAtController[1]->SetInterpolationSpeed(0.5f);

		// setup the pre rotation matrix
		Matrix m;
		m.SetRotationMatrixZ(Math::halfPi);
		m_LookAtController[1]->SetPreRotation(m);

		// setup the post rotation matrix
		m.SetRotationMatrixZ(0.0f);
		m_LookAtController[1]->SetPostRotation(m);

		// setup the rotation limits
		Vector3 vmin(-0.4f,-0.25f,-0.05f);
		Vector3 vmax( 0.4f, 0.25f, 0.05f);
		m_LookAtController[1]->SetEulerConstraints(vmin, vmax);

		// use ellipse constraints
		m_LookAtController[1]->SetConstraintType(LookAtController::CONSTRAINT_ELLIPSE);

		// apply the controller to the character's second spine
		// add the look-at controller as world space controller
		m_ActorInstance->AddWorldSpaceController( m_LookAtController[1] );

		// activate the controller
		m_LookAtController[1]->Deactivate();
	}

	// find the head node and apply the controller to it (non case sensitive version)
	Node* headNode = actor->FindNodeByNameNoCase("Bip01 Head");
	if (headNode)
	{
		// store the head node index, as we will use it later on, to only adjust the interpolation speed of the head
		// and not of the eyes
		m_headNodeIndex = headNode->GetNodeIndex();

		// create the look-at controller
		// please note that we do NOT have to delete this manually
		// after we have added it to the actor instance, it will get deleted automatically when we delete the actor instance
		m_LookAtController[2] = new LookAtController(m_ActorInstance, headNode->GetNodeIndex());

		// set the interpolation speed factor, which is how fast/smooth the node will rotate to point at the goal
		m_LookAtController[2]->SetInterpolationSpeed(0.5f);

		// setup the pre-rotation matrix
		Matrix m;
		m.SetRotationMatrixZ( -Math::halfPi );
		m_LookAtController[2]->SetPreRotation( m );

		// setup the post rotation matrix
		m.SetRotationMatrixY( -0.25f );
		m_LookAtController[2]->SetPostRotation( m );

		// setup the rotation limits for each axis
		Vector3 vmin(-1.2f, -0.5f,  0.0f);
		Vector3 vmax( 1.2f,  1.2f,  0.0f);
		m_LookAtController[2]->SetEulerConstraints(vmin, vmax);

		// use ellipse constraints
		m_LookAtController[2]->SetConstraintType(LookAtController::CONSTRAINT_ELLIPSE);

		// apply the controller to the character's head
		// add the look-at controller as world space controller
		m_ActorInstance->AddWorldSpaceController( m_LookAtController[2] );

		// activate the controller
		m_LookAtController[2]->Deactivate();
	}
}



void MActorInstance::UpdateLookAt(RVector& goalPos_)
{
	Vector3 goalPos = ConvertToEMotionVector(goalPos_);

	for( int i= 0 ; i < LOOKAT_BONE_COUNT; i++)
	{
		// set camera position as the look at controller's target
		m_LookAtController[i]->SetGoal( goalPos );

		// adjust the interpolation speed of the head, based on the slider value
		if (m_LookAtController[i]->GetNodeIndex() == m_headNodeIndex)
		{
			m_LookAtController[i]->SetInterpolationSpeed( m_LookAtSpeed  );
		}
	}
}


void MActorInstance::SetLookAtSpeed(float speed)
{
	if( speed < 0 || speed > 1)
	{
		return;
	}

	m_LookAtSpeed = speed;
}


void MActorInstance::AtivateLookAt(float fadeinTime)
{
	for( int i= 0 ; i < LOOKAT_BONE_COUNT; i++)
	{
		m_LookAtController[i]->Activate( fadeinTime );
	}
}


void MActorInstance::DeactivateLookAt(float fadeinTime)
{
	for( int i= 0 ; i < LOOKAT_BONE_COUNT; i++)
	{
		m_LookAtController[i]->Deactivate(fadeinTime);
	}
}



//////////////////////////////////////////////////////////////////////////
//						EMotionFXMgr
//////////////////////////////////////////////////////////////////////////
EMotionFXMgr* EMotionFXMgr::GetInstance()
{
	static EMotionFXMgr instance;
	return &instance; 
}



bool EMotionFXMgr::Init()
{
	if( MCore::Initializer::Init() == false )
	{
		return false;
	}

	MCORE_LOGMANAGER.CreateLogFile("EMotionFXLog.txt");

	if( EMotionFX::Initializer::Init() == false )
	{
		MCore::Initializer::Shutdown();
		return false;
	}

	EMFX_ACTORMANAGER.SetCallBack ( new CallBack() );                    // set it to use our custom callback
	EMFX_ACTORMANAGER.SetScheduler( new SingleProcessorScheduler() );    // can also be MultiProcessorScheduler

	return true;
}


void EMotionFXMgr::Shutdwon()
{
	EMotionFX::Initializer::Shutdown();
	MCore::Initializer::Shutdown();
}


bool EMotionFXMgr::LoadMesh(char* name,
							bool loadGeometryLod, 
							bool LoadProgMorphTarget)
{
	// try to load the actor
	Importer::ActorSettings actorSettings;
	actorSettings.mLoadGeometryLODs		= loadGeometryLod;	// don't load geometry LOD levels
	actorSettings.mLoadProgMorphTargets	= LoadProgMorphTarget;	// don't load progressive morph targets
	Actor*	actor = EMFX_IMPORTER.LoadActor( name, &actorSettings );//Models/Human/Human_WithSword.xac";

	if (actor == NULL)
	{
		return false;
	}
	
	MCore::String strName = name;
	actor->SetName( strName.ExtractFilename() );
	m_Actors.push_back(actor);

	// register the master actor
	// remember that you are NOT allowed to modify the Actor object anymore after doing this!
	EMFX_ACTORMANAGER.RegisterMasterActor( actor );

	return true;
}


Actor* EMotionFXMgr::GetActor(char* actorName)
{
	Actor* actor = NULL;

	for( vector<Actor*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if( strcmp( (*it)->GetName(), actorName ) == 0 )
		{	
			actor = (*it);
			break;
		}
	}

	return actor;
}



bool EMotionFXMgr::BindActor(RActor* Ractor, MActorInstance* actorInstance, char* actorName)
{
	Actor* actor = NULL;

	for( vector<Actor*>::iterator it = m_Actors.begin(); it != m_Actors.end(); it++)
	{
		if( strcmp( (*it)->GetName(), actorName ) == 0 )
		{	
			actor = (*it);
			break;
		}
	}



	if( actor == NULL )
	{
		return false;
	}



	for( int i = 0; i < Ractor->GetActorNodeCount() ;  i++)
	{
		if( actor->FindNodeByName( Ractor->GetActorNodes()[i]->GetName().c_str() ) ==  NULL)
			continue;

		actorInstance->m_Parts.push_back
		(
			MCollateEMotion
			(
				i,
				actor->FindNodeByName( Ractor->GetActorNodes()[i]->GetName().c_str() )->GetID() - 1,
				Ractor->GetActorNodes()[i]->GetName().c_str()
			)
		);

	}

	return true;
}



Vector3	ConvertToEMotionVector(RVector& vector)
{
	return Vector3(vector.x, vector.y , -vector.z);
}


RMatrix	ConvertToMaietMatrix(TMatrix<float> matrix)
{
	RMatrix matTransform = (float*)&matrix;

	RMatrix tempMatrix =matTransform;

	matTransform._31 = tempMatrix._21; 
	matTransform._32 = tempMatrix._22; 
	matTransform._33 = tempMatrix._23; 
	matTransform._34 = tempMatrix._24; 

	matTransform._21 = tempMatrix._31; 
	matTransform._22 = tempMatrix._32; 
	matTransform._23 = tempMatrix._33; 
	matTransform._24 = tempMatrix._34;

	tempMatrix =matTransform;

	matTransform._12 = tempMatrix._13; 
	matTransform._22 = tempMatrix._23; 
	matTransform._32 = tempMatrix._33; 
	matTransform._42 = tempMatrix._43; 

	matTransform._13 = tempMatrix._12; 
	matTransform._23 = tempMatrix._22; 
	matTransform._33 = tempMatrix._32; 
	matTransform._43 = tempMatrix._42;

	return matTransform;
}