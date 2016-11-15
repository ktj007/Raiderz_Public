#include "stdafx.h"
#include "XReplay.h"
#include "XNonPlayer.h"
#include "MCommandParameter.h"
#include "XMyPlayer.h"
#include "XSystem.h"
#include "XMagicArea.h"
#include "XPath.h"
#include "XNetwork.h"


// XObserverCommandList //////////////////////////
//////////////////////////////////////////////////

void XObserverCommandList::Destroy()
{
	while(!empty())
	{
		XObserverCommandItem *pItem=*begin();
		SAFE_DELETE(pItem->pCommand);
		SAFE_DELETE(pItem);
		erase(begin());
	}
	clear();
}

void XObserverCommandList::Assign( XObserverCommandList& t ) /* 대상으로부터 복사 */
{
	Destroy();

	for (iterator itor = t.begin(); itor != t.end(); ++itor)
	{
		XObserverCommandItem* pSrcItem= (*itor);

		XObserverCommandItem* pNewItem = new XObserverCommandItem;
		pNewItem->fTime = pSrcItem->fTime;
		pNewItem->pCommand = pSrcItem->pCommand->Clone();

		push_back(pNewItem);
	}
}

// XReplayFile ///////////////////////////////////
//////////////////////////////////////////////////
bool XReplayFile::Open( const wchar_t* szFileName, bool bWrite/*=false*/ )
{
	m_bWrite = bWrite;
	if (bWrite)
	{
		m_pFile = fopen(MLocale::ConvUTF16ToAnsi(szFileName).c_str(), "wb");
	}
	else
	{
		m_pFile = fopen(MLocale::ConvUTF16ToAnsi(szFileName).c_str(), "rb");
	}
	if (!m_pFile) return false;

	return true;
}

void XReplayFile::Close()
{
	if(m_pFile) 
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

int XReplayFile::Read( void* pBuffer,size_t nByte )
{
	if(!m_pFile || m_bWrite) return 0;

	return fread(pBuffer, nByte, 1, m_pFile);
}

int XReplayFile::Write( void* pBuffer,size_t nByte )
{
	if(!m_pFile || !m_bWrite) return 0;

	return fwrite(pBuffer, nByte, 1, m_pFile);
}

XReplayFile::XReplayFile() : m_pFile(NULL), m_bWrite(false)
{

}

XReplayFile::~XReplayFile()
{
	Close();
}

// XReplaySaver //////////////////////////////////
//////////////////////////////////////////////////
XReplaySaver::XReplaySaver( XRecorder* pRecorder ) : m_pRecorder(pRecorder)
{

}

wstring XReplaySaver::MakeAutoFileName()
{
	int nsscount=0;

	if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_REPLAY) == false)
	{
		return L"";
	}

	wchar_t replayfilename[_MAX_PATH] = L"";
	wstring strReplayFolderPath = XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_REPLAY);

	do {
		swprintf_s(replayfilename, L"%s%s%03d" FILEEXT_REPLAY , strReplayFolderPath.c_str() , GAME_NAME_STR, nsscount);
		nsscount++;
	}
	while( IsExist(replayfilename) && nsscount<1000);

	if(nsscount>=1000) return L"";

	return replayfilename;
}

bool XReplaySaver::Save( const wchar_t* szFileName )
{
	wstring strFileName;
	if (szFileName[0] == 0) strFileName = MakeAutoFileName();
	else strFileName = szFileName;

	if (m_File.Open(strFileName.c_str(), true) == false) return false;

	if (!SaveHeader()) 
	{
		m_File.Close();
		return false;
	}

	if (!SaveGameInfo()) 
	{
		m_File.Close();
		return false;
	}

	if (!SaveCommandStream()) 
	{
		m_File.Close();
		return false;
	}

	m_File.Close();
	return true;
}

bool XReplaySaver::SaveHeader()
{
	int nWritten;

	DWORD header;
	header=REC_FILE_ID;
	nWritten = m_File.Write(&header, sizeof(header));
	if(nWritten==0) return false;

	DWORD version=REC_FILE_VERSION;
	nWritten = m_File.Write(&version, sizeof(version));
	if(nWritten==0) return false;

	DWORD nCommandVersion = SH_COMMAND_VERSION;
	nWritten = m_File.Write(&nCommandVersion, sizeof(nCommandVersion));
	if(nWritten==0) return false;

	return true;
}

