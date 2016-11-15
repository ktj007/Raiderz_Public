#include "stdafx.h"
#include "ResourceAnalyzeTreeListCtrl.h"
#include "..\resource.h"
#include "RProfileInfo.h"
#include <algorithm>

enum ITEM_LIST
{
	IL_NAME =0,
	IL_OBJECT_COUNT =1,
	IL_NODE_COUNT = 2,
	IL_MATERIAL_COUNT =3,
	IL_DP_COUNT =4,
	IL_CPUTIME =5,
	IL_GPUTIME = 6,
	IL_TEXTUREMEMORY = 7
};

CResourceAnalyzeTreeListCtrl::CResourceAnalyzeTreeListCtrl( void )
:m_bInit(false)
{
}

CResourceAnalyzeTreeListCtrl::~CResourceAnalyzeTreeListCtrl( void )
{

}

void CResourceAnalyzeTreeListCtrl::Init()
{
	LVCOLUMN	column;
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;

	column.fmt		= LVCFMT_LEFT;
	column.cx		= 250;
	column.pszText	= _T("Name");
	column.iSubItem	= IL_NAME;
	this->InsertColumn(IL_NAME, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("Actor Count");
	column.iSubItem	= IL_OBJECT_COUNT;
	this->InsertColumn(IL_OBJECT_COUNT, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("Node Count(ActorNode)");
	column.iSubItem	= IL_NODE_COUNT;
	this->InsertColumn(IL_NODE_COUNT, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("Material Count (Used/Total)");
	column.iSubItem	= IL_MATERIAL_COUNT;
	this->InsertColumn(IL_MATERIAL_COUNT, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("Draw Count(DP Call)");
	column.iSubItem	= IL_DP_COUNT;
	this->InsertColumn(IL_DP_COUNT, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("CPU Time(DP Call)");
	column.iSubItem	= IL_CPUTIME;
	this->InsertColumn(IL_CPUTIME, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("GPU Time(DP Call)");
	column.iSubItem	= IL_GPUTIME;
	this->InsertColumn(IL_GPUTIME, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("Texture Memory(MByte)");
	column.iSubItem	= IL_TEXTUREMEMORY;
	this->InsertColumn(IL_TEXTUREMEMORY, &column);

	m_FolderImageList.Create(IDB_FOLDERS, 16, 1, RGB(255, 0, 255));
	this->SetImageList(&m_FolderImageList, LVSIL_SMALL);

	m_bInit = true;
}

void CResourceAnalyzeTreeListCtrl::UpdateProfileInfo(MBuffer& pBuffer)
{
	m_ProfileInfo.clear();

	int nBufferSize = 0;
	pBuffer.Read((char*)&nBufferSize, sizeof(nBufferSize));

	while( pBuffer.GetRemainReadSize() > 0 )
	{
		rs3::RMeshProfileInfo MeshProfileInfo(NULL);
		MeshProfileInfo.CreateFromBinary(pBuffer);

		m_ProfileInfo.push_back(MeshProfileInfo);
	}
}

void CResourceAnalyzeTreeListCtrl::UpdateItem()
{
	DeleteAllItems();
	
	const int nReadBufferSize = 255;
	char readBuffer[nReadBufferSize];

	for(UINT i=0; i<m_ProfileInfo.size(); ++i)
	{
		rs3::RMeshProfileInfo MeshProfileInfo = m_ProfileInfo[i];

		//Mesh Data
		ITEM_INDEX nMeshIndex;
		{
			rs3::MESH_PROFILE_DATA MeshData = MeshProfileInfo.GetMeshData();

			nMeshIndex = AddItem(MeshData.strName.c_str(), 0, 0);

			sprintf_s(readBuffer, nReadBufferSize,"%d", MeshProfileInfo.GetInstanceCount());
			SetItemText(nMeshIndex, IL_OBJECT_COUNT, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%d", MeshProfileInfo.GetMeshNodeCount());
			SetItemText(nMeshIndex, IL_NODE_COUNT, readBuffer);
			
			sprintf_s(readBuffer, nReadBufferSize,"%d", MeshData.nDrawCount);
			SetItemText(nMeshIndex, IL_DP_COUNT, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MeshData.fCPUTime);
			SetItemText(nMeshIndex, IL_CPUTIME, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MeshData.fGPUTime);
			SetItemText(nMeshIndex, IL_GPUTIME, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MeshData.fTextureMemory);
			SetItemText(nMeshIndex, IL_TEXTUREMEMORY, readBuffer);
		}

		
		//Material Data
		int nNotUsedMaterial =0;
		for(int i=0; i<MeshProfileInfo.GetMaterialCount(); ++i)
		{
			rs3::MESH_PROFILE_DATA MaterialData = MeshProfileInfo.GetMaterialData(i);
			if(MaterialData.nDrawCount == 0)
			{
				++nNotUsedMaterial;
				continue;
			}

			ITEM_INDEX nMaterialIndex = AddItem(MaterialData.strName.c_str(), 1, 1);

			sprintf_s(readBuffer, nReadBufferSize,"%d", MaterialData.nDrawCount);
			SetItemText(nMaterialIndex, IL_DP_COUNT, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MaterialData.fCPUTime);
			SetItemText(nMaterialIndex, IL_CPUTIME, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MaterialData.fGPUTime);
			SetItemText(nMaterialIndex, IL_GPUTIME, readBuffer);

			sprintf_s(readBuffer, nReadBufferSize,"%3.3f", MaterialData.fTextureMemory);
			SetItemText(nMaterialIndex, IL_TEXTUREMEMORY, readBuffer);
		}


		//Mesh Material Count
//		sprintf_s(readBuffer, nReadBufferSize,"%d / %d", MeshProfileInfo.GetMaterialCount()-nNotUsedMaterial, MeshProfileInfo.GetMaterialCount());
		sprintf_s(readBuffer, nReadBufferSize,"%d", MeshProfileInfo.GetMaterialCount()-nNotUsedMaterial);
		SetItemText(nMeshIndex, IL_MATERIAL_COUNT, readBuffer);


		//Not Used Material List
		if(nNotUsedMaterial>0)
		{
			ITEM_INDEX nNotUsedMaterialIndex = AddItem("Not Used Material", 1, 1);
			sprintf_s(readBuffer, nReadBufferSize,"%d", nNotUsedMaterial);
			SetItemText(nNotUsedMaterialIndex, IL_MATERIAL_COUNT, readBuffer);

			for(int i=0; i<MeshProfileInfo.GetMaterialCount(); ++i)
			{
				rs3::MESH_PROFILE_DATA MaterialData = MeshProfileInfo.GetMaterialData(i);
				if(MaterialData.nDrawCount > 0)
					continue;

				AddItem(MaterialData.strName.c_str(), 2, 2);
			}
		}

	}
}

void CResourceAnalyzeTreeListCtrl::ExportToFile(FILE* fp)
{
	fprintf_s(fp, "Actor Name, Material Name, Actor Count, Node Count, Material Count, Draw Count, Draw Time(ms), Texture Memory\n");

	for(UINT i=0; i<m_tree.size(); ++i)
	{
		tree_list_info info = m_tree[i];
		if(info.m_lstCols[IL_DP_COUNT]=="")
			continue;
		if(info.m_nLevel > 0)
			continue;

		if(info.m_nLevel==0)
			fprintf_s(fp, "%s,,", info.m_lstCols[IL_NAME]);
		else
			fprintf_s(fp, ",%s,", info.m_lstCols[IL_NAME]);

		fprintf_s(fp, "%s,%s,%s,%s,%s,%s\n", info.m_lstCols[IL_OBJECT_COUNT], info.m_lstCols[IL_NODE_COUNT], info.m_lstCols[IL_MATERIAL_COUNT],
			info.m_lstCols[IL_DP_COUNT], info.m_lstCols[IL_CPUTIME], info.m_lstCols[IL_TEXTUREMEMORY]);
	}


	int nTotalInstance	= 0;
	int nTotalMaterial	= 0;
	int nDPC			= 0;
	float fCPUTotalTime	= 0.0f;
	float fGPUTotalTime = 0.0f;
	float fTotalMemory	= 0.0f;

	for(UINT i=0; i<m_ProfileInfo.size(); ++i)
	{
		nTotalInstance	+= m_ProfileInfo[i].GetInstanceCount();
		nTotalMaterial	+= m_ProfileInfo[i].GetMaterialCount();
		nDPC			+= m_ProfileInfo[i].GetMeshData().nDrawCount;
		fCPUTotalTime	+= m_ProfileInfo[i].GetMeshData().fCPUTime;
		fGPUTotalTime	+= m_ProfileInfo[i].GetMeshData().fGPUTime;
		fTotalMemory	+= m_ProfileInfo[i].GetMeshData().fTextureMemory;
	}

	fprintf_s(fp, "%s,,%d,,%d,%d,%.3f,%.3f,%.3f\n", "Total", nTotalInstance, nTotalMaterial, nDPC, fCPUTotalTime, fGPUTotalTime, fTotalMemory);	
}


bool CompareInstanceCount(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetInstanceCount() < y.GetInstanceCount();
}

bool CompareMaterialCount(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetMaterialCount() < y.GetMaterialCount();
}

bool CompareDPC(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetMeshData().nDrawCount < y.GetMeshData().nDrawCount;
}

bool CompareCPUTime(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetMeshData().fCPUTime < y.GetMeshData().fCPUTime;
}

bool CompareGPUTime(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetMeshData().fGPUTime < y.GetMeshData().fGPUTime;
}

bool CompareMemory(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y)
{
	return x.GetMeshData().fTextureMemory < y.GetMeshData().fTextureMemory;
}

void CResourceAnalyzeTreeListCtrl::Sort( int nItemIndex, bool bSortLess )
{
	std::vector<rs3::RMeshProfileInfo> itrBegin, itrEnd;

	bool (*CompareFunc)(rs3::RMeshProfileInfo& x, rs3::RMeshProfileInfo& y);

	switch (nItemIndex)
	{
	case IL_NAME:
		return;
	case IL_OBJECT_COUNT:
		CompareFunc =CompareInstanceCount;
		break;
	case IL_MATERIAL_COUNT:
		CompareFunc =CompareMaterialCount;
		break;
	case IL_DP_COUNT:
		CompareFunc =CompareDPC;
		break;
	case IL_CPUTIME:
		CompareFunc =CompareCPUTime;
		break;
	case IL_GPUTIME:
		CompareFunc =CompareGPUTime;
		break;
	case IL_TEXTUREMEMORY:
		CompareFunc = CompareMemory;
	}

	if(bSortLess)
		std::sort(m_ProfileInfo.rbegin(), m_ProfileInfo.rend(), CompareFunc);
	else
		std::sort(m_ProfileInfo.begin(), m_ProfileInfo.end(), CompareFunc);

	if(nItemIndex==IL_DP_COUNT)
		std::for_each(m_ProfileInfo.begin(), m_ProfileInfo.end(), std::bind2nd(mem_fun_ref(&rs3::RMeshProfileInfo::SortMaterialByDPC), bSortLess));
	
	if(nItemIndex==IL_CPUTIME)
		std::for_each(m_ProfileInfo.begin(), m_ProfileInfo.end(), std::bind2nd(mem_fun_ref(&rs3::RMeshProfileInfo::SortMaterialByCPUTime), bSortLess));

	if(nItemIndex==IL_GPUTIME)
		std::for_each(m_ProfileInfo.begin(), m_ProfileInfo.end(), std::bind2nd(mem_fun_ref(&rs3::RMeshProfileInfo::SortMaterialByGPUTime), bSortLess));
	
	UpdateItem();
}