#ifndef _GDBTASK_GETCHARLOOKLIST_H
#define _GDBTASK_GETCHARLOOKLIST_H

#include "GDBAsyncTask.h"
#include "SCommonBodyCharGetLookList.h"

class GDBTaskCharGetLookList : public GDBAsyncTask, public MTestMemPool<GDBTaskCharGetLookList>
{
public :
	GDBTaskCharGetLookList(const MUID& uidReqPlayer);
	~GDBTaskCharGetLookList();

	enum
	{
		CHAR_SIMPLE_INFO = 0,
		CHAR_EQUIPMENT_INFO
	};

	void								OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT				_OnCompleted() override;


	SCommonBodyCharGetLookList&			GetData();

protected :
	bool								GetSimpleInfo(mdb::MDatabase& rfDB);
	bool								GetEquipInfo(mdb::MDatabase& rfDB);
	
private :
	class Completer
	{
	public :
		Completer(SCommonBodyCharGetLookList& commonBody);
		void Do();

	private :
		SCommonBodyCharGetLookList&	m_commonBody;
	};
	

protected :
	SCommonBodyCharGetLookList		m_commonBody;
};

#endif
