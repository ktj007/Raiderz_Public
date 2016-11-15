#include "stdafx.h"
#include "SFieldInfo.h"
#include "SDef.h"

std::wstring SFieldInfo::MakeFieldFileName( const wchar_t* szFieldPath )
{	
	wstring strFileName = wstring(szFieldPath) + this->m_strFieldFile + wstring(FILEEXT_FIELD);
	return strFileName;
}

void SFieldInfo::Assign( CSFieldFileInfo* pFileInfo )
{
	this->m_nFieldID = pFileInfo->nFieldID;
	this->m_strFieldName = pFileInfo->strName;
	this->m_strFieldFile = pFileInfo->strFile;
	this->m_bDynamic = pFileInfo->bDynamic;
	this->m_bAILODEnable = pFileInfo->bAILODEnable;
	this->m_nDefaultChannelCount = pFileInfo->nDefaultChannelCount;
	this->m_nCapacity = pFileInfo->nCapacity;
	this->m_bValidation = pFileInfo->bValidation;	
}
