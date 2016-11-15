#include "stdafx.h"
#include "MTerrainToNavMesh.h"
#include <MMSystem.h>
#include <set>
#include <deque>
#include <queue>
#include <map>

MTerrainTriangle::MTerrainTriangle()
: m_pChild1(NULL)
, m_pChild2(NULL)
, m_pParent(NULL)
, m_fError(0)

, m_pLeft(NULL)
, m_pRight(NULL)
, m_pDiagonal(NULL)
, m_bSplited(false)
, m_nLevel(0)
, m_bSteep(false)
{
	p1 = p2 = p3 = RVector::ZERO;
}

void MTerrainTriangle::Set(const MVector3& _p1, const MVector3& _p2, const MVector3& _p3, MTerrainTriangle* pDiag, int nLevel)
{
	p1 = _p1;
	p2 = _p2;
	p3 = _p3;
	m_pDiagonal = pDiag;
	m_nLevel = nLevel;
}


void MTerrainTriangle::MakeFullTree(RTerrain* pTerrain)
{
	if (m_nLevel>0)
	{
		MVector3 vMiddle = (p2 + p3) / 2;
		MTerrainTriangle* pLeftChild = new MTerrainTriangle;
		MTerrainTriangle* pRightChild = new MTerrainTriangle;

		MVector3 m;
		pTerrain->GetPickVerticalWithHole(m, NULL, vMiddle.x, vMiddle.y);
		float d = fabs(vMiddle.z - m.z);

		vMiddle.z = m.z;

		pLeftChild->p1 = vMiddle;
		pLeftChild->p2 = p1;
		pLeftChild->p3 = p2;
		pLeftChild->m_pParent = this;
		pLeftChild->m_nLevel = m_nLevel - 1;

		pRightChild->p1 = vMiddle;
		pRightChild->p2 = p1;
		pRightChild->p3 = p3;
		pRightChild->m_pParent = this;
		pRightChild->m_nLevel = m_nLevel - 1;

		m_pChild1 = pLeftChild;
		m_pChild2 = pRightChild;

		m_pChild1->MakeFullTree(pTerrain);
		m_pChild2->MakeFullTree(pTerrain);

#if 0
		if (d > m_fError)
			m_fError = d;
		if (m_pChild1->m_fError > m_fError)
			m_fError = m_pChild1->m_fError;
		if (m_pChild2->m_fError > m_fError)
			m_fError = m_pChild2->m_fError;
#endif

		m_bSplited = true;
	}
	else
	{
		m_bSplited = false;
		m_fError = 0;
	}

}

void MTerrainTriangle::SetLeftRightDiagonalNode()
{
	if (m_pChild1 != NULL)
	{
		m_pChild1->m_pLeft = m_pChild2;		

		if (m_pDiagonal != NULL)
			m_pChild1->m_pRight = m_pDiagonal->m_pChild1;
		
		if (m_pLeft != NULL)
			m_pChild1->m_pDiagonal = m_pLeft->m_pChild1;
	}

	if (m_pChild2 != NULL)
	{
		m_pChild2->m_pLeft = m_pChild1;		

		if (m_pDiagonal != NULL)
			m_pChild2->m_pRight = m_pDiagonal->m_pChild2;

		if (m_pRight != NULL)
			m_pChild2->m_pDiagonal = m_pRight->m_pChild2;
	}

	if (m_pChild1 != NULL)	
		m_pChild1->SetLeftRightDiagonalNode();
	if (m_pChild2 != NULL)
		m_pChild2->SetLeftRightDiagonalNode();
}


void MTerrainTriangle::Split(float fThreshold)
{
	if (m_fError < fThreshold) 
		return;

	m_bSplited = true;

 	if (m_pDiagonal != NULL)
 	{
 		if (!m_pDiagonal->m_bSplited)
 			m_pDiagonal->SplitParent();
 	}

	if (m_pChild1 != NULL)
		m_pChild1->Split(fThreshold);

	if (m_pChild2 != NULL)
		m_pChild2->Split(fThreshold);
}

void MTerrainTriangle::SplitParent()
{
	if (m_bSplited) return;

	m_bSplited = true;
	if (m_pParent != NULL)
	{
		m_pParent->SplitParent();
		if (m_pParent->m_pDiagonal != NULL)
			m_pParent->m_pDiagonal->SplitParent();

	}

}

MTerrainTriangle::~MTerrainTriangle()
{
	Destroy();
}

