#pragma once

#include "MPathFinder.h"
#include "MPathAgentShape.h"
#include <vector>
#include "MVector3.h"

/////////////////////////////////////////////////////////////////////////////
//

class MAgent;
class MPath;
class MCollisionInfo;
class MContext;
class RHeightField;
class MPathTerrainCallback;
class GPathResMgr;

class GPathFinderImpl
{
	/////////////////////////////////////////////////////////////////////////////
	//

	class MPathFinderError : public iErrorHandler
	{
		virtual eAction handle(const char* type, const char* description, const char *const* attributes); 
	};

	/////////////////////////////////////////////////////////////////////////////
	//


	/////////////////////////////////////////////////////////////////////////////
	//

	class MQueryCallBack : public iQueryCallBack
	{
		tSigned32 _frequency;
		tSigned32 _count;
		tSigned32 __count;
	public:
		void Set(tSigned32 frequency, tSigned32 count)
		{
			_frequency = frequency;
			_count = __count = count;
		}

		void reset() 
		{
			_count = __count;
		}

		tSigned32 desiredCallBackFrequency()
		{
			return _frequency;
		}
		bool abort()
		{
			--_count;
			return _count == 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

public:
	GPathResMgr*			m_pPathResMgr;

	SPathMesh*				GetMesh(int id);
	iMesh*					GetIMesh(int id);
	iShape*					GetShape();
private:

		bool					LoadPathEngine();

		MPathFinderError		m_ErrorHandler;
		
		bool					PreProcessQuery(iMesh* pMesh, iShape* pShape, const MVector3& vPos, cPosition &cPos
												,int nHorizonRange = 0, int nVerticalRange = 10);
		void					BuildPath(iMesh* pMesh, iPath* pPath, vector<MVector3>& ret, int startoffset);

		iPathEngine*			m_pPathEngine;
public:
		MQueryCallBack			m_QueryCallBack;

		bool					Create();
		bool					AddZone(int nID, const wstring& strFilename, RHeightField *pHeightField = NULL);
 
		/**
		좌표의 유효성 검사 및 높이 추출
		 @param	pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
	    */
		PATHENGINE_RESULT_TYPE	IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position);
		PATHENGINE_RESULT_TYPE	IsValidPosition(float *height, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position);

		/**
		좌표의 유효성 검사 및 높이 추출
		*/
		bool IsValidPosition(SPathMesh* pPathMesh, const MVector3& position);

		PATHENGINE_RESULT_TYPE	GetHeightAtPosition(float *height, SPathMesh* pPathMesh, const MVector3& position);
		static PATHENGINE_RESULT_TYPE	GetHeightAtPosition(float *height, SPathMesh* pPathMesh, const cPosition& position);

		/**
		 인근의 유효한 좌표를 찾는다.

		 @param pOut 찾아낸 좌표(3D)를 저장할 MVector3 포인터
		 @param nID Mesh ID
		 @param	pContext	충돌 문맥 ( 이 값이 NULL 이 아니면, context 에 동록된 agent에 대한 충돌검사도 행한다 )
		 @param position 기준 좌표
		 @param nHorizonRange	X,Y 범위(+-)로 인근 좌표를 찾는다. 
		 @param nVerticalRange	Z 범위(+-)로 좌표를 찾는다.
		 @param strShape Shape 스트링, 현재 small/medium/large 세가지

		 @return 찾는데 성공하면 true를 리턴, 실패하면 false를 리턴
		 */
		PATHENGINE_RESULT_TYPE	FindNearPosition(MVector3 *pOut, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position
			,int nHorizonRange, int nVerticalRange);

		// true를 리턴하면 성공적으로 MAgent를 얻어온 것이다.
		bool					NewPlaceAgent(MAgent &agent, const MVector3& position, int nZoneID, SPathMesh* pPathMesh);

		// true를 리턴하면 성공적으로 MContext를 얻어온 것이다.
		bool					NewContext(MContext &context, SPathMesh* pPathMesh);
		
		// PATHENGINE_RESULT_TYPE 을 리턴, SUCCESS = 0 이 아니면 무언가 잘못된 것이다.
		PATHENGINE_RESULT_TYPE	AdvancedAlongPath(MCollisionInfo *pCollisionInfoOut, int nID, MAgent &agent, MPath &path, float fDistance);
		
		// ???
		float					FindPath(SPathMesh* pPathMesh, const MVector3 &vStart, const MVector3 &vEnd, vector<MVector3>& vPath);


		// vCenter 중심 fSize 길이의 정사각형 밖으로 탈출하는 최단경로
		// 반환값 : 경로 길이
		float					FindPathAway(SPathMesh* pPathMesh, const MVector3& vStart, const MVector3& vCenter, float fSize, int nFreq, int nCount, vector<MVector3>& vPath);

public:

		// MPath를 실제적인 3D 정점 배열로 가져온다. 크게 필요 없을 듯 싶다.
		void					PathTo3DPoints(std::vector<MVector3> &out3DPoints, SPathMesh* pPathMesh, MPath &basePath, int offset = 0);
		void					PathTo3DPoint(MVector3 &outPos, SPathMesh* pPathMesh, MPath &basePath, int index);

		GPathFinderImpl();
		~GPathFinderImpl();
		
};
