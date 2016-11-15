#pragma once


namespace mint3 {


/// ScriptHelper
class MScriptHelper
{
protected:
	MAlignmentMode	m_nAlign;


public:
	MScriptHelper();

	void Line( int x1, int y1, int x2, int y2);
	void Rect( int x, int y, int w, int h);
	void FillRect( int x, int y, int w, int h);
	void Draw( int x, int y, int w, int h);
	void DrawEx( int tx, int ty, int tw, int th, int sx, int sy, int sw, int sh);
	void Text( int x, int y, const char* szText);
	void TextEx( int x, int y, int w, int h, const char* szText);
	MWLua::ret_int4 SetColor( int r, int g, int b);
	MWLua::ret_int4 SetColorEx( int r, int g, int b, int a);
	MWLua::ret_int4 GetColor() const;
	MWLua::ret_int4 SetBitmapColor( int r, int g, int b);
	MWLua::ret_int4 SetBitmapColorEx( int r, int g, int b, int a);
	MWLua::ret_int4 GetBitmapColor() const;
	const char* SetEffect( const char* szEffect);
	const char* GetEffect();
	float SetOpacity( float fOpacity);
	float GetOpacity() const;
	void SetBitmap( const char* szBitmap);
	const char* GetBitmap() const;
	void SetFont( const char* szFont);
	void SetTextAlign( const char* szHorz, const char* szVert);
	int GetTextWidth( const char* szFont, const char* szText);
	const char* SubTextWidth( const char* szFont, const char* szText, int nWidth);
};


} // namespace mint3
