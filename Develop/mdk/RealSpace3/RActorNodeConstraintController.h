#pragma once

#include "RActorController.h"

namespace rs3 {

RS_API extern const char *RCID_ACTORNODECONSTRAINT;

class RActorNode;

// 해당 SceneNode 를 ActorNode 의 child 처럼 붙여준다
class RS_API RActorNodeConstraintController : public RActorController
{
public:
	RActorNodeConstraintController();
	virtual ~RActorNodeConstraintController();

	DECLARE_NODE_ID_STRING( RCID_ACTORNODECONSTRAINT );

	virtual bool	CreateFromXML(MXmlElement &element);
	virtual void	SaveToXML(MXmlElement &element);

	virtual RSceneNodeController* Clone();

	virtual void OnAdd() override;
	virtual void OnRemove() override;

	virtual void OnEnable() override;

	virtual void Update(RCameraSceneNode* pCamera) override;

	void	Invalidate();
	void	SetIgnoreActorNodeDirection(bool bIgnore);
	void	SetNeedUpdateParentAnimation(bool bNeedAnimation);
	void	SetTargetActorNode(const string& strTargetName);

	const RActorNode* GetTargetActorNode();

private:
	void		ForceUpdateWorldTransform();
	void		ForceUpdateVisibilityResult();
	void		ForceUpdateNoiseRefResult();

private:
	string		m_TargetActorNodeName;
	RActorNode* m_pTargetActorNode;
	bool		m_bIgnoreActorNodeDirection;
	bool		m_bNeedUpdateParentAnimation;
};

//////////////////////////////////////////////////////////////////////////
inline const RActorNode* RActorNodeConstraintController::GetTargetActorNode() { return m_pTargetActorNode; }
//////////////////////////////////////////////////////////////////////////
inline void	RActorNodeConstraintController::SetIgnoreActorNodeDirection(bool bIgnore)	{ m_bIgnoreActorNodeDirection = bIgnore; }
//////////////////////////////////////////////////////////////////////////
inline void	RActorNodeConstraintController::SetNeedUpdateParentAnimation(bool bNeedAnimation)	{ m_bNeedUpdateParentAnimation = bNeedAnimation; }
//////////////////////////////////////////////////////////////////////////
inline void	RActorNodeConstraintController::SetTargetActorNode(const string& strTargetName)		{ m_TargetActorNodeName = strTargetName; }

// Helper Function
RS_API RActorNodeConstraintController* GetFirstActorNodeConstraintController( RSceneNode* pSceneNode );

}