bool XReplaySaver::SaveGameInfo()
{
	int nWritten;

	// 시작 시간
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.fStartTime, sizeof(m_pRecorder->m_GameInfo.fStartTime));
	if(nWritten==0) return false;

	// 종료 시간
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.fFinishTime, sizeof(m_pRecorder->m_GameInfo.fFinishTime));
	if(nWritten==0) return false;

	// field id
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.nFieldID, sizeof(m_pRecorder->m_GameInfo.nFieldID));
	if(nWritten==0) return false;

	// GameTime
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.nGameTime, sizeof(m_pRecorder->m_GameInfo.nGameTime));
	if(nWritten==0) return false;

	// Weather
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.nWeather, sizeof(m_pRecorder->m_GameInfo.nWeather));
	if(nWritten==0) return false;

	// My UID
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.MyUID, sizeof(m_pRecorder->m_GameInfo.MyUID));
	if(nWritten==0) return false;

	// My UIID
	nWritten = m_File.Write(&m_pRecorder->m_GameInfo.nUIID, sizeof(m_pRecorder->m_GameInfo.nUIID));
	if(nWritten==0) return false;

	return true;
}

bool XReplaySaver::SaveCommandStream()
{
	bool bError = false;

	// CommandList
	XObserverCommandList::iterator itorCommand = m_pRecorder->m_GameInfo.ReplayCommandList.begin();
	for(size_t i=0;i<m_pRecorder->m_GameInfo.ReplayCommandList.size();i++)
	{
		XObserverCommandItem *pItem = *itorCommand;
		MCommand *pCommand = pItem->pCommand;

		const size_t BUF_SIZE = 8192;
		char CommandBuffer[BUF_SIZE];

		int nCmdSize = pCommand->GetSize();
		if (nCmdSize >= BUF_SIZE) continue;

		int nSize = pCommand->GetData(CommandBuffer, nCmdSize);

		int nWritten;
		nWritten = m_File.Write(&pItem->fTime, sizeof(pItem->fTime));
		if(nWritten==0) { bError=true; break; }
		nWritten = m_File.Write(&pCommand->m_Sender, sizeof(pCommand->m_Sender));
		if(nWritten==0) { bError=true; break; }

		nWritten = m_File.Write(&nSize, sizeof(nSize));
		if(nWritten==0) { bError=true; break; }

		nWritten = m_File.Write(CommandBuffer, nSize);
		if(nWritten==0) { bError=true; break; }

		++itorCommand;
	}

	if (bError)
	{
		_ASSERT(0);
		return false;
	}

	return true;
}



// XReplayLoader /////////////////////////////////
//////////////////////////////////////////////////
XReplayLoader::XReplayLoader( XReplay* pReplay ) : m_pReplay(pReplay)
{

}

bool XReplayLoader::Load( const wchar_t* szFileName )
{
	if (m_File.Open(szFileName, false) == false) return false;

	if (!LoadHeader()) return false;
	if (!LoadGameInfo()) return false;
	if (!LoadCommandStream()) return false;

	m_File.Close();
	return true;
}

bool XReplayLoader::LoadHeader()
{
	DWORD version = 0;
	DWORD header=0;
	DWORD nCommandVersion = 0;

	int nRead;

	nRead = m_File.Read(&header, sizeof(header));
	if(nRead==0 || header!=REC_FILE_ID) return false;

	nRead = m_File.Read(&version, sizeof(version));
	if ( ( nRead == 0) || ( version > REC_FILE_VERSION)) return false;

	m_pReplay->m_nVersion = version;

	nRead = m_File.Read(&nCommandVersion, sizeof(nCommandVersion));
	if ( ( nRead == 0) || ( nCommandVersion > SH_COMMAND_VERSION)) return false;

	return true;
}

bool XReplayLoader::LoadGameInfo()
{
	int nRead;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.fStartTime, sizeof(m_pReplay->m_GameInfo.fStartTime));
	if (nRead == 0) return false;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.fFinishTime, sizeof(m_pReplay->m_GameInfo.fFinishTime));
	if (nRead == 0) return false;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.nFieldID, sizeof(m_pReplay->m_GameInfo.nFieldID));
	if (nRead == 0) return false;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.nGameTime, sizeof(m_pReplay->m_GameInfo.nGameTime));
	if (nRead == 0) return false;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.nWeather, sizeof(m_pReplay->m_GameInfo.nWeather));
	if (nRead == 0) return false;

	// My UID
	nRead = m_File.Read(&m_pReplay->m_GameInfo.MyUID, sizeof(m_pReplay->m_GameInfo.MyUID));
	if (nRead == 0) return false;

	nRead = m_File.Read(&m_pReplay->m_GameInfo.nUIID, sizeof(m_pReplay->m_GameInfo.nUIID));
	if (nRead == 0) return false;

	return true;
}


