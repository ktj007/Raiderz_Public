#ifndef _RSBSPEXPORTER_H
#define _RSBSPEXPORTER_H

#pragma warning( disable : 4002 )

#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "max.h"
#include "export.h"

#include "cmlist.h"

#include "RTypes.h"
#include "RMaxMaterial.h"
#include "RsBuilder.h"

#include <list>
#include <vector>
#include <string>
#include <map>

//using namespace std;
using namespace rs3;

typedef CMLinkedList<rpolygon> RSPolygonList;
typedef list<INode*> NODELIST;

class RCell;

/// 두 셀 사이의 문
class RPortal
{
public:
	string		m_strName;
	int			nCellID1;
	int			nCellID2;

	int			nVertices;
	Point3		*m_pVertices;

	// 아직 cellid 를 몰라서 저장해두는것
	string		m_strParentCellName;
	string		m_strTargetCellName;

	// 부모 Cell을 참조하기 위해
	RCell*		m_pParentCell;
};

class RGeometry
{
public:
	DWORD			m_dwFVF;
	RPolygonList	m_sourcePolygons;

	typedef pair<int,int>					MIDLIDPAIR;	// materialID ,lightmapID pair 
	typedef multimap< MIDLIDPAIR , RsBuilder*>	IDBUILDERMAP;
	IDBUILDERMAP	m_trees;

	RGeometry();
	~RGeometry();
	RsBuilder* GetTree( MIDLIDPAIR idPair);
};

struct RCellFace
{
	RCellFace() : nVerts(0), pVerts(NULL) {}
	~RCellFace() { SAFE_DELETE_ARRAY( pVerts ); }

	int			nVerts;
	RVector*	pVerts;
};

/// cell 은 scene의 분할된 공간으로 max에서 폴리곤(평면)으로 정의한다
class RCell
{
public:
	string				m_strName;
	Box3				m_bb;
	list<RPlane>		m_planes;			// 경계를 이루는 평면들
	vector<RCellFace*>	m_cellFaces;

	typedef map<DWORD, RGeometry*>	IDGEOMETRYMAP;
	IDGEOMETRYMAP		m_mapGeometryList;
	
	~RCell();
	RGeometry* GetGeometry(DWORD idFVF);
	bool Contains(const RVector &v) 
	{ 
		for( list<RPlane>::iterator i = m_planes.begin(); i!=m_planes.end(); i++)
		{
            if((*i).GetDistanceTo(v)>=0)
				return false;
		}
		return true; // 모든 평면의 뒤쪽에 있으면 포함이다
	}

	bool IsAlign( int nVertices, const Point3* pVertices )
	{
		for( list<RPlane>::iterator i = m_planes.begin(); i!=m_planes.end(); i++)
		{
			bool bAlign = true;
			for( int j = 0; j < nVertices; j++)
			{
				const Point3& pV = pVertices[j];
				float dist = (*i).GetDistanceTo( RVector(pV.x, pV.y, pV.z) );
				if( !MMath::Equals( 0.0f, dist, 0.1f ) )
				{
					bAlign = false;
					break;
				}
			}

			if( bAlign )
				return true;
		}

		return false;
	}
};

typedef list<RPortal*> RPortalList;

class RCellList : public list<RCell*> 
{
public:
	pair<int, RCell*> Find(const char *szName) {
		int nIndex = 0;
		for( iterator i = begin(); i!=end(); i++,nIndex++) {
			if(strcmp(szName,(*i)->m_strName.c_str())==0)
				return pair<int, RCell*>(nIndex, *i);
		}
		return pair<int, RCell*>(-1, NULL);
	}
};

class RsSceneBuilder  
{
public:
	RsSceneBuilder();
	virtual ~RsSceneBuilder();

	Interface*	ip;

	RVector			m_AmbientLightColor;	
	NODELIST		m_ObjectsList;

	char			MapName[256];
	char			MaxFileName[256];

	RSPolygonList	faceNavigation;
	
//	RPolygonList	m_sourcePolygons;	// 익스포터에서 이리로 폴리곤을 저장한다
	RCellList		m_cells;			// sector : 분할된 공간
	RPortalList		m_portals;			// portal : 공간을 연결하는 문

	bool Build(Interface *ip, RMaxMaterialKeeper* pMaterials, int nPolygonPerNodes);
	bool SaveRS(const char *fileName);
	void SaveRS_Tree(RGeometry* pGeometry, FILE *file);
	void AddCell(RCell*);
	void Destroy();

	bool IsEmpty();				///< 비어있나

private:
	void RelocatePolygons();	// 폴리곤을 적절한 셀에 배치한다
	void ConnectPortals();		// 포탈을 셀와 연결한다
	void ValidatePortals();		// 포탈 데이터 검증

	RVector ViewPoint;
	MPoint	m_lightMapSize;
};

#endif // !defined(AFX_RsSceneBuilder_H__A7395ED0_81F3_11D2_9B5F_00AA006E4A4E__INCLUDED_)
