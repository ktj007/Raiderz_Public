

#pragma once



#ifndef _XCONFIG
#define _XCONFIG


#include <map>
using namespace std;


class XConfigOption;




/** class XOptionUnit **/
class XOptionUnit
{
private:
	string						m_strValue;
	map< string, string>		m_AttributeList;


public:
	XOptionUnit();
	XOptionUnit( const string& strValue);

	void SetValue( const string& strValue);
	string GetValue();

	void SetAttribute( const string& strAttrName, const string& strAttrValue);
	string GetAttribute( const string& strAttrName);


friend XConfigOption;
};




/** class XConfigOption **/
class XConfigOption
{
	// Member variables
private:
	map< string, XOptionUnit>	m_OptionList;


	// Member functions
public:
	XConfigOption();
	virtual ~XConfigOption();

	virtual void Clear();

	virtual bool LoadXmlFile( const wchar_t* szFileName);
	virtual bool SaveXmlFile( const wchar_t* szFileName);


	void	Set( const char* szName, const string& strValue);
	void	Set( const char* szName, bool   bValue);
	void	Set( const char* szName, int    nValue);
	void	Set( const char* szName, float  fValue);

	string	Get( const char* szName, const string& strDefaultValue);
	bool	Get( const char* szName, bool   bDefaultValue);
	int		Get( const char* szName, int    nDefaultValue);
	float	Get( const char* szName, float  fDefaultValue);


	void	SetAttribute( const char* szName, const string& strAttrName, const string& strAttrValue);
	string	GetAttribute( const char* szName, const string& strAttrName, const string& strDefaultValue);


private:
	bool _parse_xml( MXmlElement* pParent, MXml* pXml, const char* szName = "");
};




/** class XConfig **/
class XConfig : public XConfigOption
{
private:
	// Member variables


	// Member functions
public:
	XConfig();

	void Reset();

	static XConfig* GetInstance();
};



inline XConfig* XGetConfig()
{
	return XConfig::GetInstance();
}




/** Pre-definded option name **/
#define XGETCFG_GAME_SHOWBUFFNAME					XGetConfig()->Get( "GAME,SHOWBUFFNAME", true)
#define XSETCFG_GAME_SHOWBUFFNAME(_val)				XGetConfig()->Set( "GAME,SHOWBUFFNAME", _val)

#define XGETCFG_GAME_AUTONORMALATTACK				XGetConfig()->Get( "GAME,AUTONORMALATTACK", true)
#define XSETCFG_GAME_AUTONORMALATTACK(_val)			XGetConfig()->Set( "GAME,AUTONORMALATTACK", _val)

#define XGETCFG_GAME_COMBATSTANCESITTING			XGetConfig()->Get( "GAME,COMBATSTANCESITTING", false)
#define XSETCFG_GAME_COMBATSTANCESITTING(_val)		XGetConfig()->Set( "GAME,COMBATSTANCESITTING", _val)

#define XGETCFG_GAME_AUTOCHANGE_TONORMALSTANCE			XGetConfig()->Get( "GAME,AUTOCHANGE_TONORMALSTANCE", false)
#define XSETCFG_GAME_AUTOCHANGE_TONORMALSTANCE(_val)	XGetConfig()->Set( "GAME,AUTOCHANGE_TONORMALSTANCE", _val)

#define XGETCFG_GAME_SHOWHELPMESSAGE				XGetConfig()->Get( "GAME,SHOWHELPMESSAGE", true)
#define XSETCFG_GAME_SHOWHELPMESSAGE(_val)			XGetConfig()->Set( "GAME,SHOWHELPMESSAGE", _val)

#define XGETCFG_GAME_SHOWHELMET						XGetConfig()->Get( "GAME,SHOWHELMET", true)
#define XSETCFG_GAME_SHOWHELMET(_val)				XGetConfig()->Set( "GAME,SHOWHELMET", _val)

#define XGETCFG_GAME_SHOWMANTLE						XGetConfig()->Get( "GAME,SHOWMANTLE", true)
#define XSETCFG_GAME_SHOWMANTLE(_val)				XGetConfig()->Set( "GAME,SHOWMANTLE", _val)

#define XGETCFG_GAME_SHOWCHATMSG					XGetConfig()->Get( "GAME,SHOWCHATMSG", true)
#define XSETCFG_GAME_SHOWCHATMSG(_val)				XGetConfig()->Set( "GAME,SHOWCHATMSG", _val)

#define XGETCFG_GAME_SHOWWHISPERMSG					XGetConfig()->Get( "GAME,SHOWWHISPERMSG", true)
#define XSETCFG_GAME_SHOWWHISPERMSG(_val)			XGetConfig()->Set( "GAME,SHOWWHISPERMSG", _val)

