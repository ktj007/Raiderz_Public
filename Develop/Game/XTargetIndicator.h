#pragma once

class XTargetCollisionBox;
class XMouseInteractor;
struct stTargetID;

class XTargetIndicator
{
protected:
	bool					m_bShow;		///< 인디케이터 이펙트 보여줄지 여부

	XTargetCollisionBox*	m_pTargetCollisionBox;
	XMouseInteractor*		m_pMouseInteractor;

	void		_UpdateTarget();
	void		_UpdateTargetInfo();
	void		_UpdateTargetIndicatorVisible();

public:
	XTargetIndicator(XObject* pObject);
	virtual ~XTargetIndicator(void);

	void		Init();
	void		OnUpdate();
	void		TargetHide();
	void		ChangeIndicatorEffectPos();
	void		SetShow(bool bShow=true);
};
