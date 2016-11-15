#include "StdAfx.h"
#include "MQuadTree.h"
#include "MFileSystem.h"

// 정사각형이며, 사이즈는 2^n 이라 가정한다.

////////////////////////////////////////////////////////////////////
// MPathWalkableMap : 쿼드트리를 생성하는데 필요한 정보를 담고있다

void MPathWalkableMap::CreateNew(int nSize,float fScaleX,float fScaleY)
{ 
	m_nSize = nSize; 
	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;

	m_pWalkable = new bool[nSize*nSize];
	m_pHeightMap = new float[nSize*nSize];
	m_pAreaID = new int[nSize*nSize];
	for(int i=0;i<nSize*nSize;i++)
	{
		m_pAreaID[i]=0;
	}
}

bool MPathWalkableMap::CreateFromFile(const char* szFileName)
{
	MFile file;

	if(!file.Open(szFileName))
		return false;

	file.Read(&m_nSize,sizeof(int));
	file.Read(&m_fScaleX,sizeof(float));
	file.Read(&m_fScaleY,sizeof(float));

	CreateNew(m_nSize,m_fScaleX,m_fScaleY);
	file.Read(m_pWalkable,sizeof(bool)*m_nSize*m_nSize);
	file.Read(m_pHeightMap,sizeof(float)*m_nSize*m_nSize);
	file.Read(m_pAreaID,sizeof(int)*m_nSize*m_nSize);
	
	file.Close();

	return true;
}

bool MPathWalkableMap::SaveFile(const char* szFileName)
{
	FILE *file = fopen(szFileName,"wb+");
	fwrite(&m_nSize,sizeof(int),1,file);
	fwrite(&m_fScaleX,sizeof(float),1,file);
	fwrite(&m_fScaleY,sizeof(float),1,file);
	fwrite(m_pWalkable,sizeof(bool),m_nSize*m_nSize,file);
	fwrite(m_pHeightMap,sizeof(float),m_nSize*m_nSize,file);
	fwrite(m_pAreaID,sizeof(int),m_nSize*m_nSize,file);
	fclose(file);

	return true;
}

MPathWalkableMap::MPathWalkableMap() : m_pWalkable(NULL), m_pHeightMap(NULL), m_pAreaID(NULL), m_nSize(0)
{
}

MPathWalkableMap::~MPathWalkableMap()
{ 
	if(m_pHeightMap) delete m_pHeightMap;
	if(m_pWalkable) delete m_pWalkable; 
	if(m_pAreaID) delete m_pAreaID;
}


int MPathWalkableMap::GetLinkedCount(int x,int y)
{
	// seed node를 넣는다
	m_visitNode.push_back(pair<int,int> (x,y));
	SetVisited(x,y,true);

	list< pair<int,int> >::iterator i = m_visitNode.begin();

	while(i != m_visitNode.end()) {

		if(!IsWalkable(i->first,i->second)) 
			i = m_visitNode.erase(i);
		else
		{
//			mlog("( %d , %d ) \n",i->first,i->second);
			// 8방향으로 연결된 노드를 찾는다.
			int nCount = 0;
			for(int j=0;j<8;j++)
			{
				const int successor[8][2] = { 0,-1,  1,-1, 1,0,  1,1, 0,1,  -1,1, -1,0, -1,-1 };

				int nx = i->first  +successor[j][0];
				int ny = i->second +successor[j][1];
				if(nx>=0 && ny>=0 && nx<m_nSize && ny<m_nSize && !IsVisited(nx,ny))
				{
					m_visitNode.push_back( pair<int,int> (nx,ny) );
					SetVisited(nx,ny,true);
				}
			}
			i++;
		}
	}

	return (int)m_visitNode.size();	// 자기자신 포함
}

// 고립된 갈수있는곳을 삭제하며 areaID를 기록한다
int MPathWalkableMap::EraseIsolatedWalkable(int nLimit)
{
	CreateVisit();
	m_nAreaIDs = 0;

	for(int i=0;i<m_nSize*m_nSize;i++) {
		m_pVisited[i] = false;
	}

	int nTotalErase = 0;

	for(int i=0;i<m_nSize;i++) {
		for(int j=0;j<m_nSize;j++) {
			if(!IsVisited(i,j))
			{
				m_visitNode.clear();
				int nLinkedCount = GetLinkedCount(i,j);
				if(nLinkedCount<nLimit) {
					nTotalErase+= (int)m_visitNode.size();
					while(!m_visitNode.empty())
					{
						int ex = m_visitNode.front().first;
						int ey = m_visitNode.front().second;
						SetWalkable(ex,ey,false);
						m_visitNode.pop_front();
					}
				}else
				{
					m_nAreaIDs++;
					while(!m_visitNode.empty())
					{
						int ex = m_visitNode.front().first;
						int ey = m_visitNode.front().second;
						SetAreaID(ex,ey,m_nAreaIDs);
						m_visitNode.pop_front();
					}
				}
			}
		}
	}

	DestroyVisit();

	return nTotalErase;
}