#define XGETCFG_GAME_SHOWPARTYMSG					XGetConfig()->Get( "GAME,SHOWPARTYMSG", true)
#define XSETCFG_GAME_SHOWPARTYMSG(_val)				XGetConfig()->Set( "GAME,SHOWPARTYMSG", _val)

#define XGETCFG_GAME_SHOWGUILDMSG					XGetConfig()->Get( "GAME,SHOWGUILDMSG", true)
#define XSETCFG_GAME_SHOWGUILDMSG(_val)				XGetConfig()->Set( "GAME,SHOWGUILDMSG", _val)

#define XGETCFG_GAME_SHOWCHATBUBBLE					XGetConfig()->Get( "GAME,SHOWCHATBUBBLE", true)
#define XSETCFG_GAME_SHOWCHATBUBBLE(_val)			XGetConfig()->Set( "GAME,SHOWCHATBUBBLE", _val)

#define XGETCFG_GAME_ABUSEFILTERING					XGetConfig()->Get( "GAME,ABUSEFILTERING", true)
#define XSETCFG_GAME_ABUSEFILTERING(_val)			XGetConfig()->Set( "GAME,ABUSEFILTERING", _val)

#define XGETCFG_GAME_PLAYERNAME						XGetConfig()->Get( "GAME,PLAYERNAME", true)
#define XSETCFG_GAME_PLAYERNAME(_val)				XGetConfig()->Set( "GAME,PLAYERNAME", _val)

#define XGETCFG_GAME_PLAYERGUILD					XGetConfig()->Get( "GAME,PLAYERGUILD", true)
#define XSETCFG_GAME_PLAYERGUILD(_val)				XGetConfig()->Set( "GAME,PLAYERGUILD", _val)

#define XGETCFG_GAME_MYPLAYERNAME					XGetConfig()->Get( "GAME,MYPLAYERNAME", false)
#define XSETCFG_GAME_MYPLAYERNAME(_val)				XGetConfig()->Set( "GAME,MYPLAYERNAME", _val)

#define XGETCFG_GAME_MYPLAYERGUILD					XGetConfig()->Get( "GAME,MYPLAYERGUILD", false)
#define XSETCFG_GAME_MYPLAYERGUILD(_val)			XGetConfig()->Set( "GAME,MYPLAYERGUILD", _val)

#define XGETCFG_GAME_NPCNAME						XGetConfig()->Get( "GAME,NPCNAME", true)
#define XSETCFG_GAME_NPCNAME(_val)					XGetConfig()->Set( "GAME,NPCNAME", _val)

#define XGETCFG_GAME_NPCGUILD						XGetConfig()->Get( "GAME,NPCGUILD", true)
#define XSETCFG_GAME_NPCGUILD(_val)					XGetConfig()->Set( "GAME,NPCGUILD", _val)

#define XGETCFG_GAME_INDICATOR						XGetConfig()->Get( "GAME,INDICATOR", true)
#define XSETCFG_GAME_INDICATOR(_val)				XGetConfig()->Set( "GAME,INDICATOR", _val)

#define XGETCFG_GAME_MINIHPBAR						XGetConfig()->Get( "GAME,MINIHPBAR", true)
#define XSETCFG_GAME_MINIHPBAR(_val)				XGetConfig()->Set( "GAME,MINIHPBAR", _val)

#define XGETCFG_GAME_CAPTION						XGetConfig()->Get( "GAME,CAPTION", true)
#define XSETCFG_GAME_CAPTION(_val)					XGetConfig()->Set( "GAME,CAPTION", _val)

#define XGETCFG_GAME_RECIPE_REMOVE					XGetConfig()->Get( "GAME,RECIPE_REMOVE", false)
#define XSETCFG_GAME_RECIPE_REMOVE(_val)			XGetConfig()->Set( "GAME,RECIPE_REMOVE", _val)

#define XGETCFG_GAME_USE_AUTOPARTY					XGetConfig()->Get( "GAME,USE_AUTOPARTY", true)
#define XSETCFG_GAME_USE_AUTOPARTY(_val)			XGetConfig()->Set( "GAME,USE_AUTOPARTY", _val)

#define XGETCFG_GAME_REJECT_PARTY					XGetConfig()->Get( "GAME,REJECT_PARTY", false)
#define XSETCFG_GAME_REJECT_PARTY(_val)				XGetConfig()->Set( "GAME,REJECT_PARTY", _val)

#define XGETCFG_GAME_REJECT_GUILD					XGetConfig()->Get( "GAME,REJECT_GUILD", false)
#define XSETCFG_GAME_REJECT_GUILD(_val)				XGetConfig()->Set( "GAME,REJECT_GUILD", _val)

