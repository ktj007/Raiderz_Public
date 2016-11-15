#include "TreeListCtrl.h"
#include ".\Profile\profile.h"
#include ".\Profile\profileVisitor.h"

class CCPUAnalyzeTreeListCtrl;


class CTreeViewMakingProfileVisitor : public CProfileVisitor
{
public:
	CTreeViewMakingProfileVisitor(CProfileManager* pProfileManager,CProfileIDToName* pProfileString,CCPUAnalyzeTreeListCtrl* pProfileTreeCtrl);
	~CTreeViewMakingProfileVisitor(void);

public:
	virtual	void OnPreTraverse(CProfileManager* pProfileManager) override {}
	virtual void OnPreOrderVisit( CProfileNode* pNode );
	virtual void OnPostOrderVisit( CProfileNode* pNode);

private:
	CProfileManager*		m_pProfileManager;
	CCPUAnalyzeTreeListCtrl*	m_pProfileTreeCtrl;
	CProfileIDToName*		m_pProfileString;
	int						m_Count;
};


class CCPUAnalyzeTreeListCtrl : public CTreeListCtrl
{
public:
	CCPUAnalyzeTreeListCtrl(void);
	~CCPUAnalyzeTreeListCtrl(void);

public:
	void	Init(void);
	void	SetProfileManager(CProfileManager* pProfileManager,CProfileIDToName* pProfileIDToName);

	bool	IsInitialized(void)	{	return m_bInit;	}

	void	ExportToFile(FILE* fp);

private:
	CImageList					m_FolderImageList;
	bool						m_bInit;
};