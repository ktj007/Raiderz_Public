#include "StdAfx.h"
#include "RProfileInfo.h"
#include "RFont.h"
#include <MMSystem.h>

#include "RMesh.h"
#include "RMaterialResource.h"

namespace rs3 {

RResourceProfiler::RResourceProfiler() : m_bPauseTimeGraph(false)
{
	ZeroMemory(m_nPolygonsCurrent, sizeof(m_nPolygonsCurrent) );
	ZeroMemory(m_nPolygonsLast, sizeof(m_nPolygonsLast) );

	ZeroMemory(m_nDPcallsCurrent, sizeof(m_nDPcallsCurrent) );
	ZeroMemory(m_nDPcallsLast, sizeof(m_nDPcallsLast) );

	m_nExcludedByFogCulling = 0;
	m_nExcludedByDistanceCulling = 0;

	m_pProfileNode = new CProfileNode(0, "Material", NULL, 0);
	m_pProfileNode->Call();
}

RResourceProfiler::~RResourceProfiler()
{
	TIMEPROFILENODEMAP::iterator itr = m_timeProfileNodes.begin();
	for( ; itr!=m_timeProfileNodes.end() ; ++itr)
	{
		delete itr->second;
	}
	m_timeProfileNodes.clear();

	delete m_pProfileNode;
}

void RResourceProfiler::Reset()
{
	memcpy(m_nPolygonsLast, m_nPolygonsCurrent, sizeof(m_nPolygonsCurrent) );
	ZeroMemory(m_nPolygonsCurrent, sizeof(m_nPolygonsCurrent) );

	memcpy(m_nDPcallsLast, m_nDPcallsCurrent, sizeof(m_nDPcallsCurrent) );
	ZeroMemory(m_nDPcallsCurrent, sizeof(m_nDPcallsCurrent) );

	m_nExcludedByFogCulling = 0;
	m_nExcludedByDistanceCulling = 0;

#ifdef _USE_ENGINE_RESOURCE_PROFILE
	m_MeshProfileInfoLast.clear();
	m_MeshProfileInfoLast.insert(m_MeshProfileInfoCurrent.begin(), m_MeshProfileInfoCurrent.end());

	m_MeshProfileInfoCurrent.clear();
#endif
}

unsigned int RResourceProfiler::GetTotalPolygons()
{
	int nSum = 0;
	for(int i=0; i<PPT_END; ++i)
		nSum += GetPolygonCount((PROFILE_POLYGON_TYPE)i);

	return nSum;
}

unsigned int RResourceProfiler::GetTotalDrawCalls()
{
	int nSum = 0;
	for(int i=0; i<PPT_END; ++i)
		nSum += GetDrawCallCount((PROFILE_POLYGON_TYPE)i);

	return nSum;
}

TIMEPROFILENODE* RResourceProfiler::GetTimeProfileNode(const char* szName)
{
#ifdef _USE_ENGINE_RESOURCE_PROFILE
	map<string, TIMEPROFILENODE*>::iterator itr = 
		m_timeProfileNodes.find( szName );
	if( m_timeProfileNodes.end()!=itr)
		return itr->second;

	TIMEPROFILENODE* pNewNode = new TIMEPROFILENODE;
	m_timeProfileNodes.insert( map<string, TIMEPROFILENODE*>::value_type( szName, pNewNode ) );

	// 색상 테이블
	DWORD dwColorTable[] = {
		0xffff00,
		0x0000ff,
		0xff0000,
		0x00ffff,
	};
	const int nTableSize = _countof(dwColorTable);

	pNewNode->bVisible = true;
	pNewNode->dwColor = 
		m_timeProfileNodes.size()<nTableSize ?
		dwColorTable[m_timeProfileNodes.size()-1] :	// 테이블에 있는 색 쓰고
		(0x9172cd * m_timeProfileNodes.size() ) ;	// 넘어가면 랜덤
	pNewNode->dwColor |= 0xff000000;
	pNewNode->dwCurrentTime = 0;
	return pNewNode;
#else
	return NULL;
#endif
}

void RResourceProfiler::UpdateTimeQueue(const char* szName, unsigned int nTime)
{
	TIMEPROFILENODE* pNode = GetTimeProfileNode(szName);
	if(pNode)
		pNode->dwCurrentTime += nTime;
}

void RResourceProfiler::UpdateTimeProfileNodes()
{
	TIMEPROFILENODEMAP::iterator itr = m_timeProfileNodes.begin();
	for( ; itr!=m_timeProfileNodes.end() ; ++itr)
	{
		TIMEPROFILENODE* pNode = itr->second;
		if(!m_bPauseTimeGraph)
			pNode->m_queueTime.push_back( pNode->dwCurrentTime);

		pNode->dwCurrentTime = 0;
		if(pNode->m_queueTime.size() > FRAMETIME_RING_BUFFER_COUNT)
			pNode->m_queueTime.pop_front();
	}
}

struct FRAME_LINE_VERTICES
{
	float x,y,z,rhw;
	enum { FVF = (D3DFVF_XYZRHW) };
};

void RResourceProfiler::RenderGuideLines(RFont* pFont, int x, int y, bool bBeginFont)
{
	// guidline
	const int nY1 = y;
	const int nY2 = nY1+100;
	const int nX1 = x;
	const int nX2 = nX1 + FRAMETIME_RING_BUFFER_COUNT;
	const FRAME_LINE_VERTICES verticesGuideLines[] =
	{	// 한줄에 라인하나씩
		// 가로라인
		nX1,nY1, 0, 1,		nX2,nY1,0, 1,
		nX1,nY1+25,0, 1,	nX2,nY1+25,0, 1,
		nX1,nY1+50,0, 1,	nX2,nY1+50,0, 1,
		nX1,nY1+75,0, 1,	nX2,nY1+75,0, 1,
		nX1,nY1+100,0, 1,	nX2,nY1+100,0, 1,
		// 세로라인
		nX1,nY1,0, 1,		nX1,nY1+100,0, 1,
		nX2,nY1,0, 1,		nX2,nY1+100,0, 1
	};

	if (bBeginFont)	pFont->BeginFont();
	pFont->DrawText_UNICODE( nX1 - 30, nY1+0, L"100");
	pFont->DrawText_UNICODE( nX1 - 30, nY1+25, L"75");
	pFont->DrawText_UNICODE( nX1 - 30, nY1+50, L"50");
	pFont->DrawText_UNICODE( nX1 - 30, nY1+75, L"25");
	pFont->DrawText_UNICODE( nX1 - 30, nY1+100, L"0");
	pFont->DrawText_UNICODE( nX1 , nY2 + 5, L"Pause Key to Pause , CTRL + ( keypad,a~z,- ) to Toggle graph ");

	TIMEPROFILENODEMAP::iterator itr = m_timeProfileNodes.begin();
	unsigned int nIndex = 0;
	for( ; itr!=m_timeProfileNodes.end() ; ++itr)
	{
		TIMEPROFILENODE* pNode = itr->second;
		char szIndex[256];
		if(nIndex>=10&& nIndex<10+26)
			sprintf(szIndex,"%c",nIndex-10+'A');	// 10~35 -> a~z
		else
			sprintf(szIndex,"%d",nIndex);
		char buffer[256];
		sprintf(buffer,"%s %s %s", szIndex, itr->first.c_str(), (pNode->bVisible ? " o" : " x"));
		pFont->DrawText_UNICODE( nX2 + 10, nY1+nIndex*15, MLocale::ConvAnsiToUTF16(buffer).c_str(), -1, pNode->dwColor);
		nIndex++;
	}
	if (bBeginFont) pFont->EndFont();

	REngine::GetDevice().SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	REngine::GetDevice().SetTextureStageState(0, RTSS_COLORARG1, RTA_TFACTOR);
	REngine::GetDevice().SetTextureFactor(0xff808080);

	REngine::GetDevice().SetFvF( FRAME_LINE_VERTICES::FVF );
	REngine::GetDevice().DrawPrimitiveUP( RPT_LINELIST, _countof(verticesGuideLines)/2, verticesGuideLines, sizeof(FRAME_LINE_VERTICES) );
	REngine::GetResourceProfiler().AddPolygon( PPT_RS3DEBUG, _countof(verticesGuideLines)/2);
}

void RResourceProfiler::RenderGraph(TIMEPROFILENODE* pNode, int x, int y)
{
	const deque< unsigned int >& frameTime = pNode->m_queueTime;
	if(frameTime.size()<2) return;

	const int nY2 = y+100;

	// frame time 그래프
	int nCount = 0;

	FRAME_LINE_VERTICES vertices[FRAMETIME_RING_BUFFER_COUNT] = {0, };
	for( deque< unsigned int >::const_reverse_iterator i = frameTime.rbegin() ; i!= frameTime.rend(); ++i)
	{
		vertices[nCount].x = x + FRAMETIME_RING_BUFFER_COUNT - nCount;
		vertices[nCount].y = max(0, nY2 - (int)*i);
		++nCount;
		if(nCount>=FRAMETIME_RING_BUFFER_COUNT) break;
	}
	REngine::GetDevice().SetTextureFactor( pNode->dwColor );
	REngine::GetDevice().DrawPrimitiveUP( RPT_LINESTRIP, nCount-1, vertices, sizeof(FRAME_LINE_VERTICES) );
	REngine::GetResourceProfiler().AddPolygon( PPT_RS3DEBUG, nCount-1);
}

void RResourceProfiler::ToggleVisible(int nIndex)
{
	int i = 0;

	TIMEPROFILENODEMAP::iterator itr = m_timeProfileNodes.begin();
	for( itr = m_timeProfileNodes.begin(); itr!=m_timeProfileNodes.end() ; ++itr, ++i)
	{
		TIMEPROFILENODE* pNode = itr->second;
		if((i==nIndex) || (RP_TOGGLE_ALL==nIndex))
			pNode->bVisible = !pNode->bVisible;
	}
}

void RResourceProfiler::RenderTimeGraph(RFont* pFont, int x,  int y)
{
	RenderGuideLines(pFont, x, y);

	// 실제 그래프
	int i = 0;

	TIMEPROFILENODEMAP::iterator itr = m_timeProfileNodes.begin();
	for( itr = m_timeProfileNodes.begin(); itr!=m_timeProfileNodes.end() ; ++itr, ++i)
	{
		TIMEPROFILENODE* pNode = itr->second;
		if(pNode->bVisible)
			RenderGraph(pNode, x, y);
	}
}

void RResourceProfiler::RenderTimeGraph(RFont* pFont)
{
	const int y = 5;
	const int x = 150;

	RenderTimeGraph(pFont, x, y);
}

void RResourceProfiler::ProcessDebugKey(int nKey)
{
	// pause
	if(nKey==VK_PAUSE)
		REngine::GetResourceProfiler().TogglePauseTimeGraph();
	// 켜고 끄고
	if(GetAsyncKeyState(VK_CONTROL))
	{
		if(VK_SUBTRACT==nKey)
			REngine::GetResourceProfiler().ToggleVisible(RP_TOGGLE_ALL);
		else if(VK_NUMPAD0<=nKey && nKey<=VK_NUMPAD9)
			REngine::GetResourceProfiler().ToggleVisible(nKey-VK_NUMPAD0);
		else if('A'<=nKey && nKey<='Z')
			REngine::GetResourceProfiler().ToggleVisible(10+nKey-'A');
	}
}

void RResourceProfiler::StartMeshProfiler()
{ 
#ifdef _USE_ENGINE_RESOURCE_PROFILE
	m_pProfileNode->Return();
	m_pProfileNode->Reset();
	m_pProfileNode->Call();
#endif
}

void RResourceProfiler::AddMeshProfileInfo( RMeshNode* pMeshNode, int nSubMaterialIndex )
{
#ifdef _USE_ENGINE_RESOURCE_PROFILE

	// GPU Time /////////////////////////////////////////////////////////////
	__int64 n64_GPU_StartTime;
	GetGPUProfiler().Profile_Get_Ticks(&n64_GPU_StartTime);

	GetGPUProfiler().GetEventQuery()->EndIssue();
	GetGPUProfiler().GetEventQuery()->Data(NULL);

	__int64 fStopTime;
	GetGPUProfiler().Profile_Get_Ticks(&fStopTime);
	fStopTime -= n64_GPU_StartTime;

	float fCurrentTime = (float)fStopTime / GetGPUProfiler().Profile_Get_Tick_Rate();
	float fGPUTime = m_pProfileNode->Get_Current_GPUTime();
	fGPUTime += fCurrentTime;
	m_pProfileNode->Set_Current_GPUTime(fGPUTime);
	//////////////////////////////////////////////////////////////////////////

	// CPU Time
	m_pProfileNode->Return();

	RMesh* pMesh = pMeshNode->m_pParentMesh;
	MESHPROFILEINFOMAP::iterator itr = m_MeshProfileInfoCurrent.find(pMesh);

	if(itr == m_MeshProfileInfoCurrent.end())
	{
		MESHPROFILEINFOMAP::value_type newProfileInfo( pMesh, RMeshProfileInfo(pMesh) );
		itr = m_MeshProfileInfoCurrent.insert(newProfileInfo).first;	
	}

	float fCPUTime = m_pProfileNode->Get_Total_Time();	

	//pMeshNode->m_pVb->GetUsingSystemMemory();
	//pMeshNode->m_pVb->GetUsingVideoMemory();

	// 단위: ms
	// CPU는 GPU의 값을 포함한다
	itr->second.AddInfo(pMeshNode, nSubMaterialIndex, (fCPUTime-fGPUTime) * 1000.0f, fGPUTime * 1000.0f);

	m_pProfileNode->Reset();
	m_pProfileNode->Call();
#endif
}

void RResourceProfiler::CountMeshInstance(SCENE_SET* pActorList)
{
#ifdef _USE_ENGINE_RESOURCE_PROFILE
	SCENE_SET::iterator	iter;
	for( iter = pActorList->begin() ; iter != pActorList->end() ; ++iter)
	{
		RSceneNode *pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;

		RActor* pActor = MDynamicCast( RActor, pSceneNode); 
		if(pActor==NULL)
			continue;

		MESHPROFILEINFOMAP::iterator itr = m_MeshProfileInfoCurrent.find(pActor->m_pMesh);
		if(itr != m_MeshProfileInfoCurrent.end())
		{
			itr->second.AddRef();

			for( int i = 0; i < pActor->GetActorNodeCount(); ++i )
			{
				RActorNode* pActorNode = pActor->GetActorNodes()[i];

				if( !pActorNode->GetVisible() || !pActorNode->GetUsable() || !pActorNode->IsRenderable() )	continue;

				int nCount = itr->second.m_nMeshNodeCount;
				itr->second.m_nMeshNodeCount++;
			}
		}
	}
#endif
}

void RResourceProfiler::GetMeshProfileInfoBinary(MBuffer* pBuffer)
{
	MBuffer pTempBuffer; 
	pTempBuffer.AllocateBuffer(1024*1024); //1024 kb로 넉넉히 잡아둔다
	pTempBuffer.Reset();


	MESHPROFILEINFOMAP::iterator itr;
	for( itr=m_MeshProfileInfoLast.begin(); itr!=m_MeshProfileInfoLast.end(); ++itr )
		itr->second.AppendBinaryInfoToBuffer(pTempBuffer);


	pBuffer->Reset();

	int nBufferSize	= pTempBuffer.GetDataSize();
	pBuffer->Append((char*)&nBufferSize, sizeof(nBufferSize));
	pBuffer->Append(pTempBuffer.GetBufferPointer(), nBufferSize);
}

//////////////////////////////////////////////////////////////////////////
// Helper
TimeProfileHelper::TimeProfileHelper( const char * name )
{ 
	m_dwStartTime = timeGetTime();
	m_szName = name;
}

TimeProfileHelper::~TimeProfileHelper( void )					
{ 
	DWORD elapsed = timeGetTime() - m_dwStartTime;
	REngine::GetResourceProfiler().UpdateTimeQueue(m_szName, elapsed);
}

}