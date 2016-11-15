#pragma once

#include "TreeListCtrl.h"
#include "MBuffer.h"
#include "RMeshProfileInfo.h"

class CResourceAnalyzeTreeListCtrl : public CTreeListCtrl
{
public:
	CResourceAnalyzeTreeListCtrl(void);
	~CResourceAnalyzeTreeListCtrl(void);

public:
	void	Init(void);
	void	UpdateProfileInfo(MBuffer& pBuffer);
	void	UpdateItem();

	void	ExportToFile(FILE* fp);

	void	Sort(int nItemIndex, bool bSortLess);

	bool	IsInitialized(void)	{	return m_bInit;	}
	
private:
	CImageList	m_FolderImageList;
	bool		m_bInit;

	std::vector<rs3::RMeshProfileInfo>	m_ProfileInfo;
};