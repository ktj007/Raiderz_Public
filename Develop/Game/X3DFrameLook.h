#ifndef X3DFrameLOOK_H
#define X3DFrameLOOK_H

#include "RSceneManager.h"
#include "RAnimationDef.h"
#include "XScreenEffectManager.h"
#include "MBFrameLook.h"

class X3DFrameLook : public MBFrameLook
{
protected:
	string							m_strScreenEffectName;
	string							m_strScreenEffectFileName;

	RSceneManager *					m_pSceneManager;
	XScreenEffectManager *			m_pScreenEffectManager;

public:

protected:
	virtual void					OnDraw(MFrame* pFrame, MDrawContext* pDC );

	bool							CreateReady();

public:
	X3DFrameLook(string strScreenEffectName, string strFileName, RSceneManager * pManager, XScreenEffectManager * pScreenEffectManager);
	virtual ~X3DFrameLook();

	void							ChangeAnimation(string strAnimation, XSCREENEFFECT_ANIMATION_END_ATT aniDel = ANIMATION_END_DEL);
	void							Create();
};

#endif