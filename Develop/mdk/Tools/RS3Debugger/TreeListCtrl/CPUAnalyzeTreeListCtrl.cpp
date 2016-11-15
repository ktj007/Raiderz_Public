#include "stdafx.h"
#include "CPUAnalyzeTreeListCtrl.h"
#include "..\resource.h"




CTreeViewMakingProfileVisitor::CTreeViewMakingProfileVisitor(CProfileManager* pProfileManager,CProfileIDToName* pProfileString, CCPUAnalyzeTreeListCtrl* pProfileTreeCtrl )
	:m_pProfileManager(pProfileManager),m_pProfileString(pProfileString),m_pProfileTreeCtrl(pProfileTreeCtrl),m_Count(0)
{
}

CTreeViewMakingProfileVisitor::~CTreeViewMakingProfileVisitor( void )
{

}

void CTreeViewMakingProfileVisitor::OnPreOrderVisit( CProfileNode* pNode )
{
	std::string	NameString;
	if( NULL != m_pProfileString )
	{
		NameString = m_pProfileString->GetNameFromIDToNameMap(pNode->Get_ID());
	}
	else
	{
		NameString = "None";
	}
	

	char buffer[512];
	char bufferCalls[512];
	char bufferPercentage[512];
//	char bufferGoalPercentage[512];
	
	ITEM_INDEX iItem = m_pProfileTreeCtrl->AddItem(NameString.c_str(), m_Count % 3, pNode->Get_Depth());
	

	//time
	sprintf_s(buffer,512,"%5.2f",pNode->Get_Current_Time() * 1000);	// CPU + GPU
	m_pProfileTreeCtrl->SetItemText(iItem, 1, buffer);

	if( pNode->Get_Current_GPUTime() )
		sprintf_s(buffer,512,"%5.2f",pNode->Get_Current_GPUTime() * 1000);	// GPU Only
	else
		strcpy_s(buffer, " ");	// 보기 편하게 하기 위해 값이 없는 녀석은 공란으로 표시해준다.
	m_pProfileTreeCtrl->SetItemText(iItem, 2, buffer);

	sprintf_s(buffer,512,"%5.2f", ( pNode->Get_Current_Time() - pNode->Get_Current_GPUTime() ) * 1000);	// CPU Only
	m_pProfileTreeCtrl->SetItemText(iItem, 3, buffer);

	//calls
	sprintf_s(bufferCalls,512,"%i",pNode->Get_Current_Calls());
	m_pProfileTreeCtrl->SetItemText(iItem, 4, bufferCalls);

	float Percentage;
	if( 0.0f == m_pProfileManager->Get_Time_Since_ResetCurrentInfoCall() )
		Percentage = 0.0f;
	else
		Percentage = pNode->Get_Current_Time()/ m_pProfileManager->Get_Time_Since_ResetCurrentInfoCall() * 100;

	sprintf_s(bufferPercentage,512,"%3.3f %%", Percentage);	// CPU
	m_pProfileTreeCtrl->SetItemText(iItem, 5, bufferPercentage);

	sprintf_s(bufferPercentage,512,"%3.3f %%", Percentage);	// GPU
	m_pProfileTreeCtrl->SetItemText(iItem, 6, bufferPercentage);

	//float GoalPercentage = pNode->Get_Current_Time() / 16.0f * 100000;
	//sprintf_s(bufferGoalPercentage,512,"%3.3f %%", GoalPercentage);
	//m_pProfileTreeCtrl->SetItemText(iItem, 4, bufferGoalPercentage);

	m_Count++;
}

void CTreeViewMakingProfileVisitor::OnPostOrderVisit( CProfileNode* pNode )
{

}

















CCPUAnalyzeTreeListCtrl::CCPUAnalyzeTreeListCtrl( void )
:m_bInit(false)
{
}

CCPUAnalyzeTreeListCtrl::~CCPUAnalyzeTreeListCtrl( void )
{

}