bool XReplayLoader::LoadCommandStream()
{
	int nCommandCount=0;

	int nSize;
	float fTime;
	while( m_File.Read(&fTime, sizeof(fTime)) )
	{
		nCommandCount++;

		char CommandBuffer[8192];
		memset(CommandBuffer, 0, sizeof(CommandBuffer));

		MUID uidSender;
		m_File.Read(&uidSender, sizeof(uidSender));
		m_File.Read(&nSize, sizeof(nSize));

		if(nSize<0 || nSize>sizeof(CommandBuffer)) {
			return false;
		}

		m_File.Read(CommandBuffer, nSize);

		XObserverCommandItem* pCommandItem = new XObserverCommandItem;
		pCommandItem->pCommand= CreateCommandFromStream(CommandBuffer);
		pCommandItem->pCommand->m_Sender=uidSender;
		pCommandItem->fTime=fTime;

		m_pReplay->m_GameInfo.ReplayCommandList.push_back(pCommandItem);
	}

	return true;
}

MCommand* XReplayLoader::CreateCommandFromStream( char* pStream )
{
	MCommand* pCommand = new MCommand();
	pCommand->SetData(pStream);
	return pCommand;
}


// XRecorder /////////////////////////////////////
//////////////////////////////////////////////////


XRecorder::XRecorder() : m_bRecording(false)
{
	m_pSaver = new XReplaySaver(this);
	m_szFileName[0] = 0;
}

XRecorder::~XRecorder()
{
	if (m_bRecording) Stop();
	SAFE_DELETE(m_pSaver);
}

void XRecorder::Clear()
{
	m_szFileName[0] = 0;

	m_GameInfo.Clear();
}

bool XRecorder::Start( const wchar_t* filename/*=NULL*/ )
{
	Clear();

	if (filename) wcscpy_s(m_szFileName, filename);
	m_bRecording=true;

	SetupGameInfo();
	SetupMyInfo();
	SetupObjectsInfo();
	SetupMyPalette();

	RecordMyInfoCommand();
	RecordEntityInfoCommand();

	return true;
}

void XRecorder::Stop()
{
	if(!m_bRecording) return;
	m_bRecording=false;

	if (global.system) m_GameInfo.fFinishTime = (float)global.system->GetTimer().GetTime();

	m_pSaver->Save(m_szFileName);
	Clear();
}

void XRecorder::Record( MCommand* pCommand )
{
	if (!m_bRecording) return;

	if (PostFilterCommand(pCommand) == true) return;

	InsertCommand(pCommand);
}

void XRecorder::RecordEntityInfoCommand()
{
	void* pPlayerNodePtr=NULL;
	void* pMonsterNodePtr=NULL;
	void* pMagicArea = NULL;
	int nPlayerCount = (int)m_PlayerInfos.size();
	int nMonsterCount = (int)m_NPCInfos.size();
	int nMagicAreaCount = (int)m_MagicAreaInfo.size();
	if (nPlayerCount > 0) pPlayerNodePtr = &m_PlayerInfos[0];
	if (nMonsterCount > 0) pMonsterNodePtr = &m_NPCInfos[0];
	if (nMagicAreaCount > 0) pMagicArea = &m_MagicAreaInfo[0];

	MCommand* pC = XNewCmd(MC_FIELD_SECTOR_ENTITY_INFO); 
	pC->AP(MCommandParameterBlob(pPlayerNodePtr, sizeof(TD_UPDATE_CACHE_PLAYER), nPlayerCount));
	pC->AP(MCommandParameterBlob(pMonsterNodePtr, sizeof(TD_UPDATE_CACHE_NPC), nMonsterCount));
	pC->AP(MCommandParameterBlob(pMagicArea, sizeof(TD_UPDATE_CACHE_BUFFENTITY), nMagicAreaCount));
	pC->AP(MCommandParameterBlob(pMagicArea, sizeof(TD_TRIGGER_SENSOR_INFO), 0));

	Record(pC);

	SAFE_DELETE(pC);
}

