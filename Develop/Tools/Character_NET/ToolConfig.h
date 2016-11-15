#pragma once

class CToolConfig
{
public:
	CToolConfig(void);
	~CToolConfig(void);

	bool LoadConfig();
	bool LoadSysConfig();
	bool SaveConfig();

	float		CAMERA_FOV;		// FOV (°¢µµ)
	int			start_X;
	int			start_Y;
	bool		maximize;
	int			WinForm_width;
	int			WinForm_height;
	int			split1;
	int			split2;

	static CToolConfig& GetInstance() {
		static CToolConfig instance;
		return instance;
	}
};
