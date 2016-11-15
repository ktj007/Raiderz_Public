#ifndef _GDYNAMIC_FIELD_H_
#define _GDYNAMIC_FIELD_H_

#include "GField.h"

class GDynamicField : public GField
{
private:
	MUID	m_uidGroup;
	

protected:

	void RouteCancelExpire( vector<MUID> vecPlayers );
	void Update_Weather(float fDelta);			///< 날씨 업데이트
public:
	GDynamicField(const MUID& uid, const MUID& uidGroup);
	virtual ~GDynamicField();

	virtual bool		Create(const GFieldInfo* pFieldInfo) override;
	virtual void		Update(float fDelta) override;

	virtual bool		IsDynamicField() override	{ return true; }
	MUID				GetGroupUID() { return m_uidGroup; }

protected:
	virtual void AddEntity( GEntity* pEntity );
};


GDynamicField* ToDynamicField( GField* pField );

#endif//_GDYNAMIC_FIELD_H_
