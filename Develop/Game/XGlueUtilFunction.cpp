#include "stdafx.h"
#include "XGlueUtilFunction.h"
#include "XPost_Local.h"
#include "MCsvParser.h"

void XGlueUtilFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueUtilFunction>(pLua->GetState(), "_shUtil")
		.def("ParamCount",	&XGlueUtilFunction::ParamCount)
		.def("Param",		&XGlueUtilFunction::Param)
		;
}

int XGlueUtilFunction::ParamCount( const char* szParams )
{
	if (szParams == NULL) return 0;

	MCsvParser parser;
	parser.LoadFromStream(szParams);
	return parser.GetColCount(0);
}

const char* XGlueUtilFunction::Param( const char* szParams, int nIndex )
{
	if (szParams == NULL) return "";

	MCsvParser parser;
	parser.LoadFromStream(szParams);

	if (parser.GetData(nIndex, 0, m_szBuffer, 1024) > 0) return m_szBuffer;
	return "";
}