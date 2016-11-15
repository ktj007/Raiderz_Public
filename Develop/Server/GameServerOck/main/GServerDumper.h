#pragma once

#include "MDebug.h"
#include <string>

class GPlayerObject;
class GEntityPlayer;
class GSharedField;
class GDynamicFieldMaster;
namespace minet {class MLink;}

/// 서버 덤프 남기는 클래스
class GServerDumper : public MTestMemPool<GServerDumper>
{
private:
	MLogger m_Logger;
	void Log( MLogger* pLogger, const wchar_t* pFormat, ...);
	std::wstring MakeFileName(std::wstring strID);
	
	void Dump_Links(MLogger* pLogger);
	void Dump_Link(MLogger* pLogger, minet::MLink* pLink);

	void Dump_Players(MLogger* pLogger);
	void Dump_Player( MLogger* pLogger, GPlayerObject* pPlayerObject, GEntityPlayer* pEntityPlayer );

	void Dump_Fields(MLogger* pLogger);
	void Dump_SharedField( MLogger* pLogger, GSharedField* pField );
	void Dump_DynamicField( MLogger* pLogger, GDynamicFieldMaster* pDynamicFieldMaster );
public:
	GServerDumper();
	~GServerDumper();
	void Dump(std::wstring strID, MLogger* pLogger = NULL);
};