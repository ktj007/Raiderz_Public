#ifndef _X_TEXTURE_MT_INFO_H
#define _X_TEXTURE_MT_INFO_H

typedef map<wstring, wstring> MAP_TEXTURE_MT;

class XTextureMtInfoMgr : public MAP_TEXTURE_MT
{
private:
	void ParseTexture(MXmlElement* pElement);

public:
	XTextureMtInfoMgr(void);
	~XTextureMtInfoMgr(void);

	bool		Load(const wchar_t* szFileName);
	wstring		GetMaterial(const wchar_t* texturename);
};

#endif // _X_TEXTURE_MT_INFO_H
