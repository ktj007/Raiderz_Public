#pragma once

class MFile;

namespace rs3 {


//////////////////////////////////////////////////////////////////////////
// RsNode
//////////////////////////////////////////////////////////////////////////
class RRsNode;
struct RRsLoadHelper;
class RRsNodeLoadImpl
{
public:
	virtual bool LoadRsNode(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo) = 0;
	virtual bool LoadPolygon(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo) = 0;
};

class RRsNodeLoadImpl_v1 : public RRsNodeLoadImpl
{
public:
	virtual bool LoadRsNode(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo);
	virtual bool LoadPolygon(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo);
};

class RRsNodeLoadImpl_v2 : public RRsNodeLoadImpl_v1
{
public:
	virtual bool LoadPolygon(RRsNode* pRsNode, MFile &file, RRsLoadHelper& loading, RVertexOffsetInfo &VertexInfo);
};

//////////////////////////////////////////////////////////////////////////
// Cell
//////////////////////////////////////////////////////////////////////////
class RGeometry;
class RCell;
class RStaticMeshResource;
class RStaticMeshResourceFileLoadImpl
{
public:
	virtual bool LoadCell(MFile &file, RStaticMeshResource *pStaticMesh) = 0;
	virtual bool LoadGeometry(MFile &file, RCell* pCell, RGeometry* pGeometry, RStaticMeshResource *pStaticMesh, RRsNodeLoadImpl* pImpl) = 0;
	virtual bool LoadVertex(MFile &file, RGeometry* pGeometry) = 0;
};

class RStaticMeshResourceFileLoadImpl_v1 : public RStaticMeshResourceFileLoadImpl
{
public:
	virtual bool LoadCell(MFile &file, RStaticMeshResource *pStaticMesh);
	virtual bool LoadGeometry(MFile &file, RCell* pCell, RGeometry* pGeometry, RStaticMeshResource *pStaticMesh, RRsNodeLoadImpl* pImpl);
	virtual bool LoadVertex(MFile &file, RGeometry* pGeometry);
};

class RStaticMeshResourceFileLoadImpl_v2 : public RStaticMeshResourceFileLoadImpl_v1
{
public:
	virtual bool LoadCell(MFile &file, RStaticMeshResource *pStaticMesh);
	virtual bool LoadVertex(MFile &file, RGeometry* pGeometry);
};

class RStaticMeshResourceFileLoadImpl_v3 : public RStaticMeshResourceFileLoadImpl_v2
{
public:
	virtual bool LoadCell(MFile &file, RStaticMeshResource *pStaticMesh);

};

class RStaticMeshResourceFileLoadImpl_v4 : public RStaticMeshResourceFileLoadImpl_v3
{
public:
	virtual bool LoadCell(MFile &file, RStaticMeshResource *pStaticMesh);

};

};