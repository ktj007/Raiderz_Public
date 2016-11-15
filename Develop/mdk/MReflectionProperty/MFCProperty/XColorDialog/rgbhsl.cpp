#include "stdafx.h"

#include "windows.h"
#include "rgbhsl.h"

//=============================================================================
static double HuetoRGB(double m1, double m2, double h)
//=============================================================================
{
	if (h < 0) h += 1.0;
	if (h > 1) h -= 1.0;
	if (6.0 * h < 1 )
		return (m1 + (m2 - m1) * h * 6.0);
	if (2.0 * h < 1 )
		return m2;
	if (3.0 * h < 2.0 )
		return (m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6.0);
	return m1;
}

//=============================================================================
void RGBtoHSL(COLORREF cr, double *H, double *S, double *L)
//=============================================================================
{
	double delta;
	double r = (double) GetRValue(cr) / 255;
	double g = (double) GetGValue(cr) / 255;
	double b = (double) GetBValue(cr) / 255;
	double cmax = max(r, max(g,b));
	double cmin = min(r, min(g,b));
	
	*L = (cmax + cmin) / 2.0;
	
	if (cmax == cmin)
	{
		*S = 0;
		*H = 0; // it's really undefined
	}
	else
	{
		if (*L < 0.5)
			*S = (cmax - cmin) / (cmax + cmin);
		else
			*S = (cmax - cmin) / (2.0 - cmax - cmin);
		
		delta = cmax - cmin;
		
		if (r == cmax)
			*H = (g - b) / delta;
		else if (g == cmax)
			*H = 2.0 + (b - r) / delta;
		else
			*H = 4.0 + (r - g) / delta;
		
		*H /= 6.0;
		
		if (*H < 0.0)
			*H += 1;
	}
	*H *= 240.0;
	*S *= 240.0;
	*L *= 240.0;
}

//=============================================================================
void RGBtoHSL(COLORREF cr, BYTE *h, BYTE *s, BYTE *l)
//=============================================================================
{
	double H, S, L;
	RGBtoHSL(cr, &H, &S, &L);
	*h = (BYTE) (H + 0.5);
	*s = (BYTE) (S + 0.5);
	*l = (BYTE) (L + 0.5);
	if (*h > 239)
		*h = 239;
	if (*s > 240)
		*s = 240;
	if (*l > 240)
		*l = 240;
}

//=============================================================================
COLORREF HSLtoRGB(double H, double S, double L)
//=============================================================================
{
	double r, g, b;
	double m1, m2;

	H /= 240.0;
	S /= 240.0;
	L /= 240.0;

	if (S == 0)
	{
		r = g = b = L;
	}
	else
	{
		if (L <= 0.5)
			m2 = L * (1.0 + S);
		else
			m2 = L + S - L * S;
		
		m1 = 2.0 * L - m2;
		r = HuetoRGB(m1, m2, H + 1.0/3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H - 1.0/3.0);
	}
	
	return RGB((BYTE)(r*255), (BYTE)(g*255), (BYTE)(b*255));
}
