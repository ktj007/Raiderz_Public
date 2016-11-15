#pragma once

#include "RTimer.h"
#include "RTypes.h"
#include "RResource.h"
#include "RListener.h"
#include "REffectBase.h"
#include "RSceneNodeController.h"

#include <vector>
#include <string>

namespace rs3 {

RS_API extern const char *RSID_EFFECT;

class RCameraSceneNode;

class REmitter;
class RTrail;
class REffectActor;
class REffectInstanceSceneNode;
class RActorNodeConstraintController;

enum EFFECT_RENDERABLE_TYPE {
	ERT_EMITTER = 0,
	ERT_TRAIL,
	ERT_ACTOR,
	RENDERABLE_TYPE_NUM,
};

class RS_API REffectSceneNode :	public REffectBase, public RResource::LoadingController
{
	friend class REffectInstanceSceneNode;

public:
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_EFFECT);

	static void				ControlEffectSceneNodeLife( REffectSceneNode** ppEffectSceneNode );

protected:
	RTimer					m_Timer;
	float					m_fElapsedTime;

	bool					m_bLastFramePause;

	bool					m_bDeleteNow;
	bool					m_bOncePlayed;

public:
	REffectSceneNode(void);
	virtual ~REffectSceneNode(void);

	string					m_strFileName;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//

	virtual RSceneNode*		Clone();

	virtual void			OnRemove();

	void					SetLastFramePause(bool bLastFramePause)		{ m_bLastFramePause = bLastFramePause; }
	void					SetAlignTarget( RSceneNode* pParent );

	virtual void			Start() override;
	virtual void			Stop() override;
	virtual void			Pause() override;
	virtual bool			IsAlive() override { return false; } // 이펙트 씬노드는 라이프 체크를 하지 않아야 하기 때문에 항상 false를 반환한다.
	virtual	int				GetCurrentCount();

	bool					IsLooping();
	void					SetLooping(bool bLoop);

	void					UpdateRenderableNodesList();
	virtual bool			IsLoadingCompleted();

	// Effect Children Getter
	typedef std::pair<REffectBase*, REffectActor* > EFFECT_NODETYPE;

	std::vector< std::string >	GetChildrenNameTable();
	EFFECT_NODETYPE				GetChild( const char* pSzName );

protected:
	class REffectSceneNodeLifeControlVisitor : public RSceneNodeVisitor
	{
	public:
		REffectSceneNodeLifeControlVisitor() : m_bDeleteEffect( false ){}
		virtual ~REffectSceneNodeLifeControlVisitor(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}

		bool m_bDeleteEffect;
	};
	friend class REffectSceneNodeLifeControlVisitor;

	class REffectSceneNodePlayControlVisitor : public RSceneNodeVisitor
	{
	public:
		REffectSceneNodePlayControlVisitor() : m_ePlayType(PLAY_NONE), m_pRequestEffectSceneNode( NULL ){}
		virtual ~REffectSceneNodePlayControlVisitor(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}

		enum CONTROL_TYPE
		{
			PLAY_NONE = 0,
			PLAY_START,
			PLAY_STOP,
			PLAY_PAUSE
		} m_ePlayType;
		RSceneNode* m_pRequestEffectSceneNode;

	} m_EffectSceneNodePlayControlVisitor;

	class REffectSceneNodeLoopSetterVisitor : public RSceneNodeVisitor
	{
	public:
		REffectSceneNodeLoopSetterVisitor(RSceneNode* pRequestEffectSceneNode, bool bLoop)
			: m_pRequestEffectSceneNode(pRequestEffectSceneNode), m_bLoop(bLoop)
		{
		}
		virtual ~REffectSceneNodeLoopSetterVisitor(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}

		RSceneNode*	m_pRequestEffectSceneNode;
		bool		m_bLoop;
	};

	class REffectSceneNodeLoopGetterVisitor : public RSceneNodeVisitor
	{
	public:
		REffectSceneNodeLoopGetterVisitor(RSceneNode* pRequestEffectSceneNode)
			: m_pRequestEffectSceneNode(pRequestEffectSceneNode), m_bLoopResult(false)
		{
		}
		virtual ~REffectSceneNodeLoopGetterVisitor(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}

		RSceneNode* m_pRequestEffectSceneNode;
		bool		m_bLoopResult;
	};

	class REffectSceneNodeChildrenGetterVisitor : public RSceneNodeVisitor
	{
	public:
		explicit REffectSceneNodeChildrenGetterVisitor(const char* pSzName, RSceneNode* pRequsetSceneNode)
			: m_pSzChildName(pSzName)
			, m_pRequsetSceneNode(pRequsetSceneNode)
		{}
		virtual ~REffectSceneNodeChildrenGetterVisitor(){}

		virtual bool OnPreOrderVisit(RSceneNode* pSceneNode );
		virtual void OnPostOrderVisit(RSceneNode* pSceneNode ){}

		std::vector< std::string >		m_vecNameTable;
		std::vector< REffectActor* >	m_vecEffectActorChildren;
		std::vector< REffectBase* >		m_vecEffectChildren;

		const char*						m_pSzChildName;
		RSceneNode*						m_pRequsetSceneNode;
	};

	// Target에대해 정렬하는 씬노드컨트롤러입니다.
	// 그런데, Target에 대한 포인터 무효화가 발생할수 있어서 일반적인 씬노드에 적용을 못했습니다. pok.
	class REffectAlignController : public RSceneNodeController
	{
	public:
		DECLARE_NODE_ID_STRING("EffectAlignController");

		REffectAlignController(void) : m_pAlignReferenceSceneNode( NULL ){}
		virtual ~REffectAlignController(void){}
		virtual void Update(RCameraSceneNode* pCamera) override;
		RSceneNode* m_pAlignReferenceSceneNode;
	} m_EffectAlignController;

public:
	static RVector		m_vGravity;
	static RVector		m_vWindForce;
};

} //namespace rs3
