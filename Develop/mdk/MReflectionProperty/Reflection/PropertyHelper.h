#pragma once

#define DEFINE_GET_SET_STRING( _name ) \

#define DECLARE_GET_SET(x,y)	\
	x		get##y(void)	{ return y;	}	\
	void	set##y(x _t)	{ 	y = _t;	}

#define DECLARE_GET_SET_STRING(y)	\
	char*	get##y(void)		{ return (char*)(y).c_str();	}	\
	void	set##y(char* _t)	{ 	y = _t;	}

#define DECLARE_GET_SET_WSTRING(y)	\
	wchar_t*get##y(void)		{ return (wchar_t*)(y).c_str();	}	\
	void	set##y(wchar_t* _t)	{ 	y = _t;	}

// enum 은 int 형으로 처리하고, description 스트링에 대응되는 문자열을 적는다
#define DECLARE_GET_SET_ENUM(x,y,_min,_max)	\
	int		get##y(void)	{ return (int)y;	}	\
	void	set##y(int _t)	{ 	y = (x)_t;	}	\
	int		getMin##y(void)	{ return (int)_min;	}	\
	int		getMax##y(void)	{ return (int)_max;	}

#define SWAP_RGB_BGR(x) RGB(GetBValue(x), GetGValue(x), GetRValue(x) )

#define DECLARE_GET_SET_COLOR(y)	\
	DWORD	get##y(void)		{ return SWAP_RGB_BGR(y);	}	\
	void	set##y(DWORD _t)	{ y = SWAP_RGB_BGR(_t);	}

#define REGISTER_PROPERTY(_name,_type,_var,_desc)	RegisterProperty<_type>( _name ,&get##_var,&set##_var, 0, 0, _desc);

#define REGISTER_PROPERTY_ENUM(_name,_var,_desc)	RegisterProperty<int>( _name ,&get##_var, &set##_var, &getMin##_var, &getMax##_var, _desc );

#define REGISTER_PROPERTY_COLOR(_name,_var,_desc)	RegisterProperty<DWORD>( _name ,&get##_var,&set##_var, 0, 0, _desc, DEFAULT_CATEGORY_NAME, PE_COLOR);


