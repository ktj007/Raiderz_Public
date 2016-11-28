#pragma once

#include "XMotion.h"


/// 모션 관리자
class XMotionMgr : public std::unordered_map<tstring, XMotion*>
{
private:
	void ParseMotion(MXmlElement* pElement, MXml* pXml);
	void ParseMotionType(MXmlElement* pElement, MXml* pXml, XMotion* pMotion, XMotionTypes nType);

	void ParseMotionSequence( MXmlElement* pChild, XMotion* pMotion, XMotionTypes nType );
	void InitCustomMotions();
	bool Load(const TCHAR* szFileName);
public:
	XMotionMgr();
	virtual ~XMotionMgr();
	bool Init(TCHAR* szMotionFIleName=NULL, TCHAR* szPCMotionFileName=NULL, TCHAR* szNPCMotionFileName=NULL);
	bool ReLoad();
	void Clear();
	XMotion* GetMotion(const TCHAR* szName);
	void AddMotion(XMotion* pMotion);
	void AddMotionForTest(const std::tstring& strXmlData);
	void Dump();		// for debug
	static bool IsTalentMotionName(const TCHAR* szName);
};

