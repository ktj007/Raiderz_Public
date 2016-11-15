#ifndef _X_TARGET_EFFECT_H
#define _X_TARGET_EFFECT_H

struct EFFECT_RESULT_DATA;
class XDecalEffect;

enum INTERACTION_EFFECT_TYPE
{
	INTERACTION_ENEMY_NPC = 0,			//»¡°­
	INTERACTION_ENEMY_NETPLAYER,
	INTERACTION_PARTY_MEMBER,
	INTERACTION_INTERACTIVE_NPC,		//ÆÄ¶û
	INTERACTION_INTERACTIVE_OBJECT,
	INTERACTION_INTERACTIVE_DISABLE,	//È¸»ö

	INTERACTION_NONE
};

class XInteractionEffect : public RSceneNode
{
protected:
	XObject*				m_pOwner;					///< Àû¿ëµÇ´Â ¿ÀºêÁ§Æ®
	INTERACTION_EFFECT_TYPE	m_eInteractionEffectType;	//< »ö±ò.
	vector<EFFECT_RESULT_DATA>	m_vecDecalEffectData;

protected:
	virtual void	OnUpdate();

	RBoundingBox	GetObjectAABB();

	float			GetInteractionSize(RBoundingBox& rbb);
	vec3			GetInteractionCenter(RBoundingBox& rbb);

	void			SetInteractionPos(XDecalEffect* pDecal);
	void			SetInteractionColor(XDecalEffect* pDecal);

public:
	XInteractionEffect( XObject* pObject, RRENDERPASS eRenderPass = RRP_NORMAL);
	virtual ~XInteractionEffect(void);

	virtual void	Init(wstring strEffectName, vec3 vPos, vec3 vDir, float fSize);
	virtual void	CreateInteractionEffect(INTERACTION_EFFECT_TYPE eType);

	void			Destroy();

	bool			IsEffect();
	void			SetInteractionEffectType(INTERACTION_EFFECT_TYPE eType );
};

#endif // #ifndef _X_TARGET_EFFECT_H