#define XGETCFG_GAME_REJECT_DUEL					XGetConfig()->Get( "GAME,REJECT_DUEL", false)
#define XSETCFG_GAME_REJECT_DUEL(_val)				XGetConfig()->Set( "GAME,REJECT_DUEL", _val)

#define XGETCFG_GAME_REJECT_TRADE					XGetConfig()->Get( "GAME,REJECT_TRADE", false)
#define XSETCFG_GAME_REJECT_TRADE(_val)				XGetConfig()->Set( "GAME,REJECT_TRADE", _val)

#define XGETCFG_GAME_SHOW_FEEDBACK					XGetConfig()->Get( "GAME,SHOW_FEEDBACK", false)
#define XSETCFG_GAME_SHOW_FEEDBACK(_val)			XGetConfig()->Set( "GAME,SHOW_FEEDBACK", _val)

#define XGETCFG_GAME_SHOWCRAFTICON					XGetConfig()->Get( "GAME,SHOWCRAFTICON", true)
#define XSETCFG_GAME_SHOWCRAFTICONE(_val)			XGetConfig()->Set( "GAME,SHOWCRAFTICON", _val)

#define XGETCFG_GAME_SHOWSHOPICON					XGetConfig()->Get( "GAME,SHOWSHOPICON", true)
#define XSETCFG_GAME_SHOWSHOPICON(_val)				XGetConfig()->Set( "GAME,SHOWSHOPICON", _val)

#define XGETCFG_GAME_SHOWQUESTICON					XGetConfig()->Get( "GAME,SHOWQUESTICON", true)
#define XSETCFG_GAME_SHOWQUESTICON(_val)			XGetConfig()->Set( "GAME,SHOWQUESTICON", _val)

#define XGETCFG_GAME_SHOWAREAICON					XGetConfig()->Get( "GAME,SHOWAREAICON", true)
#define XSETCFG_GAME_SHOWAREAICON(_val)				XGetConfig()->Set( "GAME,SHOWAREAICON", _val)

#define XGETCFG_GAME_CROSSHAIR						XGetConfig()->Get( "GAME,CROSSHAIR", string( "Default"))
#define XSETCFG_GAME_CROSSHAIR(_val)				XGetConfig()->Set( "GAME,CROSSHAIR", _val)





#define XGETCFG_VIDEO_WIDTH							XGetConfig()->Get( "VIDEO,WIDTH", -1)
#define XSETCFG_VIDEO_WIDTH(_val)					XGetConfig()->Set( "VIDEO,WIDTH", _val)

#define XGETCFG_VIDEO_HEIGHT						XGetConfig()->Get( "VIDEO,HEIGHT", -1)
#define XSETCFG_VIDEO_HEIGHT(_val)					XGetConfig()->Set( "VIDEO,HEIGHT", _val)

#define XGETCFG_VIDEO_LIGHTING						XGetConfig()->Get( "VIDEO,LIGHTING", 2)
#define XSETCFG_VIDEO_LIGHTING(_val)				XGetConfig()->Set( "VIDEO,LIGHTING", _val)

#define XGETCFG_VIDEO_TEXTUREMIPMAPBIAS				XGetConfig()->Get( "VIDEO,TEXTUREMIPMAPBIAS", 0.0f)
#define XSETCFG_VIDEO_TEXTUREMIPMAPBIAS(_val)		XGetConfig()->Set( "VIDEO,TEXTUREMIPMAPBIAS", _val)

#define XGETCFG_VIDEO_TEXTUREREDUCTION				XGetConfig()->Get( "VIDEO,TEXTUREREDUCTION", 0)
#define XSETCFG_VIDEO_TEXTUREREDUCTION(_val)		XGetConfig()->Set( "VIDEO,TEXTUREREDUCTION", _val)

#define XGETCFG_VIDEO_BRIGHTNESS					XGetConfig()->Get( "VIDEO,BRIGHTNESS", 50)
#define XSETCFG_VIDEO_BRIGHTNESS(_val)				XGetConfig()->Set( "VIDEO,BRIGHTNESS", _val)

#define XGETCFG_VIDEO_VISIBILITYRANGE				XGetConfig()->Get( "VIDEO,VISIBILITYRANGE", 5000)
#define XSETCFG_VIDEO_VISIBILITYRANGE(_val)			XGetConfig()->Set( "VIDEO,VISIBILITYRANGE", _val)

#define XGETCFG_VIDEO_ANTIALIASING					XGetConfig()->Get( "VIDEO,ANTIALIASING", true)
#define XSETCFG_VIDEO_ANTIALIASING(_val)			XGetConfig()->Set( "VIDEO,ANTIALIASING", _val)

