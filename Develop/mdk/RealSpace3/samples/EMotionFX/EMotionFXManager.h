#include "EmotionFX/Source/EMotionFX.h"
#include "EmotionFX/Core/Source/MCore.h"
#include "RActor.h"
#include <vector>


using namespace EMotionFX;
using namespace MCore;
using namespace rs3;



class CallBack : public ActorUpdateCallBack
{
public:
	CallBack() : ActorUpdateCallBack() {}
	~CallBack() {}

	void OnUpdateVisibilityFlags(ActorInstance* actorInstance, const float timePassedInSeconds);
	void OnUpdate(ActorInstance* actorInstance, const float timePassedInSeconds);
	void OnRender(ActorInstance* actorInstance, const float timePassedInSeconds){}
};


struct MCollateEMotion
{
	int  MaietIndex;
	int  EmotionFXIndex;
	char partName[32];

	MCollateEMotion(int MaietIndex_, int EmotionFXIndex_, const char* name)
	{
		MaietIndex = MaietIndex_;
		EmotionFXIndex = EmotionFXIndex_;
		strcpy( partName, name);
	}
};


class rs3::RActor;

class MActorInstance
{
public:
	vector<MCollateEMotion>  m_Parts;
	ActorInstance*			 m_ActorInstance;
	vector<Motion*>			 m_Motions;	
	RActor*					 m_RActor;

	MActorInstance(); 
	~MActorInstance();

//////////////////////////////////////////////////////////////////////////
// Animation
//////////////////////////////////////////////////////////////////////////
	bool		LoadMotion(char* name);
	bool		PlayMotion(char* name, PlayBackInfo* setting = NULL);
	void		StopMotion();
	bool		Create(char* name, RActor* maietActor);
	void		UpdateAnimation();

//////////////////////////////////////////////////////////////////////////
// IK
//////////////////////////////////////////////////////////////////////////
public:
	void		CreateSolver(char* startNodeName, char* endNodeName);
	void		SetIKGoal(RVector pos);
	void		StartIK();
	void		StopIK();
	void		SetWeight(float weight = 1, float blendTimeInSeconds = 0);
private:
	CCDIKSolver*			m_IKSolver;

//////////////////////////////////////////////////////////////////////////
// LookAt
//////////////////////////////////////////////////////////////////////////
#define LOOKAT_BONE_COUNT 3
public: 
	void		SetupLookAtControllers();
	void		UpdateLookAt(RVector& goalPos);
	void		SetLookAtSpeed(float speed);
	void		AtivateLookAt(float fadeinTime = 0.3f);
	void		DeactivateLookAt(float fadeinTime = 0.3f);
private:
	LookAtController*	m_LookAtController[3];
	int					m_headNodeIndex;
	float				m_LookAtSpeed;
};




class EMotionFXMgr
{
public:
	bool	Init();
	void	Shutdwon();
	bool	LoadMesh(char* name, bool loadGeometryLod = false, bool LoadProgMorphTarget = false);
	bool	BindActor(RActor* Ractor, MActorInstance* actorInstance, char* actorName);
	Actor*	GetActor(char* name);

	static EMotionFXMgr*	GetInstance();

private:
	vector<Actor*>	m_Actors;
};


#define GET_EMOTION_MGR (*EMotionFXMgr::GetInstance())


Vector3	ConvertToEMotionVector(RVector& vector);	
RMatrix	ConvertToMaietMatrix(TMatrix<float> matrix);	