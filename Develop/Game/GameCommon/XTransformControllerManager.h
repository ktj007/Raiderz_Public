#pragma once

#include "XCharacter.h"
#include "XTransformController.h"

class XTransformControllerManager
{
protected:
	XCharacter*							m_pActor;

	typedef map<TRANSFORM_CONTROLLER_LIST, XTransformController*> MAP_TC;
	MAP_TC								m_mapTransformController;

	bool								m_bUpdated;

private:
	void								TransformActorNode(vector<tstring>& vecList, vector<RActorNode*>& vecOutUpdate);
	void								UpdateActiveNode(vector<RActorNode*> vecUpdate);

public:
	XTransformControllerManager(XCharacter* pCharacter);
	virtual ~XTransformControllerManager();

	template<typename T> 
	T*									Add()
	{
		XTransformController* pFindTC = GetTransformController(T::GetID());
		if(pFindTC != NULL)
			return static_cast<T*>(pFindTC);

		T* pTC = new T(m_pActor);
		m_mapTransformController.insert( MAP_TC::value_type(T::GetID(), pTC));

		return pTC;
	}

	void								Destroy();
	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);
	virtual void						PostUpdate(float fDelta);

	XCharacter*							GetActor()				{ return m_pActor; }
	XTransformController*				GetTransformController(TRANSFORM_CONTROLLER_LIST nID);
	template<typename T> T*				GetTransformController(){ return static_cast<T*>( GetTransformController(T::GetID()) ); }

	bool								IsUpdated()				{ return m_bUpdated; }
	void								ReSetUpdated();
};
