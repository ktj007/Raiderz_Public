#pragma once

class XInteractor;
class XMouseInteractor;

class XInteraction
{
public:
	XInteraction(XModuleMyControl* pOwner);
	virtual ~XInteraction(void);

	void			Update(float fDelta);

	bool			Interact();
	bool			InteractWithPlayer(MUID uidTarget);
	bool			InteractWithNPC( XObject* pObj );
	bool			Interact_WithClickingMessageBox();
	bool			PCInteract();

	XEventResult	Event( XEvent& msg );

protected:
	XModuleMyControl*	m_pOwner;
	XInteractor*		m_pInteractor;
	XMouseInteractor*	m_pMouseInteractor;

	MUID			GetInteractionSearchUID();
	void			Loot(XNonPlayer* pTargetNPC);
	bool			CheckVSPlayer();

};