void MTerrainTriangle::Destroy()
{
	SAFE_DELETE(m_pChild1);
	SAFE_DELETE(m_pChild2);
}

void MTerrainTriangle::GetTriangleList(vector<MVector3>& vec)
{
	if (!m_bSplited)
	{
		if(m_bSteep) return;

		vec.push_back(p1);
		vec.push_back(p2);
		vec.push_back(p3);
	}
	else
	{
		m_pChild1->GetTriangleList(vec);
		m_pChild2->GetTriangleList(vec);
	}
}

void MTerrainTriangle::ClipSteepPlane(RTerrain *pTerrain, float fLimitDot)
{
	MVector3 v1, v2, norm;
	v1 = p2 - p1;
	v2 = p3 - p1;

	norm = v1.CrossProduct(v2);
	norm.Normalize();

	float d = norm.DotProduct(MVector3::AXISZ);
	if (fabs(d) < fLimitDot)
		m_bSteep = true;

	float minx = min(min(p1.x, p2.x),p3.x);
	float maxx = max(max(p1.x, p2.x),p3.x);
	float miny = min(min(p1.y, p2.y),p3.y);
	float maxy = max(max(p1.y, p2.y),p3.y);
	
	RHeightField *pHeightField = pTerrain->GetHeightFieldPtr();

	for (float y = miny; y<maxy; y+=pTerrain->GetYScale())
	{
		for (float x = minx; x<maxx; x+=pTerrain->GetXScale())
		{
			if( pHeightField->GetAttribute(x, y, RHeightField::UNDER_WATER)	// 물 밑
				//|| pHeightField->GetAttribute(x, y, RHeightField::VALID_POS)==false	// 유효한 위치가 아닐 때 ...
				)
			{
				m_bSteep = true;
				break;
			}
		}
		if ( m_bSteep )
			break;
	}


	if(m_nLevel>0 && m_bSplited)
	{
		m_pChild1->ClipSteepPlane(pTerrain, fLimitDot);
		m_pChild2->ClipSteepPlane(pTerrain, fLimitDot);
	}
}

int MTerrainTriangle::Flatten()
{
	if(m_nLevel==0)
	{
		p1.z = 0;
		p2.z = 0;
		p3.z = 0;

		return 0;
	}

	int nMerged = 0;
	nMerged += m_pChild1->Flatten();
	nMerged += m_pChild2->Flatten();

	// 두 자식 모두 분할되지 않았다면 합친다
	if(m_bSplited)
	{
		if((!m_pChild1->m_bSplited && !m_pChild1->m_bSteep) &&
			(!m_pChild2->m_bSplited && !m_pChild2->m_bSteep) )
		{
			if(m_pDiagonal && m_pDiagonal->m_bSplited)
			{
				// 빗변을 공유하는 폴리곤이 있으면 그 폴리곤도 합쳐질수 있어야 합친다
				if((!m_pDiagonal->m_pChild1->m_bSplited && !m_pDiagonal->m_pChild1->m_bSteep) &&
					(!m_pDiagonal->m_pChild2->m_bSplited && !m_pDiagonal->m_pChild2->m_bSteep) )
				{
					m_bSplited = false;
					m_pDiagonal->m_bSplited = false;
					nMerged++;
				}
			}else
			{
				m_bSplited = false;
				nMerged++;
			}
		}
	}
	return nMerged;
}


//////////////////////////////////////////////////////////////////////////
void MExportedTerrain::SetInitialTriangles(RTerrain* pTerrain)
{
	MVector3 t1, t2, t3, t4;
	int n = pTerrain->GetWidthMap();
	int i=1, p = 1; 
	for (; ; i++)
	{
		p*=2;
		if (p>=n) break;
	}

	i = i * 2;


	t1.Set(0, 0, pTerrain->GetHeightFromHeightMap(0, 0));
	t2.Set(pTerrain->GetWidthSize(), 0, pTerrain->GetHeightFromHeightMap(pTerrain->GetWidthMap(), 0));
	t3.Set(0, pTerrain->GetHeightSize(), pTerrain->GetHeightFromHeightMap(0, pTerrain->GetHeightMap()));
	t4.Set(pTerrain->GetWidthSize(), pTerrain->GetHeightSize(), pTerrain->GetHeightFromHeightMap(pTerrain->GetWidthMap(), pTerrain->GetHeightMap()));

	tri1.Set(t1, t2, t3, &tri2, i);
	tri2.Set(t4, t2, t3, &tri1, i);
}