#define XGETCFG_VIDEO_NORMALMAPPING					XGetConfig()->Get( "VIDEO,NORMALMAPPING", true)
#define XSETCFG_VIDEO_NORMALMAPPING(_val)			XGetConfig()->Set( "VIDEO,NORMALMAPPING", _val)

#define XGETCFG_VIDEO_FULLSCREEN					XGetConfig()->Get( "VIDEO,FULLSCREEN", true)
#define XSETCFG_VIDEO_FULLSCREEN(_val)				XGetConfig()->Set( "VIDEO,FULLSCREEN", _val)

#define XGETCFG_VIDEO_HDR							XGetConfig()->Get( "VIDEO,HDR", true)
#define XSETCFG_VIDEO_HDR(_val)						XGetConfig()->Set( "VIDEO,HDR", _val)

#define XGETCFG_VIDEO_DOF							XGetConfig()->Get( "VIDEO,DOF", true)
#define XSETCFG_VIDEO_DOF(_val)						XGetConfig()->Set( "VIDEO,DOF", _val)

#define XGETCFG_VIDEO_SHADOW						XGetConfig()->Get( "VIDEO,SHADOW", 2)
#define XSETCFG_VIDEO_SHADOW(_val)					XGetConfig()->Set( "VIDEO,SHADOW", _val)

#define XGETCFG_VIDEO_SSAO							XGetConfig()->Get( "VIDEO,SSAO", 0)
#define XSETCFG_VIDEO_SSAO(_val)					XGetConfig()->Set( "VIDEO,SSAO", _val)

#define XGETCFG_VIDEO_DISTORTION					XGetConfig()->Get( "VIDEO,DISTORTION", true)
#define XSETCFG_VIDEO_DISTORTION(_val)				XGetConfig()->Set( "VIDEO,DISTORTION", _val)

#define XGETCFG_VIDEO_MOTIONBLUR					XGetConfig()->Get( "VIDEO,MOTIONBLUR", true)
#define XSETCFG_VIDEO_MOTIONBLUR(_val)				XGetConfig()->Set( "VIDEO,MOTIONBLUR", _val)

#define XGETCFG_VIDEO_SOFTPARTICLE					XGetConfig()->Get( "VIDEO,SOFTPARTICLE", true)
#define XSETCFG_VIDEO_SOFTPARTICLE(_val)			XGetConfig()->Set( "VIDEO,SOFTPARTICLE", _val)

#define XGETCFG_VIDEO_WEATHEREFFECT					XGetConfig()->Get( "VIDEO,WEATHEREFFECT", true)
#define XSETCFG_VIDEO_WEATHEREFFECT(_val)			XGetConfig()->Set( "VIDEO,WEATHEREFFECT", _val)

#define XGETCFG_VIDEO_GRNDOBJVISIBLERANGE			XGetConfig()->Get( "VIDEO,GRNDOBJVISIBLERANGE", 7000)
#define XSETCFG_VIDEO_GRNDOBJVISIBLERANGE(_val)		XGetConfig()->Set( "VIDEO,GRNDOBJVISIBLERANGE", _val)

#define XGETCFG_VIDEO_MOVIEENCODINGTYPE				XGetConfig()->Get( "VIDEO,MOVIEENCODINGTYPE", string( "MJPEG"))
#define XSETCFG_VIDEO_MOVIEENCODINGTYPE(_val)		XGetConfig()->Set( "VIDEO,MOVIEENCODINGTYPE", _val)

#define XGETCFG_VIDEO_MOVIEENCODINGRES				XGetConfig()->Get( "VIDEO,MOVIEENCODINGRES", 0)
#define XSETCFG_VIDEO_MOVIEENCODINGRES(_val)		XGetConfig()->Set( "VIDEO,MOVIEENCODINGRES", _val)

#define XGETCFG_VIDEO_SHOWUIWHENREC					XGetConfig()->Get( "VIDEO,SHOWUIWHENREC", true)
#define XSETCFG_VIDEO_SHOWUIWHENREC(_val)			XGetConfig()->Set( "VIDEO,SHOWUIWHENREC", _val)

#define XGETCFG_VIDEO_SCREENSHOTFORMAT				XGetConfig()->Get( "VIDEO,SCREENSHOTFORMAT", string("JPEG(*.jpg)"))
#define XSETCFG_VIDEO_SCREENSHOTFORMAT(_val)		XGetConfig()->Set( "VIDEO,SCREENSHOTFORMAT", _val)




#define XGETCFG_AUDIO_MUTE							XGetConfig()->Get( "AUDIO,MUTE", false)
#define XSETCFG_AUDIO_MUTE(_val)					XGetConfig()->Set( "AUDIO,MUTE", _val)

