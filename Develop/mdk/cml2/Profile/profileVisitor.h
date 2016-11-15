#pragma once

#include <stdio.h>
#include "..\MBuffer.h"

class CProfileNode;
class CProfileManager;

class CML2_API CProfileVisitor
{
public:
	CProfileVisitor(void);
	virtual ~CProfileVisitor(void);

public:
	void PreTraverse(CProfileManager* pProfileManager);
	void PreOrderVisit(CProfileNode* pNode);	
	void PostOrderVisit(CProfileNode* pNode);
	void PostTraverse(CProfileManager* pProfileManager);

	virtual	void OnPreTraverse(CProfileManager* pProfileManager)	{}
	virtual void OnPreOrderVisit( CProfileNode* pNode )				{}
	virtual void OnPostOrderVisit( CProfileNode* pNode)				{}
	virtual void OnPostTraverse(CProfileManager* pProfileManager)	{}

public:
	int	GetPreOderVisitCallCount(void)	{	return m_PreOderVisitCallCounter;	}

private:
	int	m_PreOderVisitCallCounter;	// PreOrderVisit가 호출된 횟수
};


class CML2_API CValidatorProfileVisitor : public CProfileVisitor
{
public:
	CValidatorProfileVisitor(void);	
	~CValidatorProfileVisitor(void);
public:
	virtual void OnPreOrderVisit( CProfileNode* pNode )		override;
	bool	IsValid(void)	{	return m_Valid;	}

private:
	bool	m_Valid;
};

class CML2_API CBinarySavingProfileVisitor : public CProfileVisitor
{
public:
	CBinarySavingProfileVisitor(CProfileManager* pProfileManager,MBuffer* pProfileBuffer);
	~CBinarySavingProfileVisitor(void);
public:
	virtual	void OnPreTraverse(CProfileManager* pProfileManager) override;
	virtual void OnPreOrderVisit( CProfileNode* pNode ) override;
	virtual void OnPostOrderVisit( CProfileNode* pNode) override;
	virtual void OnPostTraverse(CProfileManager* pProfileManager) override;

private:
	MBuffer*		GetProfileBuffer(void)	{	return m_pProfileBuffer;	}

private:
	CProfileManager*	m_pProfileManager;
	MBuffer*			m_pProfileBuffer;
};

class CML2_API CNameBinarySavingProfileVisitor : public CProfileVisitor
{
public:
	CNameBinarySavingProfileVisitor(MBuffer* pProfileBuffer);
	~CNameBinarySavingProfileVisitor(void);

public:
	virtual	void OnPreTraverse(CProfileManager* pProfileManager) override {}
	virtual void OnPreOrderVisit( CProfileNode* pNode ) override;
	virtual void OnPostOrderVisit( CProfileNode* pNode) override;
private:
	MBuffer*		GetProfileBuffer(void)	{	return m_pProfileBuffer;	}

private:
	MBuffer*		m_pProfileBuffer;
};


typedef void(CProfileInfoVisitorCALLBACK) (const char* szString, void* pUserPointer);

class CML2_API CProfileInfoVisitor : public CProfileVisitor
{
public:
	CProfileInfoVisitor(CProfileManager* pProfileManager,void* pUserPointer,CProfileInfoVisitorCALLBACK* pCallBack);
	~CProfileInfoVisitor(void);

public:
	virtual	void OnPreTraverse(CProfileManager* pProfileManager) override;
	virtual void OnPreOrderVisit( CProfileNode* pNode ) override;
	virtual void OnPostOrderVisit( CProfileNode* pNode) override;

private:
	float			m_fTimeSinceRest;
	CProfileManager *m_pProfileManager;
	void*	m_pUserPointer;
	CProfileInfoVisitorCALLBACK* m_pCallBack;
};