void MExportedTerrain::Build(RTerrain* pTerrain, float fErrorThreshold)
{
	tri1.MakeFullTree(pTerrain);
	tri2.MakeFullTree(pTerrain);

	tri1.SetLeftRightDiagonalNode();
	tri2.SetLeftRightDiagonalNode();

	tri1.Split(fErrorThreshold);
	tri2.Split(fErrorThreshold);
}

struct STmpDat
{
	MVector3 vec;
	int		idx;
	
	STmpDat() : idx(0) {}
	STmpDat(const MVector3& v) : vec(v), idx(0) {}
};

struct compVector
{
	bool operator()(const STmpDat& v1, const STmpDat& v2) const
	{
		if ((fabs(v1.vec.x - v2.vec.x) > FLT_EPSILON))
			return v1.vec.x < v2.vec.x;
		if ((fabs(v1.vec.y - v2.vec.y) > FLT_EPSILON))
			return v1.vec.y < v2.vec.y;
		if ((fabs(v1.vec.z - v2.vec.z) > FLT_EPSILON))
			return v1.vec.z < v2.vec.z;
		return false;
	}
};

void MExportedTerrain::BuildIndexedForm()
{
	vector<STmpDat*> vTmp;
	set<STmpDat, compVector> tmpset;
	indexVerts.clear();
	indices.resize(trivec.size());
	vTmp.resize(trivec.size());
	pair<set<STmpDat, compVector>::iterator, bool> ret;

	for (size_t i=0; i<trivec.size(); i++)
	{
		ret = tmpset.insert(STmpDat(trivec[i]));
		vTmp[i] = &(*(ret.first));
	}

	int c=0;
	for (set<STmpDat, compVector>::iterator i=tmpset.begin(); i!=tmpset.end(); ++i)
	{
		(*i).idx = c;
		indexVerts.push_back((*i).vec);
		c++;
		
	}

	indexVertsRef.resize(indexVerts.size(), 0);

	for (size_t i=0; i<indices.size(); i++)
	{
		indices[i] = vTmp[i]->idx;
		indexVertsRef[indices[i]]++;
	}
}

void MExportedTerrain::FlipCCWFaces()
{
	vector<float>		normVec;

	for (size_t i=0; i<trivec.size();)
	{
		MVector3 v1, v2, norm;
		v1 = trivec[i+1] - trivec[i];
		v2 = trivec[i+2] - trivec[i];

		norm = v1.CrossProduct(v2);

		norm.Normalize();

		float dot = norm.DotProduct(MVector3::AXISZ);
		normVec.push_back(dot);

		i+=3;
	}

	for (size_t i=0; i<indices.size();)
	{
		if (normVec[i / 3] < 0)
		{
			int t = indices[i + 1];
			indices[i + 1] = indices[i + 2];
			indices[i + 2] = t;
		}
		i+=3;
	}
}

void MExportedTerrain::BuildAdjFaceList()
{
	adjData.resize(indices.size() / 3);

	map<int, vector<int> > tmpMap;

	// tmpMap : x번째 버텍스에 연결된 face의 id가 들어감
	for (size_t i=0; i<indices.size(); i++)
	{
		tmpMap[indices[i]].push_back((int)	i / 3);
	}

	// 모든 버텍스에 대해 (n번 face의 3개의 버텍스 : 3n, 3n+1, 3n+2
	for (size_t i=0; i<indices.size(); )
	{
		if (i % 3 == 0)
			adjData[i/3].clear();
		vector<int> t1, t2;
		t1 = tmpMap[indices[i]];
		t2 = tmpMap[indices[i + 1]];

		// 1,2번 버텍스를 어느어느 face가 참조하는지 확임

		vector<int> uni;
		for (size_t j=0; j<t1.size(); j++)
		{
			for (size_t k=0; k<t2.size(); k++)
			{
				if (t1[j] == t2[k])
				{
					uni.push_back(t1[j]);
					break;
				}
			}
		}

		// 1,2번 버텍스를 참조하는 face의 리스트를 가져옴

		if (uni.size() > 2)	// 두개 이상이면 에러!
			_ASSERT(false);
		else
		{
			if (uni.size() == 2)	// 딱 두개면 지금 face 외의 다른 face가 지금 face와 인접
			{
				if (uni[0] == i / 3)
					adjData[i / 3].push_back(uni[1]);
				else
					adjData[i / 3].push_back(uni[0]);
			}
		}

		uni.clear();
		t1 = tmpMap[indices[i + 1]];
		t2 = tmpMap[indices[i + 2]];

		for (size_t j=0; j<t1.size(); j++)
		{
			for (size_t k=0; k<t2.size(); k++)
			{
				if (t1[j] == t2[k])
				{
					uni.push_back(t1[j]);
					break;
				}
			}
		}

		if (uni.size() > 2)
			_ASSERT(false);
		else
		{
			if (uni.size() == 2)
			{
				if (uni[0] == i / 3)
					adjData[i / 3].push_back(uni[1]);
				else
					adjData[i / 3].push_back(uni[0]);
			}
		}

		uni.clear();
		t1 = tmpMap[indices[i]];
		t2 = tmpMap[indices[i + 2]];

		for (size_t j=0; j<t1.size(); j++)
		{
			for (size_t k=0; k<t2.size(); k++)
			{
				if (t1[j] == t2[k])
				{
					uni.push_back(t1[j]);
					break;
				}
			}
		}

		if (uni.size() > 2)
			_ASSERT(false);
		else
		{
			if (uni.size() == 2)
			{
				if (uni[0] == i / 3)
					adjData[i / 3].push_back(uni[1]);
				else
					adjData[i / 3].push_back(uni[0]);
			}
		}

		i+=3;
	}
}

