#include "stdafx.h"
#include "MPathEngineExporter.h"
#include "RTerrain.h"
#include "RActor.h"
#include "RMesh.h"
#include "RStaticMeshSceneNode.h"
#include "RTreeResourceMgr.h"
#include "RTreeSceneNode.h"
#include "RConfiguration.h"

#include "RTreeCollision.h"

#include "MTerrainToNavMesh.h"
#include "MPathAgentShape.h"
#include "MFileSystem.h"

#include <MMSystem.h>

#include <map>

class MFaceVertexMesh : public iFaceVertexMesh
{
public:
	vector<int>		indices;
	vector<RVector>	indexVerts;
	vector<int>		atts;
	vector<int>		port;

	long faces() const
	{
		return (long)indices.size() / 3;
	}
	long vertices() const
	{
		return (long)indexVerts.size();
	}

	long vertexIndex(long face, long vertexInFace) const
	{
		return (long)indices[face * 3 + (2 - vertexInFace)];
	}
	long vertexX(long vertex) const
	{
		return (long)indexVerts[vertex].x;
	}
	long vertexY(long vertex) const
	{
		return (long)indexVerts[vertex].y;
	}
	float vertexZ(long vertex) const
	{
		return indexVerts[vertex].z;
	}
	long faceAttribute(long face, long attributeIndex) const
	{
		if (attributeIndex == PE_FaceAttribute_MarksExternalShape)
			return atts[face];
		else if (attributeIndex == PE_FaceAttribute_MarksPortal)
			return port[face];
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// 일반 Portal 속성을 지형 Portal 로 변환시키는 Wrapped Interface class
class MGroundPtrOnTerrain : public iFaceVertexMesh
{
public:
	const iFaceVertexMesh *pSource;
	vector<tSigned32> ArrayTerrainPortals;

	MGroundPtrOnTerrain(const iFaceVertexMesh *faceVertex)
		: pSource(faceVertex)
	{
		tSigned32 count = faceVertex->faces();
		ArrayTerrainPortals.resize(count);

		for (tSigned32 i = 0; i<count; ++i)
			ArrayTerrainPortals[i] = faceVertex->faceAttribute(i, PE_FaceAttribute_MarksPortal);
	}

	long faces() const	
	{		return pSource->faces();	}
	long vertices() const	
	{	return pSource->vertices();	}
	long vertexIndex(long face, long vertexInFace) const
	{	return pSource->vertexIndex(face, vertexInFace); }
	long vertexX(long vertex) const
	{	return pSource->vertexX(vertex); }
	long vertexY(long vertex) const
	{	return pSource->vertexY(vertex); }
	float vertexZ(long vertex) const
	{	return pSource->vertexZ(vertex); }
	long faceAttribute(long face, long attributeIndex) const
	{
		if (attributeIndex == PE_FaceAttribute_MarksPortal )
			return 0;
		else if (attributeIndex == PE_FaceAttribute_MarksTerrainPortal)
			return ArrayTerrainPortals[face];

		return pSource->faceAttribute(face, attributeIndex);
	}
};

/////////////////////////////////////////////////////////////////////////////
//

class cFileOutputStream : public iOutputStream
{
	FILE* _file;
public:

	cFileOutputStream(const char* name)
	{
		_file = fopen(name, "wb");
		_ASSERT(_file);
	}
	~cFileOutputStream()
	{
		int errorOccurred = fclose(_file);
		_ASSERT(!errorOccurred);
	}
	void put(const char* data, unsigned long dataSize)
	{
		size_t written = fwrite(data, 1, dataSize, _file);
		_ASSERT(written == dataSize);
	}
};

/////////////////////////////////////////////////////////////////////////////
//
using namespace std;
using namespace rs3;

string MPathEngineExporter::m_strErrorBuffer = string();

iErrorHandler::eAction MPathEngineExporter::MPathEngineExporterError::handle(const char *type, const char *description, const char *const *attributes)
{
	char strBuffer[384];
	strBuffer[0] = 0;
	sprintf_s(strBuffer, sizeof(strBuffer),"Type : %s , %s\n", type, description);

	m_strErrorBuffer += strBuffer;
	return CONTINUE;
}

MPathEngineExporter::MPathEngineExporter()
: m_pRawMesh(NULL)
, m_pEngine(NULL)
{
}

MPathEngineExporter::~MPathEngineExporter()
{
}

/////////////////////////////////////////////////////////////////////////////
//
void MPathEngineExporter::GroundMeshLogging(const iFaceVertexMesh *pFaceVertex)
{
	tSigned32 nSize = pFaceVertex->faces();
	if ( nSize <= 0 )
		return;

	// ID & Count 
	typedef std::map<int, int>	ATTR_TABLE_MAP;
	ATTR_TABLE_MAP mapSectionID;
	
	int nExternalShapeCount = 0;
	int nTerrainPortalCount = 0;
	int nPortalCount = 0;

	class Impl
	{
	public:
		static void IncrementTable(ATTR_TABLE_MAP &mapTable, tSigned32 attrID)
		{
			if ( mapTable.find(attrID) == mapTable.end())
				mapTable[attrID] = 1;
			else
				mapTable[attrID] += 1;
		}
	};

	for ( tSigned32 i = 0; i<nSize; ++i )
	{
		{
			tSigned32 nResult = pFaceVertex->faceAttribute(i, PE_FaceAttribute_SectionID);
			Impl::IncrementTable(mapSectionID, nResult);
		}

		if ( pFaceVertex->faceAttribute(i, PE_FaceAttribute_MarksPortal))
			nPortalCount++;
		if ( pFaceVertex->faceAttribute(i, PE_FaceAttribute_MarksTerrainPortal))
			nTerrainPortalCount++;
		if ( pFaceVertex->faceAttribute(i, PE_FaceAttribute_MarksExternalShape))
			nExternalShapeCount++;
	}

	for ( std::map<int, int>::iterator it = mapSectionID.begin(); it != mapSectionID.end(); ++it )
		mlog("# %d id is %d count\n", it->first, it->second);

	mlog("# portal count : %d\n", nPortalCount);
	mlog("# terrain portal count : %d\n", nTerrainPortalCount);
	mlog("# external shape count : %d\n", nExternalShapeCount);
}

bool MPathEngineExporter::GetAllActor(RSceneNode *pSceneNode)
{
	if ( _stricmp(pSceneNode->GetNodeIDString(), RSID_ACTOR) == 0 )
		return true;
	else
		return false;
}

bool MPathEngineExporter::GetAllStaticMesh(RSceneNode *pSceneNode)
{
	if ( _stricmp(pSceneNode->GetNodeIDString(), RSID_STATICMESHSCENENODE) == 0 )
		return true;
	else
		return false;
}

/// Path engine sample - content chunks 의 LoadContentChunkPalcement 와 거의 동일한 코드
/// Static Mesh는 zone 영역에만 존재하고, ex라는 특별히 정해진 메시정보도 없다.
void MPathEngineExporter::MakeChunkInstanceListStaticMesh(MPathFinder *pPathEngine
														  , const std::string &strZoneFilePath
														  , vector<iContentChunk*> &chunkArray
														  , vector<iContentChunkInstance*> &chunkInstanceArray
														  , vector<RSceneNode*> &vStaticMeshArray)
{
	for ( size_t si = 0; si<vStaticMeshArray.size(); ++si )
	{
		_ASSERT(_stricmp(vStaticMeshArray[si]->GetNodeIDString(), RSID_STATICMESHSCENENODE) == 0);
		RStaticMeshSceneNode *pSceneNode = static_cast<RStaticMeshSceneNode*>(vStaticMeshArray[si]);

		string rsFilename = pSceneNode->GetFileName();
		_ASSERT(rsFilename.find(".rs") != string::npos);
		_ASSERT(rsFilename.find("\\") != string::npos || rsFilename.find("/") != string::npos);

		string tokFilename = rsFilename.replace(rsFilename.find(".rs"), 3, ".tok");
		const std::string &strFilename = tokFilename;

		MFile file;
		if ( file.Open(strFilename.c_str()) == false )
		{
			mlog("# %s cant' open\n", strFilename.c_str());
			continue;
		}

		iPathEngine *pathengine = pPathEngine->GetPathEngineInterface();
		_ASSERT(pathengine);
		
		unsigned int fileSize = file.GetLength();
		_ASSERT(fileSize>0);

		char *pBuffer = new char[fileSize];
		bool bLoadFile = file.ReadAll(pBuffer, fileSize);
		_ASSERT(bLoadFile);

		file.Close();

		iContentChunk *chunk = pathengine->loadContentChunk(pBuffer, fileSize);
		_ASSERT(chunk);

		if ( chunk )
		{
			chunkArray.push_back(chunk);

			float radian = -RVector::AXISX.AngleToXY(pSceneNode->GetRight());
			float scale = pSceneNode->GetScale().x;
			int px = (int)(pSceneNode->GetPosition().x);
			int py = (int)(pSceneNode->GetPosition().y);
			float pz = (pSceneNode->GetPosition().z);

			iContentChunkInstance *chunkInstance = chunk->instantiate_FloatRotateAndScale(radian, scale, px, py, pz, -1);
			_ASSERT(chunkInstance);
			chunkInstanceArray.push_back(chunkInstance);
			mlog("# static mesh - chunk instance 생성\n");
		}

		delete [] pBuffer;
		pBuffer = NULL;

	}
}

/// Path engine sample - content chunks 의 LoadContentChunkPalcement 와 거의 동일한 코드
/// Actor는 Actor path를 순회해야하고, ex 라는 특별히 정해진(obstacle을 생성하기 위한) 메시 정보를 가진 것도 포함단다.
void MPathEngineExporter::MakeChunkInstanceListActors(MPathFinder *pPathEngine
												, vector<iContentChunk*> &chunkArray
												, vector<iContentChunkInstance*> &chunkInstanceArray
												, vector<RSceneNode*> &chunkInstanceSources
												, vector<RSceneNode*> &vActorArray
												, vector<RSceneNode*> *pExActorArray)
{
	map<string, iContentChunk*> loadedChunkMap;

	for ( size_t si = 0; si<vActorArray.size(); ++si )
	{
		_ASSERT(_stricmp(vActorArray[si]->GetNodeIDString(), RSID_ACTOR)==0);
		
		RActor *pActor = static_cast<RActor*>(vActorArray[si]);
		RMesh *pMesh = pActor->m_pMesh;

		string eluFilename = pMesh->GetName();
		_ASSERT(eluFilename.find(".elu") != string::npos);
		string tokFilename = eluFilename.replace(eluFilename.find(".elu"), 4, ".tok");

		// Load Chunk
		iContentChunk *chunk = loadedChunkMap[tokFilename];
		if ( chunk == NULL )
		{
			MFile file;
			bool bFileOpen = false;

			if ( file.Open(tokFilename.c_str()) )
			{
				bFileOpen = true;
			}


			if ( bFileOpen )
			{
				iPathEngine *pathengine = pPathEngine->GetPathEngineInterface();
				unsigned int fileSize = file.GetLength();
				_ASSERT(fileSize>0);

				char *pBuffer = new char[fileSize];
				bool bLoadFile = file.ReadAll(pBuffer, fileSize);
				_ASSERT(bLoadFile);

				chunk = pathengine->loadContentChunk(pBuffer, fileSize);
				loadedChunkMap[tokFilename] = chunk;
				chunkArray.push_back(chunk);

				delete [] pBuffer;
				pBuffer = NULL;
			}
			else if ( pExActorArray )
			{
				pExActorArray->push_back(pActor);
			}
		}

		// create chunk-instance
		if ( chunk )
		{
			// TODO: pActor->GetWorldScale()로 얻어오면 에러가 난다.
			// World Scale이 변수가 따로 있는데, 이게 WorldTransformMatrix와 일치하지 않아 부득이하게 이렇게 수정함.
			// 변수가 제거되면 pActor->GetWorldScale()로 바로 값을 얻도록 수정해야 함.
			const RMatrix& mat = pActor->GetWorldTransform();
			RVector vScale;
			mat.ExtractScale( vScale );

			float radian = -RVector::AXISX.AngleToXY(pActor->GetWorldRight());
			int px = (int)(pActor->GetWorldPosition().x);
			int py = (int)(pActor->GetWorldPosition().y);
			float pz = (pActor->GetWorldPosition().z);

			iContentChunkInstance *chunkInstance = chunk->instantiate_FloatRotateAndScale(radian, vScale.x, px, py, pz, -1);
			_ASSERT(chunkInstance);
			chunkInstanceArray.push_back(chunkInstance);
			chunkInstanceSources.push_back(pActor);
		}
	}
}

bool MPathEngineExporter::MakeNavMesh(MPathFinder* pPathEngine, RSceneManager* pManager, vector<MVector3> &seeds)
{
	SAFE_DELETE(m_pRawMesh);

	m_pEngine = pPathEngine->GetPathEngineInterface();

	// ErrorBuffer 를 클리어
	_ASSERT(m_pEngine);
	m_strErrorBuffer.clear();

	// Exporter Error Handler를 넣는다.
	iErrorHandler *pBeforeHandler = m_pEngine->getErrorHandler();
	MPathEngineExporterError kHandler;
	
	m_pEngine->setErrorHandler(&kHandler);

	vector<const iFaceVertexMesh*> groundParts;

	// 지형 Ground 를 생성한다.
 	RTerrain* pTerrain = pManager->GetCurrentTerrain();
	iFaceVertexMesh *pTerrainFace = NULL;

	if (	pTerrain && 
			(true == pTerrain->getCreateNaviMesh()) )
	{
		MExportedTerrainMesh* tMesh = new MExportedTerrainMesh;

		// terrain - navigation mesh 를 만든다.
		tMesh->eTerrain.BuildAll( pTerrain, seeds, 100.f, 0.7f, 100);

		pTerrainFace = (iFaceVertexMesh*)tMesh;
		groundParts.push_back(pTerrainFace);
		mlog("# terrain chunk instance 생성\n");
	}

	// RActor의 Ground를 붙인다.
	vector<RSceneNode*> vNodeArray;
	vector<RSceneNode*> vExNodeArray;
	vector<RSceneNode*> vStaticArray;

	pManager->GetRootSceneNode().SerializeToVector(GetAllActor, vNodeArray);
	pManager->GetRootSceneNode().SerializeToVector(GetAllStaticMesh, vStaticArray);
	mlog("# %d 개의 Actor 개체를 얻었다.\n", vNodeArray.size());
	mlog("# %d 개의 Static Mesh 개체를 얻었다.\n", vStaticArray.size());
	
	vector<iContentChunk*> chunkArray;
	vector<iContentChunkInstance*> chunkInstanceArray;
	vector<RSceneNode*>	chunkInstanceSources;
	vector<MGroundPtrOnTerrain*> customGround;

	RHeightField *pHeightField = ( pTerrainFace == NULL ? NULL : pTerrain->GetHeightFieldPtr());

	// RActor 와 관련된 Content Chunk 를 수집한다.
	if (!vNodeArray.empty())
	{
		size_t si = chunkInstanceArray.size();
		MakeChunkInstanceListActors(pPathEngine, chunkArray, chunkInstanceArray, chunkInstanceSources, vNodeArray, &vExNodeArray);
		_ASSERT(chunkInstanceArray.size()==chunkInstanceSources.size());	// 제일 처음 Actor를 얻으므로, 일치해야 한다.

		for ( ; si<chunkInstanceArray.size(); ++si )
		{
			if ( chunkInstanceArray[si]->getContentChunk()->hasGroundComponent())
			{
				RSceneNode *pSrcNode = chunkInstanceSources[si];

				//tok의 AABB + 액터의 AABB
				RBoundingBox TotolAABB;	
				TotolAABB.Initialize();
				MGroundPtrOnTerrain *pTok = new MGroundPtrOnTerrain(chunkInstanceArray[si]->getGroundPointer());
				for(int i=0;i<pTok->faces();i++)
				{
					RVector v((float)pTok->vertexX(i),(float)pTok->vertexY(i),(float)pTok->vertexZ(i));
					TotolAABB.Add(v);
				}
				delete pTok;
				TotolAABB.Add(pSrcNode->GetAABB());

				if ( pHeightField	// NULL check
					&& pHeightField->IsIntersect(TotolAABB) ) // 지형에 충돌할 때, create new custom class for changing attribute ( 일반 portal을 terrain portal 로 바꾼다.)
				{
					MGroundPtrOnTerrain *ptr = new MGroundPtrOnTerrain(chunkInstanceArray[si]->getGroundPointer());
					groundParts.push_back(ptr);
					customGround.push_back(ptr);
				}
				else
				{
					groundParts.push_back(chunkInstanceArray[si]->getGroundPointer());
				}
			}
		}
	}
	
	// Static Mesh와 관련된 Content Chunk 를 수집한다.
	const std::string strZoneFilePath = pManager->GetRootSceneNode().GetXMLFilePath();
	if (!vStaticArray.empty())
	{
		size_t si = chunkInstanceArray.size();
		mlog("# current index : %d\n", si);
		MakeChunkInstanceListStaticMesh(pPathEngine, strZoneFilePath, chunkArray, chunkInstanceArray, vStaticArray);
		mlog("# current index : %d\n", chunkInstanceArray.size());		
		for ( ; si<chunkInstanceArray.size(); ++si )
		{
			if ( chunkInstanceArray[si]->getContentChunk()->hasGroundComponent())
				groundParts.push_back(chunkInstanceArray[si]->getGroundPointer());
		}
	}

	// buildMeshFromContent 의 속성 옵션
	const char* attributes[3] = {"optimiseWithThreshold", "150", 0};

	DWORD t = timeGetTime();
	mlog("building pathengine meshes...  ");

	if ( !groundParts.empty())
	{
		_ASSERT(m_pRawMesh==NULL);
		m_pRawMesh = m_pEngine->buildMeshFromContent(&groundParts.front(), (tSigned32)groundParts.size(), NULL);
	}

	mlog("done!... %d ms elapsed. \n", timeGetTime() - t);

	// 생성된 instance들을 해제
	if ( pTerrainFace )
		SAFE_DELETE(pTerrainFace);

	for ( size_t si = 0; si<customGround.size(); ++si )
		delete customGround[si];

	for ( size_t si = 0; si<chunkArray.size(); ++si )
		delete chunkArray[si];

	customGround.clear();
	chunkArray.clear();
	groundParts.clear();

	// Tree를 static obstacle로 처리한다.
	m_invaildExPosition.clear();

	ExportTrees(pPathEngine, pManager);
	ExportActor(pPathEngine,&vNodeArray);	// .tok 가 있는 actor도 적용되도록 하였음.
	ExportStaticMesh(pPathEngine,&vStaticArray, strZoneFilePath);

	if ( m_pRawMesh == NULL )
		m_strErrorBuffer += "Navigation-mesh isn't created.\n";

	if ( pBeforeHandler )
		m_pEngine->setErrorHandler(pBeforeHandler);

	return ( m_pRawMesh != NULL ? true : false );
}

/////////////////////////////////////////////////////////////////////////////
//

void MPathEngineExporter::SaveToFile(MPathFinder* pPathEngine, const string &strFilename)
{
	vector<string> t;
	t.push_back("small");
	SaveToFile(pPathEngine, strFilename, t);
}

/////////////////////////////////////////////////////////////////////////////
//

void MPathEngineExporter::SaveToFile(MPathFinder *pPathEngine, const string &strFilename, const vector<string> &vShapes)
{
	string meshfile = strFilename + ".navmesh.pathengine";
	cFileOutputStream* stream;
	
	stream = new cFileOutputStream(meshfile.c_str());
	
	m_pRawMesh->saveGround("tok", true, stream);
	delete stream;

	MPathAgentShapeSet pathShapeSet;
	pathShapeSet.CreateDefaultShapes(pPathEngine->GetPathEngineInterface());

	for (size_t i=0; i<vShapes.size(); i++)
	{
		MPathAgentShape* pShape = pathShapeSet.GetShape(vShapes[i]);	// 이 부분을 어떻게 맞춰야하나 ~
		if (pShape != NULL)
		{
			_ASSERT(m_pRawMesh);
#if 0
			const char* attributes[3];
			attributes[0] = "split_with_circumference_below";
			attributes[1] = "2000";
			attributes[2] = 0;

			// 전처리 파일 .collision.pathengine / .pathfind.pathengine 을 저장하는 것
			if (m_pRawMesh != NULL)
			{
				m_pRawMesh->generateCollisionPreprocessFor(pShape->m_pShape, attributes);
				m_pRawMesh->generatePathfindPreprocessFor(pShape->m_pShape, attributes);
				
				string collisionfile = strFilename + "." + vShapes[i] + ".collision.pathengine";
				stream = new cFileOutputStream(collisionfile.c_str());
				m_pRawMesh->saveCollisionPreprocessFor(pShape->m_pShape, stream);
				delete stream;

				string pathfile = strFilename + "." + vShapes[i] + ".pathfind.pathengine";
				stream = new cFileOutputStream(pathfile.c_str());
				m_pRawMesh->savePathfindPreprocessFor(pShape->m_pShape, stream);
				delete stream;
				
				m_pRawMesh->releaseAllPreprocessFor(pShape->m_pShape);
			}
			else
				_ASSERT(false);
#endif
		}
		else
			_ASSERT(false);
	}
}

//////////////////////////////////////////////////////////////////////////
//

bool MPathEngineExporter::ReadObstacleFile(std::vector<std::vector<MVector3> > &outArray, const char *filename)
{
	MXml kXml;
	if (!kXml.LoadFile(filename))
		return false;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("obstacle");
	if ( pRootElement == NULL)
		return false;

	MXmlElement *pMeshElement = pRootElement->FirstChildElement("mesh");
	int nMeshCount = 0;
	if ( pMeshElement )
		pMeshElement->Attribute("count",&nMeshCount);

	if ( nMeshCount == 0 )
		return false;

	outArray.clear();

	MXmlElement *pPolygonElement = pMeshElement->FirstChildElement("polygon");
	if ( pPolygonElement == NULL )
		return false;

	outArray.resize(nMeshCount);
	for ( int i = 0; i<nMeshCount; ++i )
	{
		std::vector<MVector3> &vVertices = outArray[i];
		int nPolygonCount;
		if ( pPolygonElement == NULL )
			break;

		pPolygonElement->Attribute("count",&nPolygonCount);

		int n = nPolygonCount;
		vVertices.resize(nPolygonCount);

		for ( MXmlElement *pVertexElement = pPolygonElement->FirstChildElement("vertex");
			pVertexElement != NULL; pVertexElement = pVertexElement->NextSiblingElement("vertex"))
		{
			std::string strContents;
			if ( _Contents(strContents, pVertexElement))
			{
				// ccw 를 cw 로 읽는다.
				MVector3 vec3df;
				vec3df = MVector3::ZERO;

				const char *pTok = strContents.c_str();
				const char *pTokEnd = pTok + strContents.length();

				for ( int i = 0; ; ++i )
				{
					if ( pTok >= pTokEnd )
						break;

					if (!isdigit(*pTok) && *pTok!='-' && *pTok!='+')
						break;

					float f = static_cast<float>(atof(pTok));
					vec3df.v[i] = f;

					pTok = strchr(pTok,' ');
					if ( pTok == NULL )
						break;

					++pTok;
				}

				--n;
				vVertices[n] = vec3df;
			}
		}
		_ASSERT(0==n);
		pPolygonElement = pPolygonElement->NextSiblingElement("polygon");
	}
	
	return true;
}

void MPathEngineExporter::ExportStaticMesh(MPathFinder* pPathEngine, vector<RSceneNode*> *pNodeArray, const string &strPath)
{
	if ( m_pRawMesh == NULL)
		return;

	if ( m_pRawMesh == NULL )
		return;

	DWORD t = timeGetTime();
	mlog("burn static-mesh information info navigation-mesh...\n");

	// prepare
	iCollisionContext* context = m_pRawMesh->newContext();
	iPathEngine* pathengine = pPathEngine->GetPathEngineInterface();

	vector<iShape*> vShapes;
	vector<iAgent*> vAgents;

	for ( vector<RSceneNode*>::iterator it = pNodeArray->begin(); it != pNodeArray->end(); ++it )
	{
		RStaticMeshSceneNode *pSceneNode = static_cast<RStaticMeshSceneNode*>(*it);
		string objFilename = pSceneNode->GetFileName();
		_ASSERT(objFilename.find(".rs") != string::npos);

		string obstacleFilename = objFilename;
		obstacleFilename = obstacleFilename.replace(obstacleFilename.find(".rs"), 4, ".obstacle");
		obstacleFilename = /*strPath + */ obstacleFilename;

		vector<vector<MVector3> > meshArray;
		if ( ReadObstacleFile(meshArray, obstacleFilename.c_str()) == false )
			continue;

		// Static mesh 이므로 항상 위치는 0, 0, 0이고 WORLD 를 기준으로 한다고 전제하고, Transform을 하지 않는다.
		_ASSERT( abs(pSceneNode->GetPosition().x) < 0.01f 
			&& abs(pSceneNode->GetPosition().y) < 0.01f 
			&& abs(pSceneNode->GetPosition().z) < 0.01f );

		for ( size_t si = 0; si<meshArray.size(); ++si )
		{
			vector<MVector3> &polyArray = meshArray[si];
			if ( polyArray.size() < 3 )	// 3개 이하 (폴리곤이 하나도 없는 것)은 제외한다.
				continue;

			// polyArray (하나의 convex 폴리곤에 대한 배열) 에 대한 shape/agent 생성 
			iAgent *agent = NULL;
			iShape *shape = NULL;

			tSigned32 nPolygonSize = (tSigned32)polyArray.size();

			vector<tSigned32> vecShapePolygon;

			vecShapePolygon.clear();
			vecShapePolygon.resize(nPolygonSize*2);

			// static - mesh 는 world 기준으로 polyArray만 사용한다.
			MVector3 vBasePosition = RVector::ZERO;
			MVector3 vCenter = MVector3::ZERO;

			cPosition cPos;
			bool bEnable = false;

			_ASSERT(m_pRawMesh);
			for ( int di = 0; di<nPolygonSize; ++di )
				vCenter += polyArray[di] - polyArray[0];

			// convex 이므로, 중심점이 거의 정확하게 구해진다.
			vCenter *= (1.0f/(float)nPolygonSize);
			vBasePosition = polyArray[0] + vCenter;

			cPos = GetClosestPosition(m_pRawMesh,vBasePosition);
			if (!m_pRawMesh->positionIsValid(cPos))
			{
				m_invaildExPosition.push_back(vBasePosition);
				continue;
			}

			/**
			 	Shape 를 구성하는 polygon 은 볼록이어야 하며, 안에 원점(0,0)이 있어야 한다.
			 */
			for ( int di = 0, xi = 0; di<nPolygonSize; ++di )
			{
				vecShapePolygon[xi++] = static_cast<int>(polyArray[di].x-vBasePosition.x);
				vecShapePolygon[xi++] = static_cast<int>(polyArray[di].y-vBasePosition.y);
			}

			float fHeight = m_pRawMesh->heightAtPositionF(cPos);
			const tSigned32 *ptr = &vecShapePolygon[0];
			shape = pathengine->newShape(nPolygonSize,ptr);
			if ( shape )
			{
				agent = m_pRawMesh->placeAgent(shape, cPos);
				if ( agent )
				{
					context->addAgent(agent);

					vAgents.push_back(agent);
					vShapes.push_back(shape);
				}
				else
				{
					delete shape;
					shape = NULL;
					m_invaildExPosition.push_back(MVector3(vBasePosition.x, vBasePosition.y, fHeight));
				}
			}
		}
	}
	mlog("The number of 'invalid ex_ position is %d. \n", m_invaildExPosition.size());

	// Burn obstacle
	m_pRawMesh->burnContextIntoMesh(context);

	delete context;
	context = NULL;

	for ( size_t si = 0; si<vAgents.size(); ++si )
		delete vAgents[si];

	for ( size_t si = 0; si<vShapes.size(); ++si )
		delete vShapes[si];

	vAgents.clear();
	vShapes.clear();

	// iAgent release, iShape release
	mlog("done!... %d ms elapsed. \n", timeGetTime() - t);
}

void MPathEngineExporter::ExportActor(MPathFinder* pPathEngine, vector<RSceneNode*> *pNodeArray)
{
	if ( m_pRawMesh == NULL )
		return;

	DWORD t = timeGetTime();
	mlog("burn actor information info navigation-mesh... ");

	// prepare
	iCollisionContext* context = m_pRawMesh->newContext();
	iPathEngine* pathengine = pPathEngine->GetPathEngineInterface();

	vector<iShape*> vShapes;
	vector<iAgent*> vAgents;

	for ( vector<RSceneNode*>::iterator it = pNodeArray->begin(); it != pNodeArray->end(); ++it )
	{
		// Actor node 를 검사하여 해당 obstacle 파일을 읽음.
		RActor *pActor = static_cast<RActor*>(*it);
		if ( pActor->GetCollisionCreationEnable() == false )
			continue;

		RMesh *pMesh = pActor->m_pMesh;

		string eluFilename = pMesh->GetName();
		_ASSERT(eluFilename.find(".elu") != string::npos);
		
		string obstacleFilename = eluFilename;
		obstacleFilename = obstacleFilename.replace(obstacleFilename.find(".elu"), 4, ".obstacle");

		vector<vector<MVector3> > meshArray;
		if ( ReadObstacleFile(meshArray, obstacleFilename.c_str()) == false )
			continue;

		// Up 벡터를 검사하여 90 도가 넘어가는 것은 제외
		if ( pActor->GetUp().DotProduct(RVector::AXISZ) <= 0.0f )
			continue;

		// 위치 선정
		RVector vPos = pActor->GetWorldPosition();
		vPos.z += 100;
		const RBoundingBox &aabb = pActor->GetAABB();

		// transform 하고, z 요소는 없앤다. 즉, z 평면에 투영한 모습이 된다.
		RMatrix matWorld = pActor->GetWorldTransform();
		// not translate
		matWorld._41 = 0.0f;		matWorld._42 = 0.0f;		matWorld._43 = 0.0f;

		for ( size_t si = 0; si<meshArray.size(); ++si )
		{
			vector<MVector3> &polyArray = meshArray[si];
			if ( polyArray.size() < 3 )
				continue;

			for ( size_t xi = 0; xi<polyArray.size(); ++xi )
			{
				MVector3 vec;
				matWorld.TransformVect(MVector3((float)polyArray[xi].x, (float)polyArray[xi].y, 0.0f), vec);
				polyArray[xi].x = (vec.x);
				polyArray[xi].y = (vec.y);
			}

			// polyArray (하나의 convex 폴리곤에 대한 배열) 에 대한 shape/agent 생성
			iAgent *agent = NULL;
			iShape *shape = NULL;

			tSigned32 nPolygonSize = (tSigned32)polyArray.size();
			
			vector<tSigned32> vecShapePolygon;

			vecShapePolygon.clear();
			vecShapePolygon.resize(nPolygonSize*2);

			MVector3 vOffset = ((polyArray[1] - polyArray[0])*.5f + (polyArray[2] - polyArray[0]))*.5f;
			MVector3 v = polyArray[0] + vOffset;
			MVector3 vAgentPos = MVector3(vPos.x + v.x, vPos.y + v.y, vPos.z);	// z(높이) 값은 offset을 더하지 않는다.
			cPosition cPos = m_pRawMesh->positionFor3DPointF((float*)&vAgentPos);
			if (!m_pRawMesh->positionIsValid(cPos))
				continue;

			float fHeight = m_pRawMesh->heightAtPositionF(cPos);
			if ( fHeight < aabb.vmin.z - 100.0f || fHeight > aabb.vmax.z + 50.f )
				continue;

			for ( int di = 0, xi = 0; di<nPolygonSize; ++di )
			{
				vecShapePolygon[xi++] = static_cast<int>(polyArray[di].x-v.x);
				vecShapePolygon[xi++] = static_cast<int>(polyArray[di].y-v.y);
			}

			const tSigned32 *ptr = &vecShapePolygon[0];
			shape = pathengine->newShape(nPolygonSize,ptr);
			if ( shape )
			{
				agent = m_pRawMesh->placeAgent(shape, cPos);
				if ( agent )
				{
					context->addAgent(agent);

					vAgents.push_back(agent);
					vShapes.push_back(shape);
				}
				else
				{
					delete shape;
					shape = NULL;
				}
			}
		}
	}

	// Burn obstacle
	m_pRawMesh->burnContextIntoMesh(context);

	delete context;
	context = NULL;

	for ( size_t si = 0; si<vAgents.size(); ++si )
		delete vAgents[si];

	for ( size_t si = 0; si<vShapes.size(); ++si )
		delete vShapes[si];

	vAgents.clear();
	vShapes.clear();

	// iAgent release, iShape release
	mlog("done!... %d ms elapsed. \n", timeGetTime() - t);
}

/////////////////////////////////////////////////////////////////////////////
//

void MPathEngineExporter::ExportTrees(MPathFinder* pPathEngine, RSceneManager* pManager)
{
	if ( m_pRawMesh == NULL )
		return;

	DWORD t = timeGetTime();
	mlog("burn tree information info navigation-mesh...  ");

	iCollisionContext* context=m_pRawMesh->newContext();
	iPathEngine* pathengine = pPathEngine->GetPathEngineInterface();

	cPosition pos;
	iAgent* agent;

	MPathAgentShape kShape;
	kShape.CreateSquareShape(pathengine, 150);

	vector<RSceneNode*> vTreeSceneNode;
	vector<iAgent*>	vAgents;
	vector<iShape*> vShapes;

	struct SSerializeTest
	{
		static bool TreeSerializeTest( RSceneNode* pSceneNode )
		{
			return MIsExactlyClass( RTreeSceneNode, pSceneNode );
		}
	};

	const RSceneNode& rootSceneNode = pManager->GetRootSceneNode();
	rootSceneNode.SerializeToVector( SSerializeTest::TreeSerializeTest, vTreeSceneNode );

	for( vector< RSceneNode* >::iterator itr = vTreeSceneNode.begin(); itr != vTreeSceneNode.end(); ++itr)
	{
		RSceneNode *pSceneNode = (*itr);
		_ASSERT(strcmp(pSceneNode->GetNodeIDString(),RSID_TREE)==0);

		RTreeSceneNode *pTree = (RTreeSceneNode*)pSceneNode;

		RVector vPos = pSceneNode->GetPosition();
		//vPos.z += 100;
		pos = GetClosestPosition(m_pRawMesh,vPos);
		if (false == m_pRawMesh->positionIsValid(pos))				continue;
		if( false == IsPositionInThreshold(m_pRawMesh,vPos,pos) )	continue;

		RTreeCollisionObject *pObject =	pTree->GetTreeCollisionInfo(RTREECOLLISION_CAPSULE);
		
		agent = NULL;
		if (pObject == NULL )
		{
			//agent = m_pRawMesh->placeAgent(kShape.m_pShape, pos);
		}
		else
		{
			const MVector3 &TreeScale = pSceneNode->GetScale();
			int nWidth = static_cast<int>(pObject->m_fRadius*TreeScale.x);
			int nHeight = static_cast<int>(pObject->m_fRadius*TreeScale.y);

			if ( nWidth > 2 && nHeight > 2)
			{
				iShape *shape = MPathAgentShape::CreateSquareShape(pathengine,nWidth,nHeight);
				_ASSERT(shape);
				agent = m_pRawMesh->placeAgent(shape, pos);

				vShapes.push_back(shape);
			}
		}

		if ( agent )
		{
			context->addAgent(agent);
			vAgents.push_back(agent);
		}
	}

	m_pRawMesh->burnContextIntoMesh(context);

	delete context;
	for (size_t i=0; i<vAgents.size(); i++)
		delete vAgents[i];
	for (size_t i=0; i<vShapes.size(); i++)
		delete vShapes[i];

	kShape.Destroy();

	mlog("done!... %d ms elapsed. \n", timeGetTime() - t);
}

cPosition MPathEngineExporter::GetClosestPosition( iMesh *pMesh,MVector3& vPos )
{
	static int		ITER	= 4;		// 반복횟수
	static float	INC_Z	= 50.0f;	// 증가분

	cPosition	cPosDown;		// vPos보다 아래에 있는 점
	cPosition	cPosUp;			// vPos보다 위에 있는 점
	bool		bValidPosDown;	// cPosDown이 유효한 점인가
	bool		bValidPosUp;	// cPosUp이 유효한 점인가
	MVector3	vPosInc(vPos);	// vPos에 z값을 더 추가한 벡터

	// vPos보다 아래에 있는 점 찾기
	cPosDown = pMesh->positionFor3DPointF((float*)&vPos);

	// vPos보다 위에 있는 점 찾기
	for(int i=0;i<ITER;i++)
	{
		vPosInc.z+= INC_Z;
		cPosUp = pMesh->positionFor3DPointF((float*)&vPosInc);
		if( cPosUp != cPosDown )	break;
	}

	//--[ 유효성 검사 ]
	bValidPosDown	= pMesh->positionIsValid(cPosDown);
	bValidPosUp		= pMesh->positionIsValid(cPosUp);

	//--[ cPosDown,cPosUp 중 어느것을 리턴해야하는지 명확한 경우 ]
	if( false == bValidPosDown && false == bValidPosUp)		return cPosDown;
	if( true  == bValidPosDown && false == bValidPosUp)		return cPosDown;
	if( false == bValidPosDown && true  == bValidPosUp)		return cPosUp;

	//--[ cPosDown,cPosUp 둘다 유효한 경우 ( cPosDown,cPosUp중 vPos와 가까운 점을 리턴한다 ) ]
	float fPosDownZ;
	float fPosUpZ;
	fPosDownZ	= pMesh->heightAtPositionF(cPosDown);
	fPosUpZ		= pMesh->heightAtPositionF(cPosUp);

	if( fabs(fPosUpZ   - vPos.z)  >	fabs(fPosDownZ - vPos.z)  )
	{// fPosDownZ가 fPosUpZ보다 vPos.z에 가깝다 => cPosDown을 선택
		return cPosDown;
	}
	//그렇지 않으면 cPosUp선택
	return cPosUp;
}

bool MPathEngineExporter::IsPositionInThreshold(iMesh *pMesh,MVector3& vPos,cPosition& cPos )
{
	static float	THRESHOLD = 200.0f;	// 실제위치(vPos)와 네비게이션메쉬 상의 위치(cPosDown,cPosUp)의 차이가 이 숫자보다 작아야 한다
	float fPosZ = pMesh->heightAtPositionF(cPos);
	float fDistance = fabs(vPos.z - fPosZ);
	if( fDistance > THRESHOLD )	return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
