#pragma once

#include "CSItemData.h"

class XItemData: public CSItemData
{
public:
	XItemData();
	virtual ~XItemData();

	const TCHAR* GetName(void);		// 문자열테이블 참조
	const TCHAR* GetDesc(void);

private:	
	using CSItemData::m_strName;	// 직접 참조 못하도록 막습니다
	using CSItemData::m_strDesc;
};
