#pragma once

class CPhysXObjectEntity;
class CControlMediator;

class CPhysXObject
{
public:
								CPhysXObject(const char* _pSzMeshName);
	virtual						~CPhysXObject();

	bool						IsActorLoaded();
	void						Update(CControlMediator* _pControlMediator, float _fTime);

	void						StartBPart(int _idx);

private:
	CPhysXObjectEntity*			m_pEntity;
};
