#ifndef _XMYACTION_DEACTIVATIVE_H
#define _XMYACTION_DEACTIVATIVE_H

#include "XMyActionState.h"
#include "XMyActionDieTransChecker.h"

struct stDeactiveData
{
	wstring			strBuffAnimation;
	int				nBuffID;
};

class XMyActionDeactivative : public XMyActionState
{
private:
	float		m_fElapsed;
	int			m_nBuffID;

	// 사용중인 애니메이션 이름
	wstring		m_strAnimationName;

	bool		m_bCanUseTalent;

	XMyActionDieTransChecker m_ActionDieTransChecker;
public:
	XMyActionDeactivative( XModuleMyControl* pOwner);

	virtual bool			CheckEnterable( void* pParam);
	virtual void			Enter( void* pParam =NULL);

	virtual bool			CheckExitable( int nNextStateID);
	virtual void			Exit();

	virtual XEventResult	Event( XEvent& msg);
	virtual void			Update( float fDelta);

	virtual int				GetID() { return ACTION_STATE_DEACTIVATIVE; }
	virtual	bool			IsPostMovement()	{ return true; }


protected:
	bool					IsActivative();
	bool					IsUseTalent(int nTalentID);

};


#endif