void MExportedTerrain::FloodFillFaces()
{
	// 0: not visited   1: processing   2: visited
	faceID.resize(adjData.size(), -1);	
	visited.resize(adjData.size(), 0);
	faceGroupSize.clear();

	int nowid = 0;

	for (size_t i=0; i<adjData.size(); i++)
	{
		if (visited[i] == 0)
		{
			queue<size_t> q;
			vector<int> processed;
			q.push(i);
			while (!q.empty())
			{
				size_t n = q.front();
				q.pop();

				visited[n] = 2;
				processed.push_back((int)n);

				for (size_t j=0; j<adjData[n].size() ; j++)
				{
					if (visited[adjData[n][j]] == 0)
					{
						q.push(adjData[n][j]);
						visited[ adjData[n][j] ] = 1;
					}
				}
			}

			for (size_t j=0; j<processed.size(); j++)
				faceID[processed[j]] = nowid;

			faceGroupSize.push_back((int)processed.size());
			nowid++;
		}
	}

	nowid;	// 떨어진 group 개수
}

void MExportedTerrain::RemoveDisconnect(RTerrain *pTerrain, vector<MVector3> &seeds)
{
	vector<int> inSeedIDs;	// 시드가 포함된 ID

	float maxz = pTerrain->GetAABB().vmax.z;
	float minz = pTerrain->GetAABB().vmin.z;

	// 모든 triangle을 검사 : pass 1
	for (size_t si=0; si<faceID.size(); ++si)
	{
		int nIndex = static_cast<int>(si*3);
		MVector3 v0 = indexVerts[indices[nIndex++]];
		MVector3 v1 = indexVerts[indices[nIndex++]];
		MVector3 v2 = indexVerts[indices[nIndex++]];

		bool IsInSeed = false;
		for (size_t i=0; i<inSeedIDs.size() && !IsInSeed; ++i)
		{
			if ( faceID[si]==inSeedIDs[i])
				IsInSeed = true;
		}

		for (size_t i=0; i<seeds.size() && !IsInSeed ; ++i)
		{
			RVector vOrg = RVector(seeds[i].x, seeds[i].y, maxz);
			RVector vDir = RVector(0, 0,-1);

			if (MMath::IsIntersectNoCull(vOrg, vDir, v0, v1, v2, NULL))
				IsInSeed = true;
		}

		if (IsInSeed)
			inSeedIDs.push_back(faceID[si]);	// id를 넣는다.
	}

	// 모든 triangle의 seed가 아닌 것들을 처리 : pass 2
	for (size_t si=0; si<faceID.size(); ++si)
	{
		bool IsInSeed = false;
		for (size_t i=0; i<inSeedIDs.size() && !IsInSeed; ++i)
		{
			if ( faceID[si]==inSeedIDs[i])
				IsInSeed = true;
		}

		if (!IsInSeed)
		{
			int nIndex = static_cast<int>(si*3);
			indices[nIndex++] = -1;
			indices[nIndex++] = -1;
			indices[nIndex++] = -1;
		}
	}

	RemoveNoneIndex();
}