void XRecorder::RecordMyInfoCommand()
{
	m_MyTalent.clear();

	vector<int> vecTalentID;
	gvar.MyInfo.Talent.GetAllTalentID(vecTalentID);

	for (size_t i = 0; i < vecTalentID.size(); i++)
	{
		int nTalentID = vecTalentID[i];
		m_MyTalent.push_back(nTalentID);
	}

	MCommand* pC = XNewCmd(MC_CHAR_MYINFO); 
	pC->AP(MCommandParameterSingleBlob(m_MyInfo));
	pC->AP(MCommandParameterBlob(m_MyInventory));
	pC->AP(MCommandParameterBlob(m_MyEquipment));
	pC->AP(MCommandParameterBlob(m_MyTDReservedDummySlotInfo));
	pC->AP(MCommandParameterBlob(m_MyTalent));
	pC->AP(MCommandParameterBlob(m_MyQuest));
	pC->AP(MCommandParameterBlob(m_MyPalette));
	pC->AP(MCommandParameterBlob(m_MyFaction));
	pC->AP(MCommandParameterBlob(m_MyRecipeID));

	Record(pC);

	SAFE_DELETE(pC);
}

void XRecorder::InsertCommand( MCommand* pCommand )
{
	XObserverCommandItem* pItem = new XObserverCommandItem;
	pItem->fTime = (global.system) ? global.system->GetTimer().GetTime() : 0.0f;
	pItem->pCommand = pCommand->Clone();
	m_GameInfo.ReplayCommandList.push_back(pItem);
}

