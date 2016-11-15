#pragma once
#include "V_PropertyGridCtrl.h"

typedef std::map< std::string, std::string> SOUNDFILELIST;

class V_CCoreToolPropertyGridCtrl : public CCoreToolPropertyGridCtrl
{
public:
	V_CCoreToolPropertyGridCtrl(void);
	~V_CCoreToolPropertyGridCtrl(void);

	static CMFCPropertyGridProperty* AddPropertyBGMItem(const SOUNDFILELIST &mapBgmList , CMFCPropertyGridProperty* parent, const char* name, _variant_t& value, bool bEditable=true);
};