/////////////////////////////////////////////////////////////////////
// MQuadTreeNode

MQuadTreeNode::~MQuadTreeNode()
{
	for(int i=0;i<4;i++)
	{
		if(m_pChildren[i])
			delete m_pChildren[i];
	}
}

int MQuadTreeNode::GetSuccessorCount(void)
{
	return (int)m_successors.size();
}

MNodeModel* MQuadTreeNode::GetSuccessor(int i)
{
	return m_successors[i];
}

// 이웃 Node들의 Cost 얻기
float MQuadTreeNode::GetSuccessorCost(MNodeModel* pSuccessor)
{
	_ASSERT(pSuccessor!=NULL);
	return (((MQuadTreeNode *)pSuccessor)->m_position - m_position).Length();
}

// Heuristic Estimation에 의한 pNode까지의 Cost 얻기
float MQuadTreeNode::GetHeuristicCost(MNodeModel* pNode)
{
	return GetSuccessorCost(pNode);	// 거리
}



/////////////////////////////////////////////////////////////////////
// MQuadTree 쿼드트리 본체
MQuadTree::MQuadTree() : m_nNodeCount(0), m_pRootNode(NULL)
{
}

MQuadTree::~MQuadTree()
{
	if(m_pRootNode) delete m_pRootNode;
}

void MQuadTree::Create(MPathWalkableMap* pWalkableMap,int x,int y,int nCellSize,float fScaleX,float fScaleY)
{
	m_fScaleX = fScaleX; 
	m_fScaleY = fScaleY; 

	m_pWalkableMap = pWalkableMap;
	m_nSize = nCellSize;

	QTCREATERESULT result;
	m_pRootNode = CreateRecurse(&result, x,y,nCellSize);
	if(result==WALKABLE && !m_pRootNode)
	{
		m_pRootNode = new MQuadTreeNode;
		float fHeight = m_pWalkableMap->GetHeight(int(.5f*nCellSize),int(.5f*nCellSize));
		m_pRootNode->m_position = 
			MVector3(GetScaleX() * nCellSize/2,
					GetScaleY() * nCellSize/2,fHeight );
	}

	if(m_pRootNode)
		CreateSuccessor(m_pRootNode);
}

MQuadTreeNode* MQuadTree::CreateRecurse(QTCREATERESULT *pResult,int x,int y,int nCellSize)
{
	if(nCellSize==1) {
		*pResult = m_pWalkableMap->IsWalkable(x,y) ? WALKABLE : UNWALKABLE;
		return NULL;
	}

	// 사이즈가 1이상이므로 분할해결한다.
	QTCREATERESULT result[4];
	MQuadTreeNode* pChildren[4];

	for(int i=0;i<4;i++)
	{
		// i에따라	0 1	
		//			2 3
		int nChildX = x + (i%2) * nCellSize/2;
		int nChildY = y + (i/2) * nCellSize/2;
		
		pChildren[i] = CreateRecurse(&result[i],nChildX,nChildY, nCellSize/2);
	}

	// 자식들이 모두 갈수있는 영역이거나 갈수 없는 영역이면 쪼갤 필요가 없다
	if(result[0]!=MIXED && result[0]==result[1] && result[1]==result[2] && result[2]==result[3]) {
		*pResult = result[0];
		return NULL;
	}

	// 다른것이 하나라도 들어있으면 쪼갠다
//	m_nNodeCount++;	// 말단 노드만 세자
	MQuadTreeNode* pCurNode = new MQuadTreeNode;
    for(int i=0;i<4;i++)
	{
		_ASSERT(!(result[i]==MIXED && pChildren[i]==NULL));

		if(pChildren[i]==NULL && result[i]==WALKABLE)
		{
			m_nNodeCount++;
			pCurNode->m_pChildren[i] = new MQuadTreeNode;

			// i에따라	0 1	
			//			2 3
			int nChildX = x + (i%2) * nCellSize/2;
			int nChildY = y + (i/2) * nCellSize/2;
			
			float fHeight = m_pWalkableMap->GetHeight(int(nChildX+0.25f*nCellSize),int(nChildY+0.25f*nCellSize));
			pCurNode->m_pChildren[i]->m_position = 
				MVector3(GetScaleX() * (float)nChildX+0.25f*nCellSize,
						GetScaleY() * (float)nChildY+0.25f*nCellSize,fHeight );
			pCurNode->m_pChildren[i]->m_nAreaID = m_pWalkableMap->GetAreaID(nChildX,nChildY);

		}
		else
			pCurNode->m_pChildren[i] = pChildren[i];
	}

	*pResult = MIXED;
	return pCurNode;
}