bool XRecorder::PostFilterCommand( MCommand* pCommand )
{
	// 만약 재가공해야 할 커맨드가 있을 경우 여기서 처리한다.
	// 예를 들어 내 이동 커맨드 같은 경우 보내기만 하고 받아서 처리하는 부분이 없기 때문에
	// 보내는 커맨드를 가지고 받는 커맨드를 직접 여기서 만들어준다.

	switch (pCommand->GetID())
	{
	case MC_ACTION_MOVE_REQ:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			TD_PC_MOVE* pMI = (TD_PC_MOVE*)pParam->GetPointer();

			MCommand* pC = XNewCmd(MC_ACTION_MOVE); 
			pC->AP(MCommandParameterUShort(XGetMyUIID())); 
			pC->AP(MCommandParameterSingleBlob(pMI,sizeof(TD_PC_MOVE)));

			InsertCommand(pC);

			SAFE_DELETE(pC);
		}
		return true;
	case MC_ACTION_STOP_REQ:
		{
			vec3 vPos;
			svec2 vDir;
			if (pCommand->GetParameter(&vPos, 0, MPT_VEC)==false) return false;
			if (pCommand->GetParameter(&vDir, 1, MPT_SVEC2)==false) return false;

			MCommand* pC = XNewCmd(MC_ACTION_STOP); 
			pC->AP(MCommandParameterUShort(XGetMyUIID())); 
			pC->AP(MCommandParameterVector(vPos));
			pC->AP(MCommandParameterShortVector2(vDir));

			InsertCommand(pC);

			SAFE_DELETE(pC);
		}
		return true;
	case MC_ACTION_STOP_MOVE_REQ:
		{
			vec3 vPos, vDir;
			if (pCommand->GetParameter(&vPos, 0, MPT_VEC)==false) return false;

			MCommand* pC = XNewCmd(MC_ACTION_STOP_MOVE); 
			pC->AP(MCommandParameterUShort(XGetMyUIID())); 
			pC->AP(MCommandParameterVector(vPos));

			InsertCommand(pC);

			SAFE_DELETE(pC);
		}
		return true;
	case MC_ACTION_GUARD_REQ:
		{
			vec3 vDir = vec3(0,1,0);
			vec3 vPos = vec3::ZERO;

			if (gvar.Game.pMyPlayer)
			{
				vDir = gvar.Game.pMyPlayer->GetDirection();
				vPos = gvar.Game.pMyPlayer->GetPosition();
			}
			MCommand* pC = XNewCmd(MC_ACTION_GUARD); 
			pC->AP(MCommandParameterUShort(XGetMyUIID())); 
			pC->AP(MCommandParameterShortVector2(vDir));
			pC->AP(MCommandParameterVector(vPos));

			InsertCommand(pC);

			SAFE_DELETE(pC);
		}
		return true;
	case MC_ACTION_GUARD_RELEASED_REQ:
		{
			MCommand* pC = XNewCmd(MC_ACTION_GUARD_RELEASED); 
			pC->AP(MCommandParameterUShort(XGetMyUIID())); 

			InsertCommand(pC);
			SAFE_DELETE(pC);
		}
		return true;
	case MC_ACTION_JUMP_REQ:
		{
			vec3 vPos, vDir;
			if (pCommand->GetParameter(&vPos, 0, MPT_VEC)==false) return false;
			if (pCommand->GetParameter(&vDir, 1, MPT_VEC)==false) return false;

			MCommand* pC = XNewCmd(MC_ACTION_JUMP); 

			pC->AP(MCommandParameterUShort(XGetMyUIID()));
			pC->AP(MCommandParameterVector(vPos));
			pC->AP(MCommandParameterVector(vDir));

			InsertCommand(pC);
			SAFE_DELETE(pC);

		}
		return true;
	case MC_ACTION_FALLING_REQ:
		{
			vec3 vPos, vDir;
			svec3 svDir;

			if (pCommand->GetParameter(&vPos, 0, MPT_VEC)==false) return false;
			if (pCommand->GetParameter(&svDir, 1, MPT_SVEC)==false) return false;

			MCommand* pC = XNewCmd(MC_ACTION_FALLING); 

			pC->AP(MCommandParameterUShort(XGetMyUIID()));
			pC->AP(MCommandParameterVector(vPos));
			pC->AP(MCommandParameterShortVector(svDir));

			InsertCommand(pC);
			SAFE_DELETE(pC);

		}
		return true;
	case MC_ACTION_CHANGE_STANCE_REQ:
		{
			wchar_t nValue = 0;

			if (pCommand->GetParameter(&nValue,		0, MPT_CHAR)==false) return false;

			MCommand* pC = XNewCmd(MC_ACTION_CHANGE_STANCE);

			pC->AP(MCommandParameterUShort(XGetMyUIID()));
			pC->AP(MCommandParameterChar(nValue));

			InsertCommand(pC);
			SAFE_DELETE(pC);
		}
		return true;
	case MC_CHAR_REBIRTH_MYPLAYER:
		{
			MCommand* pC = XNewCmd(MC_CHAR_REBIRTH_NETPLAYER);

			pC->AP(MCommandParameterUID(XGetMyUID()));

			InsertCommand(pC);
			SAFE_DELETE(pC);

		}
		return true;
	}

	return false;
}