#define XGETCFG_AUDIO_MUTEGENERICSOUND				XGetConfig()->Get( "AUDIO,MUTEGENERICSOUND", false)
#define XSETCFG_AUDIO_MUTEGENERICSOUND(_val)		XGetConfig()->Set( "AUDIO,MUTEGENERICSOUND", _val)

#define XGETCFG_AUDIO_MUTEBGMSOUND					XGetConfig()->Get( "AUDIO,MUTEBGMSOUND", false)
#define XSETCFG_AUDIO_MUTEBGMSOUND(_val)			XGetConfig()->Set( "AUDIO,MUTEBGMSOUND", _val)

#define XGETCFG_AUDIO_MUTEEFFECTSOUND				XGetConfig()->Get( "AUDIO,MUTEEFFECTSOUND", false)
#define XSETCFG_AUDIO_MUTEEFFECTSOUND(_val)			XGetConfig()->Set( "AUDIO,MUTEEFFECTSOUND", _val)

#define XGETCFG_AUDIO_MASTERVOLUME					XGetConfig()->Get( "AUDIO,MASTERVOLUME", 1.0f)
#define XSETCFG_AUDIO_MASTERVOLUME(_val)			XGetConfig()->Set( "AUDIO,MASTERVOLUME", _val)

#define XGETCFG_AUDIO_GENERICVOLUME					XGetConfig()->Get( "AUDIO,GENERICVOLUME", 1.0f)
#define XSETCFG_AUDIO_GENERICVOLUME(_val)			XGetConfig()->Set( "AUDIO,GENERICVOLUME", _val)

#define XGETCFG_AUDIO_BGMVOLUME						XGetConfig()->Get( "AUDIO,BGMVOLUME", 1.0f)
#define XSETCFG_AUDIO_BGMVOLUME(_val)				XGetConfig()->Set( "AUDIO,BGMVOLUME", _val)

#define XGETCFG_AUDIO_EFFECTVOLUME					XGetConfig()->Get( "AUDIO,EFFECTVOLUME", 1.0f)
#define XSETCFG_AUDIO_EFFECTVOLUME(_val)			XGetConfig()->Set( "AUDIO,EFFECTVOLUME", _val)

#define XGETCFG_AUDIO_HARDWAREACC					XGetConfig()->Get( "AUDIO,HARDWAREACC", true)
#define XSETCFG_AUDIO_HARDWAREACC(_val)				XGetConfig()->Set( "AUDIO,HARDWAREACC", _val)

#define XGETCFG_AUDIO_REVERB						XGetConfig()->Get( "AUDIO,REVERB", true)
#define XSETCFG_AUDIO_REVERB(_val)					XGetConfig()->Set( "AUDIO,REVERB", _val)

#define XGETCFG_AUDIO_COMPRESSOR					XGetConfig()->Get( "AUDIO,COMPRESSOR", true)
#define XSETCFG_AUDIO_COMPRESSOR(_val)				XGetConfig()->Set( "AUDIO,COMPRESSOR", _val)

#define XGETCFG_AUDIO_SPEAKERMODE					XGetConfig()->Get( "AUDIO,SPEAKERMODE", 1)
#define XSETCFG_AUDIO_SPEAKERMODE(_val)				XGetConfig()->Set( "AUDIO,SPEAKERMODE", _val)

#define XGETCFG_AUDIO_INACTIVESOUND					XGetConfig()->Get( "AUDIO,INACTIVESOUND", true)
#define XSETCFG_AUDIO_INACTIVESOUND(_val)			XGetConfig()->Set( "AUDIO,INACTIVESOUND", _val)

#define XGETCFG_AUDIO_SYSTEMMESSAGE					XGetConfig()->Get( "AUDIO,SYSTEMMESSAGE", 1)
#define XSETCFG_AUDIO_SYSTEMMESSAGE(_val)			XGetConfig()->Set( "AUDIO,SYSTEMMESSAGE", _val)




#define XGETCFG_CONTROL_INVERTEDMOUSE				XGetConfig()->Get( "CONTROL,INVERTEDMOUSE", false)
#define XSETCFG_CONTROL_INVERTEDMOUSE(_val)			XGetConfig()->Set( "CONTROL,INVERTEDMOUSE", _val)

#define XGETCFG_CONTROL_MOUSESENSITIVITY			XGetConfig()->Get( "CONTROL,MOUSESENSITIVITY", 50)
#define XSETCFG_CONTROL_MOUSESENSITIVITY(_val)		XGetConfig()->Set( "CONTROL,MOUSESENSITIVITY", _val)





inline void _get_key_data( const char* _name, string& _code, const string& _def_code, string& _ext, string _def_ext)
{
	_code = XGetConfig()->Get( _name, _def_code);
	_ext = XGetConfig()->GetAttribute( _name, "ext", _def_ext);
}

