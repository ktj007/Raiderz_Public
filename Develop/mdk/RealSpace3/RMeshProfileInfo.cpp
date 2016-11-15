#include "stdafx.h"
#include "RMeshProfileInfo.h"
#include "RMesh.h"
#include "RSceneSky.h"


namespace rs3
{

RMeshProfileInfo::RMeshProfileInfo(RMesh* pMesh)
{
	m_nInstanceCount = 0;
	m_nMeshNodeCount = 0;

	if(pMesh==NULL)
		return;

	int nMaterialCount = pMesh->GetMaterialResourceCount();
	
	m_MeshData.strName = pMesh->GetName();

	for(int i=0; i<nMaterialCount; ++i)
	{
		m_MaterialData.push_back( MESH_PROFILE_DATA( pMesh->GetMaterialResource(i)->m_pMaterial->GetName(), 0 ) );
	}
}

float RMeshProfileInfo::GetTextureMemory(RMaterial* pMaterial)
{
	RPFC_THISFUNC;
	float fTextureMemory = 0;
	int nLayerCount = pMaterial->GetMaxLayer();
	for( int j = 0; j < nLayerCount; ++j )
	{
		for( int k = 0; k < (int)pMaterial->GetTextureCountOfLayer(j); ++k )
		{
			RMtrlTexInfo* pTexInfo = pMaterial->GetTextureInfo(j, k);
			RTexture* pTexture = pTexInfo->GetTexture();
			if(pTexture)
			{
				const string &texName = pTexInfo->GetTexture()->GetName();
				unsigned int nTexSize = pTexInfo->GetTexture()->GetInfo().GetSize();	
				fTextureMemory += nTexSize * 0.000001f;	// BYTE -> MBYTE
			}
		}
	}

	return fTextureMemory;
}

void RMeshProfileInfo::AddInfo( RMeshNode* pMeshNode, int nSubMaterialIndex, float fCPUTime, float fGPUTime )
{
	RPFC_THISFUNC;
	float fTotalTextureMemory = 0;
	// 일반적인 액터와 그림자(ADDITIVE 나 BLEND 가 아닌 오브젝트를 한번에 그림)일때는 여기로 들어온다.
	if(nSubMaterialIndex >= 0)		
	{
		int nMaterialID = pMeshNode->m_pMtrlTableInfo[nSubMaterialIndex].mtrlid;

		// 재질이 없다면 빼자
		const RMaterialResource* pMaterialResource = pMeshNode->m_pBaseMesh->GetMaterialResource(nMaterialID);
		if( pMaterialResource == NULL )
			return;

		float fTextureMemory = GetTextureMemory(pMaterialResource->m_pMaterial);
		fTotalTextureMemory += fTextureMemory;

		MESH_PROFILE_DATA MtrlData("", 1, fCPUTime, fGPUTime, fTextureMemory);
		m_MaterialData[nMaterialID] += MtrlData;

	}
	else	// 그림자맵 으로 생성할 오브젝트가 ADDITIVE나 BLEND 일때 전체 재질에 대해서 계산해준다. 실제로도 재질 마다 따로 그리게 된다.
	{
		int nMaterialCount = pMeshNode->m_nMtrlTableInfoCnt;
	
		for(int i=0; i<nMaterialCount; ++i)
		{
			int nMaterialID = pMeshNode->m_pMtrlTableInfo[i].mtrlid;

			// 재질이 없다면 빼자
			const RMaterialResource* pMaterialResource = pMeshNode->m_pBaseMesh->GetMaterialResource(nMaterialID);
			if( pMaterialResource == NULL )
				continue;

			float fTextureMemory = GetTextureMemory(pMaterialResource->m_pMaterial);
			fTotalTextureMemory += fTextureMemory;
						
			MESH_PROFILE_DATA MtrlData("", 1, fCPUTime/(float)nMaterialCount, fGPUTime/(float)nMaterialCount, fTextureMemory);
			m_MaterialData[nMaterialID] += MtrlData;
		}
	}

	MESH_PROFILE_DATA currentData( "", 1, fCPUTime, fGPUTime, fTotalTextureMemory );
	m_MeshData += currentData;
}

void RMeshProfileInfo::AppendBinaryInfoToBuffer(MBuffer& Buffer)
{
	// Mesh Info 의 TextureMemory 를 다시 계산해준다.
	m_MeshData.fTextureMemory  = 0;
	int nMaterialCount = m_MaterialData.size();
	for( int i = 0; i < nMaterialCount; ++i )
	{
		MESH_PROFILE_DATA& MaterialData = m_MaterialData[i];
		if(MaterialData.nDrawCount == 0)
			continue;

		MaterialData.fTextureMemory /= MaterialData.nDrawCount;		// 개수만큼 나눠서 실제 메모리에 올라가는 비용을 구한다.

		m_MeshData.fTextureMemory += MaterialData.fTextureMemory;	// 재질들의 메모리의 총합
	}

	//Mesh Info
	{
		string MeshName = m_MeshData.strName;
		int MeshNameSize = MeshName.size();
		Buffer.Append((char*)&MeshNameSize, sizeof(MeshNameSize));
		Buffer.Append(const_cast<char*>(MeshName.c_str()), MeshName.size());

		Buffer.Append((char*)&m_MeshData.nDrawCount, sizeof(m_MeshData.nDrawCount));
		Buffer.Append((char*)&m_MeshData.fCPUTime, sizeof(m_MeshData.fCPUTime));
		Buffer.Append((char*)&m_MeshData.fGPUTime, sizeof(m_MeshData.fGPUTime));
		Buffer.Append((char*)&m_MeshData.fTextureMemory, sizeof(m_MeshData.fTextureMemory));

		int nInstanceCount = GetInstanceCount();
		Buffer.Append((char*)&nInstanceCount, sizeof(nInstanceCount));
		int nMeshNodeCount = GetMeshNodeCount();
		Buffer.Append((char*)&nMeshNodeCount, sizeof(nMeshNodeCount));
	}


	//Material Info
	{
		//Material Count
		int nMaterialCount = m_MaterialData.size();
		Buffer.Append((char*)&nMaterialCount, sizeof(nMaterialCount));

		for(int i=0; i<nMaterialCount; ++i)
		{
			MESH_PROFILE_DATA MaterialData = m_MaterialData[i];

			string MaterialName = MaterialData.strName;
			int MaterialNameSize = MaterialName.length();

			Buffer.Append((char*)&MaterialNameSize, sizeof(MaterialNameSize));
			Buffer.Append(const_cast<char*>(MaterialName.c_str()), MaterialName.size());
			Buffer.Append((char*)&MaterialData.nDrawCount, sizeof(MaterialData.nDrawCount));
			Buffer.Append((char*)&MaterialData.fCPUTime, sizeof(MaterialData.fCPUTime));
			Buffer.Append((char*)&MaterialData.fGPUTime, sizeof(MaterialData.fGPUTime));
			Buffer.Append((char*)&MaterialData.fTextureMemory, sizeof(MaterialData.fTextureMemory));
		}
	}
}

void RMeshProfileInfo::CreateFromBinary(MBuffer& Buffer)
{
	//Create Mesh Info
	{
		MESH_PROFILE_DATA MeshData;

		CreateDataFromBinary(Buffer, MeshData);

		//Instance count
		int nInstanceCount = 0;
		Buffer.Read((char*)&nInstanceCount, sizeof(nInstanceCount));
		if(nInstanceCount <= 0)
			nInstanceCount = 1;

		int nMeshNodeCount = 0;
		Buffer.Read((char*)&nMeshNodeCount, sizeof(nMeshNodeCount));
		if(nMeshNodeCount <= 0)
			nMeshNodeCount = 0;

		m_MeshData = MeshData;
		m_nInstanceCount = nInstanceCount;
		m_nMeshNodeCount = nMeshNodeCount;
	}

	//Create Material Info
	{
		int nMaterialCount = 0;
		Buffer.Read((char*)&nMaterialCount, sizeof(nMaterialCount));

		for(int i=0; i<nMaterialCount; ++i)
		{
			MESH_PROFILE_DATA MaterialData;

			CreateDataFromBinary(Buffer, MaterialData);

			m_MaterialData.push_back(MaterialData);
		}
	}
}

void RMeshProfileInfo::CreateDataFromBinary(MBuffer& Buffer, MESH_PROFILE_DATA& outData)
{
	char readBuffer[255];

	int nNameSize = 0;
	Buffer.Read((char*)&nNameSize,sizeof(nNameSize));
	Buffer.Read(readBuffer, nNameSize);		
	readBuffer[nNameSize] = 0;

	outData.strName = readBuffer;

	Buffer.Read((char*)&outData.nDrawCount, sizeof(outData.nDrawCount));
	Buffer.Read((char*)&outData.fCPUTime, sizeof(outData.fCPUTime));
	Buffer.Read((char*)&outData.fGPUTime, sizeof(outData.fGPUTime));
	Buffer.Read((char*)&outData.fTextureMemory, sizeof(outData.fTextureMemory));
}


bool CompareDPC(MESH_PROFILE_DATA& x, MESH_PROFILE_DATA& y)
{
	return x.nDrawCount < y.nDrawCount;
}

bool CompareCPUTime(MESH_PROFILE_DATA& x,MESH_PROFILE_DATA& y)
{
	return x.fCPUTime < y.fCPUTime;
}

bool CompareGPUTime(MESH_PROFILE_DATA& x,MESH_PROFILE_DATA& y)
{
	return x.fGPUTime < y.fGPUTime;
}

bool CompareTextureMemory(MESH_PROFILE_DATA& x, MESH_PROFILE_DATA& y)
{
	return x.fTextureMemory < y.fTextureMemory;
}

void RMeshProfileInfo::SortMaterialByDPC(bool bSortLess)
{
	SortMaterial(CompareDPC, bSortLess);
}

void RMeshProfileInfo::SortMaterialByCPUTime(bool bSortLess)
{
	SortMaterial(CompareCPUTime, bSortLess);
}

void RMeshProfileInfo::SortMaterialByGPUTime(bool bSortLess)
{
	SortMaterial(CompareGPUTime, bSortLess);
}

void RMeshProfileInfo::SortMaterialByMemory(bool bSortLess)
{
	SortMaterial(CompareTextureMemory, bSortLess);
}

void RMeshProfileInfo::SortMaterial(bool (*CompareFunc)(MESH_PROFILE_DATA& x,MESH_PROFILE_DATA& y), bool bSortLess)
{
	std::vector<MESH_PROFILE_DATA> itrBegin, itrEnd;

	if(bSortLess)
		std::sort(m_MaterialData.rbegin(), m_MaterialData.rend(), CompareFunc);
	else
		std::sort(m_MaterialData.begin(), m_MaterialData.end(), CompareFunc);
}

}