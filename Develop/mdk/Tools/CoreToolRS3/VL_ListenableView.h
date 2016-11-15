#pragma once
#include "CoreTypes.h"

class CControlMediator;
class CWorkSpaceManager;

struct CoreMessage
{
	CoreMessage(VIEW_MESSAGE::ID _id) : nID(_id), pLogicObject(NULL) {}
	CoreMessage(VIEW_MESSAGE::ID _id, CLogicObject* _pLogicObject) : nID(_id), pLogicObject(_pLogicObject) {}

	VIEW_MESSAGE::ID	nID;
	CLogicObject*		pLogicObject;
};

/** @brief Listenable View */
class CORETOOL_API CListenableView
{
public:
							CListenableView(void){}
	virtual					~CListenableView(void){}

	virtual int				GetViewID()							= 0;	///< 뷰의 아이디
	virtual const char*		GetViewName()						= 0;	///< 뷰의 이름
	virtual void			Listen(CoreMessage& _message)		= 0;	///< 메시지의 수신
	
protected:
	friend class			CWorkSpaceManager;
	CControlMediator*		m_pControlMediator_;						///< 뷰에서 접근할 컨트롤 중계자
	HINSTANCE				m_hResourceHandle;							///< 뷰가 속한 모듈 리소스 핸들
};

/** @brief Listenable View Impl for ViewID */
template <typename T>
class CListenableViewImpl : public CListenableView
{
public:
	virtual int GetViewID()
	{
		return GetID();
	}

	static const int& GetID()
	{
		static int i = -1;
		return i;
	}
};


/** @brief Listenable View Impl general */
template <typename T>
class CListenableViewImpl_General : public CListenableViewImpl<T>
{
public:
	static const CORE_VIEW_TYPE	GetType()	{ return CVT_GENERAL; }
	enum { IDD = 0 };	// 의미없지만 컴파일을 위해 삽입
};

/** @brief Listenable View Impl Rollup */
template <typename T>
class CListenableViewImpl_Rollup : public CListenableViewImpl<T>
{
public:
	static const CORE_VIEW_TYPE	GetType()	{ return CVT_ROLLUP; }
};

/** @brief Listenable View Impl dialog */
template <typename T>
class CListenableViewImpl_Dialog : public CListenableViewImpl<T>
{
public:
	static const CORE_VIEW_TYPE	GetType()	{ return CVT_DIALOG; }
};


/** @brief Listenable View Impl fixed dialog */
template <typename T>
class CListenableViewImpl_Fixed : public CListenableViewImpl<T>
{
public:
	static const CORE_VIEW_TYPE	GetType()	{ return CVT_FIXED; }
};