inline void _set_key_data( const char* _name, const string& _code, const string& _ext)
{
	XGetConfig()->Set( _name, _code);
	XGetConfig()->SetAttribute( _name, "ext", _ext);
}


#define XGETCFG_KEY_FORWARD(_code,_ext)				_get_key_data( "KEY,FORWARD", _code, "KEY_W", _ext, "")
#define XSETCFG_KEY_FORWARD(_code,_ext)				_set_key_data( "KEY,FORWARD", _code, _ext)

#define XGETCFG_KEY_BACKWARD(_code,_ext)			_get_key_data( "KEY,BACKWARD", _code, "KEY_S", _ext, "")
#define XSETCFG_KEY_BACKWARD(_code,_ext)			_set_key_data( "KEY,BACKWARD", _code, _ext)

#define XGETCFG_KEY_LEFT(_code,_ext)				_get_key_data( "KEY,LEFT", _code, "KEY_A", _ext, "")
#define XSETCFG_KEY_LEFT(_code,_ext)				_set_key_data( "KEY,LEFT", _code, _ext)

#define XGETCFG_KEY_RIGHT(_code,_ext)				_get_key_data( "KEY,RIGHT", _code, "KEY_D", _ext, "")
#define XSETCFG_KEY_RIGHT(_code,_ext)				_set_key_data( "KEY,RIGHT", _code, _ext)

#define XGETCFG_KEY_ACTION(_code,_ext)				_get_key_data( "KEY,ACTION", _code, "KEY_LBUTTON", _ext, "")
#define XSETCFG_KEY_ACTION(_code,_ext)				_set_key_data( "KEY,ACTION", _code, _ext)

#define XGETCFG_KEY_ALTACTION(_code,_ext)			_get_key_data( "KEY,ALTACTION", _code, "KEY_RBUTTON", _ext, "")
#define XSETCFG_KEY_ALTACTION(_code,_ext)			_set_key_data( "KEY,ALTACTION", _code, _ext)

#define XGETCFG_KEY_JUMP(_code,_ext)				_get_key_data( "KEY,JUMP", _code, "KEY_SPACE", _ext, "")
#define XSETCFG_KEY_JUMP(_code,_ext)				_set_key_data( "KEY,JUMP", _code, _ext)

#define XGETCFG_KEY_CHANGESTANCE(_code,_ext)		_get_key_data( "KEY,CHANGESTANCE", _code, "KEY_F", _ext, "")
#define XSETCFG_KEY_CHANGESTANCE(_code,_ext)		_set_key_data( "KEY,CHANGESTANCE", _code, _ext)

#define XGETCFG_KEY_CHANGEWEAPON(_code,_ext)		_get_key_data( "KEY,CHANGEWEAPON", _code, "KEY_TAB", _ext, "")
#define XSETCFG_KEY_CHANGEWEAPON(_code,_ext)		_set_key_data( "KEY,CHANGEWEAPON", _code, _ext)

#define XGETCFG_KEY_SIT(_code,_ext)					_get_key_data( "KEY,SIT", _code, "KEY_Z", _ext, "")
#define XSETCFG_KEY_SIT(_code,_ext)					_set_key_data( "KEY,SIT", _code, _ext)

#define XGETCFG_KEY_AUTORUN(_code,_ext)				_get_key_data( "KEY,AUTORUN", _code, "KEY_R", _ext, "")
#define XSETCFG_KEY_AUTORUN(_code,_ext)				_set_key_data( "KEY,AUTORUN", _code, _ext)

#define XGETCFG_KEY_NPCINTERACTION(_code,_ext)		_get_key_data( "KEY,NPCINTERACTION", _code, "KEY_E", _ext, "")
#define XSETCFG_KEY_NPCINTERACTION(_code,_ext)		_set_key_data( "KEY,NPCINTERACTION", _code, _ext)

#define XGETCFG_KEY_PCINTERACTION(_code,_ext)		_get_key_data( "KEY,PCINTERACTION", _code, "KEY_G", _ext, "")
#define XSETCFG_KEY_PCINTERACTION(_code,_ext)		_set_key_data( "KEY,PCINTERACTION", _code, _ext)

#define XGETCFG_KEY_LOCKTARGET(_code,_ext)			_get_key_data( "KEY,LOCKTARGET", _code, "KEY_Y", _ext, "")
#define XSETCFG_KEY_LOCKTARGET(_code,_ext)			_set_key_data( "KEY,LOCKTARGET", _code, _ext)

