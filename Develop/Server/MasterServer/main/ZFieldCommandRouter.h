#pragma once

#include "MTypes.h"
#include "MCommand.h"
#include "STransData_M2G.h"

class ZSharedField;
class ZFieldManager;
class ZField;
class ZDynamicFieldMaster;

/// 필드 관련 커맨드 전송자
class ZFieldCommandRouter
{
private:
	static ZFieldManager* GetFieldManager();
	static void AssignSharedFieldInfoTransData(ZSharedField* pSrcField, TDMG_SHARED_FIELD_INFO* pTar);
	static void AssignDynamicFieldGroupInfoTransData(ZDynamicFieldMaster* pSrcFieldGroup, TDMG_DYNAMIC_FIELD_GROUP_INFO* pTar);
	static void AssignDynamicFieldInfosTrasData(ZDynamicFieldMaster* pSrcFieldGroup, std::vector<TDMG_DYNAMIC_FIELD_NODE >* pTar);

	static minet::MCommand* MakeCommand_AllSharedFieldInfo();
	static minet::MCommand* MakeCommand_AddSharedField(ZSharedField* pField);
	static minet::MCommand* MakeCommand_DelSharedField(MUID uidField);
	static minet::MCommand* MakeCommand_AllFieldGroupInfo();
	static minet::MCommand* MakeCommand_UpdateProxyFieldInfo(ZField* pField);
	static minet::MCommand* MakeCommand_AddDynamicFieldGroup(ZDynamicFieldMaster* pDynamicFieldMaster);
	static minet::MCommand* MakeCommand_DelDynamicFieldGroup(MUID uidDynamicFieldMaster);
public:
	static void RouteAddSharedField(ZSharedField* pField);
	static void RouteDelSharedField(MUID uidField);

	static void RouteAddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster);
	static void RouteDelDynamicFieldMaster(MUID uidDynamicFieldMaster);

	static void SendAllFieldInfo(MUID uidTarget);
	static void RouteAllFieldInfo();

	static void SendAllFieldGroupInfo(MUID uidTarget);
	static void RouteUpdateFieldInfo(ZField* pField);
};