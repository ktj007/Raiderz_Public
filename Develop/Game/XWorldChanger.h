#pragma once

class XObjectManager;
class XWorld;
class XSensorManager;
class XFieldInfo;

class XWorldChanger
{
protected:
	XFieldInfo*			m_pNextFieldInfo;		// NextFieldInfo는 이 클래스에서 관리한다.
	int					m_nLastFieldZoneID;

	bool IsSameZone();

	void FinalGame_ForChangeField(XObjectManager* pObjectManager, XWorld* pWorld, int nNextFieldID);
	void InitGame_ForChangeField( XObjectManager* pObjectManager, XWorld* pWorld, XSensorManager* pSensorManager, int nNextFieldID, vec3 vMyPos, vec3 vMyDir );

	// Clean -------------------
	void CleanSound();
	void CleanCamera();
	void CleanMyPlayer(XObjectManager* pObjectManager);
	void CleanController();
	void CleanHeaps();
	void SetupNextFieldInfo(int nNewFieldID);

	// Init --------------------
	void InitMyPlayer( XObjectManager* pObjectManager, vec3 vMyPos, vec3 vMyDir );
	void InitRes( int nFieldID );
public:
	XWorldChanger();
	virtual ~XWorldChanger();

	void ChangeField(XObjectManager* pObjectManager, XWorld* pWorld, XSensorManager* pSensorManager, int nNewFieldID, vec3 vMyPos, vec3 vMyDir);
};