#define XGETCFG_KEY_MAINMENU(_code,_ext)			_get_key_data( "KEY,MAINMENU", _code, "KEY_ESCAPE", _ext, "")
#define XSETCFG_KEY_MAINMENU(_code,_ext)			_set_key_data( "KEY,MAINMENU", _code, _ext)

#define XGETCFG_KEY_CHARACTER(_code,_ext)			_get_key_data( "KEY,CHARACTER", _code, "KEY_H", _ext, "")
#define XSETCFG_KEY_CHARACTER(_code,_ext)			_set_key_data( "KEY,CHARACTER", _code, _ext)

#define XGETCFG_KEY_INVENTORY(_code,_ext)			_get_key_data( "KEY,INVENTORY", _code, "KEY_I", _ext, "")
#define XSETCFG_KEY_INVENTORY(_code,_ext)			_set_key_data( "KEY,INVENTORY", _code, _ext)

#define XGETCFG_KEY_TALENT(_code,_ext)				_get_key_data( "KEY,TALENT", _code, "KEY_T", _ext, "")
#define XSETCFG_KEY_TALENT(_code,_ext)				_set_key_data( "KEY,TALENT", _code, _ext)

#define XGETCFG_KEY_JOURNAL(_code,_ext)				_get_key_data( "KEY,JOURNAL", _code, "KEY_J", _ext, "")
#define XSETCFG_KEY_JOURNAL(_code,_ext)				_set_key_data( "KEY,JOURNAL", _code, _ext)

#define XGETCFG_KEY_SOCIAL(_code,_ext)				_get_key_data( "KEY,SOCIAL", _code, "KEY_P", _ext, "")
#define XSETCFG_KEY_SOCIAL(_code,_ext)				_set_key_data( "KEY,SOCIAL", _code, _ext)

#define XGETCFG_KEY_MAP(_code,_ext)					_get_key_data( "KEY,MAP", _code, "KEY_M", _ext, "")
#define XSETCFG_KEY_MAP(_code,_ext)					_set_key_data( "KEY,MAP", _code, _ext)

#define XGETCFG_KEY_CONTROLSHELP1(_code,_ext)		_get_key_data( "KEY,CONTROLSHELP1", _code, "KEY_F1", _ext, "")
#define XSETCFG_KEY_CONTROLSHELP1(_code,_ext)		_set_key_data( "KEY,CONTROLSHELP1", _code, _ext)

#define XGETCFG_KEY_CONTROLSHELP2(_code,_ext)		_get_key_data( "KEY,CONTROLSHELP2", _code, "KEY_F2", _ext, "")
#define XSETCFG_KEY_CONTROLSHELP2(_code,_ext)		_set_key_data( "KEY,CONTROLSHELP2", _code, _ext)

#define XGETCFG_KEY_TOGGLEMOUSE(_code,_ext)			_get_key_data( "KEY,TOGGLEMOUSE", _code, "KEY_CONTROL", _ext, "")
#define XSETCFG_KEY_TOGGLEMOUSE(_code,_ext)			_set_key_data( "KEY,TOGGLEMOUSE", _code, _ext)

#define XGETCFG_KEY_TOGGLE_FREECAMERA(_code,_ext)	_get_key_data( "KEY,TOGGLE_FREECAMERA", _code, "KEY_MBUTTON", _ext, "")
#define XSETCFG_KEY_TOGGLE_FREECAMERA(_code,_ext)	_set_key_data( "KEY,TOGGLE_FREECAMERA", _code, _ext)

#define XGETCFG_KEY_GUARD(_code,_ext)				_get_key_data( "KEY,GUARD", _code, "KEY_Q", _ext, "")
#define XSETCFG_KEY_GUARD(_code,_ext)				_set_key_data( "KEY,GUARD", _code, _ext)

#define XGETCFG_KEY_DODGE(_code,_ext)				_get_key_data( "KEY,DODGE", _code, "KEY_SHIFT", _ext, "")
#define XSETCFG_KEY_DODGE(_code,_ext)				_set_key_data( "KEY,DODGE", _code, _ext)

