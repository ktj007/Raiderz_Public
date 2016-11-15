/**********************************************************************
 *<
	FILE: MCplug3.h

	DESCRIPTION:	max-plug Utility

	CREATED BY: elhorus

	HISTORY:

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef __MCPLUG2__H
#define __MCPLUG2__H

#include "RTypes.h"

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "polyobj.h"
#include "asciitok.h"

#include "ELMaxMesh.h"
#include "LightList.h"

#include "MCConfig.h"
#include "RsSceneBuilder.h"
#include "RCollisionTreeBuilder.h"

#include "XRef/iXrefObj.h"
#include "XRef/iXRefObjRecord.h"

#include "IGame.h"

#pragma warning ( disable : 4819 4996)

extern ClassDesc2* GetMCplug2Desc();
extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#define VERSION			200					// Version number * 100
#define CFGFILENAME		_T("mcp2.xml")		// Configuration file

class MCRCollisionTreeBuilder : public rs3::RCollisionTreeBuilder
{
public:
	Interface *m_ip;

	virtual void OnUpdateProgress(int, int);
};

class MCplug2 : public SceneExport 
{
	enum ERESULT
	{
		CONTINUE_EXPORT,
		DONT_EXPORT_CHILDREN
	};
public:
	//////////////////////////////////////////////////////////////////////////
	// 전역
	int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file

	void	GetNodeCount(INode* node, int& nodeCount);

	void	ExportGlobalInfo();

	void	ExportPrerequisite(INode* node);

	BOOL	ExportAll(INode* node);

	//////////////////////////////////////////////////////////////////////////
	// Material 관련

	//////////////////////////////////////////////////////////////////////////
	// 메쉬 관련
	void	ExportGeometryObject(INode* node); 
	void	ExportMesh(INode* node, TimeValue t, RMaxMeshNode* pMeshNode); 
	void	ExportStaticMesh(INode* node, TimeValue t); 
	bool	ExportPhysique( ObjectState *os, INode *node , RMaxMeshNode* mesh_node);
	bool	ExportSkin(INode* node,RMaxMeshNode* pMeshNode);

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션 관련
	void	ExportAnimation(INode* node, RMaxMeshNode* mesh_node);
	BOOL	CheckForAnimation(INode* node, BOOL& pos, BOOL& rot, BOOL& scale);
	void	GetRefinedTransform_loc_dir(INode* node, TimeValue t, Matrix3& tmDummyRefined, Matrix3& tmDummyRemains);
	void	GetRefinedTransform_loc(INode* node, TimeValue t, Matrix3& tmDummyRefined, Matrix3& tmDummyRemains);
	void	DumpSRTSample(INode* node, RMaxMeshNode* pMeshNode, BOOL pos, BOOL rot, BOOL scale); 
	void	DumpVisSample(INode* node, RMaxMeshNode* mesh_node);
	void	DumpVertexAni(INode* node, RMaxMeshNode* mesh_node);
	void	DumpMaxAniAABB(INode* node, RMaxMeshNode* mesh_node);

	//////////////////////////////////////////////////////////////////////////
	// 라이트
	void	ExportLight(INode* node);
	void	ExportLightObject(INode* node);

	// 이펙트
	void	ExportEffect( INode* node, RMaxMeshNode* pMeshNode);

	//////////////////////////////////////////////////////////////////////////
	// 헬퍼 오브젝트
	ERESULT	ExportHelperObject(INode* node);
	void	ExportRTTGroup(INode* node);	///< 라이트맵을 위한 이름규칙

	//////////////////////////////////////////////////////////////////////////
	// 기타
	void	ExportProperties();

	//////////////////////////////////////////////////////////////////////////
	// 헬퍼 펑션
	bool	IsXRefRootGroup(INode* node);		// XREF ROOT GROUP 인가
	bool	IsXRefinRecord(IObjXRefRecord *XRecord, string dstNodeName);
	bool	IsXRefinXRef(INode* node);		
	bool	IsStatic(INode* node);
	bool	IsAllStatic(INode* node);		// 하위 노드가 모두 static 인가
	bool	IsXRefExport(INode* node);		// XREF 로 배치되는 노드인가
	bool	IsExportSelected() { return ( exportSelected ? true : false ); }

	//////////////////////////////////////////////////////////////////////////
	// 충돌
	bool	ExportCollision(INode* node, RMaxMeshNode* mesh_node);

	//////////////////////////////////////////////////////////////////////////
	// occluder
	void	ExportOccluder(INode* node, RMaxMeshNode* mesh_node);
	

	//////////////////////////////////////////////////////////////////////////
	// Static Mesh
	bool	SaveStaticMesh(const char* szStaticMeshFileName, int nPolygonPerNodes);
	bool	SaveSceneXML(const char* szEluFileName);
	bool	SaveSceneXML_StaticMeshNode( MXmlElement* pElement, const char* szPureFileName );
	bool	SaveSceneXML_Actor( MXmlElement* pElement, const char* szPureFileName );
	bool	SaveSceneXML_Occluder( MXmlElement* pElement);
	bool	SaveSceneXML_WaterPlane( MXmlElement* pElement );
	void	SaveSceneXML_XRefActor( MXmlElement* pElement );
	bool	SaveSceneXML_Common( MXmlElement* pElement, INode* pNode );


	RCell*		MakeCell(INode* node);
	RPortal*	MakePortal(INode* node);

	//////////////////////////////////////////////////////////////////////////
	// Helper 펑션들

	Point3	GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	MPlane	GetPolygonPlane(MNMesh& mm, int iFace);
	bool	CheckCoplanarAndConvex(MNMesh& mm, int iFace);	///< 한평면위의 convex 인지 판단한다
	BOOL	TMNegParity(Matrix3 &m);
	TCHAR*	FixupName(TCHAR* name);
	void	ConvertToD3DXMatrix(D3DXMATRIX& outD3DMat, const Matrix3 &m);
	void	ConvertToMMatrix(MMatrix& outMatrix, const Matrix3 &tm );
	TriObject* GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
	rpolygon* MergePolygon( vector<rpolygon*>& rpolygons );

	BOOL	IsKnownController(Control* cont);

	TSTR	GetCfgFilePath();
	TSTR	GetCfgFilename();
	bool	GetUserPropertyString(INode* node, const char* szPropertyName, string* pOutString);
	bool	GetUserPropertyString(INode* node, const char* szPropertyName, bool* pBool);

	inline int			GetKeyFrameStep()			{ return m_nKeyFrameStep; }
	inline TimeValue	GetStaticFrame()			{ return nStaticFrame; }

	inline void SetKeyFrameStep(int val)			{ m_nKeyFrameStep = val; }
	inline void SetStaticFrame(TimeValue val)		{ nStaticFrame = val; }

	MCConfig	m_options;

	//////////////////////////////////////////////////////////////////////////
	// MAX Plug-in 관련
	static HWND hParams;

	MCplug2();
	~MCplug2();	

	int				ExtCount();				// Number of extensions supported 
	const TCHAR *	Ext(int n);				// Extension #n (i.e. "ASC")
	const TCHAR *	LongDesc();				// Long ASCII description (i.e. "Ascii Export") 
	const TCHAR *	ShortDesc();			// Short ASCII description (i.e. "Ascii")
	const TCHAR *	AuthorName();			// ASCII Author name
	const TCHAR *	CopyrightMessage();		// ASCII Copyright message 
	const TCHAR *	OtherMessage1();		// Other message #1
	const TCHAR *	OtherMessage2();		// Other message #2
	unsigned int	Version();				// Version number * 100 (i.e. v3.01 = 301) 
	void			ShowAbout(HWND hWnd);	// Show DLL's "About..." bowx

	BOOL			SupportsOptions(int ext, DWORD options);
	inline Interface*	GetInterface()		{ return ip; }

private:
	BOOL		exportSelected;

	TimeValue	nStaticFrame;
	int			m_nKeyFrameStep;
	int			m_nBeginFrame;

	// custom sampling 구간
	int			m_nCustomSamplingStart;
	int			m_nCustomSamplingEnd;
	int			m_nCustomSamplingInterval;
	bool		m_bCustomSampling;

	INode*		m_pBip01Node;			///< bip01 node
	INode*		m_pDummyLocNode;		///< dummy_loc node

	enum DUMMY_LOC_TYPE
	{
		DUMMY_LOC_NONE,
		DUMMY_LOC,
		DUMMY_LOC_DIR,
		DUMMY_LOC_MANUAL
	};

	DUMMY_LOC_TYPE	m_eDummyLocType;

	///////////////////////////////////////////////////////////////

	IGameScene* m_pIGame;
	Interface*	ip;

	FILE*		pStream;
	FILE*		pDebug;

	char		m_szFileName[_MAX_PATH];

	int			m_nTotalNodeCount;
	int			m_nCurNode;

	RMaxMesh	m_meshList;
	RsSceneBuilder	m_sceneBuilder;
	RCell*		m_pGlobalCell;
	RCell*		m_pCurrentCell;

	MCRCollisionTreeBuilder	m_collisionTreeBuilder;

	vector<INode*> m_waterList;

	vector<INode*> m_XRefList;

	EffectList		m_effectList;
	LightList		m_lightList;
};

// PointSet에 중복되지 않게 Point를 저장하기 위한 비교연산 
template<class _Ty>	struct MVector3_less		: public binary_function<_Ty, _Ty, bool>
{	// functor for operator<
	bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	// apply operator< to operands
			 if( _Left.x < _Right.x ) return true;
		else if( _Left.x > _Right.x ) return false;
		else if( _Left.y < _Right.y ) return true;
		else if( _Left.y > _Right.y ) return false;
		else if( _Left.z < _Right.z ) return true;
		else if( _Left.z > _Right.z ) return false;

		return false;
	}
};

const Matrix3 matNegative = ScaleMatrix(Point3 ( -1.0f, -1.0f, -1.0f )); 

#endif // __MCPLUG2__H