void MExportedTerrain::RemoveNoneIndex()
{
	vector<int> difference;
	vector<MVector3> newVerts;
	int diffID = 0;

	difference.resize(indexVerts.size());
	for (size_t i=0; i<indexVerts.size(); i++)
	{
		_ASSERT(indexVertsRef[i] >= 0);
		if (indexVertsRef[i] == 0)
		{
			diffID++;
		}
		else
			newVerts.push_back(indexVerts[i]);

		difference[i] = diffID;
	}

	indexVerts = newVerts;

	vector<int> newIndices;
	for (size_t i=0; i<indices.size(); i++)
	{
		if (indices[i] >= 0)	
		{
			int t = indices[i] - difference[indices[i]];
			_ASSERT(t>=0);
			newIndices.push_back(t);
		}
	}

	indices = newIndices;
}

void MExportedTerrain::ClipTooSmallChunk(int nLimitSize)
{
	vector<int> notclipID;
	for (size_t i=0; i<faceGroupSize.size(); i++)
	{
		if (faceGroupSize[i] > nLimitSize)
		{
			notclipID.push_back((int)i);
		}
	}

	for (size_t i=0; i<faceID.size(); i++)
	{
		bool b = true;
		for (size_t j=0; j<notclipID.size(); j++)
		{
			if (notclipID[j] == faceID[i])
			{
				b = false;
				break;
			}
		}

		if (b)
		{
#if 0
			indexVertsRef[indices[i * 3]]--;
			indexVertsRef[indices[i * 3 + 1]]--;
			indexVertsRef[indices[i * 3 + 2]]--;
#endif

			indices[i * 3] = -1;
			indices[i * 3 + 1] = -1;
			indices[i * 3 + 2] = -1;
		}
	}

	RemoveNoneIndex();
}


void MExportedTerrain::BuildAll(RTerrain* pTerrain, vector<MVector3> &seeds, float fErrorThreshold, float fLimitDot, int nLimitSize)
{
	DWORD t;

	t = timeGetTime();
	mlog("making triangles from terrain...  ");
	SetInitialTriangles(pTerrain);
	Build(pTerrain, fErrorThreshold );

	mlog("done!... %d ms elapsed.\n", timeGetTime() - t );

	t = timeGetTime();
	mlog("clipping too steep planes...  ");

	tri1.ClipSteepPlane(pTerrain, fLimitDot);
	tri2.ClipSteepPlane(pTerrain, fLimitDot);

	mlog("done!... %d ms elapsed. \n", timeGetTime() - t );

	mlog("flatten and merging...  ");
	// flatten, 합칠수 있는게 없을때 까지 반복
	int nMerged = 0;
	do {
		nMerged = 0;
		nMerged += tri1.Flatten();
		nMerged += tri2.Flatten();
		mlog("%d ", nMerged);
	} while(nMerged);

	tri1.GetTriangleList(trivec);
	tri2.GetTriangleList(trivec);

	mlog("done!... %d ms elapsed.  %d polygons \n", timeGetTime() - t, trivec.size() /3 );

	t = timeGetTime();
	mlog("transform from triangle list to indexed list...  ");
	BuildIndexedForm();
	mlog("done!... %d ms elapsed. %d polygons \n", timeGetTime() - t, trivec.size() /3 );

	t = timeGetTime();
	mlog("flip counterclockwise faces (seen from above)...  ");
	FlipCCWFaces();
	mlog("done!... %d ms elapsed.\n", timeGetTime() - t );

	t = timeGetTime();
	mlog("build adjacent faces list...  ");
	BuildAdjFaceList();
	mlog("done!... %d ms elapsed.\n", timeGetTime() - t );

	t = timeGetTime();
	mlog("flood fill face groups...  ");
	FloodFillFaces();
	mlog("done!... %d ms elapsed.\n", timeGetTime() - t );

	t = timeGetTime();
	mlog("clipping face groups...  ");
	if (!seeds.empty())
		RemoveDisconnect(pTerrain, seeds);
	else
		ClipTooSmallChunk(nLimitSize);

	mlog("done!... %d ms elapsed. %d polygons \n", timeGetTime() - t, indices.size() /3 );
}

	
void MExportedTerrain::DestroyBuffers()
{
	tri1.Destroy();
	tri2.Destroy();
	trivec.clear();
	indexVertsRef.clear();
	adjData.clear();
	faceID.clear();
	visited.clear();
	faceGroupSize.clear();
}

MExportedTerrain::MExportedTerrain()
{

}

MExportedTerrain::~MExportedTerrain()
{
	DestroyBuffers();
}

MExportedTerrainMesh::MExportedTerrainMesh()
{
}

MExportedTerrainMesh::~MExportedTerrainMesh()
{
}
