#pragma once

#include "RActor.h"

namespace rs3 
{

struct MESH_PROFILE_DATA
{
	MESH_PROFILE_DATA(std::string name ="No Name", UINT DPC =0, float cputime =0.0f, float gputime = 0.0f, float fMemory = 0.0f)
	{
		if(name == "")
			name = "No Name";

		strName			= name;
		nDrawCount		= DPC;
		fCPUTime		= cputime;
		fGPUTime		= gputime;
		fTextureMemory	= fMemory;
	}

	MESH_PROFILE_DATA&	operator+=( const MESH_PROFILE_DATA& rhs )
	{
		nDrawCount		+= rhs.nDrawCount;
		fCPUTime		+= rhs.fCPUTime;
		fGPUTime		+= rhs.fGPUTime;
		fTextureMemory	+= rhs.fTextureMemory;
		return *this;
	}

	MESH_PROFILE_DATA operator+(const MESH_PROFILE_DATA& rhs) const 
	{
		MESH_PROFILE_DATA temp;
		temp.nDrawCount		= nDrawCount + rhs.nDrawCount;
		temp.fCPUTime		= fCPUTime + rhs.fCPUTime;
		temp.fGPUTime		= fGPUTime + rhs.fGPUTime;
		temp.fTextureMemory = fTextureMemory + rhs.fTextureMemory;

		return temp;
	}

	std::string		strName;
	UINT			nDrawCount;
	float			fCPUTime;
	float			fGPUTime;
	float			fTextureMemory;
};


class RS_API RMeshProfileInfo
{
public:
	RMeshProfileInfo(RMesh* pMesh);
	~RMeshProfileInfo() {};

	void	AddInfo( RMeshNode* pMeshNode, int nSubMaterialIndex, float fCPUTime, float fGPUTime);
	void	AppendBinaryInfoToBuffer(MBuffer& Buffer);
	void	CreateFromBinary(MBuffer& Buffer);
	void	CreateDataFromBinary(MBuffer& Buffer, MESH_PROFILE_DATA& outData);
	void	AddRef()	{ ++m_nInstanceCount; }

	int					GetInstanceCount()				{ return m_nInstanceCount; }
	int					GetMeshNodeCount()				{ return m_nMeshNodeCount; }
	MESH_PROFILE_DATA	GetMeshData()					{ return m_MeshData; }
	int					GetMaterialCount()				{ return m_MaterialData.size(); }
	MESH_PROFILE_DATA	GetMaterialData(int nIndex)		{ return m_MaterialData[nIndex]; }

	void				SortMaterialByDPC(bool bSortLess);
	void				SortMaterialByCPUTime(bool bSortLess);
	void				SortMaterialByGPUTime(bool bSortLess);
	void				SortMaterialByMemory(bool bSortLess);
	void				SortMaterial(bool (*CompareFunc)(MESH_PROFILE_DATA& x,MESH_PROFILE_DATA& y), bool bSortLess);

	float				GetTextureMemory(RMaterial* pMaterial);

	int					m_nMeshNodeCount;	// 메쉬 노드의 개수

private:
	int					m_nInstanceCount;	// 인스턴싱 처리 되는 오브젝트 개수

	MESH_PROFILE_DATA					m_MeshData;
	std::vector<MESH_PROFILE_DATA>		m_MaterialData;
};

}