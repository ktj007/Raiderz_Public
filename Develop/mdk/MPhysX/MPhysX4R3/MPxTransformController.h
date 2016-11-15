#pragma once

class NxActor;
namespace rs3 { class RActor; }
namespace physx
{

class MPxTransformController;
class MPxDynamicObjectEntity;

class MPxTcEntity
{
public:
	MPxTcEntity(){}
	virtual ~MPxTcEntity(){}

	virtual rs3::RActor*				GetActor() = 0;
	virtual MPxTransformController*		GetTransformController(int nId) = 0;
	virtual bool						IsAvailableActor(rs3::RActor* pActor) = 0;

	virtual void						OnDynamicObjectCollided(NxActor* _pNxActor, MPxDynamicObjectEntity* _pDynamicObjEntity) = 0;

protected:
};

class MPxTransformController
{

public:
	MPxTransformController(MPxTcEntity* pTcEntity)
		: m_bEnabled(false)
		, m_pTcEntity(pTcEntity)
	{}
	virtual ~MPxTransformController(){}

	bool				IsEnabled(){ return m_bEnabled; }
	void				SetEnable(bool bEnable){ m_bEnabled = bEnable; }

	virtual void		Update(float fDelta) = 0;
	virtual void		Stop() = 0;

protected:

	rs3::RActor* GetActor()
	{
		if(m_pTcEntity)
			return m_pTcEntity->GetActor();

		return NULL;
	}

	template<typename T>
	T* GetTransformController()
	{
		int nId = T::GetID();
		if (m_pTcEntity && nId > -1)
			return (static_cast<T*>(m_pTcEntity->GetTransformController(nId)));

		return NULL;
	}

	bool IsAvailableActor(rs3::RActor* pActor)
	{
		if (!m_pTcEntity)
			return false;

		return m_pTcEntity->IsAvailableActor(pActor);
	}

	bool			m_bEnabled;
	MPxTcEntity*	m_pTcEntity;
};

template< typename T_Derived >
class MPxTcImpl : public MPxTransformController
{
public:
	MPxTcImpl(MPxTcEntity* pTcEntity) : MPxTransformController(pTcEntity) {}
	virtual ~MPxTcImpl(){}

	static int GetID()
	{
		return m_nID;
	}

	static void SetID(int _nID)
	{
		m_nID = _nID;
	}

private:
	static int m_nID;
};

template <typename T_Derived > int MPxTcImpl<T_Derived>::m_nID = -1;

}