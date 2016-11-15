#pragma once

#include <map>
#include <vector>
#include <string>

class CORETOOL_API CAttributeTypeExtraManager
{
public:
	inline static CAttributeTypeExtraManager& GetReference()
	{
		static CAttributeTypeExtraManager aRef;
		return aRef;
	}

	const std::vector< std::string >* GetStringTable( const char* _pSzTableName );

private:
	CAttributeTypeExtraManager();
	typedef std::map< std::string, std::vector< std::string > > STRING_TYPE_TABLE;

	STRING_TYPE_TABLE m_mapTypeExtraTable;
};

struct SElementAtXML;
struct CORETOOL_API SAttributeAtXML
{
	enum TYPE
	{
		E_UINT = 0,
		E_FLOAT,
		E_BOOL,
		E_STRING,
		E_POS,
		E_DIR,
		E_UP,
		E_RADIUS,
	};

	enum SPECIAL_SYMBOL
	{
		SYM_NONE = -1,
		SYM_EMPTY = -2
	};

	std::string m_strAttrName;
	std::string m_strAttrType;
	std::string m_strAttrTypeExtra;
	TYPE m_eAttrType;

	//Attribute Value
	std::vector<double> m_vGenericNum;
	std::string m_strGenericString;

	//parent Element
	SElementAtXML* m_pClientElement;

	HTREEITEM UpdateMFCTreeItem(CTreeCtrl& _refTreeCtrl, HTREEITEM _hParentToUpdate = NULL, SAttributeAtXML* _pMultiSelected = NULL);
	HTREEITEM m_hTreeItem;

	//Attribute Setter
	static double SetAttributeBool(const std::string& _pStrBool);
	static const char* GetAttributeBool(float _fBool);
};

struct CORETOOL_API SElementAtXML
{
public:
	SElementAtXML(void);

	std::string m_strNodeName;
	double* m_pdElementID;
	void DestroyElement();

	typedef std::vector<SAttributeAtXML*> AttributeList;
	AttributeList m_vecAttribute;

	typedef std::vector<SElementAtXML*> ElementList;
	ElementList m_vecChildElement;

	//parent Element
	SElementAtXML* m_pParentElement;
	//bool bCommon;	// 확장 오브젝트 인가 -
	std::string m_strSubInfo;

private:
	//DestroyElement를 통해서만 제거 가능
	~SElementAtXML(void);
};