void MQuadTree::CreateSuccessor(MQuadTreeNode *pNode)
{
//	0 1	
//	2 3

	LinkNode(pNode->m_pChildren[0],pNode->m_pChildren[1],QRIGHT);
	LinkNode(pNode->m_pChildren[1],pNode->m_pChildren[3],QBOTTOM);
	LinkNode(pNode->m_pChildren[3],pNode->m_pChildren[2],QLEFT);
	LinkNode(pNode->m_pChildren[2],pNode->m_pChildren[0],QTOP);

	for(int i=0;i<4;i++) {
		if(pNode->m_pChildren[i])
			CreateSuccessor(pNode->m_pChildren[i]);
	}
}

void MQuadTree::LinkNode(MQuadTreeNode *pStartNode, MQuadTreeNode *pToNode, QTLINKTYPE linkType)
{
//	0 1	
//	2 3

	if(NULL==pStartNode || NULL==pToNode) return;

	// link 방향에 따른 두개의 start쪽의 자식노드, to쪽의 자식노드 테이블
	const int linkTable[4][2][2] = {
		0,1,2,3, //top
		1,3,0,2, //right
		0,2,1,3, //left
		2,3,0,1 //bottom
	};

	MQuadTreeNode *pStartNodeChild[2] = { 
		pStartNode->m_pChildren[linkTable[linkType][0][0]],
		pStartNode->m_pChildren[linkTable[linkType][0][1]] };
	bool bStartNodeHasChild = (NULL!=pStartNodeChild[0]) || (NULL!=pStartNodeChild[1]);

	MQuadTreeNode *pToNodeChild[2] = { 
		pToNode->m_pChildren[linkTable[linkType][1][0]],
		pToNode->m_pChildren[linkTable[linkType][1][1]] };
	bool bToNodeHasChild = (NULL!=pToNodeChild[0]) || (NULL!=pToNodeChild[1]);

	if(bStartNodeHasChild)
	{
		if(bToNodeHasChild)
		{
			// 양쪽다 자식이 있으면 자식노드들끼리만 연결한다
			LinkNode(pStartNodeChild[0],pToNodeChild[0],linkType); // 첫번재 자식노드 
			LinkNode(pStartNodeChild[1],pToNodeChild[1],linkType); // 두번째 자식노드
		}else
		{
			// start는 자식이 있고, to는 없는경우 start쪽을 분할해결한다.
			LinkNode(pStartNodeChild[0],pToNode,linkType); // 첫번재 자식노드 
			LinkNode(pStartNodeChild[1],pToNode,linkType); // 두번째 자식노드
		}
	}else
	{
		if(bToNodeHasChild)
		{
			// start는 자식이 없고, to는 있는경우 to쪽을 분할해결한다.
			LinkNode(pStartNode,pToNodeChild[0],linkType); // 첫번재 자식노드 
			LinkNode(pStartNode,pToNodeChild[1],linkType); // 두번째 자식노드
		}else
		{
			// 양쪽다 자식이 없으면 연결하고 끝낸다.
			pStartNode->m_successors.push_back(pToNode);
			pToNode->m_successors.push_back(pStartNode);
		}
	}
}

MQuadTreeNode* MQuadTree::GetNode(float x,float y) 
{ 
	x = x/GetScaleX();
	y = y/GetScaleY();
	if(x<0 || y<0 || x>m_nSize || y>m_nSize) return NULL;

	return GetNodeRecurse(x , y ,0,0,m_pRootNode,m_nSize); 
}

MQuadTreeNode* MQuadTree::GetNodeRecurse(float px,float py,int x,int y,MQuadTreeNode* pNode,int nCellSize)
{
	if(!pNode) return NULL;

	// 자식노드가 없으면 이 노드를 리턴
	if(!pNode->m_pChildren[0] && !pNode->m_pChildren[1] && !pNode->m_pChildren[2] && !pNode->m_pChildren[3])
		return pNode;

//	0 1	
//	2 3
	int centerX = x+nCellSize/2;
	int centerY = y+nCellSize/2;

	if(px<centerX)	// 왼쪽
	{
		if(py<centerY)	// 위
			return GetNodeRecurse(px,py,x,y,pNode->m_pChildren[0],nCellSize/2);
		else			// 아래
			return GetNodeRecurse(px,py,x,centerY,pNode->m_pChildren[2],nCellSize/2);
	}else			// 오른쪽
	{
		if(py<centerY)	// 위
			return GetNodeRecurse(px,py,centerX,y,pNode->m_pChildren[1],nCellSize/2);
		else			// 아래
			return GetNodeRecurse(px,py,centerX,centerY,pNode->m_pChildren[3],nCellSize/2);
	}
}
