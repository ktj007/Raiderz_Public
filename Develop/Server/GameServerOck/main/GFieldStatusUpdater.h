#ifndef _GFIELD_STATUS_UPDATER_H
#define _GFIELD_STATUS_UPDATER_H

const static float FILED_STATUS_UPDATE_ELAPS_SEC = 3600.0f;



class GFieldStatusUpdater : public MTestMemPool<GFieldStatusUpdater>
{
public :
	GFieldStatusUpdater();

	void Update(const float fDelta);


protected :
	MRegulator m_Rglt;
};


#endif