void XRecorder::SetupObjectsInfo()
{
	if (gg.omgr)
	{
		for (XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
		{
			XObject* pObject = (*itor).second;
			if (MIsExactlyClass(XNetPlayer, pObject))
			{
				XNetPlayer* pNetPlayer = AsNetPlayer(pObject);
				TD_UPDATE_CACHE_PLAYER	node;
				node.uid = pNetPlayer->GetUID();
				node.nUIID = pNetPlayer->GetUIID();
				node.vPos = pNetPlayer->GetPosition();
				node.svDir = pNetPlayer->GetDirection();
				node.nStance = pNetPlayer->GetStance();
				node.nStatusFlag = 0;
				if (pNetPlayer->IsDead()) SetBitSet(node.nStatusFlag, UPS_DEAD);
				if (pNetPlayer->IsSwim()) SetBitSet(node.nStatusFlag, UPS_SWIMMING);
				wcsncpy_s(node.szName, pNetPlayer->GetName(), _TRUNCATE);
				node.Feature = pNetPlayer->GetFeature().AsTransData();

				m_PlayerInfos.push_back(node);
			}
			else if (MIsExactlyClass(XMyPlayer, pObject))
			{
				XMyPlayer* pMyPlayer = pObject->AsMyPlayer();
				TD_UPDATE_CACHE_PLAYER	node;				
				node.uid = pMyPlayer->GetUID();
				node.nUIID = pMyPlayer->GetUIID();
				node.vPos = pMyPlayer->GetPosition();
				node.svDir = pMyPlayer->GetDirection();
				node.nStance = pMyPlayer->GetStance();
				node.nStatusFlag = 0;
				if (pMyPlayer->IsDead()) SetBitSet(node.nStatusFlag, UPS_DEAD);
				if (pMyPlayer->IsSwim()) SetBitSet(node.nStatusFlag, UPS_SWIMMING);
				wcsncpy_s(node.szName, gvar.MyInfo.ChrInfo.szName, _TRUNCATE);
				node.Feature.nSex = gvar.MyInfo.ChrInfo.nSex;
				node.Feature.nHair = gvar.MyInfo.ChrInfo.nFeatureHair;
				node.Feature.nFace = gvar.MyInfo.ChrInfo.nFeatureFace;
				node.Feature.nHairColor = gvar.MyInfo.ChrInfo.nFeatureHairColor;
				node.Feature.nSkinColor = gvar.MyInfo.ChrInfo.nFeatureSkinColor;
				node.Feature.nMakeUp = gvar.MyInfo.ChrInfo.nMakeUp;
				node.Feature.nWeaponSet = gvar.MyInfo.EquipmentSlot.GetWeaponSet();

				TD_PLAYER_FEATURE_TATTOO	featureTattooNode;
				featureTattooNode.nTattooType = gvar.MyInfo.ChrInfo.nTattooType;
				featureTattooNode.nTattooPosX = gvar.MyInfo.ChrInfo.nTattooPosX;
				featureTattooNode.nTattooPosY = gvar.MyInfo.ChrInfo.nTattooPosY;
				featureTattooNode.nTattooScale = gvar.MyInfo.ChrInfo.nTattooScale;				

				for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
				{
					node.Feature.nItemID_DyedColor[i] = 0;
				}
				node.Feature.nItemID[FEATURE_ITEMSLOT_FACE] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_FACE);
				node.Feature.nItemID[FEATURE_ITEMSLOT_HEAD] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_HEAD);
				node.Feature.nItemID[FEATURE_ITEMSLOT_HANDS] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_HANDS);
				node.Feature.nItemID[FEATURE_ITEMSLOT_FEET] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_FEET);
				node.Feature.nItemID[FEATURE_ITEMSLOT_BODY] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_BODY);
				node.Feature.nItemID[FEATURE_ITEMSLOT_LEG] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LEG);

				if (gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_HEAD))
					node.Feature.nItemID[FEATURE_ITEMSLOT_HEAD] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_HEAD);
				if (gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_HANDS))
					node.Feature.nItemID[FEATURE_ITEMSLOT_HANDS] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_HANDS);
				if (gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_FEET))
					node.Feature.nItemID[FEATURE_ITEMSLOT_FEET] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_FEET);
				if (gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_BODY))
					node.Feature.nItemID[FEATURE_ITEMSLOT_BODY] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_BODY);
				if (gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_LEG))
					node.Feature.nItemID[FEATURE_ITEMSLOT_LEG] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LOOK_LEG);

				node.Feature.nItemID[FEATURE_ITEMSLOT_LWEAPON] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LWEAPON);
				node.Feature.nItemID[FEATURE_ITEMSLOT_LWEAPON2] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_LWEAPON2);
				node.Feature.nItemID[FEATURE_ITEMSLOT_RWEAPON] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_RWEAPON);
				node.Feature.nItemID[FEATURE_ITEMSLOT_RWEAPON2] = gvar.MyInfo.EquipmentSlot.GetItemID(ITEMSLOT_RWEAPON2);

				m_PlayerInfos.push_back(node);
			}
			else if (MIsExactlyClass(XNonPlayer, pObject))
			{
				XNonPlayer* pNPC = AsNPC(pObject);
				TD_UPDATE_CACHE_NPC	node;
				node.uid = pNPC->GetUID();
				node.nUIID = pNPC->GetUIID();
				node.nNPCID = pNPC->GetInfo()->nID;
				node.vPos = pNPC->GetPosition();
				node.svDir = pNPC->GetDirection();
				node.nLevel = pNPC->GetLevel();

				node.vTarPos = pNPC->GetPosition();
				node.nSpeed = (uint16)pNPC->GetInfo(true)->fSpeed;
				node.nStatusFlag = 0;
				if (pNPC->IsDead()) SetBitSet(node.nStatusFlag, UPS_DEAD);
				
				m_NPCInfos.push_back(node);
			}
			//else if (MIsExactlyClass(XMagicArea, pObject))
			//{
			//	XMagicArea* pMagicArea = static_cast<XMagicArea*>(pObject);
			//	TD_UPDATE_CACHE_BUFFENTITY node;
			//	node.nTalentID = pMagicArea->GetTalentID() ;
			//	node.dwFinishTime = pMagicArea->GetFinishTime();
			//	node.vPos = pMagicArea->GetPosition();

			//	m_MagicAreaInfo.push_back(node);
			//}
			else
			{
				_ASSERT(NULL);
			}
		}
	}
}

