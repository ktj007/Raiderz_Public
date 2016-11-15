#pragma once

#include <map>
#include <deque>
#include "RMeshNode.h"
#include "MBuffer.h"
#include ".\Profile\profile.h"
#include "RMeshProfileInfo.h"

namespace rs3 {

enum PROFILE_POLYGON_TYPE {
	PPT_TREELEAF = 0,
	PPT_TREEFROND,
	PPT_TREEBRANCH,
	PPT_TREEBILLBOARD,
	PPT_GRASS,
	PPT_AABBTREE,
	PPT_TERRAIN,
	PPT_PARTICLE,
	PPT_ACTOR,
	PPT_ACTORPHYSIQUE,
	PPT_WATER,
	PPT_RS3DEBUG,
	PPT_MINT,
	PPT_FONT,

	PPT_END
};


const char* const PPT_NAME[ PPT_END ] = {
	"treeleaf",
	"treefrond",
	"treebranch",
	"treebillboard",
	"grass",
	"aabbtree",
	"terrain",
	"particle",
	"actor",
	"actorphysique",
	"water",
	"rs3debug",
	"mint",
	"font"
};


// 하나의 시간 그래프 노드
struct TIMEPROFILENODE
{
	bool	bVisible;
	DWORD	dwColor;
	DWORD	dwCurrentTime;
	deque< unsigned int > m_queueTime;
};

const int FRAMETIME_RING_BUFFER_COUNT = 400;
const int RP_TOGGLE_ALL = -1;

// RealSpace3 의 프로파일링 정보를 수집하는 분석자
// 실제 수집할 정보의 종류에 따라 GPUAnalyze, CPUAnalyze, ResourceAnalyze 로 나뉜다.
class RS_API RResourceProfiler
{
public:
	RResourceProfiler();
	~RResourceProfiler();

	void			Reset();
	void			RenderTimeGraph(RFont* pFont);
	void			RenderTimeGraph(RFont* pFont, int x, int y);
	// polygon profile functions
	unsigned int	GetTypeCount();
	const char*		GetTypeName(PROFILE_POLYGON_TYPE type);

	void			AddPolygon(PROFILE_POLYGON_TYPE type, int nCount);
	unsigned int	GetPolygonCount(PROFILE_POLYGON_TYPE type);
	unsigned int	GetTotalPolygons();

	void			AddDrawCall(PROFILE_POLYGON_TYPE type, int nCount);
	unsigned int	GetDrawCallCount(PROFILE_POLYGON_TYPE type);
	unsigned int	GetTotalDrawCalls();


	void			StartMeshProfiler();
	void			AddMeshProfileInfo(RMeshNode* pMeshNode, int nSubMaterialIndex);
	void			CountMeshInstance(SCENE_SET* pActorList);
	void			GetMeshProfileInfoBinary(MBuffer* pBuffer);	


	// time profile functions
	void			UpdateTimeQueue(const char* szName, unsigned int nTime);
	void			UpdateTimeProfileNodes();
	void			ProcessDebugKey(int nKey);


	void			AddExcludedByFogCulling() { m_nExcludedByFogCulling++; }
	unsigned int	GetExcludedByFogCullingCount() { return m_nExcludedByFogCulling; }
	void			AddExcludedByDistanceCulling() { m_nExcludedByDistanceCulling++; }
	unsigned int	GetExcludedByDistanceCullingCount() { return m_nExcludedByDistanceCulling; }

protected:
	void			TogglePauseTimeGraph();
	bool			GetPausedTimeGraph();
	void			ToggleVisible(int nIndex);

	// 폴리곤 수
	unsigned int m_nPolygonsCurrent[PPT_END];
	unsigned int m_nPolygonsLast[PPT_END];

	// DP call 수
	unsigned int m_nDPcallsCurrent[PPT_END];
	unsigned int m_nDPcallsLast[PPT_END];

	// 시간 큐
	typedef map<string, TIMEPROFILENODE*> TIMEPROFILENODEMAP;
	TIMEPROFILENODEMAP	m_timeProfileNodes;
	bool				m_bPauseTimeGraph;

	//Material 정보
	typedef map<RMesh*, RMeshProfileInfo> MESHPROFILEINFOMAP;
	MESHPROFILEINFOMAP m_MeshProfileInfoCurrent;
	MESHPROFILEINFOMAP m_MeshProfileInfoLast;

	// 컬링에서 걸러진 수
	unsigned int m_nExcludedByFogCulling;
	unsigned int m_nExcludedByDistanceCulling;

private:
	TIMEPROFILENODE*	GetTimeProfileNode(const char* szName);
	void				RenderGuideLines(RFont* pFont, int x, int y, bool bBeginFont=true);
	void				RenderGraph(TIMEPROFILENODE* pNode, int x, int y);

	CProfileNode*		m_pProfileNode;
};

//////////////////////////////////////////////////////////////////////////
// inline functions
inline unsigned int	RResourceProfiler::GetTypeCount() { return PPT_END; }
inline const char*	RResourceProfiler::GetTypeName(PROFILE_POLYGON_TYPE type) { return PPT_NAME[type]; }

inline void			RResourceProfiler::AddPolygon(PROFILE_POLYGON_TYPE type, int nCount) { m_nPolygonsCurrent[type]+=nCount; }
inline unsigned int	RResourceProfiler::GetPolygonCount(PROFILE_POLYGON_TYPE type) { return m_nPolygonsLast[type]; }

inline void			RResourceProfiler::AddDrawCall(PROFILE_POLYGON_TYPE type, int nCount = 1) { 
	m_nDPcallsCurrent[type]+=nCount; 
}
inline unsigned int	RResourceProfiler::GetDrawCallCount(PROFILE_POLYGON_TYPE type) { return m_nDPcallsLast[type]; }

inline void			RResourceProfiler::TogglePauseTimeGraph() { m_bPauseTimeGraph = !m_bPauseTimeGraph; }
inline bool			RResourceProfiler::GetPausedTimeGraph()	{ return m_bPauseTimeGraph; }

//#ifdef DEBUG
class RS_API TimeProfileHelper
{
	DWORD m_dwStartTime;
	const char* m_szName;
public:
	TimeProfileHelper( const char * name );
	~TimeProfileHelper( void );
};

#define	TIMEPROFILE( name )			TimeProfileHelper __timeprofile( name )

#define TIMEPROFILE_THIS_FUNCTION	TIMEPROFILE(__FUNCTION__)

}