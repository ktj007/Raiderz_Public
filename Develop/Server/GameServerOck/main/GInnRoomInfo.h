#ifndef _G_INN_ROOM_INFO_H_
#define _G_INN_ROOM_INFO_H_

enum INN_ROOM_TYPE
{
	IRT_NONE = 0,

	IRT_NORMAL,
	IRT_BASIC,
	IRT_STORAGE,
	IRT_STEWARD,
	IRT_PREMIUM,

	IRT_MAX
};

class GInnRoomInfo : public MTestMemPool<GInnRoomInfo>
{
	friend class GInnRoomInfoMgr;
private:
	GInnRoomInfo();

public:
	uint8			m_nID;
	INN_ROOM_TYPE	m_nType;
	int				m_nFieldGroupID;	
	int				m_nDefaultBedNPCID;	
	int				m_nDefaultStorageNPCID;
	int				m_nDefaultGuildStorageNPCID;

	unsigned long	m_nCraftingMarkerID;
	unsigned long	m_nBedMarkerID;
	unsigned long	m_nCarpetMarkerID;
	unsigned long	m_nFirePlaceMarkerID;
	unsigned long	m_nMuralMarkerID;
	unsigned long	m_nFunitureMarkerID;
	unsigned long	m_nTableMarkerID;
	unsigned long	m_nStewardMarkerID;	
	unsigned long	m_nStorageMarkerID;
	unsigned long	m_nGuildStorageMarkerID;
};

#endif