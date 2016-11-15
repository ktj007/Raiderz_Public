/// @file C_ControlDoActionImpl.h
#pragma once

#include "C_ControlDoAction.h"

//////////////////////////////////////////////////////////////////////////
//
// Object DoAction
//
//////////////////////////////////////////////////////////////////////////

//! DoAction - ObjectNew
class CORETOOL_SOULHUNT_API CDoActionObjectNewImpl : public CDoActionObjectNew
{
public:
							CDoActionObjectNewImpl(){}
	virtual					~CDoActionObjectNewImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController);

	virtual CLogicObject*	OnGetReferenceObjectAndSetCursor(CControlMediator* _pController, CLogicObject* _pOldReference) override;
	virtual bool			OnCheckValidLocatePosition(CControlMediator* _pController, rs3::RVector& _currentCandidatePos) override;

	virtual void			OnEditSawedObject(CControlMediator* _pController, CLogicObject* _pSawedObject) override;
	virtual void			OnObjectCloned(CControlMediator* _pController, CLogicObject* _pSawedObject) override;
};

//! DoAction - ObjectSelect
class CORETOOL_SOULHUNT_API CDoActionObjectSelectImpl : public CDoActionObjectSelect
{
public:
							CDoActionObjectSelectImpl(){}
	virtual					~CDoActionObjectSelectImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController){ return false; }

	virtual void			OnSelectChanged(CControlMediator* _pController, bool _IsEntity);
};

//! DoAction - ObjectMove
class CORETOOL_SOULHUNT_API CDoActionObjectMoveImpl : public CDoActionObjectMove
{
public:
							CDoActionObjectMoveImpl(){}
	virtual					~CDoActionObjectMoveImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController);

	virtual void			OnMove( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, const rs3::RVector3& _moveVec, CGroupedCommandBuffer* _pCmdBufferForActionEntered );
};

//! DoAction - ObjectRotate
class CORETOOL_SOULHUNT_API CDoActionObjectRotateImpl : public CDoActionObjectRotate
{
public:
							CDoActionObjectRotateImpl(){}
	virtual					~CDoActionObjectRotateImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController){ return false; }

	virtual void			OnRotate( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, rs3::RMatrix& _tmForRot, UINT _nAxis, CGroupedCommandBuffer* _pCmdBufferForActionEntered );
};

//! DoAction - ObjectScale
class CORETOOL_SOULHUNT_API CDoActionObjectScaleImpl : public CDoActionObjectScale
{
public:
							CDoActionObjectScaleImpl(){}
	virtual					~CDoActionObjectScaleImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController){ return false; }

	virtual void			OnScale( CControlMediator* _pController, ENTITY_OBJECTLIST* _pList, float _fScaleFactor, CGroupedCommandBuffer* _pCmdBufferForActionEntered );
};

//////////////////////////////////////////////////////////////////////////
//
// Etc DoAction
//
//////////////////////////////////////////////////////////////////////////

//! DoAction - EtcPathSelect
class CORETOOL_SOULHUNT_API CDoActionEtcPathSelectImpl : public CDoActionEtcPathSelect
{
public:
	CDoActionEtcPathSelectImpl(){}
	virtual ~CDoActionEtcPathSelectImpl(){}

	virtual bool			DoPreAction(CControlMediator* _pController){ return false; }
	virtual void			Act( CControlMediator* _pController );
};
