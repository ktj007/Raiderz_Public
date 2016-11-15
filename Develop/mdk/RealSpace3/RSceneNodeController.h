#pragma once

// RSceneNodeController
// TODO : RActor 의 RActorContrller 와 통합예정

namespace rs3 {

class RSceneNode;
class RCameraSceneNode;

enum RCONTROLLER_ATTRIBUTE : DWORD
{
	RCA_STICKY			= 1,			///< 저장될때 같이 저장되어야 하는지
	RCA_MANAGED			= 1<<1			///< 신노드가 지워질때 같이 지워져야 하는지
};

class RS_API RSceneNodeController : public RHaveAttributes< RCONTROLLER_ATTRIBUTE >
{
	friend			RSceneNode;
protected:
	RSceneNode*		m_pSceneNode;
	bool			m_bEnable;
	bool			m_bManaged;	///< 신노드 삭제시 같이 삭제
	RSNCTRL_TYPE	m_type;
	string			m_strControllerName;

	void			SetAutoUpdate( RSceneNode* pSceneNodeToSet, bool bUseAutoUpdate );
	bool			IsAutoUpdate( RSceneNode* pSceneNode );

	virtual void	OnAdd(){}
	virtual void	OnRemove(){}

	virtual void	OnEnable(){}
	virtual void	OnDisable(){}

public:
	RSceneNodeController(RSNCTRL_TYPE eType = RSNCTRL_UPDATE);
	virtual ~RSceneNodeController(void);

	DECLARE_NODE_ID_STRING("unknown_controller");

	virtual bool	CreateFromXML(MXmlElement &element) { return false; }
	virtual void	SaveToXML(MXmlElement &element) { }

	virtual			RSceneNodeController* Clone() { return NULL; }

	void			Enable(bool bEnable);
	bool			IsEnable()	{ return m_bEnable; }
	RSNCTRL_TYPE	GetType()	{ return m_type; }

	virtual void	Update(RCameraSceneNode* pCamera /*= NULL*/) { }

	virtual void	ReNew() { }

protected:
	void			SetWorldTransform(const RMatrix& matWorld);		///< 신노드의 world transform 을 설정
	void			SetVisibilityResult( float fVisibilityResult ); ///< 신노드의 Visibility Result 값을 설정
	void			SetNoiseRefResult( float fNoiseRefResult );		///< 신노드의 NoiseRef Result 값을 설정
};

//////////////////////////////////////////////////////////////////////////
// inlines

inline void RSceneNodeController::Enable(bool bEnable) 
{ 
	if(bEnable) OnEnable(); else OnDisable(); 
	m_bEnable = bEnable;
}

}