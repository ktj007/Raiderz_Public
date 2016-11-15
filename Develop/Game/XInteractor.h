#pragma once

class XInteractor
{
private:
	XObject*		m_pOwner;
	XRegulator		m_TargetNPCRegulator;

	enum _CheckLootingResult
	{
		CLR_FALSE,
		CLR_TRUE,
		CLR_EXIST_RECENTLY
	};

	void	Init(XObject* pOwner);

	void	UpdateTarget(float fDelta);

	bool	CheckInteractionPC();
	bool	CheckInteractionNPC();
	bool	CheckInteractionGather();

	bool	CheckBaseLootable(XNonPlayer* pTargetNPC);
	_CheckLootingResult	CheckLootingInteractionObject();

	void	CheckInteractionEnd();
public:
	XInteractor() {}
	XInteractor(XObject* pOwner);
	virtual ~XInteractor() {}

	void	Update(float fDelta);

	XObject* FindInteractionPC();
};