void XRecorder::SetupMyInfo()
{
	m_GameInfo.MyUID = gvar.MyInfo.MyUID;
	m_GameInfo.nUIID = XGetMyUIID();
	m_MyInfo = gvar.MyInfo.AsTransDataMyInfo();
}

void XRecorder::SetupGameInfo()
{
	m_GameInfo.fStartTime = (global.system) ? (float)global.system->GetTimer().GetTime() : 0.0f;
	m_GameInfo.nFieldID = gvar.World.GetFieldID();
	m_GameInfo.nGameTime = gvar.World.nGameTime;
	m_GameInfo.nWeather = gvar.World.nWeather;
}

void XRecorder::SetupMyPalette()
{
	TD_PALETTE tdPalette;
	m_MyPalette.push_back(tdPalette);
	//int nIndexCount = 0;
	//deque<XPaletteItem>::iterator it = gvar.MyInfo.PaletteList.m_vContains.begin();
	//while(it != gvar.MyInfo.PaletteList.m_vContains.end())
	//{
	//	if((*it).itemID != 0)
	//	{
	//		m_MyPalette.paletteItems[nIndexCount].nItemIDorTalentID = (*it).itemID;
	//		m_MyPalette.paletteItems[nIndexCount].nType = PIT_ITEM;
	//	}
	//	else if((*it).talentID != 0)
	//	{
	//		m_MyPalette.paletteItems[nIndexCount].nItemIDorTalentID = (*it).talentID;
	//		m_MyPalette.paletteItems[nIndexCount].nType = PIT_TALENT;
	//	}

	//	it++;
	//	nIndexCount++;
	//}
}
// XReplay ///////////////////////////////////////
//////////////////////////////////////////////////
void XReplay::Play()
{
	m_ReplayCommandList.Assign(m_GameInfo.ReplayCommandList);
	m_fElapsedTime = 0;
	m_bPause = false;
	m_bDone = false;
	m_Speed = 1;

	if (m_ReplayCommandList.size())
	{
		XObserverCommandItem* pItem= *(m_ReplayCommandList.begin());
		m_ReplayCommandList.erase(m_ReplayCommandList.begin());

		XNetClient* pNetClient = global.net->GetNetClient();
		if (pNetClient)
		{
			pNetClient->OnCommand(pItem->pCommand);
		}

		delete pItem->pCommand;
		delete pItem;
	}
}


void XReplay::Update( float fDelta )
{
	if (m_bPause) return;

	// 이미 Delta가 배속조절이 된 상태
	m_fElapsedTime += fDelta;

	if(m_ReplayCommandList.size()==0) 
	{
		m_fElapsedTime = GetMaxTime();
		m_bDone = true;
		return;
	}

	while(m_ReplayCommandList.size())
	{
		XObserverCommandItem* pItem= *(m_ReplayCommandList.begin());

		if (m_GameInfo.fStartTime+m_fElapsedTime < pItem->fTime) 
		{
			return;
		}

		m_ReplayCommandList.erase(m_ReplayCommandList.begin());

		XNetClient* pNetClient = global.net->GetNetClient();
		if (pNetClient)
		{
			pNetClient->OnCommand(pItem->pCommand);
		}

		delete pItem->pCommand;
		delete pItem;
	}
}

XReplay::XReplay() : m_bDone(false), m_fElapsedTime(0), m_nVersion(-1), m_bPause(false), m_Speed(1)
{
	m_pLoader = new XReplayLoader(this);
}

XReplay::~XReplay()
{
	SAFE_DELETE(m_pLoader);
}

bool XReplay::Load( const wchar_t* szFileName )
{
	return m_pLoader->Load(szFileName);
}

void XReplay::SetPause( bool bPause )
{
	m_bPause = bPause;
}

bool XReplay::IsPlaying()
{
	return !m_bPause;
}