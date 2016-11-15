#pragma once

class XBPartWeaponController
{
private:
	set<int>			m_setBuffID;

protected:
	bool				m_bWork;

private:
	void				BPartWeaponEffect( int nItemID, XObject* pObject, wstring strEffectName );

public:
	XBPartWeaponController();
	virtual ~XBPartWeaponController() {}

	void				OnStart(XPlayer* pPlayer, int nItemID);
	void				OnEnd(XObject* pObject, int nItemID);
	virtual void		OnUpdate(XObject* pObject);

	bool				CheckBuffLost(XObject* pObject, int nBuffID);

	bool				IsWork()		{ return m_bWork; }
	virtual bool		UsableActionState(int nState, int nParam)			{ return true; }
	virtual bool		BPartsAction(XVirtualKey nVirtualKey)	{ return false; }
};

class XMyBPartWeaponController : public XBPartWeaponController
{
public:
	XMyBPartWeaponController();
	virtual ~XMyBPartWeaponController() {}

	virtual bool		UsableActionState(int nState, int nParam);
	virtual bool		BPartsAction(XVirtualKey nVirtualKey);
};

class XNetBPartWeaponController : public XBPartWeaponController
{
public:
	XNetBPartWeaponController();
	virtual ~XNetBPartWeaponController() {}
};