#define XGETCFG_KEY_PALETTE0(_code,_ext)			_get_key_data( "KEY,PALETTE0", _code, "KEY_1", _ext, "")
#define XSETCFG_KEY_PALETTE0(_code,_ext)			_set_key_data( "KEY,PALETTE0", _code, _ext)
#define XGETCFG_KEY_PALETTE1(_code,_ext)			_get_key_data( "KEY,PALETTE1", _code, "KEY_2", _ext, "")
#define XSETCFG_KEY_PALETTE1(_code,_ext)			_set_key_data( "KEY,PALETTE1", _code, _ext)
#define XGETCFG_KEY_PALETTE2(_code,_ext)			_get_key_data( "KEY,PALETTE2", _code, "KEY_3", _ext, "")
#define XSETCFG_KEY_PALETTE2(_code,_ext)			_set_key_data( "KEY,PALETTE2", _code, _ext)
#define XGETCFG_KEY_PALETTE3(_code,_ext)			_get_key_data( "KEY,PALETTE3", _code, "KEY_4", _ext, "")
#define XSETCFG_KEY_PALETTE3(_code,_ext)			_set_key_data( "KEY,PALETTE3", _code, _ext)
#define XGETCFG_KEY_PALETTE4(_code,_ext)			_get_key_data( "KEY,PALETTE4", _code, "KEY_5", _ext, "")
#define XSETCFG_KEY_PALETTE4(_code,_ext)			_set_key_data( "KEY,PALETTE4", _code, _ext)
#define XGETCFG_KEY_PALETTE5(_code,_ext)			_get_key_data( "KEY,PALETTE5", _code, "KEY_6", _ext, "")
#define XSETCFG_KEY_PALETTE5(_code,_ext)			_set_key_data( "KEY,PALETTE5", _code, _ext)
#define XGETCFG_KEY_PALETTE6(_code,_ext)			_get_key_data( "KEY,PALETTE6", _code, "KEY_7", _ext, "")
#define XSETCFG_KEY_PALETTE6(_code,_ext)			_set_key_data( "KEY,PALETTE6", _code, _ext)
#define XGETCFG_KEY_PALETTE7(_code,_ext)			_get_key_data( "KEY,PALETTE7", _code, "KEY_8", _ext, "")
#define XSETCFG_KEY_PALETTE7(_code,_ext)			_set_key_data( "KEY,PALETTE7", _code, _ext)
#define XGETCFG_KEY_PALETTE8(_code,_ext)			_get_key_data( "KEY,PALETTE8", _code, "KEY_9", _ext, "")
#define XSETCFG_KEY_PALETTE8(_code,_ext)			_set_key_data( "KEY,PALETTE8", _code, _ext)
#define XGETCFG_KEY_PALETTE9(_code,_ext)			_get_key_data( "KEY,PALETTE9", _code, "KEY_0", _ext, "")
#define XSETCFG_KEY_PALETTE9(_code,_ext)			_set_key_data( "KEY,PALETTE9", _code, _ext)

#define XGETCFG_KEY_PALETTESET0(_code,_ext)			_get_key_data( "KEY,PALETTESET0", _code, "KEY_1", _ext, "shift")
#define XSETCFG_KEY_PALETTESET0(_code,_ext)			_set_key_data( "KEY,PALETTESET0", _code, _ext)
#define XGETCFG_KEY_PALETTESET1(_code,_ext)			_get_key_data( "KEY,PALETTESET1", _code, "KEY_2", _ext, "shift")
#define XSETCFG_KEY_PALETTESET1(_code,_ext)			_set_key_data( "KEY,PALETTESET1", _code, _ext)
#define XGETCFG_KEY_PALETTESET2(_code,_ext)			_get_key_data( "KEY,PALETTESET2", _code, "KEY_3", _ext, "shift")
#define XSETCFG_KEY_PALETTESET2(_code,_ext)			_set_key_data( "KEY,PALETTESET2", _code, _ext)
#define XGETCFG_KEY_PALETTESET3(_code,_ext)			_get_key_data( "KEY,PALETTESET3", _code, "KEY_4", _ext, "shift")
#define XSETCFG_KEY_PALETTESET3(_code,_ext)			_set_key_data( "KEY,PALETTESET3", _code, _ext)
#define XGETCFG_KEY_PALETTESET4(_code,_ext)			_get_key_data( "KEY,PALETTESET4", _code, "KEY_5", _ext, "shift")
#define XSETCFG_KEY_PALETTESET4(_code,_ext)			_set_key_data( "KEY,PALETTESET4", _code, _ext)



// 메뉴에는 포함되어 있지 않으나 시스템에서 필요한 옵션들...
#define XGETCFG_VIDEO_COLORBITS						XGetConfig()->Get( "VIDEO,COLORBITS", 32)
#define XSETCFG_VIDEO_COLORBITS(_val)				XGetConfig()->Set( "VIDEO,COLORBITS", _val)

#define XGETCFG_VIDEO_DEPTHBITS						XGetConfig()->Get( "VIDEO,DEPTHBITS", 24)
#define XSETCFG_VIDEO_DEPTHBITS(_val)				XGetConfig()->Set( "VIDEO,DEPTHBITS", _val)

#define XGETCFG_VIDEO_SHADER						XGetConfig()->Get( "VIDEO,SHADER", true)
#define XSETCFG_VIDEO_SHADER(_val)					XGetConfig()->Set( "VIDEO,SHADER", _val)



#endif