void CCPUAnalyzeTreeListCtrl::Init( void )
{
	LVCOLUMN	column;
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;

	column.fmt		= LVCFMT_LEFT;
	column.cx		= 200;
	column.pszText	= _T("Name");
	column.iSubItem	= 0;
	this->InsertColumn(0, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 60;
	column.pszText	= _T("Time(ms)");
	column.iSubItem	= 1;
	this->InsertColumn(1, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 50;
	column.pszText	= _T("GPU Time(ms)");
	column.iSubItem	= 2;
	this->InsertColumn(2, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 50;
	column.pszText	= _T("CPU Time(ms)");
	column.iSubItem	= 3;
	this->InsertColumn(3, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 40;
	column.pszText	= _T("Calls");
	column.iSubItem	= 4;
	this->InsertColumn(4, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("CPU Percentage");
	column.iSubItem	= 5;
	this->InsertColumn(5, &column);

	column.fmt		= LVCFMT_RIGHT;
	column.cx		= 75;
	column.pszText	= _T("GPU Percentage");
	column.iSubItem	= 6;
	this->InsertColumn(6, &column);

	//column.fmt		= LVCFMT_RIGHT;
	//column.cx		= 75;
	//column.pszText	= _T("Goal Percentage");
	//column.iSubItem	= 4;
	//this->InsertColumn(4, &column);

	// Step three: We create and set the image list 
	m_FolderImageList.Create(IDB_FOLDERS, 16, 1, RGB(255, 0, 255));
	this->SetImageList(&m_FolderImageList, LVSIL_SMALL);

	m_bInit = true;
}

void CCPUAnalyzeTreeListCtrl::SetProfileManager( CProfileManager* pProfileManager,CProfileIDToName* pProfileIDToName )
{
	this->DeleteAllItems();
	if( NULL != pProfileManager)
	{
		CTreeViewMakingProfileVisitor ProfileTreeVisitor(pProfileManager,pProfileIDToName,this);
		pProfileManager->Traverse(&ProfileTreeVisitor,pProfileManager->GetCallStackProfileRootNode());

		CTreeViewMakingProfileVisitor ProfileTreeVisitor2(pProfileManager,pProfileIDToName,this);
		pProfileManager->Traverse(&ProfileTreeVisitor2,pProfileManager->GetNoCallStackProfileRootNode());
	}
}

void CCPUAnalyzeTreeListCtrl::ExportToFile(FILE* fp)
{
	int nMaxFirstColumnLength = 0;
	for(UINT i=0; i<m_tree.size(); ++i)
	{
		tree_list_info info = m_tree[i];
		int nThisFirstColumnLength = 2*info.m_nLevel + info.m_lstCols[0].GetLength();

		if( nMaxFirstColumnLength < nThisFirstColumnLength )
			nMaxFirstColumnLength = nThisFirstColumnLength;
	}

	// print header
	fprintf_s(fp, "%s", GetColumnName(0) );
	int nPaddingSpace = nMaxFirstColumnLength - strlen(GetColumnName(0)) + 2;
	for( int j=0; j<nPaddingSpace; j++)
		fprintf_s(fp, " ");

	for( int j=1; j< GetColumnsCount(); j++ )
	{
		fprintf_s(fp, "%s ,", GetColumnName(j) );
	}
	fprintf_s(fp,"\n");

	for(UINT i=0; i<m_tree.size(); ++i)
	{
		tree_list_info info = m_tree[i];
		for( int j=0; j< info.m_nLevel; j++ )
		{
			fprintf_s(fp, "  ");
		}

		fprintf_s(fp, "%s", info.m_lstCols[0]);

		int nThisFirstColumnLength = 2*info.m_nLevel + info.m_lstCols[0].GetLength();

		int nPaddingSpace = nMaxFirstColumnLength - nThisFirstColumnLength + 2;
		for( int j=0; j<nPaddingSpace; j++)
			fprintf_s(fp, " ");

		for( int j=1; j< GetColumnsCount(); j++ )
		{
			fprintf_s(fp, "%s ,", info.m_lstCols[j]);
		}

		fprintf_s(fp,"\n");
	}
}