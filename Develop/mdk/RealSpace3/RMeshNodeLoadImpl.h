#pragma once

#include "RFile.h"

class MFile;

namespace rs3 {

class RMeshNode;
class RMeshNodeLoaderImpl
{
protected:

public:
	RMeshNodeLoaderImpl()
	{
	}

	virtual ~RMeshNodeLoaderImpl()
	{
	}
	virtual bool Load(RMeshNode *pNode, RFile *pFile) = 0;
};

class RMeshNodeLoaderImpl_v12 : public RMeshNodeLoaderImpl
{
	struct RFaceSubData_v12
	{
		WORD p;		// pos		index	
		WORD uv;	// tex uv	index
		WORD n;		// normal	index
		WORD n_tan;	// tangent	index
		WORD n_bin;	// tangent  index
	};

	struct RVertexIndex_v12
	{
		union {
			struct{
				WORD p;
				WORD n;
				WORD uv;
				WORD n_tan;
				WORD n_bin;
			};
			WORD index[5];
		};
	};

public:
	virtual bool Load(RMeshNode *pNode, RFile *pFile);

	virtual bool LoadName(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadInfo(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadFace(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadVertexInfo(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadEtc(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v13 : public RMeshNodeLoaderImpl_v12
{
public:
	virtual bool Load(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadEtc(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v14 : public RMeshNodeLoaderImpl_v13
{
public:
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v15 : public RMeshNodeLoaderImpl_v14
{
public:
	virtual bool LoadFace(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadEtc(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v16 : public RMeshNodeLoaderImpl_v15
{
public:
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v17 : public RMeshNodeLoaderImpl_v16
{
public:
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v18 : public RMeshNodeLoaderImpl_v17
{
public:
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
};

class RMeshNodeLoaderImpl_v20 : public RMeshNodeLoaderImpl_v18
{
public:
	//virtual bool Load(RMeshNode *pNode, RFile *pFile);

	virtual bool LoadName(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadInfo(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadVertex(RMeshNode *pNode, RFile *pFile);
	virtual bool LoadEtc(RMeshNode *pNode, RFile *pFile);
	
	
	//virtual bool LoadFace(RMeshNode *pNode, RFile *pFile);
	//virtual bool LoadVertexInfo(RMeshNode *pNode, RFile *pFile);

};

};