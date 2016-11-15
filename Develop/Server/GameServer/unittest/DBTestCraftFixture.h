#ifndef _DBTEST_CRAFT_FIXTURE_H
#define _DBTEST_CRAFT_FIXTURE_H


#include "DBTestHelper_Craft.h"


class FIXTURE_CRAFT
{
public :
	void CHECK_CHARACTER(DBT_CRAFT_CHAR& cc);
	void CHECK_ITEM_RECIPE(DBT_RECP_ITEM_VEC& vRecpItem);
};


#endif