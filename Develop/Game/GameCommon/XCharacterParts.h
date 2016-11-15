#pragma once

#include "RActor.h"
#include "CSItemData.h"
#include "XModelParts.h"

using namespace rs3;

class XCharacter;
class XCharacterParts;
class XCoverMeshList;

class XPartsLoadingController : public RResource::LoadingController
{
private:
	XCharacterParts* m_pPartsSlots;
public:
	XPartsLoadingController( XCharacterParts* pPartsSlots ) : m_pPartsSlots( pPartsSlots ) {}

	virtual int	CalcPriority();
	virtual void OnLoadingComplete(RResource*);
};

class XCharacterParts
{
private:
	XPartsLoadingController*	m_pPartsLoadingController;
	XCoverMeshList*				m_pCoverMeshList;
	XModelPartsInfo				m_PartsInfo;


protected:
	XCharacter*					m_pCharacter;

	bool						m_bUsingBackgroundLoad;

private:
	void ClearParts();
	void DeletePartsNodes( XPartInfo* pSlot );
	
	bool DeleteActorNode( const TCHAR* szActorNodeName, bool bNotifyToController = true );
	RActorNode* CreatePartsNode( const TCHAR* szMeshName, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName );

	void CompletePartsLoading( XPartInfo* pPart );
	void UpdatePartsListForCreatingMesh();
	void UpdatePartsLoading();
	void CancelFromLoadingQueue( int nPartsID );
protected:
	virtual void ReleaseMesh( RMesh* pMesh );
	virtual RMesh* CreateMesh( const TCHAR* szMeshName );
	virtual RActorNode* CreatePartsNode( RMesh* pMesh, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName, bool bNotifyToController = true );

public:
	XCharacterParts(XCharacter* pCharacter);
	virtual ~XCharacterParts();

	void Update();
	void UpdateForNoneBackgroundIfAllMeshLoaded();
	void Destroy();

	void CreatePartsNodeAsync( XPartsParam* pPartsParam );
	void DeleteParts( int nSlotID );

	bool IsLoadingCompleted();
	tstring GetReservedSlotMeshName( int nSlotID );

	vector<XPartsNodeInfo>* GetPartsActorNode(int nSlotID);
};
