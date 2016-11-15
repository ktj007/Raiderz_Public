#ifndef _GDB_LOGOUTFIELD_BUILDER_H
#define _GDB_LOGOUTFIELD_BUILDER_H


class GEntityPlayer;
class GField;


class GDBLogoutFieldBuilder
{
public :
	GDBLogoutFieldBuilder(GEntityPlayer* pPlayer);

	void	Build();

	vec3&	GetPlayerPos();
	int		GetSharedFieldID();
	MUID&	GetDyanmicFieldGroupUID();
	int		GetDynamicFieldID();
	vec3&	GetEnterPos();


private :
	void	Init();
	bool	IsEnteringToOtherField();
	bool	SetField();
	void	SetPlayerPos();
	void	SetSharedFieldID();
	void	SetDyanmicField();


private :
	struct LOGOUT_DYANMICFIELD
	{
		MUID	uidDynamicFieldGroup;
		int		nDynamicFieldID;
		vec3	vEnterPos;
	};

	
private :		
	GEntityPlayer*			m_pPlayer;
	GField*					m_pField;

	vec3					m_vPlayerPos;
	int						m_nSharedFieldID;
	LOGOUT_DYANMICFIELD		m_Field;
};


#endif