#include "stdafx.h"
#include "profile.h"
#include "profileVisitor.h"


CProfileVisitor::CProfileVisitor( void)
	:m_PreOderVisitCallCounter(0)
{
}

CProfileVisitor::~CProfileVisitor( void )
{

}

void CProfileVisitor::PreTraverse(CProfileManager* pProfileManager)
{
	OnPreTraverse(pProfileManager);
}

void CProfileVisitor::PreOrderVisit(CProfileNode* pNode)
{
	OnPreOrderVisit(pNode);
	m_PreOderVisitCallCounter++;
}

void CProfileVisitor::PostOrderVisit( CProfileNode* pNode )
{
	OnPostOrderVisit(pNode);	
}

void CProfileVisitor::PostTraverse( CProfileManager* pProfileManager )
{
	OnPostTraverse(pProfileManager);
}

CBinarySavingProfileVisitor::CBinarySavingProfileVisitor(CProfileManager* pProfileManager,MBuffer* pProfileBuffer)
	:m_pProfileManager(pProfileManager),m_pProfileBuffer(pProfileBuffer)
{
}

CBinarySavingProfileVisitor::~CBinarySavingProfileVisitor( void )
{
}

void CBinarySavingProfileVisitor::OnPreTraverse( CProfileManager* pProfileManager)
{
	float TimeSinceCurrentReset = m_pProfileManager->Get_Time_Since_CurrentReset();
	m_pProfileBuffer->Append((char*)&TimeSinceCurrentReset,sizeof(TimeSinceCurrentReset));
}

void CBinarySavingProfileVisitor::OnPreOrderVisit( CProfileNode* pNode)
{
	m_pProfileBuffer->Append((char*)&pNode->Depth,sizeof(pNode->Depth));
	m_pProfileBuffer->Append((char*)&pNode->ID,sizeof(pNode->ID));
	m_pProfileBuffer->Append((char*)&pNode->Current_Time,sizeof(pNode->Current_Time));
	m_pProfileBuffer->Append((char*)&pNode->Current_GPUTime,sizeof(pNode->Current_GPUTime));
	m_pProfileBuffer->Append((char*)&pNode->CurrentFrameCalls,sizeof(pNode->CurrentFrameCalls));
}

void CBinarySavingProfileVisitor::OnPostOrderVisit( CProfileNode* pNode)
{

}

void CBinarySavingProfileVisitor::OnPostTraverse( CProfileManager* pProfileManager )
{
}

CNameBinarySavingProfileVisitor::CNameBinarySavingProfileVisitor( MBuffer* pProfileBuffer )
	:m_pProfileBuffer(pProfileBuffer)
{
}

CNameBinarySavingProfileVisitor::~CNameBinarySavingProfileVisitor( void )
{
}

void CNameBinarySavingProfileVisitor::OnPreOrderVisit( CProfileNode* pNode )
{
	m_pProfileBuffer->Append((char*)&pNode->ID,sizeof(pNode->ID));

	int NameSize = strlen(pNode->Get_Name());
	m_pProfileBuffer->Append((char*)&NameSize,sizeof(NameSize));		

	m_pProfileBuffer->Append((char*)pNode->Get_Name(),NameSize);

}

void CNameBinarySavingProfileVisitor::OnPostOrderVisit( CProfileNode* pNode )
{

}





CProfileInfoVisitor::CProfileInfoVisitor( CProfileManager* pProfileManager,void* pUserPointer,CProfileInfoVisitorCALLBACK* pCallBack )
	:m_fTimeSinceRest(0.0f),m_pProfileManager(pProfileManager),m_pUserPointer(pUserPointer),m_pCallBack(pCallBack)
{
}

CProfileInfoVisitor::~CProfileInfoVisitor( void )
{

}

void CProfileInfoVisitor::OnPreTraverse(CProfileManager* pProfileManager)
{
	m_fTimeSinceRest = m_pProfileManager->Get_Time_Since_Reset();
}

void CProfileInfoVisitor::OnPreOrderVisit( CProfileNode* pNode )
{
	char buffer[2048];
	float fChildRatio = pNode->Get_Total_Time()/ m_fTimeSinceRest;		

	int nIndent = pNode->Get_Depth()*4;
	memset(buffer,' ',nIndent);
	if( pNode->Get_Total_Calls() != 0 )
	{
		sprintf(buffer+nIndent,"%s    %3.3f%% / %5.3f sec , call %d , sec/call : %f \n",pNode->Get_Name(),fChildRatio * 100.f, pNode->Get_Total_Time(), pNode->Get_Total_Calls(), pNode->Get_Total_Time() / pNode->Get_Total_Calls() );
	}
	else
	{
		sprintf(buffer+nIndent,"%s    %3.3f%% / %5.3f sec \n",pNode->Get_Name(),fChildRatio * 100.f, pNode->Get_Total_Time() );
	}

	m_pCallBack(buffer,m_pUserPointer);
}

void CProfileInfoVisitor::OnPostOrderVisit( CProfileNode* pNode )
{

}

//---------------------------------------------------------------------------------------------------------------

CValidatorProfileVisitor::CValidatorProfileVisitor( void )
{
	m_Valid = true; 
}

CValidatorProfileVisitor::~CValidatorProfileVisitor( void )
{

}

void CValidatorProfileVisitor::OnPreOrderVisit( CProfileNode* pNode )
{
	if( NULL != pNode->Get_Parent() )
	{
		if( (pNode->Get_Depth()-1) != pNode->Get_Parent()->Get_Depth() )
		{
			m_Valid = false;
		}
		_ASSERT( (pNode->Get_Depth()-1) == pNode->Get_Parent()->Get_Depth() );
	}
}