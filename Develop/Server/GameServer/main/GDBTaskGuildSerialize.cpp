#include "stdafx.h"
#include "GDBTaskGuildSerialize.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"


GDBTaskGuildSerialize::GDBTaskGuildSerialize(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GULDSERIALISE, 2)
{
	m_Data.uidPlayer = uidReqPlayer;
}

GDBTaskGuildSerialize::~GDBTaskGuildSerialize()
{

}

void GDBTaskGuildSerialize::Input( const int64 nGID )
{
	m_Data.nGID	= nGID;
}

bool GDBTaskGuildSerialize::GetGuildInfo( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!Execute(rs, GetSQLW(GUILD_INFO)))
		return false;

	if (0 == rs.GetFetchedRowCount())
		return true;

	static const size_t nNameHash = rs.MakeHashValueW(L"NAME");

	m_Data.strGuildName = rs.FieldHash(nNameHash).AsWString();
	m_Data.nStorageMoney = rs.FieldW(L"STORAGE_MONEY").AsInt();

	return true;
}

bool GDBTaskGuildSerialize::GetGuildMemberList( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(GUILD_MEMBERLIST)))
		return false;

	if (0 == rs.GetFetchedRowCount())
		return true;

	GDBTASK_GUILD_MEMBER gm;
	for (; !rs.IsEOF(); rs.MoveNext())
	{
		gm.nAID			= rs.FieldW(L"ACCN_ID").AsInt64();
		gm.nCID			= rs.FieldW(L"CHAR_ID").AsInt64();
		gm.strCharName	= rs.FieldW(L"NAME").AsWString();
		gm.nLevel		= rs.FieldW(L"LEV").AsByte();
		gm.nGuildGrade	= rs.FieldW(L"GRADE").AsByte();

		m_Data.vecGuildMember.push_back(gm);
	}

	return true;
}

bool GDBTaskGuildSerialize::GetGuildStorageItemList(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(GUILD_STORAGEITEMLIST)))
	{
		return false;
	}

	return m_Data.qItemInst.GetFromDB(rs);
}

void GDBTaskGuildSerialize::OnExecute( mdb::MDatabase& rfDB )
{
	if (!GetGuildInfo(rfDB))
		return;

	if (!GetGuildMemberList(rfDB))
		return;

	if (!GetGuildStorageItemList(rfDB))
		return;
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildSerialize::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

GDBTaskGuildSerialize::Completer::Completer( GDBTaskGuildSerializeData& Data ) : m_Data(Data)
{
}

void GDBTaskGuildSerialize::Completer::Do()
{
gsys.pGuildSystem->ForDBTask().GuildSerialize(m_Data);	
}